/*
 * dbAccess.c
 *
 *      Author: University of Salento and CMCC 
 */
#include <sys/types.h>
#include <fcntl.h>
#include <curl/curl.h>

#include <stdlib.h>
#include <stdio.h>
#include "libpq-fe.h"
#include <string.h>
#include "../include/ping.h"
#include "../include/dbAccess.h"
#include "../include/config.h"
#include "../include/debug.h"

#define CONNECTION_STRING "host=%s port=%d dbname=%s user=%s password=%s"
// todo: to be added the path DASHBOARD_SERVICE_PATH IN THE STATS FILE
#define TEMP_STATS_FILE ".stats.tmp"
// todo: to be added the path DASHBOARD_SERVICE_PATH IN THE STATS FILE
#define TOP_FILE ".top.txt"
#define TOP_COMMAND "/usr/bin/top -b -n 1 i "

int retrieve_localhost_metrics()
{
   retrieve_localhost_cpu_metrics();
   retrieve_localhost_memory_metrics();
   return 0;
}


int retrieve_localhost_memory_metrics()
{
 // retrieving memory (swap/ram) metrics

  char esgf_nodetype_filename[256] = { '\0' };
  char query_memory_metric_insert[2048] = { '\0' };
  char freeram[256] = { '\0' };
  char totram[256] = { '\0' };
  char freeswap[256] = { '\0' };
  char totswap[256] = { '\0' };
  long int fram,tram,fswap,tswap,uram,uswap;
  int ret_code;
  int i=0;
  int j=0;
  char line [ 1024 ];

  sprintf (esgf_nodetype_filename, "/proc/meminfo");
  FILE *file = fopen (esgf_nodetype_filename, "r");

  if (file == NULL)		
    return -1;

 while ( fgets ( line, sizeof line, file ) != NULL ) 
       {
        char value_buffer[256] = { '\0' };
        char *position;
        position = strchr (line, ':');
        if (position != NULL)     // the ':' has been found
        {
          	strcpy (value_buffer, position + 1);  // now value_buffer stores the VALUE
          	*position = '\0';     // now buffer stores the ATTRIBUTE
	}

  	char * pch= NULL;
  	pch = strtok (value_buffer," ");
	if (!strcmp(line,"MemTotal"))	
           {
    		snprintf(totram, sizeof(totram),"%s",pch);
		tram=atol(totram);
		//fprintf(stdout,"valori memoria [%s] [%s] [%s]\n",line, value_buffer,pch);
	   }
	if (!strcmp(line,"MemFree"))	
           {
    		snprintf(freeram, sizeof(freeram),"%s",pch);
		fram=atol(freeram);
		//fprintf(stdout,"valori memoria [%s] [%s] [%s]\n",line, value_buffer,pch);
	   }
	if (!strcmp(line,"SwapTotal"))	
           {
    		snprintf(totswap, sizeof(totswap),"%s",pch);
		tswap=atol(totswap);
		//fprintf(stdout,"valori memoria [%s] [%s] [%s]\n",line, value_buffer,pch);
	   }
	if (!strcmp(line,"SwapFree"))	
           {
 		snprintf(freeswap, sizeof(freeswap),"%s",pch);
		fswap=atol(freeswap);
		//fprintf(stdout,"valori memoria [%s] [%s] [%s]\n",line, value_buffer,pch);
	   }
       }
 fclose ( file );

 //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics RAM [%s] [%s] SWAP [%s] [%s]\n", totram,freeram,totswap, freeswap);
 //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics RAM [%ld] [%ld] SWAP [%ld] [%ld]\n", tram,fram,tswap,fswap);

 uram = tram-fram;
 uswap= tswap-fswap;
 pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics RAM [%ld] [%ld] SWAP [%ld] [%ld]\n", fram,uram,fswap,uswap);
 snprintf(query_memory_metric_insert,sizeof(query_memory_metric_insert),STORE_MEMORY_METRICS,fram,uram,fswap,uswap);
	
  if (ret_code = transaction_based_query(query_memory_metric_insert,START_TRANSACTION_MEMORY_METRICS,END_TRANSACTION_MEMORY_METRICS))
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Error storing the memory metrics in the DB! [Code %d]\n",ret_code);

  return 0;
}

/*int retrieve_localhost_memory_metrics()
{
 // retrieving memory (swap/ram) metrics

  char esgf_nodetype_filename[256] = { '\0' };
  char query_memory_metric_insert[2048] = { '\0' };
  char freeram[256] = { '\0' };
  char totram[256] = { '\0' };
  char freeswap[256] = { '\0' };
  char totswap[256] = { '\0' };
  long int fram,tram,fswap,tswap,uram,uswap;
  int ret_code;
  int i=0;
  int j=0;
  char line [ 1024 ];

  sprintf (esgf_nodetype_filename, "/proc/meminfo");
  FILE *file = fopen (esgf_nodetype_filename, "r");

  if (file == NULL)		
    return -1;

 while ( fgets ( line, sizeof line, file ) != NULL ) 
       {
        i++;
	if (i==1 || i==2 || i==12 || i==13) {
        	//fputs ( line, stdout ); 
  		char * pch= NULL;
  		pch = strtok (line," ");
		j=0;
  		while (pch != NULL)
  		{
			j++;
    			pch = strtok (NULL, " ");
			if (j==1) {
				switch (i) {
				case 1:
    					snprintf(totram, sizeof(totram),"%s",pch);
					tram=atol(totram);
					break;
				case 2:
    					snprintf(freeram, sizeof(freeram),"%s",pch);
					fram=atol(freeram);
					break;
				case 12:
    					snprintf(totswap, sizeof(totswap),"%s",pch);
					tswap=atol(totswap);
					break;
				case 13:
    					snprintf(freeswap, sizeof(freeswap),"%s",pch);
					fswap=atol(freeswap);
					break;
				default:
					break;
				}
			}
  		}
	}
       }
 fclose ( file );

 //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics RAM [%s] [%s] SWAP [%s] [%s]\n", totram,freeram,totswap, freeswap);
 //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics RAM [%ld] [%ld] SWAP [%ld] [%ld]\n", tram,fram,tswap,fswap);

 uram = tram-fram;
 uswap= tswap-fswap;
 pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics RAM [%ld] [%ld] SWAP [%ld] [%ld]\n", fram,uram,fswap,uswap);
 snprintf(query_memory_metric_insert,sizeof(query_memory_metric_insert),STORE_MEMORY_METRICS,fram,uram,fswap,uswap);
	
  if (ret_code = transaction_based_query(query_memory_metric_insert,START_TRANSACTION_MEMORY_METRICS,END_TRANSACTION_MEMORY_METRICS))
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Error storing the memory metrics in the DB! [Code %d]\n",ret_code);

  return 0;
}*/

