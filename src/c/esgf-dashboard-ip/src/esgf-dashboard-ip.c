/*
 * main.c
 *
 *      Author: University of Salento and CMCC 
 */
#include <string.h>
#include <pthread.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include "libpq-fe.h"
#include <dirent.h>
#include "../include/ping.h"
#include "../include/stats.h"
#include "../include/dbAccess.h"
#include "../include/config.h"
#include "../include/debug.h"
#include "../include/ftpget.h"

static const char *project[]={"CMIP5","CORDEX","OBS4MIPS","all projects",NULL};
 
static const char *table[]={"cmip5_data_usage","cordex_data_usage","obs4mips_data_usage","all_data_usage",NULL};

int msglevel; // global variable for log purposes 

static struct option long_options[] = {
  {"version", 0, 0, 'v'},
  {"help", 0, 0, 'h'},
  {0, 0, 0, 0}
};

static char *USAGE =
  "USAGE: %s [-v | -h] \n"
  "  --v, --version                            \n"
  "  --h, --help                               \n\n"
  "EXAMPLE:\n"
  "esgf-dashboard-ip version => %s -v \n"
  "esgf-dashboard-ip start ===> %s \n";

#define PRINT_USAGE fprintf(stdout, USAGE, argv[0],argv[0], argv[0])
#define VERSION "@version_num@"
//#define XMLPARSER_THREAD_FREQ 60  // release value 
//#define XMLPARSER_THREAD_FREQ 3 // test value

//pthread_barrier_t barr;

void readConfig (void);
int myfree (char *mystring);
void print_all_properties (void);
//void get_event (int fd, const char *target);
//void handle_error (int error);

void
print_all_properties (void)
{
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"POSTGRES_HOST value = [%s]\n", POSTGRES_HOST);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"POSTGRES_DB_NAME value = [%s]\n", POSTGRES_DB_NAME);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"POSTGRES_USER value = [%s]\n", POSTGRES_USER);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"POSTGRES_PORT_NUMBER value = [%d]\n",POSTGRES_PORT_NUMBER);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"CONNECTION_TIMEOUT = [%d]\n", CONNECTION_TIMEOUT);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"THREAD_OPEN_MAX = [%d]\n", THREAD_OPEN_MAX);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"PING_SPAN = [%d]\n", PING_SPAN);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"PING_SPAN_NO_HOSTS = [%d]\n", PING_SPAN_NO_HOSTS);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"HOSTS_LOADING_SPAN = [%d]\n", HOSTS_LOADING_SPAN);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"REGISTRATION_XML_PATH = [%s]\n", REGISTRATION_XML_PATH);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"REGISTRATION_XML_URL = [%s]\n", REGISTRATION_XML_URL);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"DASHBOARD_SERVICE_PATH = [%s]\n", DASHBOARD_SERVICE_PATH);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"NODE_TYPE = [%d]\n",NODE_TYPE);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"HOSTNAME = [%s]\n",ESGF_HOSTNAME);
}


/* Allow for 1024 simultanious events */
/* #define BUFF_SIZE ((sizeof(struct inotify_event)+FILENAME_MAX)*1024)

void
get_event (int fd, const char *target)
{
  ssize_t len, i = 0;
  char action[81 + FILENAME_MAX] = { 0 };
  char buff[BUFF_SIZE] = { 0 };

  len = read (fd, buff, BUFF_SIZE);

  while (i < len)
    {
      struct inotify_event *pevent = (struct inotify_event *) &buff[i];
      char action[81 + FILENAME_MAX] = { 0 };

      if (pevent->len)
	strcpy (action, pevent->name);
      else
	strcpy (action, target);

      if (pevent->mask & IN_ACCESS)
	strcat (action, " was read");
      if (pevent->mask & IN_ATTRIB)
	strcat (action, " Metadata changed");
      if (pevent->mask & IN_CLOSE_WRITE)
	strcat (action, " opened for writing was closed");
      if (pevent->mask & IN_CLOSE_NOWRITE)
	strcat (action, " not opened for writing was closed");
      if (pevent->mask & IN_CREATE)
	strcat (action, " created in watched directory");
      if (pevent->mask & IN_DELETE)
	strcat (action, " deleted from watched directory");
      if (pevent->mask & IN_DELETE_SELF)
	strcat (action, "Watched file/directory was itself deleted");
      if (pevent->mask & IN_MODIFY)
	strcat (action, " was modified");
      if (pevent->mask & IN_MOVE_SELF)
	strcat (action, "Watched file/directory was itself moved");
      if (pevent->mask & IN_MOVED_FROM)
	strcat (action, " moved out of watched directory");
      if (pevent->mask & IN_MOVED_TO)
	strcat (action, " moved into watched directory");
      if (pevent->mask & IN_OPEN)
	strcat (action, " was opened");
*/
      /*
         printf ("wd=%d mask=%d cookie=%d len=%d dir=%s\n",
         pevent->wd, pevent->mask, pevent->cookie, pevent->len, 
         (pevent->mask & IN_ISDIR)?"yes":"no");

         if (pevent->len) printf ("name=%s\n", pevent->name);
       */

