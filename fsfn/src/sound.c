/***************************************************************************
 *   Copyright (C) 2005 by Poissinger Pierre                               *
 *   pierre.poissinger@gmail.com                                           *
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
#include <sys/ioctl.h>

#include <linux/soundcard.h>
#include "sound.h"

int
get_volume (int *value)
{
  int mixer = open (MIXER_DEV, O_RDONLY);

  if (mixer)
    {
      ioctl (mixer, SOUND_MIXER_READ_VOLUME, value);
      close (mixer);
      return *value;
    }
  else
    return 0;
}

int
set_volume (int *value)
{
  int mixer = open (MIXER_DEV, O_RDWR);

  if (mixer)
    {
      ioctl (mixer, SOUND_MIXER_WRITE_VOLUME, value);
      close (mixer);
      return *value;
    }
  else
    return 0;
}

int
volume_up ()
{
  int value = 0;

  get_volume (&value);

  if (value < 0x5a5a)
    value += 0x0a0a;
  else
    value = 0x6464;

  return set_volume (&value);
}

int
volume_down ()
{
  int value = 0;

  get_volume (&value);

  if (value > 0x0a0a)
    value -= 0x0a0a;
  else
    value = 0;

  return set_volume (&value);
}

static int oldvalue;
int
mute ()
{
  int value;

  get_volume (&value);

  if (value)
    {
      oldvalue = value;
      value = 0;
      return set_volume (&value);
    }

  if (!oldvalue)
    {
      return volume_up ();
    }

  return set_volume (&oldvalue);
}