int retrieve_localhost_cpu_metrics()
{
 // retrieving CPU load average metrics
  char esgf_nodetype_filename[256] = { '\0' };
  char query_cpu_metric_insert[2048] = { '\0' };
  char loadavg1[256] = { '\0' };
  char loadavg5[256] = { '\0' };
  char loadavg15[256] = { '\0' };
  int ret_code;

  sprintf (esgf_nodetype_filename, "/proc/loadavg");
  FILE *file = fopen (esgf_nodetype_filename, "r");

  if (file == NULL)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Open /proc/loadavg failed\n");
    return -1;
  }

  if ((fscanf (file, "%s", loadavg1)) == EOF)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Read /proc/loadavg loadavg1 failed\n");
    return -1;
  }
  if ((fscanf (file, "%s", loadavg5)) == EOF)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Read /proc/loadavg loadavg5 failed\n");
    return -1;
  }
  if ((fscanf (file, "%s", loadavg15)) == EOF)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Read /proc/loadavg loadavg15 failed\n");
    return -1;
  }

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Cpu load average metrics [%s] [%s] [%s]\n", loadavg1, loadavg5,loadavg15);

  snprintf (query_cpu_metric_insert,sizeof (query_cpu_metric_insert),STORE_CPU_METRICS,atof(loadavg1),atof(loadavg5),atof(loadavg15));
	
  if (ret_code = transaction_based_query(query_cpu_metric_insert,START_TRANSACTION_CPU_METRICS,END_TRANSACTION_CPU_METRICS))
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Error storing the cpu load average metrics in the DB! [Code %d]\n",ret_code);

  fclose (file);
  return 0;
}

int transaction_based_query(char *submitted_query, char* open_transaction, char* stop_transaction) 
{
  PGconn *conn;
  PGresult *res;

  //char query_history[2048] = { '\0' };
  char conninfo[1024] = {'\0'};

  /* Connect to database */
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction based query - START\n");

  snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
  conn = PQconnectdb ((const char *) conninfo);

  if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return -1;
        }

  // start transaction
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying open transaction: [%s]\n",QUERY6);
  //res = PQexec(conn, QUERY6);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying open transaction: [%s]\n",open_transaction);
  res = PQexec(conn, open_transaction);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed [%s]\n",open_transaction);
	        PQclear(res);
		PQfinish(conn);
		return -2;
    	}
  PQclear(res);

  // Query submission 
  // e.g old metrics DELETE Query or pre-computing analytics table 

  //snprintf (query_history,sizeof (query_history),QUERY5,HISTORY_MONTH, HISTORY_DAY);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Removing old metrics: [%s]\n",query_history);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query submission [%s]\n",submitted_query);
  //res = PQexec(conn, query_history);
  res = PQexec(conn, submitted_query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_WARNING,__FILE__,__LINE__,"Query submission failed [%s]\n", submitted_query);
	        PQclear(res);
		PQfinish(conn);
		return -4;
    	}
  PQclear(res);

       // close transaction
  //res = PQexec(conn, QUERY4);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction: [%s]\n",QUERY4);
  res = PQexec(conn, stop_transaction);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction: [%s]\n",stop_transaction);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed [%s]\n",stop_transaction);
	        PQclear(res);
		PQfinish(conn);
		return -3;
    	}
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction based query - END\n");

  PQclear(res);
  PQfinish(conn);

  return 0;
}


struct host * loadHosts(unsigned *numHosts) {
	PGconn *conn;
	PGresult *res;
	int numTuples;

	/* Connect to database */
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"*** Reloading host configuration ***\n");

	char conninfo[1024] = {'\0'};

        snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
	conn = PQconnectdb ((const char *) conninfo);

	if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		*numHosts = 0;
		struct host* hosts = NULL; 
		return hosts;
        }

	// start transaction
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying open transaction\n");
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY3);
	res = PQexec(conn, QUERY3);

	if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
	        PQclear(res);
		PQfinish(conn);
		*numHosts = 0;
		struct host* hosts = NULL;
		return hosts;
    	}
	PQclear(res);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction opened\n");

	// submit SELECT query
	res = PQexec(conn, QUERY1);

	if ((!res) || (PQresultStatus(res) != PGRES_TUPLES_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"SELECT host/services did not work properly\n");
	        PQclear(res);
		PQfinish(conn);
		*numHosts = 0;
		struct host* hosts = NULL;
		return hosts;
    	}

	numTuples = PQntuples(res);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Number of Host/Services loaded from the DB = %d \n",numTuples);

	struct host* hosts = (struct host *) malloc(sizeof(struct host) * numTuples);
	unsigned t;
	for(t = 0; t < numTuples; t ++) {
		hosts[t].id = atoi(PQgetvalue(res, t, 0));
		strcpy(hosts[t].hostName, PQgetvalue(res, t, 1));
		hosts[t].portNumber = atoi(PQgetvalue(res, t, 2));
	}
	PQclear(res);

	// stop transaction
	res = PQexec(conn, QUERY4);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction\n");
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY4);

	if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed\n");
	        PQclear(res);
		PQfinish(conn);
		*numHosts = 0;
		free(hosts);
		struct host* hosts = NULL;
		return hosts;
    	}
	PQclear(res);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction closed\n");
	
    	PQfinish(conn);
	*numHosts = numTuples;
	return hosts;
}