/*      printf ("%s\n", action);

      i += sizeof (struct inotify_event) + pevent->len;

    }

}*/				/* get_event */

/* ----------------------------------------------------------------- */
/*
void
handle_error (int error)
{
  fprintf (stderr, "Error: %s\n", strerror (error));

}*/				/* handle_error */

/* ----------------------------------------------------------------- */



/* This is our thread function.  It is like main(), but for a thread*/

//void *
int automatic_registration_xml_feed (void *arg)
{
  char *esgf_registration_xml_path;

 char target[FILENAME_MAX];
  int result;
  int fd;

  esgf_registration_xml_path = (char *) arg;
  sprintf (target, "%s", esgf_registration_xml_path);

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Locking procedure for: %s\n",target);

         // l_type   l_whence  l_start  l_len  l_pid  
  struct flock fl = {F_WRLCK, SEEK_SET,   0,      0,     0 };
  fl.l_pid = getpid();
  fl.l_type = F_RDLCK;

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to open file\n");
  if ((fd = open(esgf_registration_xml_path, O_RDWR)) == -1) {
	pmesg(LOG_ERROR,__FILE__,__LINE__,"Open error... skip parsing\n");
	return -1;
	}

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to get lock...\n");
  if (fcntl(fd, F_SETLKW, &fl) == -1) {
        pmesg(LOG_ERROR,__FILE__,__LINE__,"Lock error... skip parsing\n");
	close(fd);
	return -1;
	}

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Locked (%s)\n",target);
       
  _automatic_registration_xml_feed (target);

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to release lock...\n");
  fl.l_type = F_UNLCK;  // set to unlock same region 

  if (fcntl(fd, F_SETLK, &fl) == -1) {
    	pmesg(LOG_ERROR,__FILE__,__LINE__,"Unlock error... \n"); 
 	close(fd);
	return -1;
        }

 close(fd);
 pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s Unlocked.\n",target);

  return 0;
}

void * realtime_monitoring(void *arg)
{
	int i; 

	i=0; 
	while (1) // while(i<3) TEST_  ---- while (1) PRODUCTION_
	{
	    if (!i) //the first time it creates the files
  		realtime_monitoring_setup();
    	    realtime_cpu_get_stats();
	    realtime_mem_get_stats();
	    sleep(1); 
	    i++;  
	}

	return NULL;
}

void * data_download_metrics_dw_reconciliation(void *arg)
{
	int i; 

	i=0; 
	while (1) // while(i<3) TEST_  ---- while (1) PRODUCTION_
	{
	    // skip the first time, because the process is called once before this loop	
	    if (i>0) {
  		compute_remote_clients_data_mart();
                int num_proj;
                for(num_proj=0; project[num_proj]!=NULL; num_proj++)
                   reconciliation_process_planB(project[num_proj], table[num_proj],num_proj);  
		compute_aggregate_data_user_metrics();	
		//if (FEDERATED_STATS) 
		//	federation_level_aggregation_metrics_planB();
		}
	    sleep(DATA_METRICS_SPAN*3600); // PRODUCTION_ once a day
	    i++;  
	}

	return NULL;
}

void * data_federA(void *arg)
{
     int res=0;
     DIR *dir;
     char path_feder[2048] = { '\0' };
     struct dirent *entry;

     while (1) // while(i<3) TEST_  ---- while (1) PRODUCTION_
     {
          dir = opendir (FED_DIR);
          while ((entry = readdir (dir)) != NULL) {
            if ( !strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") )
            {
              continue;
            } else {
              sprintf(path_feder, "%s/%s", FED_DIR, entry->d_name);
              unlink(path_feder);
            }
            //printf("\n%s",entry->d_name);
         }
         closedir(dir);
         res=compute_federation();
	 sleep(DATA_METRICS_SPAN*3600); // PRODUCTION_ once a day
	 //sleep(1800); // PRODUCTION_ once a day
	 //sleep(120);
     }
     return NULL;
}

void * data_planA(void *arg)
{
        int res=0;
        int res1=0;

        PGconn * conn;
        PGresult *res2;
        char conninfo[1024] = {'\0'};

        /* Connect to database */
        snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
        conn = PQconnectdb ((const char *) conninfo);

        if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s", PQerrorMessage(conn));
                PQfinish(conn);
                return -1;
        }
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying open transaction\n");

        // start transaction
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY8);
        res2 = PQexec(conn, QUERY8);
        if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
        {
           pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
           PQclear(res2);
           PQfinish(conn);
           return -2;
        }
        PQclear(res2);
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction opened\n");

        char update_query[2048] = { '\0' };
 
        sprintf(update_query, "%s", QUERY_UPDATE_REGISTRY_INIT);        

        res2 = PQexec(conn, update_query);
        PQclear(res2);

        res2 = PQexec(conn, QUERY4);
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction\n");
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY4);
        if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
        {
          pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed\n");
          PQclear(res2);
          PQfinish(conn);
          return -2;
        }
        PQclear(res2);
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction closed\n");

        PQfinish(conn);
        
	while (1) // while(i<3) TEST_  ---- while (1) PRODUCTION_
	{
            res=get_download_shards(".work", ".work/shards.xml");
            if(res==0)
              pmesg(LOG_DEBUG,__FILE__,__LINE__,"Download shards.xml with success\n");
            else
              pmesg(LOG_DEBUG,__FILE__,__LINE__,"Download shards.xml with unsuccess\n");
            
            res1=read_shards("./.work/shards.xml");
            fprintf(stderr, "%s\n", "START PLANA");

	    // skip the first time, because the process is called once before this loop	
	    while (1) // while(i<3) TEST_  ---- while (1) PRODUCTION_
	    {
               if(res1==-1)
                 res1=0;
	         //break;
                  
               res=compute_solr_process_planA(res1);
               if(res==-25)
	       {
                 fprintf(stderr, "%s\n", "There is no entries to be processed");
                 break;
	       }
	    }
	    sleep(DATA_METRICS_SPAN*3600); // PRODUCTION_ once a day
            //sleep(60);
            fprintf(stderr, "%s\n", "DONE PLANA");
	}

	return NULL;
}

