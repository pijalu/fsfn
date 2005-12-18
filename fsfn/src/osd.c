/***************************************************************************
 *   Copyright (C) 2005 by Spencer Shimko                                  *
 *   spencer@beyondabstraction.net                                         *
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBXOSD
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <xosd.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <syslog.h>

#include "generics.h"
#include "readconfig.h"

// the volume osd color
#define OSD_VCOLOR 	getConfig("OSD_VCOLOR")
//the volume osd color when volume is set to zero
#define OSD_VCOLORZ 	getConfig("OSD_VCOLORZ")
// the brightness osd color
#define OSD_BCOLOR 	getConfig("OSD_BCOLOR")
// the osd font... change the 7th position for size
// or use xfontsel to explore the options
#define OSD_FONT 	getConfig("OSD_FONT")
// the length of time the osd is shown
#define OSD_TIMEOUT	getConfigInt("OSD_TIMEOUT")
// Custom messages (based on SilSha idea)
#define OSD_MSG_BRIGHT	getConfig("OSD_MSG_BRIGHT")
#define OSD_MSG_VOLUME	getConfig("OSD_MSG_VOLUME")		


// volume scaling factor for OSD
#define OSD_VSCALE 250

// the onscreen display
static xosd *disp_obj = NULL;

/* load disp_obj if needed 
 *	return true if disp_obj now exists
 **/
int
osd_load ()
{
  if (disp_obj == NULL)
    {
      disp_obj = xosd_create (2);
      if (setlocale (LC_ALL, "") == NULL || !XSupportsLocale ())
	{
	  syslog (LOG_NOTICE,
		   "Locale not available, expect problems with fonts.");
	}
    }
  return (disp_obj != NULL);
}

/* unload disp_obj */
void
osd_unload ()
{
  if (disp_obj)
    {
      xosd_destroy (disp_obj);
      disp_obj = NULL;
    }
}

// loop the string to check for potential security issue.
// TODO: this code should be reviewed - dirty hack to avoid rerun
int
validate_osd_message(char* message) {
	int i;
	int maxsize=strlen(message);
	int count=0;
	for (i=0;i<maxsize-1;i++) {
		if (message[i]=='%') {
			if ( (i) && message[i-1]=='\\') { // escaped, do not care
				continue; 
			}
			else {
				if ((message[i+1]!='d')&&(message[i+1]!='%')) {
					return 0; // a non escaped non dec. ==> incorrect
				}
				else {
					if (message[i+1]=='d') {
						count++;
					}
					if (count > 1) { // more than 1
						return 0;
					}
				}
			}
			i++;
		}
	}
	return 1;
}

int bright_validated=0; // to keep validation status

int
osd_brightness (int level)
{
  // the size of the increment
  static int inc_size = 100.0 / (MAX_BRIGHT - 1);
  int retval = 0;
  int retval_nc = 0; // non critical errors
  int pos;

  if (!osd_load ())
    {
      syslog (LOG_NOTICE, "Failed to display information onscreen: %s",
	      xosd_error);
      syslog (LOG_NOTICE, "Are you running X?");
      return -1;
    }

  retval |= xosd_set_bar_length (disp_obj, 100);
  retval |= xosd_set_pos (disp_obj, XOSD_bottom);
  retval |= xosd_set_align (disp_obj, XOSD_center);
  retval |= xosd_set_shadow_offset (disp_obj, 1);
  retval |= xosd_set_outline_offset (disp_obj, 1);
  retval |= xosd_set_vertical_offset (disp_obj, 30);
  retval |= xosd_set_colour (disp_obj, OSD_BCOLOR);
  retval |= xosd_set_timeout (disp_obj, OSD_TIMEOUT);
  
  retval_nc |= xosd_set_font (disp_obj, OSD_FONT);

  if (retval)
    {
      syslog (LOG_CRIT, "Failed critical setup onscreen display: %s\n", xosd_error);
      // if we arrive here, object must be dead...
      osd_unload ();
      return -1;
    }
  else
    {
      // no need to stop for these errors
      if (retval_nc) {
      	syslog (LOG_CRIT, "Failed non critical setup onscreen display: %s\n", xosd_error);
      }
      
      if (level == MAX_BRIGHT)
	{
	  pos = 100;
	}
      else
	{
	  pos = level * inc_size - inc_size;
	}

      retval = xosd_display (disp_obj, 0, XOSD_slider, pos);

	if (OSD_MSG_BRIGHT) // we got a message
	{
		char message[255];
		if (bright_validated || validate_osd_message(OSD_MSG_BRIGHT)) {
			bright_validated=1;
			if (snprintf(message,255,OSD_MSG_BRIGHT,level)>0) {
				retval = xosd_display (disp_obj, 1, XOSD_string,message);
			}
			else {
				syslog(LOG_NOTICE,
			       		"Failed to display custom message for brightness");
				// try hardcoded one
				retval = xosd_display (disp_obj, 1, XOSD_string,"Brightness");
			};
		}
		else {
			syslog(LOG_NOTICE,
					"OSD_MSG_BRIGHT value is invalid");
			// invalidate OSD_MSG_BRIGHT
			setConfig("OSD_MSG_BRIGHT",NULL);

			retval = xosd_display (disp_obj, 1, XOSD_string,"Brightness"); // use default
		}

	}
	else {
		retval = xosd_display (disp_obj, 1, XOSD_string,"Brightness");
	}

      if (retval == -1)
	{
	  syslog (LOG_NOTICE,
		  "Failed to display information onscreen: %s", xosd_error);
	  syslog (LOG_NOTICE, "Are you running X?");
	  // we can get here with a dead x connection or
	  // No connection... recreate plz
	  osd_unload ();
	}
    }

  return 1;
}