int writeResults(struct host *hosts, const unsigned numHosts) {

	PGconn * conn;
	PGresult *res;
        char conninfo[1024] = {'\0'};
	long int index;

        /* Connect to database */

        snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
        conn = PQconnectdb ((const char *) conninfo);

        if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
	  	return -1;	
        }

	for(index = 0; index < numHosts; index ++) 
	{
		char insert_query[2048]= {'\0'};
		char update_query[2048]= {'\0'};

		// Query composition //
		snprintf (insert_query, sizeof (insert_query), "%s VALUES(%d,%d,%d);", QUERY2,hosts[index].status, hosts[index].elapsedTime,hosts[index].id);
		snprintf (update_query, sizeof (update_query), QUERY2_UPDATE, hosts[index].status, hosts[index].elapsedTime,hosts[index].hostName);
		//pmesg(LOG_DEBUG,__FILE__,__LINE__,"Service Metrics [%d] -> %s\n",index, insert_query);
		//pmesg(LOG_DEBUG,__FILE__,__LINE__,"Global Metrics [%d] -> %s\n",index, update_query);

		res = PQexec(conn, insert_query);

  		if ((!res) || (PQresultStatus(res) != PGRES_COMMAND_OK))
       		 	pmesg(LOG_ERROR,__FILE__,__LINE__,"Insert SERVICE METRICS query failed\n");
    		PQclear(res);

		res = PQexec(conn, update_query);

		if ((!res) || (PQresultStatus(res) != PGRES_COMMAND_OK))
			pmesg(LOG_ERROR,__FILE__,__LINE__,"Update global-host availability metrics query failed\n");
		PQclear(res);
	}
        PQfinish(conn);
	return 0;
}


int get_single_value(char *submitted_query, long long int *metrics) 
{
  PGconn *conn;
  PGresult *res;
  long int numTuples;

  char conninfo[1024] = {'\0'};

  /* Connect to database */
  *metrics=0;
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Get value - START\n");

  snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
  conn = PQconnectdb ((const char *) conninfo);

   if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return -1;
        }

   // Query submission 

   res = PQexec(conn, submitted_query);

   if ((!res) || (PQresultStatus(res) != PGRES_TUPLES_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__," Get value - Query ERROR [%s]\n", submitted_query);
	        PQclear(res);
		PQfinish(conn);
		return -2;
    	}

   numTuples = PQntuples(res);
   //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Value [Tuples=%ld][%s] \n",numTuples,submitted_query);
   if (numTuples!=1) 
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__," Get value - Too many Tuples ERROR [%ld]\n",numTuples);
	        PQclear(res);
		PQfinish(conn);
		return -3;
    	}

   // setting return metrics	
   
   *metrics = atoll(PQgetvalue(res, 0, 0));

   //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Get value - END [value=%lld] \n", *metrics);
   PQclear(res);

   PQfinish(conn);

  return 0;
}


long int get_last_processed_id (PGconn * conn, char *query)
{
  PGresult *res;
  long int fk;

  res = PQexec (conn, query);
  if ((!res) || (PQresultStatus (res) != PGRES_TUPLES_OK))
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"SELECT command did not return tuples properly\n");
      PQclear (res);
      return -1;
    }

  if ((PQntuples (res)) != 1)
    {
      PQclear (res);
      pmesg(LOG_ERROR,__FILE__,__LINE__,"SELECT command returns more than 1 tuple\n");
      return -2;
    }

  fk = atol (PQgetvalue (res, 0, 0));

  PQclear (res);
  return fk;
}

int store_last_id_feddw(PGconn * conn, long int last_id_feddw, char* peername)
{
  PGresult *res;
  char insert_last_id[1024] = { '\0' };

  snprintf (insert_last_id, sizeof (insert_last_id),QUERY_UPDATE_PEER_LAST_ID,last_id_feddw,peername);
  res = PQexec(conn, insert_last_id);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
       	pmesg(LOG_ERROR,__FILE__,__LINE__,"Query update lastprocessed_id in federated-dw failed [%s]\n",peername);

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Processing entry [%s] END ==> Last_id=%ld\n",peername,last_id_feddw);
  PQclear(res);
}