int compute_aggregate_data_user_metrics()
{
  	int ret_code;
	long long int downdatacount;
	long long int downdatasize;
	long long int registeredusers;
	char metrics_filename[1024] = { '\0' };
	char metrics_content[2048] = { '\0' };
	char query_registered_users[2048] = { '\0' };
	FILE *fp;

	// Pre-computation of metrics: total data download (size,count) and total number of registered users x host 
	if (DASHBOARD_SERVICE_PATH) {	 
	
		if (!IDP_TYPE) { // IDP_TYPE==0 means that this node is an ESGF P2P IDP node
    			pmesg(LOG_DEBUG,__FILE__,__LINE__,"Node acting as a proxy for external idp registered users metrics. It is expected to have an external sensor providing the metrics!\n");
			return 0;
		}  
	    	//if (ret_code = get_single_value(GET_DOWNLOADED_DATA_COUNT, &downdatacount))
		//	pmesg(LOG_ERROR,__FILE__,__LINE__,"There was an issue retrieving the data download count metrics [Code %d]\n",ret_code);

	    	//if (ret_code = get_single_value(GET_DOWNLOADED_DATA_SIZE, &downdatasize))
		//	pmesg(LOG_ERROR,__FILE__,__LINE__,"There was an issue retrieving the data download size metrics [Code %d]\n",ret_code);

		downdatacount = 0;
		downdatasize  = 0;
        	registeredusers = 0;

	    	snprintf (query_registered_users,sizeof (query_registered_users),GET_REGISTERED_USERS_COUNT, ESGF_HOSTNAME);
	
			
		// todo: if DATANODETYPE = idp (16 dec ,10000 bin ,0x10 exac)
		// to do: external idp: new code testing a new esgf property related to internal/external idp node	
		// To do: this part must be changed to have different files containing these values separately 
		if ((NODE_TYPE & 10000) > 0) {
	    		if (ret_code = get_single_value(query_registered_users, &registeredusers))
				pmesg(LOG_ERROR,__FILE__,__LINE__,"Error retrieving the total number of users from esgf_security DB! [Code %d]\n",ret_code);
	    		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Retrieved metrics (data,users) [%lld] , [%lld] , [%lld] !\n", downdatacount, downdatasize, registeredusers);

    			// Storing data into data_users.metrics
    			snprintf (metrics_filename,sizeof (metrics_filename),"%s/data_users.metrics",DASHBOARD_SERVICE_PATH);
    			snprintf (metrics_content,sizeof (metrics_content),"DOWNLOADCOUNT=%lld,DOWNLOADSIZE=%lld,DOWNLOADUSERS=0,REGISTEREDUSERS=%lld",downdatacount,downdatasize,registeredusers);
    			fp=fopen(metrics_filename, "w+");
    			if (fp!=NULL) {
    				fprintf(fp, "%s",metrics_content);
    				fclose(fp);	
    				pmesg(LOG_DEBUG,__FILE__,__LINE__,"Data users Metrics successfully stored!\n");
    			} else {
    				pmesg(LOG_ERROR,__FILE__,__LINE__,"Failed to open the data_users.metrics file.The 'dashboard.ip.app.home' property must be properly set in the esgf.properties file to store the data and users metrics. Please check!\n");
    			}
		} 
        } else { // the DASHBOARD_SERVICE_PATH is NULL <=> 'dashboard.ip.app.home' not set in the esgf.properties file
		pmesg(LOG_ERROR,__FILE__,__LINE__,"The 'dashboard.ip.app.home' property must be properly set in the esgf.properties file to provide data and users metrics. Please check!\n");	
 	} 

	return 0;
}

