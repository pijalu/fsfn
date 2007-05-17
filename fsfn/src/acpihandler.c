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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

#include "readconfig.h"
#include "acpihandler.h"
#include "generics.h"


int _brightness=-1; /* for FSJ laptops (they don't return current brightness) */
/*
 * Return current brightness of the screen 
 */
int
getBrightness ()
{
  FILE *handle;
  int ret;

  if (getConfigInt("BRT_HACK_FJS"))
  {
	syslog (LOG_NOTICE,"FSJ HACK:current brightness %d",_brightness); 
	if (_brightness < 0) 
	{
		_brightness=getConfigInt("BRT_HACK_FJS");
	}
	return _brightness;
  }

  if ((handle = fopen (getConfig("BRIGHTNESS_DEVICE"), "rb")) == NULL)
    {
	 syslog (LOG_CRIT,"Error opening %s: %m",getConfig("BRIGHTNESS_DEVICE"));
      exit (-1);
    }
  if (fscanf (handle, "%d", &ret) != 1)
    {
	 syslog (LOG_CRIT,"Error reading %s: %m",getConfig("BRIGHTNESS_DEVICE"));
      exit (-1);
    }
  fclose (handle);

  return ret;
}


/*
 * Set the current brightness of the screen 
 */
int
setBrightness (int b)
{
  FILE *handle;

  // validate values
  if (b > MAX_BRIGHT)
    {
      b = MAX_BRIGHT;
    }
  else if (b < MIN_BRIGHT)
    {
      b = MIN_BRIGHT;
    }

  if ((handle = fopen (getConfig("BRIGHTNESS_DEVICE"), "wb")) == NULL)
    {
	 syslog (LOG_CRIT,"Error opening %s: %m",getConfig("BRIGHTNESS_DEVICE"));
      exit (-1);
    }
  if (fprintf (handle, "%d", b) != 1)
    {
	 syslog (LOG_CRIT,"Error %s: %m",getConfig("BRIGHTNESS_DEVICE"));
      exit (-1);
    }
  fclose (handle);

  if (getConfigInt("BRT_SETDEFAULT")) 
    {
	  syslog (LOG_INFO,"Writing to default brigthness");
	  if ((handle = fopen (getConfig("BRIGHTNESS_DEFAULT_DEVICE"), "wb")) == NULL)
		{
		     syslog (LOG_CRIT,"Error opening %s: %m",
			     getConfig("BRIGHTNESS_DEFAULT_DEVICE"));
		     exit (-1);
		}
	  if (fprintf (handle, "%d", b) != 1)
		{
		     syslog (LOG_CRIT,"Error writing %s: %m",
			     getConfig("BRIGHTNESS_DEFAULT_DEVICE"));
		     exit (-1);
		}
	  fclose (handle);
    }
  
  if (getConfigInt("BRT_HACK_FJS")) 
    {
		syslog (LOG_NOTICE,"FSJ HACK: brightnes set to %d",b); 
		_brightness=b; // store new value
    }

  return b;
}


/*
 * Pool the fnkey status
 */
int
getCodes ()
{
  FILE *handle;
  int ret;
  if ((handle = fopen (getConfig("FNKEY_DEVICE"), "rb")) == NULL)
    {
	 syslog (LOG_CRIT,"Error opening %s: %m",
		 getConfig("FNKEY_DEVICE"));
	 exit (-1);
    }
  if (fscanf (handle, "%d", &ret) != 1)
    {
	 syslog (LOG_CRIT,"Error reading %s: %m",
		 getConfig("FNKEY_DEVICE"));
	 exit (-1);
    }
  fclose (handle);
  return ret;
}