int federation_level_aggregation_metrics_planB()
{
	PGconn *conn;
	PGresult *res;
	char conninfo[1024] = {'\0'};
	long int numTuples;
	long long int t,w; // w is the action
	int ret_code, nFields;
  	char peername[512] = { '\0' };

	// OPEN CONNECTION  
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Federation-level stats aggregation process planB START\n");

        snprintf (conninfo, sizeof (conninfo), CONNECTION_STRING, POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
	conn = PQconnectdb ((const char *) conninfo);

	if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Stats-Aggregator planB: Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return -1;
        }

	// SELECT START 

	res = PQexec(conn,QUERY_STATS_AGGREGATOR_GET_HOSTLIST_PLANB);

	if ((!res) || (PQresultStatus(res) != PGRES_TUPLES_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Stats-Aggregator planB: SELECT data from aggregation_process FAILED\n");
	        PQclear(res);
		PQfinish(conn);
		return -2;
    	}

	numTuples = PQntuples(res);
	nFields = PQnfields(res);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Stats-Aggregator planB: Number of fields/entries from the aggregation_process table to be processed [%ld,%d] \n",nFields,numTuples);

	for(t = 0; t < numTuples; t ++) 
		{
		 char remove_stats_feddw[1024] = { '\0' };
  		 char update_query_timestamp_counter_aggr[1024] = { '\0' };
  		 char get_last_id_query[1024] = { '\0' };
		 long int last_id_feddw;

  		 last_id_feddw = 0;
		 snprintf (peername,sizeof (peername),"%s",PQgetvalue(res, t, 0));
		 w = atol(PQgetvalue(res, t, 1)); 
 		 pmesg(LOG_DEBUG,__FILE__,__LINE__,"Processing entry planB [%s,%ld]\n",peername,w);
		 snprintf (update_query_timestamp_counter_aggr, sizeof (update_query_timestamp_counter_aggr),QUERY_UPDATE_PEER_TIMESTAMP_PLANB,peername);
		 snprintf (remove_stats_feddw, sizeof (remove_stats_feddw),QUERY_REMOVE_STATS_FEDDW_PLANB,peername);
		 //snprintf (get_last_id_query, sizeof (get_last_id_query),QUERY_GET_LAST_PROCESSED_ID_FED,peername);
		 // to be added: a transactional mechanism that delete stats only if I have the new ones!!!
		 if (w==-1) {
 		 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Action for [%s] planB = delete node stats from federationdw\n",peername);
			remove_stats_from_federation_level_dw(conn,remove_stats_feddw,update_query_timestamp_counter_aggr,START_TRANSACTION_FEDDW_PLANB,END_TRANSACTION_FEDDW_PLANB);
			}
		 if (w==0) {
 		 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Action for [%s] planB = delete node stats from federationdw & get them again from scratch (non-transact)\n",peername);
			remove_stats_from_federation_level_dw(conn,remove_stats_feddw,update_query_timestamp_counter_aggr,START_TRANSACTION_FEDDW_PLANB,END_TRANSACTION_FEDDW_PLANB);
			harvest_stats_planB(conn,peername);
			}
		 if (w==1) {
 		 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Action for [%s] planB = skipping node\n",peername);
			}
		}

	// CLOSE CONNECTION  
	PQclear(res);
    	PQfinish(conn);

 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Federation-level stats planB aggregation process END\n");

  return 0;
}
 
/*int federation_level_aggregation_metrics()
{
	PGconn *conn;
	PGresult *res;
	char conninfo[1024] = {'\0'};
	long int numTuples;
	long long int lastimport_id,t,w;
	int ret_code, nFields;
  	char peername[512] = { '\0' };

	// OPEN CONNECTION  
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Federation-level stats aggregation process START\n");

        snprintf (conninfo, sizeof (conninfo), CONNECTION_STRING, POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
	conn = PQconnectdb ((const char *) conninfo);

	if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Stats-Aggregator: Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return -1;
        }

	// SELECT START 

	res = PQexec(conn,QUERY_STATS_AGGREGATOR_GET_HOSTLIST);

	if ((!res) || (PQresultStatus(res) != PGRES_TUPLES_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Stats-Aggregator: SELECT data from aggregation_process FAILED\n");
	        PQclear(res);
		PQfinish(conn);
		return -2;
    	}

	numTuples = PQntuples(res);
	nFields = PQnfields(res);
	//pmesg(LOG_DEBUG,__FILE__,__LINE__,"Stats-Aggregator: Number of fields/entries from the aggregation_process table to be processed [%ld,%d] \n",nFields,numTuples);
	
	for(t = 0; t < numTuples; t ++) 
		{
  			 char remove_stats_feddw[1024] = { '\0' };
  			 char update_query_timestamp[1024] = { '\0' };
  			 char get_last_id_query[1024] = { '\0' };
			 long int last_id_feddw;

	  		 last_id_feddw = 0;
			 snprintf (peername,sizeof (peername),"%s",PQgetvalue(res, t, 0));
			 w = atol(PQgetvalue(res, t, 1)); 
 			 pmesg(LOG_DEBUG,__FILE__,__LINE__,"Processing entry [%s,%ld]\n",peername,w);
			 snprintf (update_query_timestamp, sizeof (update_query_timestamp),QUERY_UPDATE_PEER_TIMESTAMP,peername);
			 snprintf (remove_stats_feddw, sizeof (remove_stats_feddw),QUERY_REMOVE_STATS_FEDDW,peername);
			 snprintf (get_last_id_query, sizeof (get_last_id_query),QUERY_GET_LAST_PROCESSED_ID_FED,peername);

			 if (w==-1) {
 			 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Action for [%s] = delete node stats from federationdw\n",peername);
				remove_stats_from_federation_level_dw(conn,remove_stats_feddw,update_query_timestamp,START_TRANSACTION_FEDDW,END_TRANSACTION_FEDDW);
				}
			 if (w==0) {
 			 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Action for [%s] = delete node stats from federationdw & get them from scratch\n",peername);
				remove_stats_from_federation_level_dw(conn,remove_stats_feddw,update_query_timestamp,START_TRANSACTION_FEDDW,END_TRANSACTION_FEDDW);
				harvest_stats(w,conn,peername);
				if ((last_id_feddw = get_last_processed_id(conn,get_last_id_query)) > 0 )
					store_last_id_feddw(conn,last_id_feddw, peername);
				}
			 if (w>0) {
 			 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Action for [%s] = get stats from id=%ld\n",peername,w);
				harvest_stats(w,conn,peername);	
				if ((last_id_feddw = get_last_processed_id(conn,get_last_id_query)) > 0 )
					store_last_id_feddw(conn, last_id_feddw,peername);
				}
		}

	// CLOSE CONNECTION  
	PQclear(res);
    	PQfinish(conn);

 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Federation-level stats aggregation process END\n");


  return 0;
}*/


int harvest_stats_planB(PGconn *conn,char *peername)
{
  CURL *curl;
  PGresult *res;
  CURLcode curl_res;
  CURLINFO info;
  long http_code;
  double c_length;  
  FILE *tmp;
  FILE *file;
  char buffer[10024];
  char url_action[10024];
  long int i;
  int right_url;  

  snprintf (url_action, sizeof (url_action),URL_STATS_PLANB,peername);
  //snprintf (url_action, sizeof (url_action),URL_STATS_PLANB);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Contacting [hostname=%s]\n",peername);

  // filename to be stats_<host>_<processed_id>.tmp

  tmp=fopen(TEMP_STATS_FILE, "w");
  if(tmp==NULL) 
	{
    	 pmesg(LOG_ERROR,__FILE__,__LINE__,"ERROR to open file stats.tmp\n");
    	 return -2; 
  	}

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url_action);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,  tmp);
  curl_res = curl_easy_perform(curl);
  if(curl_res) 
   	{
    	pmesg(LOG_ERROR,__FILE__,__LINE__,"ERROR contatting [%s] or downloading stats\n",peername);
  	remove(TEMP_STATS_FILE);
    	fclose(tmp);
    	curl_easy_cleanup(curl);
    	return -1;
   	}	
  fclose(tmp);
  curl_easy_cleanup(curl);

  file=fopen(TEMP_STATS_FILE, "r");

  if (file == NULL)    
       	{
  	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Error opening file\n");
    	return -1;
     	} 

  i=0;
  right_url = 1; 
  char line [ 1024 ];

  while ( (fgets ( line, sizeof line, file ) != NULL) && (right_url) ) 
       	{
  		char insert_remote_stat[10024] = { '\0' };
		i++;
		if (i==1)
        	{
		   if (strcmp(line,"USAGESTATS\n"))
			right_url=0;	
		   continue;
        	} 
                //fputs ( line, stdout ); 
    		snprintf (insert_remote_stat, sizeof (insert_remote_stat),INSERT_REMOTE_STAT_PLANB,line);
		//fprintf(stdout, "Harvesting : %s",insert_remote_stat);	
	
  		res = PQexec(conn, insert_remote_stat);

  		if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
                	pmesg(LOG_ERROR,__FILE__,__LINE__,"Query insert entry in federated-dw failed [%s]\n",insert_remote_stat);

  		PQclear(res);
       	} 
  
  fclose ( file );
  remove(TEMP_STATS_FILE);
  
 return 0;
}


