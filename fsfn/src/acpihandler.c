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

#include "acpihandler.h"
#include "generics.h"

/*
 * Return current brightness of the screen 
 */
int
getBrightness ()
{
  FILE *handle;
  int ret;

  if ((handle = fopen ("/proc/acpi/sony/brightness", "rb")) == NULL)
    {
      perror ("Error opening /proc/acpi/sony/brightness");
      exit (-1);
    }
  if (fscanf (handle, "%d", &ret) != 1)
    {
      perror ("Error reading /proc/acpi/sony/brightness");
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

  if ((handle = fopen ("/proc/acpi/sony/brightness", "wb")) == NULL)
    {
      perror ("Error opening /proc/acpi/sony/brightness");
      exit (-1);
    }
  if (fprintf (handle, "%d", b) != 1)
    {
      perror ("Error writing /proc/acpi/sony/brightness");
      exit (-1);
    }
  fclose (handle);
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
  if ((handle = fopen ("/proc/acpi/sony/fnkey", "rb")) == NULL)
    {
      perror ("Error opening /proc/acpi/sony/fnkey");
      exit (-1);
    }
  if (fscanf (handle, "%d", &ret) != 1)
    {
      perror ("Error reading /proc/acpi/sony/fnkey");
      exit (-1);
    }
  fclose (handle);
  return ret;
}