int initialize_stats_file(char* filename)
{
    FILE *binaryFile;
    char metrics_filename[1024] = { '\0' };

    snprintf (metrics_filename,sizeof (metrics_filename),"%s/%s",DASHBOARD_SERVICE_PATH,filename);
    pmesg(LOG_DEBUG,__FILE__,__LINE__,"Initialize stats file [%s]!\n",metrics_filename);
    
    if (open_create_file(&binaryFile , metrics_filename,"r")) // r  - open for reading 
        open_create_file(&binaryFile ,metrics_filename, "w+");
    close_file(binaryFile);
    return 0;
}

int realtime_monitoring_setup(void)
{
    initialize_stats_file(REALTIME_CPU_1M);
    initialize_stats_file(REALTIME_CPU_5M);
    initialize_stats_file(REALTIME_CPU_15M);
    initialize_stats_file(REALTIME_MEM_RAM);
    initialize_stats_file(REALTIME_MEM_SWAP);
    // 3 files for realtime cpu monitoring exists now! 
    return 0;
}


int
main (int argc, char **argv)
{
  pthread_t pth;		// this is our thread identifier
  pthread_t pth_realtime;		// this is our thread identifier
  pthread_t pth_planA;		// this is our thread identifier
  pthread_t pth_feder;		// this is our thread identifier
  char *esgf_properties = NULL;
  char esgf_properties_default_path[1024] = { '\0' };
  char esgf_registration_xml_path[1024] = { '\0' };
  int res = 0;
  unsigned numHosts = 0;
  struct host *hosts = NULL;
  int counter = 0;
  int c;
  int option_index = 0;
  int iterator = 1;  // TEST_   PRODUCTION_ 1 
  int opt_t = 0;
  int mandatory;
  int allprop;
  int ret_code;
  char query_remove_old_service_metrics[2048] = { '\0' };
  char query_remove_old_local_cpu_metrics[2048] = { '\0' };
  char query_remove_old_local_memory_metrics[2048] = { '\0' };
  struct sensor_struct sens_struct[MAX_SENSORS];
  pthread_t threads[MAX_SENSORS]; 
  unsigned int num_sensors;  // real number of sensors in the config file
 
  // setting log level to the lowest one (DEBUG) 
  msglevel=2; //default = WARNING 

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  //LIBXML_TEST_VERSION


  DASHBOARD_SERVICE_PATH=NULL;

// reading the command line arguments
  while ((c =
	  getopt_long (argc, argv, "v:h", long_options, &option_index)) != -1)
    {
      switch (c)
	{
	case 'v':
	  fprintf (stdout, "%s\n", VERSION);
	  return 0;
	case 'h':
	  PRINT_USAGE;
	  return 0;
	case '?':
	  fprintf (stdout, "Bad Arguments\n");
	  PRINT_USAGE;
	  return 0;
	default:
	  fprintf (stdout, "Bad Arguments\n");
	  PRINT_USAGE;
	  return 0;
	}
    }

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Starting esgf-dashboard-ip\n");	
  fprintf(stderr,"[START] Starting esgf-dashboard-ip\n");	

// reading the ESGF_HOME attribute

  if (ESGF_config_path (&esgf_properties))
    {				// default setting /esg
      strcpy (esgf_properties_default_path, "/esg/"); // setting for release
      esgf_properties =
	(char *) malloc (strlen (esgf_properties_default_path) + 1);
      strcpy (esgf_properties, esgf_properties_default_path);
      //pmesg(LOG_WARNING,__FILE__,__LINE__,"ESGF_HOME attribute not found... setting /esg as default\n");	 
      fprintf(stderr,"[START] ESGF_HOME attribute not found... setting /esg as default\n");	 
    }

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"ESGF_HOME = [%s]\n", esgf_properties);	
  fprintf(stderr,"[START] ESGF_HOME = [%s]\n", esgf_properties);	

// reading the esgf-dashboard-ip properties

  if (ESGF_properties (esgf_properties, &mandatory, &allprop))
    {
      // check on mandatory properties
      if (mandatory)
	{
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"Please note that %d DB properties are missing in the esgf.properties file. Please check! Exit\n",mandatory);
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"Mandatory properties are:\n");
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"[db.host], [db.database], [db.port], [db.user]\n");
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"[esgf.host]\n");
	  //pmesg(LOG_ERROR,__FILE__,__LINE__,"[node.manager.service.app.home]\n");
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"[esgf.registration.xml.path]\n");
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"[esgf.registration.xml.download.url]\n");
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"[dashboard.ip.app.home]\n");
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"Please check!!!\n");
	  myfree (esgf_properties);
	  myfree (POSTGRES_HOST);
	  myfree (POSTGRES_DB_NAME);
	  myfree (POSTGRES_USER);
      	  myfree (DASHBOARD_SERVICE_PATH);
      	  myfree (REGISTRATION_XML_PATH);
      	  myfree (REGISTRATION_XML_URL);
      	  myfree (ESGF_HOSTNAME);
      	  myfree (ESGF_NODE_SOLR);
	  return 0;
	}
      // check on non-mandatory properties
      if (allprop)
	pmesg(LOG_WARNING,__FILE__,__LINE__,"Please note that %d non-mandatory properties are missing in the esgf.properties file. Default values have been loaded\n",allprop);
    }