int harvest_stats(long long int processed_id, PGconn *conn,char *peername)
{
  CURL *curl;
  PGresult *res;
  CURLcode curl_res;
  CURLINFO info;
  long http_code;
  double c_length;  
  FILE *tmp;
  FILE *file;
  char buffer[10024];
  char url_action[10024];
  long int delta_id,i;
  int exit_while, right_url;  

  delta_id=500;
  exit_while=1;
  
  while (exit_while)
   {
    snprintf (url_action, sizeof (url_action),URL_STATS,peername,processed_id,delta_id);
    pmesg(LOG_DEBUG,__FILE__,__LINE__,"Contacting [hostname=%s] [startID=%ld] [deltaID=%ld] \n",peername,processed_id,delta_id);

    // filename to be stats_<host>_<processed_id>.tmp

    tmp=fopen(TEMP_STATS_FILE, "w");
    if(tmp==NULL) 
	{
    	 pmesg(LOG_ERROR,__FILE__,__LINE__,"ERROR to open file stats.tmp\n");
    	 return -2; 
  	}

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url_action);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,  tmp);
    curl_res = curl_easy_perform(curl);
    if(curl_res) 
   	{
    	pmesg(LOG_ERROR,__FILE__,__LINE__,"ERROR in dowloading file\n");
  	remove(TEMP_STATS_FILE);
    	fclose(tmp);
    	curl_easy_cleanup(curl);
    	return -1;
   	}	

    fclose(tmp);
    curl_easy_cleanup(curl);

    file=fopen(TEMP_STATS_FILE, "r");

    if (file == NULL)    
    	return -1;

    i=0;
    right_url = 1; 
    char line [ 1024 ];

    while ( (fgets ( line, sizeof line, file ) != NULL) && (right_url) ) 
       	{
  		char insert_remote_stat[10024] = { '\0' };
		i++;
		if (i==1)
        	{
		   if (strcmp(line,"REMOTE_STATS_ACTION\n"))
        		{
			right_url=0;	
			exit_while=0;
        		} 
		   continue;
        	} 
                //fputs ( line, stdout ); 
    		snprintf (insert_remote_stat, sizeof (insert_remote_stat),INSERT_REMOTE_STAT,line);
		//printf("Query %s\n",insert_remote_stat);	
	
  		res = PQexec(conn, insert_remote_stat);

  		if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
                	pmesg(LOG_ERROR,__FILE__,__LINE__,"Query insert entry in federated-dw failed\n");

  		PQclear(res);
       	} 

  fclose ( file );
  remove(TEMP_STATS_FILE);

  if ((i==1) && (right_url==1))
	exit_while=0;

  processed_id += delta_id;
 } 
 
 return 0;
}

int reconciliation_process_planB()
{
  	char update_query_hostname[2048] = { '\0' };

 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Reconciliation process planB START\n");
	
	if (transaction_based_query(QUERY_PLANB_SUMMARY_DB_TMP, QUERY8, QUERY4))
		return -1;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 1 [OK]\n");

	if (transaction_based_query(QUERY_PLANB_ADD_HOSTNAME_COLUMN, QUERY8, QUERY4))
		return -1;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 2 [OK]\n");
	snprintf (update_query_hostname, sizeof (update_query_hostname),QUERY_PLANB_ADD_HOSTNAME_VALUE,ESGF_HOSTNAME);

	if (transaction_based_query(update_query_hostname, QUERY8, QUERY4))
		return -1;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 3 [OK]\n");

	if (transaction_based_query(QUERY_PLANB_SUMMARY_DB, QUERY8, QUERY4))
		return -1;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 4 [OK]\n");

 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Reconciliation process planB END\n");
 	return 0;
}

int compute_remote_clients_data_mart()
{
	PGconn *conn;
	PGresult *res;
	PGresult *res2;
	long int numTuples;
	long long int lastimport_id;
	int ret_code, nFields;
	char conninfo[1024] = {'\0'};
  	char attributes_list[2048] = { '\0' };
  	char attributes_list_temp[2048] = { '\0' };
	int code;
  	struct geo_output_struct geo_output;
  	int geo_outputmask=255;
  	char buffer[256] = { '\0' };
        int t;

	//QUERY_GET_LIST_CLIENT_IPS
      	pmesg(LOG_DEBUG,__FILE__,__LINE__,"START - Building new client data mart (once a day)\n");
      	if (ret_code=transaction_based_query(QUERY_CREATE_CLIENT_DATAMART,QUERY8, QUERY4))
		{
		  pmesg(LOG_ERROR,__FILE__,__LINE__,"Building new client data mart FAILED! [Code %d]\n",ret_code);
		  return ret_code;
		}

	// OPEN CONNECTION  
        snprintf (conninfo, sizeof (conninfo), CONNECTION_STRING, POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
	conn = PQconnectdb ((const char *) conninfo);

	if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return -1;
        }
	// SELECT START 
	res = PQexec(conn,QUERY_GET_LIST_CLIENT_IPS);

	if ((!res) || (PQresultStatus(res) != PGRES_TUPLES_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"SELECT remote clients data from access_logging FAILED\n");
	        PQclear(res);
		PQfinish(conn);
		return -2;
    	}
	numTuples = PQntuples(res);
	nFields = PQnfields(res);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Number of remote client entries from the access_logging table to be processed [%ld] \n",numTuples);

	for(t = 0; t < numTuples; t ++) {
			// tuple level geoip resolv
  			char insert_query_client_dm[10024] = { '\0' };
  			char ip_str[1024] = { '\0' };

			snprintf (ip_str,sizeof (ip_str),"%s",PQgetvalue(res, t, 0));

  			if (code = esgf_geolookup (ip_str,&geo_output))
    				{
				pmesg(LOG_WARNING,__FILE__,__LINE__,"Local client data mart. Failing to resolve [%s]\n",ip_str);
    				} 
			else  
			{
			snprintf (insert_query_client_dm, sizeof (insert_query_client_dm), QUERY_INSERT_CLIENT_INFO, ESGF_HOSTNAME, geo_output.latitude, geo_output.longitude,geo_output.country_code);
  			res2 = PQexec(conn, insert_query_client_dm);

  			if ((!res2) || (PQresultStatus (res2) != PGRES_COMMAND_OK))
	        		pmesg(LOG_WARNING,__FILE__,__LINE__,"Query insert entry in local client data mart failed\n");
  			PQclear(res2);
    			} 
	}

	PQclear(res);

    	PQfinish(conn);

      	pmesg(LOG_DEBUG,__FILE__,__LINE__,"END - Building new client data mart (once a day)\n");
 	return 0;
}

