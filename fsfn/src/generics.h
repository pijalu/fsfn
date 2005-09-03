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

#ifndef _GENERIC_H_
// keys 
#define  FN_F2	1		// cut sound
#define  FN_F3  2		// volume -
#define  FN_F4  4		// volume +
#define  FN_F5	8		// Brightness -
#define  FN_F6	16		// Brightness +
#define  FN_F7  32		// LCD/SCREEN
#define  FN_F10 128		// Zoom in
#define	 FN_F12 64		// Suspend
#define  S1_BTN 4096		// S1 custom button
#define	 S2_BTN 8192		// S2 custom button

#define  FN_INPUT_VALUE	245	// Fn key generate a 245 value
#define  FN_INPUT_TYPE	4	// a 4 type
#define  FN_INPUT_CODE	4	// and a 4 code

// The FS series seems to support 8 brightness levels
#define MAX_BRIGHT 8
#define MIN_BRIGHT 1

// pidfile for deamon
#define PID_FILE 	"/var/run/fsfn.pid"
#endif
