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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <syslog.h>

#include "basicmsg.h"

#define KEY_PATH	"/proc/acpi/sony"
#define KEY_PROJID	'S'


/* Message def */
#define MSG_TYPE_INFO	1
#define MSG_TYPE_CMD    2

struct infodata
{
  int flag;			/* flag - contains modification flag */
  int brightness_level;		/* current brightness */
  int sound_level;		/* current sound level */
};

struct basicmsg
{
  long mtype;			/* type of the message */
  struct infodata data;		/* link to struc containing data */
};

struct cmdmsg
{
  long mtype;			/* type of the message */
  int  cmd_code;		/* command code */
};

/* helpers code */

/* Generate a key if needed */
static key_t mykey = -1;
key_t
genkey ()
{
  if (mykey == -1)
    {
      if ((mykey = ftok (KEY_PATH, KEY_PROJID)) == -1)
	{
	  syslog (LOG_CRIT,"Failed to generate key: %m");
	  syslog (LOG_NOTICE,
		  "Please check sony_acpi module is correctly loaded");
	  exit (-1);
	}
    }
  return mykey;
}

/* End helper code */

/* Creating/opening of message queue */
/* hold ref to the queue */
static int msgqueue_id = -1;
/* Create if needed queue - should only be used by the deamon */
void
createqueue ()
{
  if (msgqueue_id != -1)
    {				/* no need */
      return;
    }

  if ((msgqueue_id = msgget (genkey (), IPC_CREAT | 0666)) == -1)
    {
      syslog (LOG_CRIT,"Cannot load/create IPC message queue: %m");
      exit (-1);
    }
}

/* Load msgqueue but do not create it, only for client */
int
loadqueue ()
{
  if (msgqueue_id != -1)
    {				/* no need */
      return EXIT_SUCCESS;
    }
  else if ((msgqueue_id = msgget (genkey (), 0666)) == -1)
    {
      syslog (LOG_CRIT,"Cannot create IPC message queue: %m");

      fprintf(stderr,"fsfn client cannot find fsfn deamon:\n");
      fprintf(stderr,"Please be sure you first started fsfn as deamon.\n");
      fprintf(stderr,"(Note: To start deamon, run fsfn as root "
	      "without -o option.)\n");
      return -1;
    }
  return EXIT_SUCCESS;
}

/* delete queue - only for deamon */
void
killqueue ()
{
  if (msgctl (msgqueue_id, IPC_RMID, 0))
    {
      syslog (LOG_NOTICE,"Error deleting queue: %m");
    }
}

/* send a message - only for deamon */
int
sendmsg (int flag, int brightness_level, int sound_level)
{
  struct basicmsg msg;

  msg.mtype = MSG_TYPE_INFO;
  msg.data.flag = flag;
  msg.data.brightness_level = brightness_level;
  msg.data.sound_level = sound_level;

  if ((msgsnd (msgqueue_id, (const void*) &msg,
	       sizeof (struct infodata), 0)) == -1)
    {
      syslog (LOG_CRIT,"Failed to send message: %m");
      exit (-1);
    }
  return EXIT_SUCCESS;
}

int
getmsg (int *flag, int *brightness_level, int *sound_level)
{
  struct basicmsg msg;

  msg.mtype = MSG_TYPE_INFO;
  if (msgrcv (msgqueue_id, (void*) &msg,
	      sizeof (struct infodata), MSG_TYPE_INFO, 0) == -1)
    {
      syslog (LOG_NOTICE,"Failed to get message: %m");
      return -1;
    }
  *flag = msg.data.flag;
  *brightness_level = msg.data.brightness_level;
  *sound_level = msg.data.sound_level;

  return EXIT_SUCCESS;
}

int
sendcmd(int cmd)
{
  struct cmdmsg msg;
  
  msg.mtype = MSG_TYPE_CMD;
  msg.cmd_code = cmd;

  if ((msgsnd(msgqueue_id, (const void*) &msg,
	      sizeof(int), 0)) == -1)
    {
      syslog (LOG_CRIT,"Failed to send message: %m");
      exit(-1);
    }
  return EXIT_SUCCESS;
}

int 
getcmd(int* cmd)
{
  struct cmdmsg msg;
  
  msg.mtype = MSG_TYPE_CMD;
  if (msgrcv(msgqueue_id, (void*) &msg,
	     sizeof (struct infodata), MSG_TYPE_CMD, 0) == -1)
    {
      syslog (LOG_NOTICE,"Failed to get message: %m");
      return -1;
    }
  *cmd = msg.cmd_code;
  
  return EXIT_SUCCESS;
}