/*int reconciliation_process()
{
	PGconn *conn;
	PGresult *res;
	long int numTuples;
	long long int lastimport_id;
	int ret_code, nFields;
	char conninfo[1024] = {'\0'};
  	char attributes_list[2048] = { '\0' };
  	char attributes_list_temp[2048] = { '\0' };

	lastimport_id=-1;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Reconciliation process START\n");
	
	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP1, QUERY8, QUERY4))
		return -1;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 1 [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP2, QUERY8, QUERY4))
		return -2;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 2 [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP3, QUERY8, QUERY4))
		return -3;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 3 [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP4, QUERY8, QUERY4))
		return -4;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 4 [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5, QUERY8, QUERY4)) 
		return -5;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 5 [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5B, QUERY8, QUERY4)) 
		return -5;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 5B [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5C, QUERY8, QUERY4)) 
		return -5;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 5C [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5D, QUERY8, QUERY4)) 
		return -5;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 5D [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5E, QUERY8, QUERY4)) 
		return -5;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 5E [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5F, QUERY8, QUERY4)) 
		return -5;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 5F [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6, QUERY8, QUERY4)) 
		return -6;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 6 [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6B, QUERY8, QUERY4)) 
		return -6;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 6B [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6C, QUERY8, QUERY4)) 
		return -6;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 6C [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6D, QUERY8, QUERY4)) 
		return -6;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 6D [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6E, QUERY8, QUERY4)) 
		return -6;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 6E [OK]\n");

	if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6F, QUERY8, QUERY4)) 
		return -6;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 6F [OK]\n");

	if (ret_code = get_single_value(GET_LAST_IMPORT_ID, &lastimport_id))
		return -7;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 7.1 [OK]\n");
	
	if (lastimport_id<0)
		return -8;
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 7.2 [OK] [LastID=%lld]\n",lastimport_id);
	
	if (!lastimport_id) {
		if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_FINALDW_CREATE, QUERY8, QUERY4)) 
			return -9;
 		else 
			pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 7.3 [OK]\n");
	}

	// OPEN CONNECTION  
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 8: START\n");

        snprintf (conninfo, sizeof (conninfo), CONNECTION_STRING, POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
	conn = PQconnectdb ((const char *) conninfo);

	if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Step 8.1: Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return -10;
        }
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 8.1 [OK]\n");

	// SELECT START 

	res = PQexec(conn,QUERY_DATA_DOWNLOAD_METRICS_GET_RAW_DATA);

	if ((!res) || (PQresultStatus(res) != PGRES_TUPLES_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Step 8.2: SELECT data from dwstep6 FAILED\n");
	        PQclear(res);
		PQfinish(conn);
		return -11;
    	}
 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 8.2 [OK]\n");

	numTuples = PQntuples(res);
	nFields = PQnfields(res);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 8.3: Number of NEW entries from the access_logging table to be processed [%ld] \n",numTuples);

	if (nFields>0)
	{
		unsigned t,w;
		long long int last_al_id;
  		snprintf (attributes_list_temp,sizeof (attributes_list_temp),"%s",PQfname(res, 0));
		if (nFields>1)
			for (w=1 ; w < nFields ; w++)
			{
	  			snprintf (attributes_list,sizeof (attributes_list),"%s,%s",attributes_list_temp, PQfname(res, w));
	  			snprintf (attributes_list_temp,sizeof (attributes_list_temp),"%s",attributes_list);
			}
		//pmesg(LOG_DEBUG,__FILE__,__LINE__,"(%s)\n",attributes_list);

		for(t = 0; t < numTuples; t ++) {
			// reconciliation at the tuple level
  			char values_list_temp[5012] = { '\0' };
  			char values_list[5012] = { '\0' };
  			char insert_query_finaldw[10024] = { '\0' };
  			char update_query_lastalid[1024] = { '\0' };
			int res_query;

			for (w=0 ; w < nFields ; w++)
			{	
				long long int long_int_value;
				if (!strcmp(PQfname(res, w),"year") || !strcmp(PQfname(res, w),"mv") || !strcmp(PQfname(res, w),"success") || !strcmp(PQfname(res, w),"month") || !strcmp(PQfname(res, w),"day") || !strcmp(PQfname(res, w),"hour") || !strcmp(PQfname(res, w),"datasetid") || !strcmp(PQfname(res, w),"file_id") ) 
					{
				  	if (w)
	  					snprintf (values_list,sizeof (values_list),"%s,%ld",values_list_temp, atol(PQgetvalue(res, t, w)));
					else
	  					snprintf (values_list_temp,sizeof (values_list_temp),"%ld",atol(PQgetvalue(res, t, w)));
						//pmesg(LOG_DEBUG,__FILE__,__LINE__,"(%s,integer)\n",PQfname(res, w));
					}	
					else if (!strcmp(PQfname(res, w),"size") || !strcmp(PQfname(res, w),"al_id") || !strcmp(PQfname(res, w),"duration"))
					{
					long_int_value=atoll(PQgetvalue(res, t, w));
					if (w)
	  					snprintf (values_list,sizeof (values_list),"%s,%lld",values_list_temp, long_int_value);
					else
						snprintf (values_list_temp,sizeof (values_list_temp),"%lld",long_int_value);
					if (!strcmp(PQfname(res, w),"al_id"))
						last_al_id = long_int_value; 
					
					//pmesg(LOG_DEBUG,__FILE__,__LINE__,"(%s,bigint)\n",PQfname(res, w));
					}
					else 
					{
					 if (w)
	  				 	snprintf (values_list,sizeof (values_list),"%s,'%s'",values_list_temp, PQgetvalue(res, t, w));
					 else
						snprintf (values_list_temp,sizeof (values_list_temp),"'%s'",PQgetvalue(res, t, w));
					 //pmesg(LOG_DEBUG,__FILE__,__LINE__,"(%s,character varying)\n",PQfname(res, w));
					}

				if (w)
	  				snprintf (values_list_temp,sizeof (values_list_temp),"%s",values_list);

			}	
			//pmesg(LOG_DEBUG,__FILE__,__LINE__,"Last al_id = %lld\n",last_al_id);
			//pmesg(LOG_DEBUG,__FILE__,__LINE__,"(%s)\n",values_list);
			snprintf (insert_query_finaldw, sizeof (insert_query_finaldw),QUERY_INSERT_DATA_DOWNLOAD_METRICS_FINALDW,attributes_list,values_list_temp,ESGF_HOSTNAME);
			snprintf (update_query_lastalid, sizeof (update_query_lastalid),QUERY_UPDATE_LAST_PROCESSED_AL_ID,last_al_id);
			if (insert_data_into_finaldw(conn,insert_query_finaldw,update_query_lastalid,START_TRANSACTION_FINALDW_INGESTION,END_TRANSACTION_FINALDW_INGESTION))
			{
			pmesg(LOG_ERROR,__FILE__,__LINE__,"Step 8.4: ERROR Processing entry [%ld/%ld][EntryID=%lld] \n",t+1,numTuples,last_al_id);
			//pmesg(LOG_ERROR,__FILE__,__LINE__,"Step 8.4: STOP Processing entries\n");
			// exit from the ingestion process (now disabled)
			//w=nFields;
			//t=numTuples;
			}
			else
				pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 8.4: Processing entry OK [%ld/%ld][EntryID=%lld] \n",t+1,numTuples,last_al_id);

		}
	}

	//pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 8.4: Processing entry [100%] \n");
	PQclear(res);
	// SELECT END

	// to do: creating the CMIP5 data warehouse with VARIABLE as a new dimension

	//if (transaction_based_query(QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6, QUERY8, QUERY4)) 
	//	return -6;
 	//pmesg(LOG_DEBUG,__FILE__,__LINE__,"Step 6 [OK]\n");

	// CLOSE CONNECTION  
    	PQfinish(conn);

 	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Reconciliation process END\n");
 	return 0;
}*/


