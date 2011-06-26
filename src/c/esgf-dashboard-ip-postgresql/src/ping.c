/*
 * ping.c
 *
 *      Author: University of Salento and CMCC 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>	// for nonblocking connection
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "ping.h"
#include "config.h"

void * ping(void *arg) {
	struct host *item = (host*)arg;
	item->elapsedTime = 0;
	item->details = NULL;
	int sd;         				  		// socket descriptor
	char *hostname = item->hostName;		// host name
	unsigned short port = item->portNumber; // port number
	int rval;         						// socket descriptor for connect

	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //create the TCP socket
	if (sd == -1)
	{
		// perror("Socket()\n");
		return &errno;
	}

	struct addrinfo	hints, *res;
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;	// always return canonical name
	hints.ai_family = AF_INET;		// 0, AF_INET, AF_INET6, etc.
	hints.ai_socktype = SOCK_STREAM;	// 0, SOCK_STREAM, SOCK_DGRAM, etc.

	char buf[6];
	sprintf(buf, "%u", port);

	if (getaddrinfo(hostname, buf, &hints, &res) != 0) {
		// printf("error in %s:%u\n", hostname, port);
		close(sd);
		return &errno;
	}

	// Set non-blocking
	long arg1;
	if((arg1 = fcntl(sd, F_GETFL, NULL)) < 0) {
		// fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
		close(sd);
		return &errno;
	}
	arg1 |= O_NONBLOCK;
	if(fcntl(sd, F_SETFL, arg1) < 0) {
		// fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
		close(sd);
		return &errno;
	}

	// below connects to the specified ip in hostaddr
	rval = connect(sd, res->ai_addr, res->ai_addrlen);
	if(rval == -1) {
		if(errno != EINPROGRESS) {
			// printf("connect() failed, Err: %d \"%s\"\n", errno, strerror(errno));
			close(sd);
			return &errno;
		}

		struct timeval tv;
		fd_set fdr, fdw;
		int ris;
		tv.tv_sec  = CONNECTION_TIMEOUT / 1000000; // seconds
		tv.tv_usec = CONNECTION_TIMEOUT % 1000000; // microseconds
		FD_ZERO(&fdr);
		FD_SET(sd, &fdr);
		fdw = fdr;
		ris = select(sd+1, &fdr, &fdw, NULL, &tv); // blocking
		item->elapsedTime = CONNECTION_TIMEOUT - tv.tv_sec * 1000000 - tv.tv_usec;

		if(ris < 0) {
			// puts("select fallita");
			close(sd);
			return &errno;
		}

		rval = connect(sd, res->ai_addr, res->ai_addrlen);

		if(rval == -1 && errno != EISCONN)
			if(ris == 0)
				item->status = HOST_TIMEOUT; // printf("%s:%u timeout\n" , hostname, port);
			else
				item->status = HOST_INACTIVE; // printf("%s:%u is closed spent %d microseconds\n", hostname, port, msec);
		else item->status = HOST_ACTIVE; // printf("%s:%u is opened spent %d microseconds\n", hostname, port, msec);
	}
	else // puts("OK instantaneously connected"); // in general this is impossible
		item->status = HOST_ACTIVE;
	close(sd);         // close socket descriptor
	return NULL;
}

int pingHostList(struct host *hosts, const unsigned numHosts)
{
	const int MIN = numHosts < THREAD_OPEN_MAX ? numHosts : THREAD_OPEN_MAX;
	pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * MIN);
	unsigned h = 0;

	while(h < numHosts) {
		unsigned c;
		for (c = 0; h < numHosts && c < MIN; h ++, c ++)
			pthread_create(threads + c, NULL, ping, hosts + h);

		for(c = 0; c <= (h-1)%MIN; c ++) {
			void* ptr = NULL;
			pthread_join(threads[c], &ptr);
			if(ptr != NULL) {
				unsigned pos = h - (h-1) % MIN + c - 1;
				hosts[pos].details = strerror(*((int*)ptr));
				hosts[pos].status = HOST_ERROR;
			}
		}
	}

	free(threads);
	return 0;
}
