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
#include <errno.h>
#include <xosd.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <syslog.h>

#include "generics.h"

// the volume osd color
#define OSD_VCOLOR "red"
// the brightness osd color
#define OSD_BCOLOR "blue"
// the osd font... change the 7th position for size
// or use xfontsel to explore the options
#define OSD_FONT "-*-*-*-*-*-*-20-*-*-*-*-*-*-*"
// the length of time the osd is shown
#define OSD_TIME 3

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
  retval |= xosd_set_timeout (disp_obj, OSD_TIME);
  
  retval_nc |= xosd_set_font (disp_obj, OSD_FONT);

  if (retval)
    {
      syslog (LOG_CRIT, "Failed setup onscreen display: %s\n", xosd_error);
      // if we arrive here, object must be dead...
      osd_unload ();
      return -1;
    }
  else
    {
      // no need to stop for these errors
      if (retval_nc) {
      	syslog (LOG_CRIT, "Setup onscreen display: %s\n", xosd_error);
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
      retval = xosd_display (disp_obj, 1, XOSD_string, "Brightness");
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
  retval |= xosd_set_vertical_offset (disp_obj, 30);
  retval |= xosd_set_colour (disp_obj, OSD_VCOLOR);
  retval |= xosd_set_timeout (disp_obj, OSD_TIME);
  
  retval_nc |= xosd_set_font (disp_obj, OSD_FONT);

  if (retval)
    {
      syslog (LOG_NOTICE, "Failed setup onscreen display: %s", xosd_error);
      // kill possible osd and return
      osd_unload ();
      return -1;
    }
  else
    {
      // no need to stop for these errors
      if (retval_nc) {
      	syslog (LOG_CRIT, "Setup onscreen display: %s\n", xosd_error);
      }

//      retval = xosd_display (disp_obj, 0, XOSD_slider, level / OSD_VSCALE);
      retval = xosd_display (disp_obj, 0, XOSD_slider, level);
      retval = xosd_display (disp_obj, 1, XOSD_string, "Volume");
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
