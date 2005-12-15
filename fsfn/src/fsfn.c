/***************************************************************************
 *   Copyright (C) 2005 by Poissinger Pierre   *
 *   pierre.poissinger@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <syslog.h>

// input thing
#include <linux/input.h>

#include "generics.h"
#include "acpihandler.h"
#include "alsasound.h"

// config
#include "readconfig.h"

// autodev
#include "autodev.h"

#ifdef HAVE_LIBXOSD
#include "basicmsg.h"
#include "osd.h"
#endif

#define SOUND_STEP	getConfigInt("SOUND_STEP") //mod by SilSha

#define MAX_DEVINPUT_SIZE	255
static char devinput[MAX_DEVINPUT_SIZE];

// one way out
void cleanExit(int ret) {
#ifdef HAVE_LIBXOSD
	killqueue ();
#endif
	releaseConfig();
	closelog();
	exit(ret);	
}

// Check and run possible config
// ret 1 if something executed - 0 otherwise
int checkConfig(char* key) 
  {
	char *buffer;
	buffer=getConfig(key);
	
	if ((buffer)&&(buffer[0]!='\0')) {
		syslog(LOG_NOTICE,"Executing [%s]",buffer);
	 	if (fork () == 0)
		{
		    if (execv (buffer, NULL) == -1)
		    {
		      syslog (LOG_NOTICE,"Cannot run [%s]: %m",buffer);
		      exit(0);
		    }
		}
		return 1;
	}
	return 0;
  }

/* signal handler */
void signal_handler(sig)
{
	cleanExit(EXIT_FAILURE);
}

