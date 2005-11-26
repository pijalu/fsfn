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
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include <alsa/asoundlib.h>
#include <sys/poll.h>
#include <syslog.h>

#include "alsasound.h"

static char card[64] = "default";

static int smixer_level = 0;
static struct snd_mixer_selem_regopt smixer_options;

static snd_mixer_t *handle;
static snd_mixer_elem_t *elem;
static snd_mixer_selem_id_t *sid;

static long pmin,pmax;

static void error(const char *fmt,...)
{
	va_list va;

	va_start(va, fmt);
	fprintf(stderr, "amixer: ");
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
	va_end(va);
	va_start(va, fmt);
	vsyslog(LOG_NOTICE, fmt, va);
	va_end(va);
}


int loadMixer() {	
	/* only if needed */
	if (handle==NULL) {	
		int err;
		
		if ((err = snd_mixer_open(&handle,0)) < 0) {
			error("Mixer open error: %s", snd_strerror(err));
			return err;
		}
	
		if ((err = snd_mixer_attach(handle, card)) < 0) {
			error("Mixer attach %s error: %s", card, snd_strerror(err));
			snd_mixer_close(handle);
			return err;
		}
	
		if ((err = snd_mixer_selem_register(handle, smixer_level > 0 ? &smixer_options : NULL, NULL)) < 0) {
			error("Mixer register error: %s", snd_strerror(err));
			snd_mixer_close(handle);
			return err;
		}
	
		err = snd_mixer_load(handle);
		if (err < 0) {
			error("Mixer %s load error: %s", card, snd_strerror(err));
			snd_mixer_close(handle);
			return err;
		}
		
		/* load sid */
		snd_mixer_selem_id_alloca(&sid);
		snd_mixer_selem_id_set_index(sid, 0);
		snd_mixer_selem_id_set_name(sid,"Front");
		
		/* load elem */
		elem = snd_mixer_find_selem(handle, sid);
		if (!elem) {
			error("Unable to find simple control '%s',%i", 
			 snd_mixer_selem_id_get_name(sid), 
			 snd_mixer_selem_id_get_index(sid)
			);
			snd_mixer_close(handle);
			return -ENOENT;
		}

		/* load max and min */
		snd_mixer_selem_get_playback_volume_range(elem, &pmin, &pmax);
	}

	return 1; // okay
}

/**** from amixer ****/
/* Fuction to convert from volume to percentage. val = volume */
static int convert_prange(int val, int min, int max)
{
	int range = max - min;
	int tmp;

	if (range == 0)
		return 0;
	val -= min;
	tmp = rint((double)val/(double)range * 100);
	return tmp;
}

/* Function to convert from percentage to volume. val = percentage */
static int convert_prange1(int val, int min, int max)
{
	int range = max - min;
	int tmp;

	if (range == 0)
		return 0;

	tmp = rint((double)range * ((double)val*.01)) + min;
	return tmp;
}


static int AlsaToPercent(long val) {
	return convert_prange(val,pmin,pmax);
}

static long PercentToAlsa(int val) {
	return convert_prange1(val,pmin,pmax);
}

int
get_volume (int *value)
{
	long lval;
	long rval;
	
	if (!loadMixer()) {
		error("Unable to load mixer");
		return 0;
	}

	snd_mixer_selem_get_playback_volume(elem,0,&lval);
	snd_mixer_selem_get_playback_volume(elem,1,&rval);

	// set max value in lval
	if (lval<rval) { lval=rval; }
	
	// give it in percent
	*value = AlsaToPercent(lval);
	return *value;
}

int
set_volume (int *value)
{
	long val;
	
	if (!loadMixer()) {
		error("Unable to load mixer");
		return 0;
	}

	val=PercentToAlsa(*value);
		
	snd_mixer_selem_set_playback_volume(elem,0,val);
	snd_mixer_selem_set_playback_volume(elem,1,val);
	
      	return *value;
}

int
volume_up ()
{
  int value = 0;

  get_volume (&value);

  if (value < 100)
  {
	  if (value > 90) {
		value=100;
	  }
  	  else value += 10;
  }
  else
    value = 100;

  return set_volume (&value);
}

int
volume_down ()
{
  int value = 0;

  get_volume (&value);

  if (value > 9)
    value -= 10;
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
      
      // alsa seems to agree on volume 0 and continue to get normal level ???
      // just to be sure...
      value = 1;
      set_volume(&value);
      
      value = 0;
      return set_volume (&value);
    }

  if (!oldvalue)
    {
      return volume_up ();
    }

  return set_volume (&oldvalue);
}
