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


typedef struct {
	// keys
	char F2_CMD[MAX_CFG_LENGTH];
	char F3_CMD[MAX_CFG_LENGTH];
	char F4_CMD[MAX_CFG_LENGTH];
	char F5_CMD[MAX_CFG_LENGTH];
	char F6_CMD[MAX_CFG_LENGTH];
	char F7_CMD[MAX_CFG_LENGTH];
	char F10_CMD[MAX_CFG_LENGTH];
	char F12_CMD[MAX_CFG_LENGTH];
	char S1_CMD[MAX_CFG_LENGTH];
	char S2_CMD[MAX_CFG_LENGTH];
	// others
	// osd
	char OSD_VCOLOR[MAX_CFG_LENGTH];
	char OSD_BCOLOR[MAX_CFG_LENGTH];
	char OSD_FONT[MAX_CFG_LENGTH];
	// device
	char DEVICE[MAX_CFG_LENGTH];
	// alsa name
	char ALSA_NAME[MAX_CFG_LENGTH];
} config_list;

config_list* UserConfig=NULL;

void proceedConfig(char* name,char* value);
void setDefConfig();


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
	
	if (strcasecmp(name,"DEVICE")==0) {
		strncpy(UserConfig->DEVICE,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"DEVICE=%s",UserConfig->DEVICE);
	}
#ifdef HAVE_LIBXOSD
	else if (strcasecmp(name,"OSD_VCOLOR")==0) {
		strncpy(UserConfig->OSD_VCOLOR,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"OSD_VCOLOR=%s",UserConfig->OSD_VCOLOR);
	}
	else if (strcasecmp(name,"OSD_BCOLOR")==0) {
		strncpy(UserConfig->OSD_BCOLOR,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"OSD_BCOLOR=%s",UserConfig->OSD_BCOLOR);
	}
	else if (strcasecmp(name,"OSD_FONT")==0) {
		strncpy(UserConfig->OSD_FONT,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"OSD_FONT=%s",UserConfig->OSD_FONT);
	}
#endif
	else if (strcasecmp(name,"ALSA_NAME")==0) {
		strncpy(UserConfig->ALSA_NAME,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"ALSA_NAME=%s",UserConfig->ALSA_NAME);
	}
	else if (strcasecmp(name,"F2_CMD")==0) {
		strncpy(UserConfig->F2_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F2_CMD=%s",UserConfig->F2_CMD);
	}	
	else if (strcasecmp(name,"F2_CMD")==0) {
		strncpy(UserConfig->F2_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F2_CMD=%s",UserConfig->F2_CMD);
	}
	else if (strcasecmp(name,"F3_CMD")==0) {
		strncpy(UserConfig->F3_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F3_CMD=%s",UserConfig->F3_CMD);
	}
	else if (strcasecmp(name,"F4_CMD")==0) {
		strncpy(UserConfig->F4_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F4_CMD=%s",UserConfig->F4_CMD);
	}
	else if (strcasecmp(name,"F5_CMD")==0) {
		strncpy(UserConfig->F5_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F5_CMD=%s",UserConfig->F5_CMD);
	}
	else if (strcasecmp(name,"F6_CMD")==0) {
		strncpy(UserConfig->F6_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F6_CMD=%s",UserConfig->F6_CMD);
	}
	else if (strcasecmp(name,"F7_CMD")==0) {
		strncpy(UserConfig->F7_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F7_CMD=%s",UserConfig->F7_CMD);
	}
	else if (strcasecmp(name,"F10_CMD")==0) {
		strncpy(UserConfig->F10_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F10_CMD=%s",UserConfig->F10_CMD);
	}
	else if (strcasecmp(name,"F12_CMD")==0) {
		strncpy(UserConfig->F12_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"F12_CMD=%s",UserConfig->F12_CMD);
	}
	else if (strcasecmp(name,"S1_CMD")==0) {
		strncpy(UserConfig->S1_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"S1_CMD=%s",UserConfig->S1_CMD);
	}
	else if (strcasecmp(name,"S2_CMD")==0) {
		strncpy(UserConfig->S2_CMD,value,MAX_CFG_LENGTH);
		syslog(LOG_INFO,"S2_CMD=%s",UserConfig->S2_CMD);
	}
	else {
		syslog(LOG_NOTICE,"Unknow config entry:  %s=%s\n",name,value);
	}
}

// set default value for config
void setDefConfig() 
  {
	strncpy(UserConfig->DEVICE,"/dev/input/event0",MAX_CFG_LENGTH);
	strncpy(UserConfig->ALSA_NAME,"Front",MAX_CFG_LENGTH);
	strncpy(UserConfig->F12_CMD,"/bin/hibernate",MAX_CFG_LENGTH);
	strncpy(UserConfig->OSD_VCOLOR,"red",MAX_CFG_LENGTH);
	strncpy(UserConfig->OSD_BCOLOR,"blue",MAX_CFG_LENGTH);
	strncpy(UserConfig->OSD_FONT,"-*-*-*-*-*-*-20-*-*-*-*-*-*-*",MAX_CFG_LENGTH);
  }

// load
void loadConfig() {
	FILE*	configFile;
	char	buffer[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];
	char 	name[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];
	char	value[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];

	syslog(LOG_INFO,"Loading config file %s",USER_CONFIG_FILE);
	
	if (UserConfig==NULL) {
		UserConfig=(config_list*) malloc(sizeof(config_list));
	}

	// reset
	memset(UserConfig,0,sizeof(config_list));

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
	if (UserConfig!=NULL) {
		free(UserConfig);
	}	
}

// return stored command - config 
char* getConfig(int CONFIGCODE) {
	if (UserConfig==NULL) {
		loadConfig(); // proceed with config loading
	}
	// recheck in case of a config problems...
	if (UserConfig!=NULL) {
		switch(CONFIGCODE) {
			// button commands
			case FN_F2: return UserConfig->F2_CMD;
			case FN_F3: return UserConfig->F3_CMD;	
			case FN_F4: return UserConfig->F4_CMD;
			case FN_F5: return UserConfig->F5_CMD;
			case FN_F6: return UserConfig->F6_CMD;
			case FN_F7: return UserConfig->F7_CMD;
			case FN_F10: return UserConfig->F10_CMD;
			case FN_F12: return UserConfig->F12_CMD;
			case S1_BTN: return UserConfig->S1_CMD;
			case S2_BTN: return UserConfig->S2_CMD;
			// other configs
			case CFG_DEVICE: return UserConfig->DEVICE;
			case CFG_OSDVCOLOR: return UserConfig->OSD_VCOLOR;
			case CFG_OSDBCOLOR: return UserConfig->OSD_BCOLOR;
			case CFG_OSDFONT: return UserConfig->OSD_FONT;
			case CFG_ALSA_NAME: return UserConfig->ALSA_NAME;
			default: break;
		}
	}	
	return NULL;
}