// reading the postgres password

  if ((ESGF_passwd (esgf_properties)))
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Some error occurred while opening the .esgf_pass file. Please check!\n");
      myfree (esgf_properties);
      myfree (POSTGRES_HOST);
      myfree (POSTGRES_DB_NAME);
      myfree (POSTGRES_USER);
      myfree (REGISTRATION_XML_PATH);
      myfree (REGISTRATION_XML_URL);
      myfree (DASHBOARD_SERVICE_PATH);
      myfree (ESGF_HOSTNAME);
      myfree (ESGF_NODE_SOLR);
      return 0;
    }

// reading the nodetype code 

  if ((ESGF_node_type (esgf_properties)))
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Some error occurred while opening the config_type file. Please check!\n");
      myfree (esgf_properties);
      myfree (POSTGRES_HOST);
      myfree (POSTGRES_DB_NAME);
      myfree (POSTGRES_USER);
      myfree (REGISTRATION_XML_PATH);
      myfree (REGISTRATION_XML_URL);
      myfree (DASHBOARD_SERVICE_PATH);
      myfree (ESGF_HOSTNAME);
      myfree (ESGF_NODE_SOLR);
      myfree (POSTGRES_PASSWD);
      return 0;
    }

  print_all_properties (); // TEST_ --- PRODUCTION_ // da commentare

  sprintf (esgf_registration_xml_path, "%s/registration.xml",
	   REGISTRATION_XML_PATH);

  res=get_download_registration(REGISTRATION_XML_PATH, esgf_registration_xml_path);
  if(res==0)
      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Download registration.xml with success\n");
  else
      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Download registration.xml with unsuccess\n");


  snprintf (query_remove_old_service_metrics,sizeof (query_remove_old_service_metrics),QUERY5,HISTORY_MONTH, HISTORY_DAY);
  snprintf (query_remove_old_local_cpu_metrics,sizeof (query_remove_old_local_cpu_metrics),REMOVE_OLD_CPU_METRICS,HISTORY_MONTH, HISTORY_DAY);
  snprintf (query_remove_old_local_memory_metrics,sizeof (query_remove_old_local_memory_metrics),REMOVE_OLD_MEMORY_METRICS,HISTORY_MONTH, HISTORY_DAY);

  // at the beginning of the information provider	
  num_sensors = read_sensors_list_from_file(esgf_properties,&sens_struct[0]);
  //display_sensor_structures_info(num_sensors,&sens_struct[0]);
  //read_dmart_feder("./xml"); 
  //return 0;

  DIR* pDir = opendir(WORK_DIR);
  struct dirent *pFile;
  char file_n[128] = { '\0' };
  if(pDir!=NULL)
  {
     while ((pFile = readdir(pDir))) {
       sprintf(file_n, "%s/%s",WORK_DIR,pFile->d_name);
       unlink(file_n);
     }
     closedir(pDir);
  }

if(strcmp(ALLOW_FEDERATION, "yes")==0)
{
  DIR* pDir2 = opendir(FED_DIR);
  if(pDir2!=NULL)
  {
    while ((pFile = readdir(pDir2))) {
       sprintf(file_n, "%s/%s",FED_DIR,pFile->d_name);
       unlink(file_n);
    }
    closedir(pDir2);
  }
}
  
  struct stat st = {0};
  rmdir(WORK_DIR);
  if (stat(WORK_DIR, &st) == -1) {
            mkdir(WORK_DIR, 0700);
  }

if(strcmp(ALLOW_FEDERATION, "yes")==0)
{
  rmdir(FED_DIR);
  if (stat(FED_DIR, &st) == -1) {
            mkdir(FED_DIR, 0700);
  }
}
  
  ptr_mng         ptr_handle = NULL;
  pthread_mutex_t plana_feder;

if(strcmp(ALLOW_FEDERATION, "yes")==0)
{
  pthread_mutex_init(&plana_feder, NULL);
}
  pthread_mutex_t plana_mutex;
  pthread_mutex_init(&plana_mutex, NULL);
  
//if(strcmp(ALLOW_FEDERATION, "yes")==0)
//  ptr_register(&ptr_handle, (void **) &plana_feder, 0);

//  ptr_register(&ptr_handle, (void **) &plana_mutex, 0);