int insert_data_into_finaldw(PGconn *conn, char *submitted_query, char* update_last_al_id_query ,char* open_transaction, char* stop_transaction) 
{
 // PGconn *conn;
  PGresult *res;

  //char conninfo[1024] = {'\0'};

  /* Connect to database */
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction based query - START\n");

  //snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
  //conn = PQconnectdb ((const char *) conninfo);

  /*if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return -1;
        }*/

  // start transaction
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying open transaction: [%s]\n",open_transaction);
  /*res = PQexec(conn, open_transaction);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
	        PQclear(res);
		//PQfinish(conn);
		return -2;
    	}
  PQclear(res);*/

  // Query submission ingestion entry in finaldw 
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query submission\n");
  res = PQexec(conn, submitted_query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Query insert entry in localdw failed\n");
	        PQclear(res);
		//PQfinish(conn);
		return -4;
    	}
  PQclear(res);

  // Query update last_al_id 
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query submission\n");
  res = PQexec(conn, update_last_al_id_query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Query update last processed id failed\n");
	        PQclear(res);
		//PQfinish(conn);
		return -4;
    	}
  PQclear(res);

  // close transaction
  /*res = PQexec(conn, stop_transaction);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction: [%s]\n",stop_transaction);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed\n");
	        PQclear(res);
		//PQfinish(conn);
		return -3;
    	}
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction ended successfully!\n");
  
  PQclear(res);
   */
  //PQfinish(conn);

  return 0;
}

int remove_stats_from_federation_level_dw(PGconn *conn, char *submitted_query, char* update_timestamp_query ,char* open_transaction, char* stop_transaction) 
{
  PGresult *res;

  char conninfo[1024] = {'\0'};

  res = PQexec(conn, open_transaction);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
	        PQclear(res);
		return -2;
    	}
  PQclear(res);

  res = PQexec(conn, submitted_query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Query DELETE stats from federation-level dw failed\n");
	        PQclear(res);
		return -4;
    	}
  PQclear(res);

  res = PQexec(conn, update_timestamp_query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Query update timestamp failed\n");
	        PQclear(res);
		return -4;
    	}
  PQclear(res);

  res = PQexec(conn, stop_transaction);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    	{
	        pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed\n");
	        PQclear(res);
		return -3;
    	}
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction ended successfully!\n");

  PQclear(res);

  return 0;
}

int realtime_processes_get_stats(void)
{
 char top_command[1024] = { '\0' };
 char top_file_command[1024] = { '\0' };
 FILE *file;

 snprintf (top_file_command,sizeof (top_file_command),"%s/%s",DASHBOARD_SERVICE_PATH,TOP_FILE);
 snprintf (top_command,sizeof (top_command),"%s > %s",TOP_COMMAND,top_file_command);
 system(top_command);
 file = fopen (top_file_command, "r");
 if (file == NULL)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Open top file failed\n");
    return -1;
  }
 // to be added the parsing of the .top.txt file

 return 0;
}


// realtime cpu monitoring

int realtime_cpu_get_stats(void)
{
  char esgf_nodetype_filename[256] = { '\0' };
  char loadavg1[256] = { '\0' };
  char loadavg5[256] = { '\0' };
  char loadavg15[256] = { '\0' };
  int counter;

  sprintf (esgf_nodetype_filename, "/proc/loadavg");
  FILE *file = fopen (esgf_nodetype_filename, "r");

  if (file == NULL)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Open /proc/loadavg failed\n");
    return -1;
  }

  if ((fscanf (file, "%s", loadavg1)) == EOF)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Read /proc/loadavg loadavg1 failed\n");
    return -1;
  }
  if ((fscanf (file, "%s", loadavg5)) == EOF)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Read /proc/loadavg loadavg5 failed\n");
    return -1;
  }
  if ((fscanf (file, "%s", loadavg15)) == EOF)
  {
    pmesg(LOG_ERROR,__FILE__,__LINE__,"Read /proc/loadavg loadavg15 failed\n");
    return -1;
  }

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Realtime Cpu - load average metrics [%s] [%s] [%s]\n", loadavg1, loadavg5,loadavg15);

  fclose (file);
  rotate_realtime_stats(REALTIME_CPU_1M, REALTIME_CPU_1M_TEMP, loadavg1);
  rotate_realtime_stats(REALTIME_CPU_5M, REALTIME_CPU_5M_TEMP, loadavg5);
  rotate_realtime_stats(REALTIME_CPU_15M, REALTIME_CPU_15M_TEMP, loadavg15);

 return 0;
}

