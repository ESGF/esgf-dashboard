/*
 * ping.h
 *
 *      Author: University of Salento and CMCC 
 */

#ifndef PING_H_
#define PING_H_

#define HOSTNAME_MAX_LENGTH 256		// max length + 1
#define HOST_ACTIVE 		1
#define HOST_INACTIVE		2
#define HOST_TIMEOUT		3
#define HOST_ERROR			4

typedef struct host {
	int id;
	char hostName[HOSTNAME_MAX_LENGTH];
	unsigned short portNumber;
	char status;						// 0 = not active, 1 = active, 2 = timeout, 3 = error
	unsigned elapsedTime;				// microseconds
	char *details;
} host;


// Function prototype
void * ping(void *arg);
int pingHostList(struct host hosts[], const unsigned numHosts);

#endif /* PING_H_ */