if(strcmp(ALLOW_FEDERATION, "yes")==0)
{
  // start thread PLANA
  pthread_mutex_lock(&plana_feder);
  pthread_create (&pth_feder, NULL, &data_federA,NULL);
  pthread_mutex_unlock(&plana_feder);
}
  //compute_federation();

  pthread_mutex_lock(&plana_mutex);
  // start thread PLANA
  //compute_solr_process_planA(1);
  pthread_create (&pth_planA, NULL, &data_planA,NULL);
  pthread_mutex_unlock(&plana_mutex);

  //compute_remote_clients_data_mart();

  int num_proj;
  for(num_proj=0; project[num_proj]!=NULL; num_proj++)
      reconciliation_process_planB(project[num_proj], table[num_proj], num_proj);
  //compute_aggregate_data_user_metrics();

  //if (FEDERATED_STATS)
	//federation_level_aggregation_metrics_planB();

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Starting the forever loop for the metrics collector\n");

  // start thread 
  //pthread_create (&pth, NULL, &data_download_metrics_dw_reconciliation,NULL);

  //if (ENABLE_REALTIME)
  //	pthread_create (&pth_realtime, NULL, &realtime_monitoring,NULL);
  

  // enabling threads pool for sensors
  //if (num_sensors!=-1)
  	//thread_manager_start (&threads[0],&sens_struct,num_sensors);

#if 0
  counter = 0;
 // PRODUCTION_  while (iterator)
 // TEST_  while (iterator--)
  while (iterator)   
    {
      // Removing old metrics once 1 day
      //
      //if ((counter % 288) == 0) {
      //	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Removing old metrics (once a day)\n");
      //	if (ret_code=transaction_based_query(query_remove_old_service_metrics,QUERY6, QUERY4))
      //		  pmesg(LOG_ERROR,__FILE__,__LINE__,"Remove old service metrics FAILED! [Code %d]\n",ret_code);
      //	if (ret_code=transaction_based_query(query_remove_old_local_cpu_metrics,START_TRANSACTION_CPU_METRICS, END_TRANSACTION_CPU_METRICS))
      //		  pmesg(LOG_ERROR,__FILE__,__LINE__,"Remove old local cpu metrics FAILED! [Code %d]\n",ret_code);
      //	if (ret_code=transaction_based_query(query_remove_old_local_memory_metrics,START_TRANSACTION_MEMORY_METRICS, END_TRANSACTION_MEMORY_METRICS))
      //		  pmesg(LOG_ERROR,__FILE__,__LINE__,"Remove old local memory metrics FAILED! [Code %d]\n",ret_code);
      //	counter=0;
      //}
      // Calling the automatic registration_xml_feed into the parser
      automatic_registration_xml_feed (esgf_registration_xml_path);

      if (hosts)
	 free (hosts);
      hosts = loadHosts (&numHosts);

      // Retrieving local metrics	  
      //retrieve_localhost_metrics();	

      // Retrieving global metrics	  
      if (numHosts != 0 && hosts != NULL)
	{
	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Host/services found. Let's check them...\n");
	  pingHostList (hosts, numHosts);
	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Availability check - START\n");
	  writeResults (hosts, numHosts);
	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Availability check - END\n");
	  //counter = (counter + 1) % HOSTS_LOADING_SPAN;
	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Metrics have been collected. Now waiting for %d sec\n",PING_SPAN);
	  sleep (PING_SPAN);
	}
      else
	{
	  pmesg(LOG_WARNING,__FILE__,__LINE__,"Host/services not found...\n");
	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Waiting for %d sec\n", PING_SPAN_NO_HOSTS);
	  sleep (PING_SPAN_NO_HOSTS);
	}
      counter++; 
    }				// forever loop end

  // end thread

  if (pthread_join (pth, NULL))
  	pmesg(LOG_ERROR,__FILE__,__LINE__,"pthread_join error!!!\n");
  else
  	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Pre-compute data download metrics thread joined the master!\n");
  if (ENABLE_REALTIME)
	{
  	if (pthread_join (pth_realtime, NULL))
  		pmesg(LOG_ERROR,__FILE__,__LINE__,"pthread_join error - realtime !!!\n");
  	elsepth_planA,
  		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Realtime monitoring thread joined the master!\n");
    	}		
#endif
  if (pthread_join (pth_planA, NULL))
  	pmesg(LOG_ERROR,__FILE__,__LINE__,"pthread_join PLANA error!!!\n");
  else
  	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Pre-compute data download metrics thread joined the master for PLANA!\n");

  pthread_detach(pth_planA);
  pthread_mutex_destroy(&plana_mutex);
 
