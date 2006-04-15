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
#ifndef _READCONFIG_H_
#define _READCONFIG_H_
//#include <config.h>

void loadConfig();
void loadUserConfig();
void releaseConfig();

char* getConfig(char*);
int   getConfigInt(char* key); // same as previous, but return an int
int   getConfigValueBool(char* key); // same as previous, but return a bool

int   setConfig(char* key,char* value); // set a config

#define MAIN_CONFIG_FILE	"/etc/fsfn.conf"
#define USER_CONFIG_FILE        ".fsfn.conf"

#endif
