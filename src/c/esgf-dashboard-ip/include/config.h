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
char *ESGF_HOSTNAME;
char *REGISTRATION_XML_PATH;
char *REGISTRATION_XML_URL;
char *DASHBOARD_SERVICE_PATH;
char *ESGF_NODE_SOLR;
int CONNECTION_TIMEOUT; // microseconds
unsigned short THREAD_OPEN_MAX; // max threads #
int PING_SPAN;
int PING_SPAN_NO_HOSTS;
int HOSTS_LOADING_SPAN;
int HISTORY_MONTH;
int HISTORY_DAY;
long int DATA_METRICS_SPAN; // hours
int IDP_TYPE; // 0 external IDP use case ; 1 default, classical ESGF P2P IDP 
int FEDERATED_STATS; // 0 disabled - 1 enabled ; 0 default
int ENABLE_REALTIME; // 0 disabled - 1 enabled ; 1 default
int NODE_TYPE;
int REALTIME_SAMPLES;

struct geo_output_struct
{
  char country_code[256];
  char region[256];
  char city[256];
  char postal_code[256];
  double latitude;
  double longitude;
  long int metro_code;
  long int area_code;
};


#endif /* CONFIG_H_ */
