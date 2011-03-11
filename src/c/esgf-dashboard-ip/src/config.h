/*
 * config.h
 *
 *      Author: University of Salento and CMCC 
 */

#ifndef CONFIG_H_
#define CONFIG_H_

char *MYSQL_HOST;
unsigned short MYSQL_PORT_NUMBER;
char *MYSQL_USER;
char *MYSQL_PASSWD;
char *MYSQL_DB_NAME;
int CONNECTION_TIMEOUT; // microseconds
unsigned short THREAD_OPEN_MAX; // max threads #
int PING_SPAN;
int HOSTS_LOADING_SPAN;

#endif /* CONFIG_H_ */
