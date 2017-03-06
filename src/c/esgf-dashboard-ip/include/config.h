/*
 * config.h
 *
 *      Author: University of Salento and CMCC 
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define max_num_node 500
#define max_num_datamart 500
#define ipLength 100
#define MAX_WAITING_TIME 180
#define CONFIG_FILE_NAME "../etc/config.xml"
#define WORK_DIR "./.work"

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
extern char ipNodeAddress[max_num_node][ipLength];
extern char datamart[max_num_node][max_num_datamart][100];
extern char timestamp[max_num_node][max_num_datamart][50];
extern char url[max_num_node][max_num_datamart][1000];

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

struct _ptr_mng {
  void **** ptr;
  int *type; // 0 = string; 1 = array null termined
  int len;
};
typedef struct _ptr_mng* ptr_mng;

int ptr_register (ptr_mng* reg, void ** ptr, int type);
#endif /* CONFIG_H_ */
