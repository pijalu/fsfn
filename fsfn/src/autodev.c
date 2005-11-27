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
#include "autodev.h"

// defines here to not shows unusefull implem. details to callers  
#define MAX_SIZE	255
#define MAX_DEVICE_LIST	255
#define DEVICE_FILE	"/proc/bus/input/devices"

// simple struct to hold parse details
typedef struct {
	char name[MAX_SIZE];
	char entry[MAX_SIZE];
} entry_device;

// size of parsed result
int size=0;
// the entry device
entry_device devices_list[MAX_DEVICE_LIST];

// for finite autom
#define STATE_NORMAL		1
#define STATE_NEW		2
#define STATE_NAME		3
#define STATE_NAME_COPY		4
#define STATE_HANDLER		5
#define STATE_HANDLER_COPY	6

void parseDevice(FILE*);
int parseFile(char*);

/* Launch parse on a file */
int parseFile(char* filename) {
	FILE* file;
	if ((file=fopen(filename,"r"))==NULL) {
		perror("File open failed");
		return -1;
	}
	parseDevice(file);
	fclose(file);
	return 1;
}

void parseDevice(FILE* file) {
	char buffer;
	int state=STATE_NORMAL;
	
	int i=0;	
	size=0; // (re)set size of devices_lists
	
	// use a finite automate to parse... must exist something simpler but that's all 
	// I can think right now... sorry 
	while ( (buffer=getc(file))!=EOF) {
		switch(state) {
			case STATE_NORMAL: // nothing is defined yet
				switch(buffer) {
					case 'N': state=STATE_NAME; // we enter a name
						  break;
					case 'H': state=STATE_HANDLER; // we enter a handler
						  break;
					case '\n': state=STATE_NEW; // possible new entry ?
						  break;
					default:  break;
				}
				break;
			case STATE_NEW: // we are in a potential new device 
				switch(buffer) {
					case '\n': size++; // confirmed
						   state=STATE_NORMAL;
						   break;
					case 'N': state=STATE_NAME; // no, we are in a name
						  break;
					case 'H': state=STATE_HANDLER; // in a handler
						  break;
					default: 
						   state=STATE_NORMAL; // nothing, back to normal
						   break;
				}
				break;
			case STATE_NAME: // we are parsing a name
				switch(buffer) {
					case '=': state=STATE_NAME_COPY; // the name is here
						  i=0;
						  break;
					case '\n': state=STATE_NORMAL; // finished, back to normal
						  break;
					default: break;
				}
				break;
			case STATE_NAME_COPY:	// copy the name
				switch(buffer) {
					case '"' : // no need of this char
						break;
					case '\n': // finished copy
						state=STATE_NORMAL;
						devices_list[size].name[i]='\0';
						break;
					default: // copy
						devices_list[size].name[i]=buffer;
						i++;
						break;				
				}
				break;
			case STATE_HANDLER: // we are parsing a handler
				switch(buffer) {
					case '=': state=STATE_HANDLER_COPY; // the device starts here
						  i=0;
						  break;
					case '\n': state=STATE_NORMAL; // finished handler ?
						   break;
					default:  break;
				}
				break;
			case STATE_HANDLER_COPY: // copy the handler
				switch(buffer) {
					case '\n': // finished copy
						state=STATE_NORMAL;
						devices_list[size].entry[i]='\0';
						break;
					default: // do the copy
						devices_list[size].entry[i]=buffer;
						i++;
						break;
				}
				break;
			default: // should not arrive here...
				fprintf(stderr,"error: unknow state: %d\n",state);
				exit(-1);
				break;
		}
		
	}
}

// return event assoc with a name containing item
int getItemEvent(char* item) {
	int ret=-1;
	int e;

	if (!parseFile(DEVICE_FILE)) {
		fprintf(stderr,"Error reading %s file\n",DEVICE_FILE);
	}
	else {
		for (e=0;e<size;++e) {
			if (strstr(devices_list[e].name,item)!=NULL) {
				char* location;
				if ( (location=strstr(devices_list[e].entry,"event"))!=NULL) {
					if (sscanf(location,"event%d ",&ret)!=EOF) {
						return ret;
					}
				}			
			}
		}
	}
	return ret;
}

/*int main() {
	int e;
	e=getItemEvent("keyboard");
	printf("Event for keyboard: %d\n",e);
	e=getItemEvent("Mouse");
	printf("Event for mouse: %d\n",e);
	e=getItemEvent("Glide");
	printf("Event for glide: %d\n",e);
}*/
