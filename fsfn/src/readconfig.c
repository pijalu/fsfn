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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
  int*  ivalue; // to hold possible int 
} config_list[] = 
  {
#ifdef USE_MORECONF
    // keys definition
    {
      .name = "FN_INPUT_VALUE",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_INPUT_TYPE",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_INPUT_CODE",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_INPUT_VALUE",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_F2",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_F3",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_F4",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_F5",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_F6",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_F7",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_F10",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "FN_F12",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "S1_BTN",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "S2_BTN",
      .value = NULL,
      .ivalue = NULL,
    },
#endif
    // keys action
    {
      .name = "F2_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "F3_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "F4_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "F5_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "F6_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "F7_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "F10_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "F12_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "S1_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "S2_CMD",
      .value = NULL,
      .ivalue = NULL, 
    },
    // OSD
    {
      .name = "OSD_FONT",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "OSD_VCOLOR",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "OSD_VCOLORZ",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "OSD_BCOLOR",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "OSD_TIMEOUT",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "OSD_MSG_BRIGHT",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "OSD_MSG_VOLUME",
      .value = NULL,
      .ivalue = NULL, 
    },
    // DEVICE
    {
      .name = "DEVICE",
      .value = NULL,
      .ivalue = NULL, 
    },
    // ALSA
    {
      .name = "ALSA_NAME",
      .value = NULL,
      .ivalue = NULL, 
    },
    {
      .name = "SOUND_STEP",	/*mode by SilSha*/
      .value = NULL,
      .ivalue = NULL, 
    },
    // SET DEFAULT device aswell
    {
      .name = "BRT_SETDEFAULT", 
      .value = NULL,
      .ivalue = NULL,
    }, 
    // HACK for FSJ
    {
      .name = "BRT_HACK_FJS",
      .value = NULL,
      .ivalue = NULL,
    },
    // Some vaio got different max min...
    {
      .name = "BRIGHTNESS_MAX",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "BRIGHTNESS_MIN",
      .value = NULL,
      .ivalue = NULL,
    },
    // DEVICES
    {
      .name = "BRIGHTNESS_DEVICE",
      .value = NULL,
      .ivalue = NULL,
    },
    {
      .name = "BRIGHTNESS_DEFAULT_DEVICE",
      .value = NULL,
      .ivalue = NULL,
    },    
    {
      .name = "FNKEY_DEVICE",
      .value = NULL,
      .ivalue = NULL,
    },
    // The end
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
int fileExist(const char* path);
int readFileInt(const char* path);


// get a config key
char* getConfigValue(char* key) {
	struct config_entry* it;
	for (it=config_list; it->name ; it++) {
		if (!strcasecmp(it->name,key)) {
			return it->value;
		}
	}

	syslog(LOG_NOTICE,"%s value not found",key);
	return NULL;
}

int getConfigValueInt(char* key) {
	struct config_entry* it;
	for (it=config_list; it->name; it++) {
		if (!strcasecmp(it->name,key)) {
			if (it->ivalue==NULL) { // lazy convert
				it->ivalue = (int*) malloc(sizeof(int));
				it->ivalue[0] = atoi(it->value);
			}
			return it->ivalue[0];
		}
	}
	
	syslog(LOG_NOTICE,"%s value not found",key);
	return 0;
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
			if (it->ivalue!=NULL) { // clean int buffer
				free(it->ivalue);
				it->ivalue=NULL;
			}
			if (value!=NULL) { // only if value is not null
				it->value=(char*)malloc(strlen(value)+1);
				strcpy(it->value,value);
			}
			return 1;
		}
	}
	
	syslog(LOG_NOTICE,"%s value not found",key);
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
       char tempBuffer[10];
       syslog(LOG_INFO,"Setting default configuration");
       /* Key value default */
#ifdef USE_MORECONF
	syslog(LOG_INFO,"MORECONF loading default");
	proceedConfig("FN_F2","1");
	proceedConfig("FN_F2","1");
	proceedConfig("FN_F3","2");
	proceedConfig("FN_F4","4");
	proceedConfig("FN_F5","8");
	proceedConfig("FN_F6","16");
	proceedConfig("FN_F7","32");
	proceedConfig("FN_F10","128");
	proceedConfig("FN_F12","64");
	proceedConfig("S1_BTN","4096");
	proceedConfig("S2_BTN","8192");
	proceedConfig("FN_INPUT_VALUE","245");
	proceedConfig("FN_INPUT_TYPE","4");
	proceedConfig("FN_INPUT_CODE","4");
	syslog(LOG_INFO,"MORECONF defaults loaded");
