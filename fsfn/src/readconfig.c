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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <syslog.h>

#include "readconfig.h"
#include "generics.h"

struct config_entry {
	char* name;
	char* value;
} config_list[] = 
{
	// keys
	{
		.name = "F2_CMD",
		.value = NULL,
	},
	{
		.name = "F3_CMD",
		.value = NULL,
	},
	{
		.name = "F4_CMD",
		.value = NULL,
	},
	{
		.name = "F5_CMD",
		.value = NULL,
	},
	{
		.name = "F6_CMD",
		.value = NULL,
	},
	{
		.name = "F7_CMD",
		.value = NULL,
	},
	{
		.name = "F10_CMD",
		.value = NULL,
	},
	{
		.name = "F12_CMD",
		.value = NULL,
	},
	{
		.name = "S1_CMD",
		.value = NULL,
	},
	{
		.name = "S2_CMD",
		.value = NULL,
	},
	// OSD
	{
		.name = "OSD_FONT",
		.value = NULL,
	},
	{
		.name = "OSD_VCOLOR",
		.value = NULL,
	},
	{
		.name = "OSD_BCOLOR",
		.value = NULL,
	},
	// DEVICE
	{
		.name = "DEVICE",
		.value = NULL,
	},
	// ALSA
	{
		.name = "ALSA_NAME",
		.value = NULL,
	},
	{
		.name = NULL
	}
};

// def functions
char* getConfigValue(char* key);
int setConfigValue(char*key,char* value);
void proceedConfig(char* name,char* value);
void setDefConfig();
char* strtrim(char* name);
void loadConfig();
void releaseConfig();
char* getConfig(char* key);


// get a config key
char* getConfigValue(char* key) {
	struct config_entry* it;
	for (it=config_list; it->name ; it++) {
		if (!strcasecmp(it->name,key)) {
			return it->value;
		}
	}
	return NULL;
}

// set a config key
int setConfigValue(char*key,char* value) {
	struct config_entry* it;
	for (it=config_list; it->name ; it++) {
		if (!strcasecmp(it->name,key)) {
			if (it->value!=NULL) {
				free(it->value);
				it->value=NULL; // to keep clean...
			}
			it->value=(char*)malloc(strlen(value)+1);
			strcpy(it->value,value);
			return 1;
		}
	}
	return -1;
}



// dirty trim
char* strtrim(char* name) {
	register int i=strlen(name)-1;
	
	for (;i>0;--i) {
		if (isspace(name[i])) {
			name[i]='\0';
		}
		else {
			break;
		}
	}
	return name;
}


// store name/value for possible value
void proceedConfig(char* name,char* value) {
	// trim for any extra char
	strtrim(name);
	strtrim(value);
	
	if (setConfigValue(name,value)) {
		syslog(LOG_INFO,"Configuration: %s=%s",name,value);
	}
	else {
		syslog(LOG_NOTICE,"Configuration: %s=%s is unknown",name,value);
	}		
}

// set default value for config
void setDefConfig() 
  {
	syslog(LOG_INFO,"Setting default configuration");
	proceedConfig("DEVICE","AUTO");
	proceedConfig("ALSA_NAME","Front");
	proceedConfig("F12_CMD","/bin/hibernate");
	proceedConfig("OSD_VCOLOR","red");
	proceedConfig("OSD_BCOLOR","blue");
	proceedConfig("OSD_FONT","-*-*-*-*-*-*-20-*-*-*-*-*-*-*");
	syslog(LOG_INFO,"default configuration done");
  }

// keep track of config loading state
int _config_loaded=0;
// load
void loadConfig() {
	FILE*	configFile;
	char	buffer[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];
	char 	name[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];
	char	value[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];

	// load if needed
	if (_config_loaded) 
		return;
	// release previous config
	releaseConfig();

	_config_loaded=1; // set status as loaded

	syslog(LOG_INFO,"Loading config file %s",USER_CONFIG_FILE);
	
	// set default values
	setDefConfig();	

	// read
	if ((configFile=fopen(USER_CONFIG_FILE,"r"))==NULL) {
		syslog(LOG_NOTICE,"error opening config file");
		return;
	}
	while(fgets(buffer,255,configFile)!=NULL) {
		register int i=0;
		char *it; // iterator

		if (buffer && isalpha(buffer[0]) && buffer[0]!='#') {
			*name=*value='\0';
		
			// pointer hell... i don't find a easier way
			// note: buffer size cannot overflow name/value
			it=name;  // start filling name
			while (buffer[i]!='\0') {
				if (buffer[i]=='=') { 
					// we got a = char, finish current 
					// and start value 
					(*it)='\0'; // first mark it as empty
					it=value; // we pass on 'value' var
				}
				else if (buffer[i]!='\n') {
					// copy at current place the buffer val 
					// and inc iterator
					*(it++)=buffer[i];
				}
				i++; // next car...
			}
			*it='\0'; // add end to last copy str
		
			proceedConfig(name,value);
		}
	}
	fclose(configFile);
}

// release 
void releaseConfig() {
	struct config_entry* it;
	for (it=config_list; it->name ; ++it) {
		if (it->value) {
			free(it->value);
			it->value=NULL;
		}
	}
	_config_loaded=0;
}

// return stored command - config 
char* getConfig(char* key) {
	loadConfig();
	return getConfigValue(key);
}
