/*
 * config.h
 *
 *      Author: University of Salento and CMCC 
 */

#ifndef CONFIG_H_
#define CONFIG_H_

char *POSTGRES_HOST;
unsigned short POSTGRES_PORT_NUMBER;
char *POSTGRES_USER;
char *POSTGRES_PASSWD;
char *POSTGRES_DB_NAME;
int CONNECTION_TIMEOUT; // microseconds
unsigned short THREAD_OPEN_MAX; // max threads #
int PING_SPAN;
int HOSTS_LOADING_SPAN;

#endif /* CONFIG_H_ */