int volume_validated=0; // remember if already validated

int
osd_volume (int level)
{
  int retval = 0;
  int retval_nc = 0; // non critical errors

  // create if needed
  if (!osd_load ())
    {
      syslog (LOG_NOTICE,
	      "Failed to display information onscreen: %s", xosd_error);
      syslog (LOG_NOTICE, "Are you running X?");
      return -1;
    }

  retval |= xosd_set_bar_length (disp_obj, 100);
  retval |= xosd_set_pos (disp_obj, XOSD_bottom);
  retval |= xosd_set_align (disp_obj, XOSD_center);
  retval |= xosd_set_shadow_offset (disp_obj, 1);
  retval |= xosd_set_outline_offset (disp_obj, 1);
  retval |= xosd_set_vertical_offset (disp_obj, 30);	//mod by SilSha
  if (level == 0) {  
  	retval |= xosd_set_colour (disp_obj, OSD_VCOLORZ);
  }
  else {
  retval |= xosd_set_colour (disp_obj, OSD_VCOLOR);
  }
  retval |= xosd_set_timeout (disp_obj, OSD_TIMEOUT);
  
  retval_nc |= xosd_set_font (disp_obj, OSD_FONT);

  if (retval)
    {
      syslog (LOG_NOTICE, "Failed critical setup onscreen display: %s", xosd_error);
      // kill possible osd and return
      osd_unload ();
      return -1;
    }
  else
    {
      // no need to stop for these errors
      if (retval_nc) {
      	syslog (LOG_CRIT, "Failed non-critical setup onscreen display: %s\n", xosd_error);
      }

      retval = xosd_display (disp_obj, 0, XOSD_slider, level);


	if (OSD_MSG_VOLUME) // we got a message
	{
		if (volume_validated || validate_osd_message(OSD_MSG_VOLUME)) {
			char message[255];
			volume_validated=1; // no need to revalidate
			if (snprintf(message,255,OSD_MSG_VOLUME,level) > 0) {
				retval = xosd_display (disp_obj, 1, XOSD_string,message);
			}
			else {
				syslog(LOG_NOTICE,
			       		"Failed to display custom message for volume");
				// try hardcoded one
				retval = xosd_display (disp_obj, 1, XOSD_string,"Volume");
			}
		}
		else {
			syslog(LOG_NOTICE,
					"OSD_MSG_VOLUME value is invalid");
			// invalidate OSD_MSG_VOLUME
			setConfig("OSD_MSG_VOLUME",NULL);
			retval = xosd_display (disp_obj, 1, XOSD_string,"Volume"); // use default

		}
	}
	else {
		retval = xosd_display (disp_obj, 1, XOSD_string,"Volume");
	}
              
      if (retval == -1)
	{
	  syslog (LOG_NOTICE,
		  "Failed to display information onscreen: %s", xosd_error);
	  syslog (LOG_NOTICE, "Are you running X?");
	  osd_unload ();
	  return -1;
	}
    }

  return 1;
}
#endif