void
loop ()
{
  // event interface
  int fd = -1;			/* the file descriptor for the device */
  int i;			/* loop counter */
  size_t read_bytes;		/* how many bytes were read */
  struct input_event ev[64];	/* the events (up to 64 at once) */
  int key;			/*key code */
  /* used if event hit fn */
  int hasSomething;

#ifdef HAVE_LIBXOSD
  // prepare queue handling
  int flag = 0, brightness = 0, sound = 0;
  createqueue ();
#endif
  
  if (strcasecmp(devinput,"AUTO")==0) { // try to figure out rigth event value for keyboard
	  snprintf(devinput,MAX_DEVINPUT_SIZE,"/dev/input/event%d",getItemEvent(DEFAULT_KEYBOARD_NAME));
	  syslog(LOG_INFO,"autodevice determines %s as keyboard event",devinput);
  }
  
  if ((fd = open (devinput, O_RDONLY)) < 0)
    {
      syslog (LOG_CRIT,"event interface (%s) open failed: %m",devinput);
      // shoot auto as LAST chance
      snprintf(devinput,MAX_DEVINPUT_SIZE,"/dev/input/event%d",getItemEvent(DEFAULT_KEYBOARD_NAME));
      syslog(LOG_CRIT,"autodevice determines %s as a last chance keyboard event",devinput);
      if ((fd = open (devinput, O_RDONLY)) < 0)
        {
	      syslog(LOG_CRIT,"Event interface (%s) open failed: %m",devinput);
              cleanExit(EXIT_FAILURE);
	}
    }

  /* handle important signal */
  if (signal(SIGTERM,signal_handler) < 0)
    {
      perror("signal");
      exit(EXIT_FAILURE);
    }
  if (signal(SIGHUP,signal_handler) < 0)
    {
      perror("signal");
      exit(EXIT_FAILURE);
    }

  syslog(LOG_INFO,"fsfn loaded");


  while (1)
    {				/* loop */
      hasSomething = 0;		/* nothing yet */

      /*
       * read the event interface 
       */
      if ( (read_bytes = read (fd, ev, sizeof (struct input_event) * 64))==-1) {
	      //fprintf(stderr,"Error: %d\n",errno);
	      if (errno==ENODEV) { // event is now invalid ? must be back from sleep...
		      syslog(LOG_NOTICE,"Start sleeping...");
		      sleep(10);		      
		      syslog(LOG_NOTICE,"End sleeping...");

		      close(fd); // is this needed ??
		      
		      syslog(LOG_NOTICE,"Input device changed, back from suspend ?: %m");
		      
		      // get new event
		      snprintf(devinput,MAX_DEVINPUT_SIZE,"/dev/input/event%d",getItemEvent(DEFAULT_KEYBOARD_NAME));
      		      syslog(LOG_NOTICE,"autodevice determines %s as new event",devinput);
		      
		      // reopen - seems to be problems after a hibernate :(
      		      if ((fd = open (devinput, O_RDONLY)) < 0)
        	        {
				syslog(LOG_CRIT,"New event interface (%s) open failed: %m",devinput); 
				cleanExit(EXIT_FAILURE);
			}
		      // read it
		      if ((read_bytes = read (fd, ev, sizeof (struct input_event) * 64))==-1) 
		        {
				syslog(LOG_CRIT,"Reading new device (%s) failed: %m",devinput);
				cleanExit(EXIT_FAILURE);
		        }
	      }
	      else {
		      syslog(LOG_CRIT,"Input device reading failed: %m");
		      cleanExit(EXIT_FAILURE);
	      }
	}
      	
      if (read_bytes < (int) sizeof (struct input_event))
	{
	  syslog (LOG_CRIT,"short read: %m");
	  cleanExit(EXIT_FAILURE);
	}
      
      /*
       * Loop for all readed events until we have something
       * interesting.. 
       */
      for (i = 0;
	   !hasSomething
	   && (i < (int) (read_bytes / sizeof (struct input_event))); i++)
	{
	  hasSomething = (ev[i].type == FN_INPUT_TYPE)
	    && (ev[i].code == FN_INPUT_CODE)
	    && (ev[i].value == FN_INPUT_VALUE);
	}
      
      /*
       * If we got a FN event, plz do something... 
       */
      if (hasSomething && (key = getCodes ()))
	{
	  if ((key & FN_F5) == FN_F5)
	    { 
	      	// check config
	      	if (!checkConfig("F5_CMD"))
		  {
	      		// lower brightness
#ifdef HAVE_LIBXOSD
	      		flag = MOD_BRIGHTNESS;
	      		brightness = setBrightness (getBrightness () - 1);
	      		sendmsg (flag, brightness, sound);
#else
	      		setBrightness (getBrightness () - 1);
#endif
		  }
	    }
	  if ((key & FN_F6) == FN_F6)
	    {
	    	// check config
		if (!checkConfig("F6_CMD")) 
		  {
		  	
	    		// higher brightness
#ifdef HAVE_LIBXOSD
	      		flag = MOD_BRIGHTNESS;
	      		brightness = setBrightness (getBrightness () + 1);
	      		sendmsg (flag, brightness, sound);
#else
	      		setBrightness (getBrightness () + 1);
#endif
		  }
	    }

	  if ((key & FN_F2) == FN_F2)
	    {
		// check config
		if (!checkConfig("F2_CMD"))
		  {
#ifdef HAVE_LIBXOSD
	      		flag = MOD_SOUND;
	      		sound = mute (SOUND_STEP);
	      		sendmsg (flag, brightness, sound);
#else
	      		mute (SOUND_STEP);			
#endif
		  }
	    }
	  if ((key & FN_F3) == FN_F3)
	    {
		if (!checkConfig("F3_CMD"))
	          {
#ifdef HAVE_LIBXOSD
	      		flag = MOD_SOUND;
	      		sound = volume_down (SOUND_STEP);	//mod by SilSha
	      		sendmsg (flag, brightness, sound);
#else
	      		volume_down (SOUND_STEP);
#endif
		  }
	    }
	  if ((key & FN_F4) == FN_F4)
	    {
	       if (!checkConfig("F4_CMD"))
	         {
#ifdef HAVE_LIBXOSD
	      		flag = MOD_SOUND;
	      		sound = volume_up (SOUND_STEP);		//mod by SilSha
	      		sendmsg (flag, brightness, sound);
#else
	      		volume_up (SOUND_STEP);
#endif
		 }	
	    }
	 /* NO built in commands */
	  if ((key & FN_F7) == FN_F7)
	    {
		  checkConfig("F7_CMD");
	    }
	  if ((key & FN_F10) == FN_F10)
	    {
		  checkConfig("F10_CMD");
	    }
	  if ((key & FN_F12) == FN_F12)
	    {
		 checkConfig("F12_CMD");
	    }
	  if (( key & S1_BTN) == S1_BTN) 
	    {
		 checkConfig("S1_CMD");
	    }
	  if (( key & S2_BTN) == S2_BTN)
	    {
		 checkConfig("S2_CMD");
	    }		  
	}
    }// while
}