#endif
	/* setting default */
	proceedConfig("DEVICE","AUTO");
	proceedConfig("ALSA_NAME","Front");
	proceedConfig("SOUND_STEP","10");		/*mod by SilSha*/
	proceedConfig("F12_CMD","/bin/hibernate");
	proceedConfig("OSD_VCOLOR","red");
	proceedConfig("OSD_VCOLORZ","red");
	proceedConfig("OSD_BCOLOR","blue");
	proceedConfig("OSD_FONT","-*-*-*-*-*-*-20-*-*-*-*-*-*-*");
	proceedConfig("OSD_TIMEOUT","3");
	proceedConfig("BRT_SETDEFAULT","0"); 		// force new default value
	proceedConfig("BRT_HACK_FJS","0");
	// Let be smart and check what is loaded
	if (fileExist("/sys/devices/platform/sony-laptop/fnkey")) {
	     int maxBrightness=7;
	     syslog(LOG_INFO,"Using sony-laptop subsystem");
	     proceedConfig("BRIGHTNESS_DEVICE",
			   "/sys/class/backlight/sony/brightness");
	     proceedConfig("BRIGHTNESS_DEFAULT_DEVICE",
			   "/sys/devices/platform/sony-laptop/brightness_default");
	     proceedConfig("FNKEY_DEVICE",
			   "/sys/devices/platform/sony-laptop/fnkey");	
	     // Let's find max brightness
	     if ((maxBrightness=readFileInt(
		       "/sys/class/backlight/sony/max_brightness"))>=0) {
		  sprintf(tempBuffer,"%d",maxBrightness);
		  proceedConfig("BRIGHTNESS_MAX",tempBuffer);
		  sprintf(tempBuffer,"%d",maxBrightness-BRIGHT_STEP);
		  proceedConfig("BRIGHTNESS_MIN",tempBuffer);
	     }
	     else {
		  proceedConfig("BRIGHTNESS_MAX","7");
		  proceedConfig("BRIGHTNESS_MIN","0");
	     }
	}
	else {
	     syslog(LOG_INFO,"Using sony-acpi subsystem");
	     proceedConfig("BRIGHTNESS_DEVICE",
			   "/proc/acpi/sony/brightness");
	     proceedConfig("BRIGHTNESS_DEFAULT_DEVICE",
			   "/proc/acpi/sony/brightness_default");
	     proceedConfig("FNKEY_DEVICE",
			   "/proc/acpi/sony/fnkey");
	
	     proceedConfig("BRIGHTNESS_MAX","8");
	     proceedConfig("BRIGHTNESS_MIN","1");
	}
	
	syslog(LOG_INFO,"default configuration done");
  }

// keep track of config loading state
int _config_loaded=0;
// load config helper
void loadConfigSub(char* filename) {
	FILE*	configFile;
	char	buffer[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];
	char 	name[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];
	char	value[MAX_CFG_LENGTH+MAX_CFG_NAME_LENGTH];

	syslog(LOG_INFO,"Loading config file %s",filename);

	// read
	if ((configFile=fopen(filename,"r"))==NULL) {
		syslog(LOG_NOTICE,"error opening config file: %m");
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

void loadConfig() {
  if (_config_loaded) // load if needed
    return;
  releaseConfig(); // release previous config
  setDefConfig(); // load default
  loadConfigSub(MAIN_CONFIG_FILE);
  _config_loaded=1;
}

void loadUserConfig() {
  char* path;
  int maxsize=strlen(getenv("HOME"))+1+strlen(USER_CONFIG_FILE)+1+1; // get needed string size = $HOME+/+USER_CONFIG_FILE

  /* Create full path name to user config*/
  path=(char*) malloc(maxsize);
  strncpy(path,getenv("HOME"),maxsize);
  strncat(path,"/",maxsize);
  strncat(path,USER_CONFIG_FILE,maxsize);
  
  if (!_config_loaded) /* previous config not yet loaded */
    loadConfig(); /* load it then */
  loadConfigSub(path);

  /* free used pointer */
  free(path);
}

// release 
void releaseConfig() {
	struct config_entry* it;
	for (it=config_list; it->name ; ++it) {
		if (it->value) {
			free(it->value);
			it->value=NULL;
		}
		if (it->ivalue) {
			free(it->ivalue);
			it->ivalue=NULL;
		}
	}
	_config_loaded=0;
}

// return stored command - config 
char* getConfig(char* key) {
	loadConfig();
	return getConfigValue(key);
}

int getConfigInt(char* key) {
	loadConfig();
	return getConfigValueInt(key);
}

int setConfig(char* key,char* value) {
	loadConfig();
	return setConfigValue(key,value);
}

int fileExist(const char* path) {
     struct stat buffer;
     return (stat(path,&buffer)==0);
}

int readFileInt(const char* path) {
     FILE *handle;
     int ret;
     if ((handle = fopen(path,"r"))==NULL) {
	  syslog(LOG_WARNING,"Error reading %s: %m",path);
	  return -1;
     }
     if (fscanf(handle,"%d",&ret) != 1) {
	  syslog(LOG_WARNING,"Error reading %s: %m",path);
	  return -1;
     }
     fclose(handle);
     return ret;
}