if(strcmp(ALLOW_FEDERATION, "yes")==0)
{
  if (pthread_join (pth_feder, NULL))
  	pmesg(LOG_ERROR,__FILE__,__LINE__,"pthread_join FEDERATION error!!!\n");
  else
  	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Pre-compute data federation thread joined the master for PLANA!\n");
     pthread_mutex_destroy(&plana_feder);
}		
  
  // end of thread pool for sensors	
  //if (num_sensors!=-1)
  	//thread_manager_stop (&threads[0],&sens_struct,num_sensors);

  
  // freeing space
  fprintf(stderr,"***************************************************\n");
  fprintf(stderr,"[END] esgf-dashboard-ip with a FINITE loop\n");
  fprintf(stderr,"[END] Releasing memory\n");

  if (hosts)
    free (hosts);

  myfree (esgf_properties);
  myfree (POSTGRES_HOST);
  myfree (POSTGRES_DB_NAME);
  myfree (POSTGRES_USER);
  myfree (POSTGRES_PASSWD);
  myfree (REGISTRATION_XML_PATH);
  myfree (REGISTRATION_XML_URL);
  myfree (ESGF_HOSTNAME);
  myfree (ESGF_NODE_SOLR);
  myfree (DASHBOARD_SERVICE_PATH);

  pthread_exit(NULL);

  fprintf(stderr,"[END] esgf-dashboard-ip end\n");

  return 0;
}

int
myfree (char *mystring)
{
  if (mystring)
    free (mystring);
  return 0;
}

int
ESGF_config_path (char **esgf_properties_pointer)
{

  char esg_env[256] = { '\0' };
  char *position;
  int notfound;

  sprintf (esg_env, "/etc/esg.env");			// path for release

  FILE *file = fopen (esg_env, "r");

  if (file == NULL)		// /etc/esg.env not found
    return -1;

  notfound = 1;
  while (notfound)
    {
      char buffer[256] = { '\0' };
      char export_buffer[256] = { '\0' };
      char value_buffer[256] = { '\0' };

      if ((fscanf (file, "%s", export_buffer)) == EOF)	// skip EXPORT
        {
  	fclose (file);
	return -1;
	}

      if ((fscanf (file, "%s", buffer)) == EOF)	// now reading ATTRIBUTE=VALUE
        {
  	fclose (file);
	return -1;
	}

      position = strchr (buffer, '=');
      if (position != NULL)	// the '=' has been found
	{
	  strcpy (value_buffer, position + 1);	// now value_buffer stores the VALUE        
	  *position = '\0';	// now buffer stores the ATTRIBUTE       

	  if (!(strcmp (buffer, "ESGF_HOME")))
	    {
	      *esgf_properties_pointer =
		(char *) malloc (strlen (value_buffer) + 1);
	      strcpy (*esgf_properties_pointer, value_buffer);
	      notfound = 0;
	    }
	}
    }
  fclose (file);
  return (notfound);
}