int rotate_realtime_stats(char* filestats, char* filestats_tmp, char* metric)
 {
  FILE *fromFile;
  time_t current_time;
  char metrics_filename[1024] = { '\0' };
  char metrics_filename_tmp[1024] = { '\0' };
  FILE *toFile;
  char* c2_time_string;
  char last_value[256] = { '\0' };
  char c2_copy[256] = { '\0' };
  char str[256] = { '\0' };
  int i;

  snprintf (metrics_filename_tmp,sizeof (metrics_filename_tmp),"%s/%s",DASHBOARD_SERVICE_PATH,filestats_tmp);
  open_create_file(&toFile,metrics_filename_tmp, "w+"); //overwrite file or create it if it does not exists.
  time(&(current_time));
  c2_time_string = ctime(&(current_time));
  snprintf(c2_copy,sizeof(c2_copy),"%s",c2_time_string);
  c2_time_string[strlen(c2_time_string)-1]='\0';
  c2_copy[strlen(c2_copy)-6]='\0';
  //printf("Time is %s [%s]\n", c2_time_string,(c2_copy+11) );

  // writing the new stats on top
   //snprintf(last_value,sizeof(last_value),"%s&%s\n",metric,c2_time_string);
   snprintf(last_value,sizeof(last_value),"%s&%s\n",metric,(c2_copy+11));
   if (fputs(last_value,toFile)==EOF)
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Error writing the file\n");

  // now copy the remaining ones from the prod file
  snprintf (metrics_filename,sizeof (metrics_filename),"%s/%s",DASHBOARD_SERVICE_PATH,filestats);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Setting proper path for realtime monitoring cpu file [%s]!\n",metrics_filename);
  open_create_file(&fromFile , metrics_filename,"r"); // r  - open for reading 
  //snprintf(last_value, sizeof(last_value),metric);

  // iterate on prod file and temp file
  i=1;
  while (i<REALTIME_SAMPLES)
      {
        if (fgets(str,256,fromFile)!=NULL)
                {
                //fprintf(stdout,"Copying %s\n",str); //fputs(str,toFile);
                snprintf(last_value, sizeof(last_value),"%s",str);
                fputs(last_value,toFile);
                }
        else
                {
                //fprintf(stdout,"Padding with %s",last_value); //fputs(metric,toFile);
                fputs(last_value,toFile);
                }
        i++;
      }
  fclose(fromFile);
  fclose(toFile);
  rename(metrics_filename_tmp,metrics_filename);
  // display new file content
  //open_create_file(&fromFile , metrics_filename,"r"); // r  - open for reading 
  //while (fgets(str,256,fromFile)!=NULL)
  //      {
  //      str[strlen(str)-1]='\0';
  //      fprintf(stdout,"[%s] ",str);
  //     }
  //fprintf(stdout,"\n");
  //fclose(fromFile);

  return 0;
}

int open_create_file(FILE** file , char* filename, char* mode)
{
    *file = fopen(filename,mode);
    if ((*file) == NULL)
        {
         pmesg(LOG_DEBUG,__FILE__,__LINE__,"Can't open binary file [%s] in [%s] mode\n",filename,mode);
         return -1;
        }
    return 0;
}

int close_file(FILE* file)
{
    fclose( file );
    return 0;
}


int realtime_mem_get_stats(void)
{
 // retrieving memory (swap/ram) metrics

  char esgf_nodetype_filename[256] = { '\0' };
  char query_memory_metric_insert[2048] = { '\0' };
  char freeram[256] = { '\0' };
  char totram[256] = { '\0' };
  char freeswap[256] = { '\0' };
  char totswap[256] = { '\0' };
  char swapstr[256] = { '\0' };
  char ramstr[256] = { '\0' };
  long int fram,tram,fswap,tswap,uram,uswap;
  int ret_code;
  int i=0;
  int j=0;
  char line [ 1024 ]; /* or other suitable maximum line size */

  sprintf (esgf_nodetype_filename, "/proc/meminfo");
  FILE *file = fopen (esgf_nodetype_filename, "r");

  if (file == NULL)		
    return -1;

 while ( fgets ( line, sizeof line, file ) != NULL ) 
       {
        char value_buffer[256] = { '\0' };
        char *position;
        position = strchr (line, ':');
        if (position != NULL)     // the ':' has been found
        {
          	strcpy (value_buffer, position + 1);  // now value_buffer stores the VALUE
          	*position = '\0';     // now buffer stores the ATTRIBUTE
	}

  	char * pch= NULL;
  	pch = strtok (value_buffer," ");
	if (!strcmp(line,"MemTotal"))	
           {
    		snprintf(totram, sizeof(totram),"%s",pch);
		tram=atol(totram);
		//fprintf(stdout,"valori memoria [%s] [%s] [%s]\n",line, value_buffer,pch);
	   }
	if (!strcmp(line,"MemFree"))	
           {
    		snprintf(freeram, sizeof(freeram),"%s",pch);
		fram=atol(freeram);
		//fprintf(stdout,"valori memoria [%s] [%s] [%s]\n",line, value_buffer,pch);
	   }
	if (!strcmp(line,"SwapTotal"))	
           {
    		snprintf(totswap, sizeof(totswap),"%s",pch);
		tswap=atol(totswap);
		//fprintf(stdout,"valori memoria [%s] [%s] [%s]\n",line, value_buffer,pch);
	   }
	if (!strcmp(line,"SwapFree"))	
           {
 		snprintf(freeswap, sizeof(freeswap),"%s",pch);
		fswap=atol(freeswap);
		//fprintf(stdout,"valori memoria [%s] [%s] [%s]\n",line, value_buffer,pch);
	   }
       }

  fclose ( file );

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics RAM [%s] [%s] SWAP [%s] [%s]\n", totram,freeram,totswap, freeswap);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics RAM [%ld] [%ld] SWAP [%ld] [%ld]\n", tram/1024,fram/1024,tswap/1024,fswap/1024);
  //snprintf(ramstr,sizeof(ramstr),"%s$%s",freeram,totram);
  //snprintf(swapstr,sizeof(swapstr),"%s$%s",freeswap,totswap);
  snprintf(ramstr,sizeof(ramstr),"%ld$%ld",fram/1024,tram/1024);
  snprintf(swapstr,sizeof(swapstr),"%ld$%ld",fswap/1024,tswap/1024);
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Memory metrics strings: RAM [%s] SWAP [%s] \n", ramstr,swapstr);

  rotate_realtime_stats(REALTIME_MEM_RAM, REALTIME_MEM_RAM_TEMP, ramstr);
  rotate_realtime_stats(REALTIME_MEM_SWAP, REALTIME_MEM_SWAP_TEMP, swapstr);
  return 0;
}