#ifdef HAVE_LIBXOSD
void
looposd ()
{
  int flag = 0, brightness = 0, sound = 0;

  osd_load ();

  loadqueue ();

  while (1 == 1)
    {
      if (getmsg (&flag, &brightness, &sound)==-1) {
      	break; // exit the loop if problems
      }

      if (flag & MOD_BRIGHTNESS)
	{
	  osd_brightness (brightness);
	}
      if (flag & MOD_SOUND)
	{
	  osd_volume (sound);
	}
    }

  osd_unload ();
}
#endif

void
deamonize ()
{
  int pidfile;
  int i;
  char str[10];
  
  switch (fork ())
    {
    case 0:
	    
      setsid();
      for (i=getdtablesize();i>=0;--i) close(i);
      i=open("/dev/null",O_RDWR); /* open stdin */
      dup(i); /* stdout */
      dup(i); /* stderr */
      umask(027);
      
      if ((pidfile=open(PID_FILE,O_RDWR|O_CREAT,0640))<0) {
      	syslog(LOG_CRIT,"Failed to create pid file: %m");
	exit(EXIT_FAILURE);
      }
      if (lockf(pidfile,F_TLOCK,0)<0) {
	      syslog(LOG_CRIT,"Failed to lock pid file: %m");
	      exit(EXIT_FAILURE); /* can not lock */
      }
      sprintf(str,"%d\n",getpid());
      write(pidfile,str,strlen(str)); /* record pid to lockfile */
      
      loop ();
      break;
    case -1:
      perror ("Failed to deamonize");
      exit (EXIT_FAILURE);
    default:
      break;
    }
}

void
usage (char *thisfile)
{
  printf ("USAGE:\n");
  printf ("%s options\n", thisfile);
  printf ("Options:\n");
  printf ("\t-h, --help\t\t\tThis help\n");
  printf ("\t-n, --nodeamon\t\t\tDo not deamonize\n");
  printf
    ("\t-d, --device=inputdevice\tUse given device to handle fn keys, default: /dev/input/event0\n");
#ifdef HAVE_LIBXOSD
  printf ("\t-o, --osd\t\t\tStart as OSD deamon client\n");
#endif
}


