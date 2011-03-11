/*
 * main.c
 *
 *      Author: University of Salento and CMCC 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ping.h"
#include "dbAccess.h"
#include "config.h"

void readConfig(void);

int main(int argc, char **argv) {
	readConfig();
	unsigned numHosts = 0;
	struct host * hosts = NULL;

	int counter = 0;
	while(1) {
		if(counter == 0) {
			free(hosts);
			hosts = loadHosts(&numHosts);
			if(numHosts == 0 || hosts == NULL) break;
		}
		pingHostList(hosts, numHosts);
		writeResults(hosts, numHosts);
		counter = (counter + 1) % HOSTS_LOADING_SPAN;
		sleep(PING_SPAN);
	}
	return 0;
}

void readConfig(void) {
	FILE *file = fopen("config.dat", "r");
	if(file == NULL) {
		puts("Error! \nConfiguration file not found\n");
		exit(1);
	}
	else {
		char buffer[256];
		char *position;
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			strcpy(MYSQL_HOST=(char*)malloc(strlen(position+1)), position+1);
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			MYSQL_PORT_NUMBER = atoi(position+1);
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			strcpy(MYSQL_USER=(char*)malloc(strlen(position+1)), position+1);
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			strcpy(MYSQL_PASSWD=(char*)malloc(strlen(position+1)), position+1);
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			strcpy(MYSQL_DB_NAME=(char*)malloc(strlen(position+1)), position+1);
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			CONNECTION_TIMEOUT = atoi(position+1);
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			THREAD_OPEN_MAX = atoi(position+1);
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			PING_SPAN = atoi(position+1);
		fscanf(file, "%s", buffer);
		position = strchr(buffer, '=');
		if(position != NULL)
			HOSTS_LOADING_SPAN = atoi(position+1);
	}
	fclose(file);
}
