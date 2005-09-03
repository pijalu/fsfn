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
#ifndef _BASICMSG_H_
#define _BASICMSG_H_

#define MOD_BRIGHTNESS	1
#define MOD_SOUND	2

void createqueue();
int  loadqueue();
void killqueue();

int sendmsg(int flag,int brightness_level,int sound_level);
int getmsg(int* flag,int* brightness_level,int* sound_level);


#endif