int
main (int argc, char *argv[])
{
  int deamon = 1;		/* deamon by default */
  int xosd = 0;			/* do not start as xosd client */

  openlog("fsfn",LOG_CONS|LOG_NDELAY|LOG_PID,LOG_DAEMON);

  /* fill a default */
  //strncpy (devinput, "/dev/input/event0", 255);
  
  strncpy (devinput, getConfig("DEVICE"), MAX_CFG_LENGTH);

  /* parse command line */
  while (0 == 0)
    {
      int option_index = 0;
      int next_option;
      static struct option long_options[] = {
	{"help", 0, NULL, 'h'},
	{"nodaemon", 0, NULL, 'n'},
	{"device", 1, NULL, 'd'},
#ifdef HAVE_LIBXOSD
	{"osd", 0, NULL, 'o'},
#endif
	{NULL, 0, NULL, 0}
      };

#ifdef HAVE_LIBXOSD
      next_option =
	getopt_long (argc, argv, "hnd:o", long_options, &option_index);
#else
      next_option =
	getopt_long (argc, argv, "hnd:", long_options, &option_index);
#endif

      if (next_option == -1)
	{
	  break;
	}

      switch (next_option)
	{
	case 'n':
	  deamon = 0;
	  break;
	case 'o':
	  //      printf("We are osd\n");
	  xosd = 1;
	  break;
	case 'd':
	  if (optarg)
	    {
	      //      printf("We are changing devinput: %s\n",devinput);
	      strncpy (devinput, optarg, 255);
	      break;
	    }
	default:
	  //  printf("Unknow option: %c\n",next_option);
	  usage (argv[0]);
	  exit (-1);
	}
    }

  deamon&=!xosd; //could not be deamon if osd
  
  if (deamon)
    {
      deamonize ();
    }
  else
    {
      if (!xosd)
	{
	  loop ();
	}
#ifdef HAVE_LIBXOSD
      else
	{
	  looposd ();
	}
#endif
    }
  return EXIT_SUCCESS;
}

/* Manual page:

=pod

=head1 NAME

fsfn - Sony Vaio (FS series) laptop hotkey handler

=head1 SYNOPSIS

B<fsfn> [ -hndo ]

=head1 DESCRIPTION

fsfn is a simple daemon and client allowing use of special Sony Vaio
(FS series) keys. The daemon must be launched as super user.  To start
fsfn(1) as a graphical client, use the B<-o> or B<--osd> option.

The fsfn(1) program reads various parameters from its configuration
file upon start up.  See L</FILES> and L<fsfn(5)>.

=head1 OPTIONS

=over

=item B<-h>, B<--help>

The B<-h> option will cause fsfn(1) to print command line usage
information and then exit.

=item B<-n>, B<--nodaemon>

The B<-n> option will cause the fsfn(1) program will run in the
foreground.  This option is used for debugging purposes.

=item B<-d> I<device>, B<--device>=I<device>

The B<-d> option directs fsfn(1) to the appropriate keyboard event
device.  If this option is omitted, it will default to the value
`AUTO'.  You probably want the default.

=item B<-o>, B<--osd>

The B<-o> option will cause fsfn(1) to run in X client mode.  This
option is invalid, if the fsfn(1) daemon is not running.  You probably
want to run fsfn(1) with the B<-o> option when your desktop is
started.

=back

=head1 FILES

=over

=item F</dev/input/eventN>

The fsfn(1) daemon uses an input event handler to catch the Sony Fn
key presses.  The interface to this kernel event is through an
appropriate /dev/input node.  The correct node can change when a
hotplug event is processed.  If you start fsfn(1) with the B<-d>
B<AUTO> option, the daemon will determine the appropriate node to use.
You can override this by specifying the path for the node you want the
daemon to use.

This node can change after a suspend/wake cycle.  You probably want to
shut down fsfn(1) before suspending and start it after wake up.  This
will have the effect of stopping the fsfn(1) client, if it is running.

=item F</etc/fsfn.conf>

The fsfn(1) program reads various parameters from this file when it
starts up as a daemon or as a client.  See L<fsfn(5)> for the format
of this file.

=back

=head1 SEE ALSO

fsfn(5)

=head1 BUGS

Please submit bug reports to
S<L<http://developer.berlios.de/bugs/?group_id=4604>> .

=head1 AUTHOR

Pierre Poissinger E<lt>pierre.poissinger@gmail.comE<gt>

Spencer Shimko E<lt>spencer@beyondabstraction.netE<gt>

Garry Williams E<lt><gtwilliams@gmail.com>E<gt>

=cut

 */