int
ESGF_properties (char *esgf_properties_path, int *mandatory_properties,
		 int *notfound)
{

  char esgf_properties_filename[256] = { '\0' };
  char *position;

// this line is ok for local and production env
  sprintf (esgf_properties_filename,
	   "/%s/config/esgf.properties", esgf_properties_path);

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s\n", esgf_properties_filename);
  fprintf(stderr,"[START] %s\n", esgf_properties_filename);
  FILE *file = fopen (esgf_properties_filename, "r");

  if (file == NULL)		// /esg/config/esgf.properties not found
    return -1;

  // setting default values for non-mandatory properties 

  CONNECTION_TIMEOUT = 1000000;
  THREAD_OPEN_MAX = 20;
  PING_SPAN = 295;
  PING_SPAN_NO_HOSTS = 295;
  HOSTS_LOADING_SPAN = 120;
  HISTORY_MONTH=0;
  HISTORY_DAY=7;
  FEDERATED_STATS = 0;		// federated stats enabled=1 or disabled=0. Default disabled! 
  DATA_METRICS_SPAN=24;		// default 24 hour   
  REALTIME_SAMPLES=10; 
  ENABLE_REALTIME=0;		// realtime time stats enabled=1 or disabled=0. Default enabled! 
  IDP_TYPE=1; 			// default 1=classic idp node ; 0=external identity provider
  *notfound = 20;		// number of total properties to be retrieved from the esgf.properties file
  *mandatory_properties = 9;	// number of mandatory properties to be retrieved from the esgf.properties file

  while ((*notfound))
    {
      char buffer[256] = { '\0' };
      char value_buffer[256] = { '\0' };

      if ((fscanf (file, "%s", buffer)) == EOF)	// now reading ATTRIBUTE=VALUE
	{
  	  fprintf(stderr,"[START] Debug level %d (1=ERROR, 2=WARNING, 3=DEBUG)\n***************************************************\n",msglevel);
	  fclose (file);
	  return (-1);		// not all of the properties are there 
	}
      position = strchr (buffer, '=');
      if (position != NULL)	// the '=' has been found
	{
	  strcpy (value_buffer, position + 1);	// now value_buffer stores the VALUE        
	  *position = '\0';	// now buffer stores the ATTRIBUTE       

	  if (!(strcmp (buffer, "db.host")))
	    {
	      strcpy (POSTGRES_HOST =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*mandatory_properties)--;
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "db.database")))
	    {
	      strcpy (POSTGRES_DB_NAME =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "db.port")))
	    {
	      POSTGRES_PORT_NUMBER = atoi (value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "db.user")))
	    {
	      strcpy (POSTGRES_USER =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "esgf.host")))
	    {
	      strcpy (ESGF_HOSTNAME = (char *) malloc (strlen (value_buffer) + 1), value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "esgf.registration.xml.path")))
	    {
	      strcpy (REGISTRATION_XML_PATH =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "esgf.registration.xml.download.url")))
	    {
	      strcpy (REGISTRATION_XML_URL =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "dashboard.ip.app.home")))
	    {
	      strcpy (DASHBOARD_SERVICE_PATH =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "esgf.index.peer")))
	    {
	      strcpy (ESGF_NODE_SOLR =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.connection_timeout")))
	    {
	      CONNECTION_TIMEOUT = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.thread_open_max")))
	    {
	      THREAD_OPEN_MAX = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.ping_span")))
	    {
	      PING_SPAN = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.history.month")))
	    {
	      HISTORY_MONTH = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.history.day")))
	    {
	      HISTORY_DAY = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.downdatarefresh.hour")))
	    {
	      DATA_METRICS_SPAN = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.realtimesamples")))
	    {
	      REALTIME_SAMPLES = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.realtime_stats")))
	    {
	      ENABLE_REALTIME = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.federated_stats")))
	    {
	      FEDERATED_STATS = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.idp.type")))
	    {
	      IDP_TYPE = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.debug.level")))
	    {
	      msglevel = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.ping_span_no_hosts")))
	    {
	      PING_SPAN_NO_HOSTS = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.hosts_loading_span")))
	    {
	      HOSTS_LOADING_SPAN = atoi (value_buffer);
	      (*notfound)--;
	    }
	}
    } 
  fprintf(stderr,"[START] Debug level %d (1=ERROR, 2=WARNING, 3=DEBUG)\n***************************************************\n",msglevel);
  fclose (file);
  return 0;			// all of the properties have been found
}



int
ESGF_passwd (char *esgf_passwd_path)
{

  char esgf_passwd_filename[256] = { '\0' };
  char buffer[256] = { '\0' };

  // this line is ok for local and production env
  sprintf (esgf_passwd_filename, "/%s/config/.esg_pg_pass", esgf_passwd_path);

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s\n", esgf_passwd_filename);
  FILE *file = fopen (esgf_passwd_filename, "r");

  if (file == NULL)		// /esg/config/.esg_pg_pass not found
    return -1;

  if ((fscanf (file, "%s", buffer)) == EOF)	// now reading passwd TO DO check on mem leak (closing file) 
    return -1;			// no password found 

  strcpy (POSTGRES_PASSWD = (char *) malloc (strlen (buffer) + 1), buffer);

  fclose (file);
  return 0;
}

int
ESGF_node_type (char *esgf_passwd_path)
{

  char esgf_nodetype_filename[256] = { '\0' };
  char buffer[256] = { '\0' };

  // this line is ok for local and production env
  sprintf (esgf_nodetype_filename, "/%s/config/config_type", esgf_passwd_path);

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s\n", esgf_nodetype_filename);
  FILE *file = fopen (esgf_nodetype_filename, "r");

  if (file == NULL)		// /esg/config/config_type not found
    return -1;

  if ((fscanf (file, "%s", buffer)) == EOF)	// now reading config_type TO DO check on mem leak (closing file) 
    return -1;			// no password found 

  NODE_TYPE = atoi (buffer);

  //strcpy (POSTGRES_PASSWD = (char *) malloc (strlen (buffer) + 1), buffer);

  fclose (file);
  return 0;
}
int ptr_register (ptr_mng* reg, void ** ptr, int type)
{
  void ****tmp;
  int *tmp_i;

  if (!(*reg)){
    *reg = (ptr_mng) calloc (1, sizeof(struct _ptr_mng));
        if (!*reg)
           return -1;

        (*reg)->len = 0;
        (*reg)->ptr = (void****)calloc (1, sizeof(void*));
    if (!(*reg)->ptr)
      return -1;
        (*reg)->type = (int*)calloc(1, sizeof(int));
    if (!(*reg)->type)
      return -1;
  }

  (*reg)->len++;
  tmp = (void****) realloc((*reg)->ptr, (*reg)->len * sizeof(void*));
  if (!tmp)
        return -1;
  (*reg)->ptr = tmp;

  tmp_i = (int*) realloc((*reg)->type, (*reg)->len * sizeof(int));
  if (!tmp_i)
        return -1;
  (*reg)->type = tmp_i;

  (*reg)->ptr[(*reg)->len - 1] = (void***) ptr;

  (*reg)->type[(*reg)->len - 1] = type;

  return 0;
}
