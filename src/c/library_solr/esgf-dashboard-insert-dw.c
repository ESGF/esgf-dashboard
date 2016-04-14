#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "hashtbl.h"
#include "ftpget.h"
#include <libpq-fe.h>
#include <time.h>
#include "GeoIP.h"
#include "GeoIPCity.h"
#include "config.h"

// QUERY_INSERT_CROSS_DIM_DATE adds a new date of the downloaded file in the database
#define QUERY_INSERT_CROSS_DIM_DATE  "INSERT into esgf_dashboard.cross_dim_date(download_date,month,year) values('%s',%d,%d);"
// QUERY_INSERT_CROSS_DIM_GEOLOCATION adds a new geolocation of the downloaded file in the database
#define QUERY_INSERT_CROSS_DIM_GEOLOCATION "INSERT into esgf_dashboard.cross_dim_geolocation(latitude,longitude,country,continent) values(%14.11f,%14.11f,'%s','%s');"
// QUERY_INSERT_CROSS_DIM_PROJECT adds a new project related to the downloaded file in the database
#define QUERY_INSERT_CROSS_DIM_PROJECT "INSERT into esgf_dashboard.cross_dim_project(project_name) values('%s');"
// QUERY_INSERT_CROSS_BRIDGE_PROJECT adds a new project related to the downloaded file in the database
#define QUERY_INSERT_CROSS_BRIDGE_PROJECT "INSERT into esgf_dashboard.cross_bridge_project(project_key,project_group_key) values(%d,%d);"
// QUERY_INSERT_CROSS_FACT_DOWNLOAD adds a new fact related to the downloaded file in the database
#define QUERY_INSERT_CROSS_FACT_DOWNLOAD "INSERT into esgf_dashboard.cross_fact_download(size,success,duration,replica,user_id_hash,host_name, user_idp,hour, minute, project_group_key, geolocation_key,date_key) values(%.0f,'%s',%d,'%s','%s','%s','%s','%s','%s',%d,%d,%d);"
// Get list of CROSS DIM DATE
#define QUERY_GET_LIST_OF_CROSS_DIM_DATE "SELECT download_date,date_key from esgf_dashboard.cross_dim_date;"
// Get list of CROSS DIM GEOLOCATION 
#define QUERY_GET_LIST_OF_CROSS_DIM_GEOLOCATION "SELECT (CAST(latitude as numeric(14,11)))||':'||(CAST(longitude as numeric(14,11))) AS name,geolocation_key from esgf_dashboard.cross_dim_geolocation;"
// Get list of CROSS DIM PROJECT 
#define QUERY_GET_LIST_OF_CROSS_DIM_PROJECT "SELECT project_name,project_key from esgf_dashboard.cross_dim_project;"
// Get list of CROSS BRIDGE PROJECT 
#define QUERY_GET_LIST_OF_CROSS_BRIDGE_PROJECT "SELECT project_group_key, project_key from esgf_dashboard.cross_bridge_project;"
// Get list of CROSS BRIDGE PROJECT 
#define QUERY_GET_LIST_OF_CROSS_BRIDGE_PROJECT_TMP "SELECT array_to_string(array(SELECT s.project_key FROM esgf_dashboard.cross_bridge_project s where s.project_group_key=b.project_group_key order by s.project_key ASC), ',') AS project_key,b.project_group_key FROM esgf_dashboard.cross_bridge_project b GROUP BY project_group_key order BY b.project_group_key ASC;"
// Get list of CROSS DOWNLOAD 
#define QUERY_GET_LIST_OF_CROSS_DOWNLOAD "SELECT (CAST(size as bigint)||':'||CAST(duration as integer)||':'||CAST(replica as integer)||':'||user_id_hash||':'||host_name||':'||user_idp||':'||CAST(hour as integer)||':'||CAST(minute as integer)||':'||CAST(project_group_key as integer)||':'||CAST(geolocation_key as bigint)||':'||CAST(date_key as integer)) AS name, download_key from esgf_dashboard.cross_fact_download;"
#define QUERY_SELECT_DASHBOARD_QUEUE "SELECT timestamp,remote_addr,size,success, duration, user_id_hash, user_idp from esgf_dashboard.dashboard_queue where id=%d;"

// Insert new date adds a new date in the database
#define QUERY_INSERT_NEW_DATE "INSERT into esgf_dashboard.cross_dim_date(download_date,month,year) values('%s','%s','%s');"
// Retrieve the id value of a specific date
#define QUERY_GET_DATE_ID "SELECT date_key from esgf_dashboard.cross_dim_date where download_date='%s';"

//Select the continent starting from country_code of the remote_ip_address 
#define QUERY_SELECT_GEOLOCATION_INFO "SELECT continent_name from esgf_dashboard.continent, esgf_dashboard.country where esgf_dashboard.country.continent_code=esgf_dashboard.continent.continent_code and esgf_dashboard.country.country_code='%s';" 
//Select the geolocation_id starting from latitude and longitude
#define QUERY_GET_GEO_ID "SELECT geolocation_key from esgf_dashboard.cross_dim_geolocation where latitude=%f and longitude=%f;"
//Select the project_id starting from its name 
#define QUERY_GET_PROJ_ID "SELECT project_key from esgf_dashboard.cross_dim_project where project_name='%s';"
//Select the max group id from cross_bridge_project 
//#define QUERY_GET_MAX_GROUP_ID "SELECT COALESCE(max(project_group_key),0) from cross_bridge_project;"
#define QUERY_GET_MAX_GROUP_ID "SELECT MAX(project_group_key) AS max_bridge_proj from esgf_dashboard.cross_bridge_project;"
//Retrieve the id of fact download 
#define QUERY_GET_CROSS_FACT_DOWNLOAD "SELECT download_key from esgf_dashboard.cross_fact_download where size=%.0f and success='%s' and duration=%d and replica='%s' and user_id_hash='%s' and host_name='%s' and user_idp='%s' and hour='%s' and minute='%s' and project_group_key=%d and geolocation_key=%d and date_key=%d;"
//insert into cross_dmart_project_host
//#define QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST "INSERT INTO esgf_dashboard.cross_dmart_project_host(total_size, number_of_downloads, number_of_successful_downloads, number_of_replica_downloads, average_duration, number_of_files, number_of_users, host_name, project_name, date_key) SELECT sum(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(*) as number_of_successful_downloads, COUNT(*) as number_of_replica_downloads, ROUND(avg(cast(duration as integer)),0) as average_duration,COUNT(*) AS number_of_files, COUNT(distinct user_id_hash) AS number_of_users, host_name, project_name, esgf_dashboard.cross_fact_download.date_key from esgf_dashboard.cross_fact_download, esgf_dashboard.cross_bridge_project, esgf_dashboard.cross_dim_project, esgf_dashboard.cross_dim_date where success='true' and replica='false' and cross_fact_download.project_group_key=cross_bridge_project.project_group_key and esgf_dashboard.cross_bridge_project.project_key=esgf_dashboard.cross_dim_project.project_key and cross_fact_download.date_key=esgf_dashboard.cross_dim_date.date_key group by project_name, esgf_dashboard.cross_fact_download.date_key, host_name;" 

#define QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST "INSERT INTO esgf_dashboard.cross_dmart_project_host(total_size, number_of_downloads, number_of_successful_downloads, number_of_replica_downloads, average_duration, number_of_files, number_of_users, host_name, project_name, date_key) SELECT (select sum(size) from esgf_dashboard.cross_fact_download where size<>-1)AS total_size, COUNT(*) AS number_of_downloads, (select count(*)  from  esgf_dashboard.cross_fact_download where success) AS number_of_successful_downloads, (select count(*)  from  esgf_dashboard.cross_fact_download where replica) AS number_of_replica_downloads, (select ROUND(avg(cast(duration as integer)),0) from esgf_dashboard.cross_fact_download where success) AS average_duration, 0,(select COUNT(distinct user_id_hash) from  esgf_dashboard.cross_fact_download) AS number_of_users, host_name, project_name, esgf_dashboard.cross_fact_download.date_key FROM esgf_dashboard.cross_fact_download, esgf_dashboard.cross_bridge_project, esgf_dashboard.cross_dim_project, esgf_dashboard.cross_dim_date where cross_fact_download.project_group_key=cross_bridge_project.project_group_key and esgf_dashboard.cross_bridge_project.project_key=esgf_dashboard.cross_dim_project.project_key and cross_fact_download.date_key=esgf_dashboard.cross_dim_date.date_key group by project_name, esgf_dashboard.cross_fact_download.date_key, host_name;"

#define QUERY_UPDATE_DASHBOARD_QUEUE "update esgf_dashboard.dashboard_queue set processed=1 where id=%d;"

// HashTables dimensions
#define HAST_TABLE_CROSS_DIM_DATE		64
#define HAST_TABLE_CROSS_DIM_GEOLOCATION	64		
#define HAST_TABLE_CROSS_DIM_PROJECT		16
#define HAST_TABLE_CROSS_BRIDGE_PROJECT		64	
#define HAST_TABLE_CROSS_BRIDGE_PROJECT_TMP	64	
#define HAST_TABLE_CROSS_DOWNLOAD		64


#if 0
#define POSTGRES_HOST "localhost"
#define POSTGRES_PORT_NUMBER 5432
#define POSTGRES_DB_NAME "esgf2"
#define POSTGRES_USER "postgres"
#define POSTGRES_PASSWD ""
#endif 
#define POSTGRES_HOST "esgf-test.cmcc.it"
#define POSTGRES_PORT_NUMBER 5432
#define POSTGRES_DB_NAME "esgcet2"
#define POSTGRES_USER "dbsuper"
#define POSTGRES_PASSWD "abcdef"

// Start transaction and lock tables 
#define QUERY_OPEN_TRANSACTION  "start transaction; lock esgf_dashboard.cross_dim_date ; lock esgf_dashboard.cross_dim_geolocation ; lock esgf_dashboard.cross_dim_project ; lock esgf_dashboard.cross_bridge_project ; lock esgf_dashboard.cross_fact_download ; "

// End transaction and release locks 
#define QUERY_CLOSE_TRANSACTION  "end transaction;" 

long
int get_foreign_key_value (PGconn * conn, char *query);

int check_cross_project (struct dataset_project ***datasetproj, char* esgf_node)
{

  PGconn *conn;
  PGresult *res;
  char conninfo[1024] = { '\0' };
  char conninfo_printf[1024] = { '\0' };
  char open_transaction[2048] = { '\0' };
  char close_transaction[2048] = { '\0' };
  int create_populate_done = 0;
  char select_query[2048] = { '\0' };
  char select_id_date_query[2048] = { '\0' };
  static long long int success_lookup[2] = { 0 };	// [0] success [1] missing
  int project_group_key=0;
  long int geo_id = 0;

  HASHTBL *hashtbl_cross_dim_date;
  HASHTBL *hashtbl_cross_dim_geolocation;
  HASHTBL *hashtbl_cross_dim_project;
  HASHTBL *hashtbl_cross_bridge_project;
  HASHTBL *hashtbl_cross_bridge_project_tmp;
  HASHTBL *hashtbl_cross_fact_download;

  snprintf (conninfo, sizeof (conninfo),
	    "dbname=%s user=%s password=abcdef",POSTGRES_DB_NAME, POSTGRES_USER);

  snprintf (conninfo_printf, sizeof (conninfo_printf),
	    "host=%s port=%d dbname=%s user=%s password=******", POSTGRES_HOST,
	    POSTGRES_PORT_NUMBER, POSTGRES_DB_NAME, POSTGRES_USER);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"*********** Starting parsing routine  ************\n");
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Open connection to: %s\n", conninfo_printf);

  conn = PQconnectdb ((const char *) conninfo);
  if (PQstatus (conn) != CONNECTION_OK)
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s",
      	       PQerrorMessage (conn));
      PQfinish (conn);
      return -1;
    }


    int i,cnt=0;
    int size2=0;
    PGresult   *res1;
    int timestamp;
    char *remote_addr=NULL;
    float size_row;
    int duration_row=0;
    char *user_id_hash_row=NULL;
    char *user_idp_row=NULL;
    char *success_row=NULL;
    for(cnt=0; (*datasetproj)[cnt]!=NULL; cnt++)
    {
                char* replica=NULL;
                snprintf (select_query, sizeof (select_query), QUERY_SELECT_DASHBOARD_QUEUE, (*datasetproj)[cnt]->id_query);
                submit_query_res (conn, select_query,&res1);
                for (i = 0; i < PQntuples(res1); i++)
                {
                   printf("timestamp %ld\n", atoi(PQgetvalue(res1, i, 0)));
                   timestamp=atoi(PQgetvalue(res1, i, 0));
                   //printf("timestamp %s\n", PQgetvalue(res1, i, 0));
                   printf("remote_addr %s\n", PQgetvalue(res1, i, 1));
                   remote_addr=strdup(PQgetvalue(res1, i, 1));

                   printf("size %s\n", PQgetvalue(res1, i, 2));
                   printf("size %.0f\n", atof(PQgetvalue(res1, i, 2)));
                   size_row= atof(PQgetvalue(res1, i, 2));
                   //printf("size %s\n", PQgetvalue(res1, i, 2));
                   printf("success %s\n", PQgetvalue(res1, i, 3));
                   success_row=strdup(PQgetvalue(res1, i, 3));

                   printf("duration %ld\n", atoi(PQgetvalue(res1, i, 4)));
                   duration_row=atoi(PQgetvalue(res1, i, 4));
                   //printf("duration %s\n", PQgetvalue(res1, i, 4));
                   printf("user_id_hash %s\n", PQgetvalue(res1, i, 5));
                   user_id_hash_row=strdup(PQgetvalue(res1, i, 5));
                 
                   printf("user_idp %s\n", PQgetvalue(res1, i, 6));
                   user_idp_row=strdup(PQgetvalue(res1, i, 6));
                }
                PQclear (res1);
        int size2, size3, size4=0;
        for(size2=0; (*datasetproj)[cnt]->first[size2]!=NULL; size2++)
        {
          for(size3=0; (*datasetproj)[cnt]->first[size2]->first[size3]!=NULL; size3++)
          {
              if((*datasetproj)[cnt]->first[size2]->first[size3]!=NULL)
              {
                if((*datasetproj)[cnt]->first[size2]->first[size3]->name)
                {
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "replica")==0)
                  {
                  //printf("***numero di metadati vale %d\n", datasetproj[cnt]->first[size2]->first[size3]->size);
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      replica=(*datasetproj)[cnt]->first[size2]->first[size3]->value[size4];
                      printf("replica vale %s\n", replica);
                  }
                }
              }
         }
       }

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"OPEN Transaction and locking the database tables the parser needs...\n");
  /*if (manage_database_open_close_transaction(conn,QUERY_OPEN_TRANSACTION)) {
	  //pmesg(LOG_ERROR,__FILE__,__LINE__,"Transaction FAILED - Database Tables Lock: Failed. [Recovery action: Skip parsing]\n");
      	  PQfinish (conn);
	  return -1;
	 }*/
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction OK - Database Tables Lock: Ok\n");

// create and populate hashtables if not yet
	  if (!create_populate_done)
	    {
	     pmesg(LOG_DEBUG,__FILE__,__LINE__,"Create and populate Hash tables (first iteration)\n");
	      create_populate_done = 1;
	      // Hash tables creation
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CROSS DIM DATE \n");
	      if (!( hashtbl_cross_dim_date = hashtbl_create (HAST_TABLE_CROSS_DIM_DATE, NULL)))
		{
		 pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for CROSS DIM DATE [skip parsing]\n");
		  //continue;
		}

	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CROSS DIM GEOLOCATION\n");
	      if (!(hashtbl_cross_dim_geolocation = hashtbl_create (HAST_TABLE_CROSS_DIM_GEOLOCATION, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for CROSS DIM GEOLOCATION [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CROSS DIM PROJECT\n");
	      if (!(hashtbl_cross_dim_project = hashtbl_create (HAST_TABLE_CROSS_DIM_PROJECT, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for CROSS DIM PROJECT [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  //continue;
		}

	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CROSS BRIDGE PROJECT\n");
	      if (!(hashtbl_cross_bridge_project = hashtbl_create (HAST_TABLE_CROSS_BRIDGE_PROJECT, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable TMP for CROSS BRIDGE PROJECT\n");
	      if (!(hashtbl_cross_bridge_project_tmp = hashtbl_create (HAST_TABLE_CROSS_BRIDGE_PROJECT_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
		  //continue;
		}

	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CROSS DOWNLOAD\n");
	      if (!(hashtbl_cross_fact_download = hashtbl_create (HAST_TABLE_CROSS_DOWNLOAD, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  //continue;
		}
	      populate_hash_table (conn, QUERY_GET_LIST_OF_CROSS_DIM_DATE,
				   &hashtbl_cross_dim_date);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_CROSS_DIM_GEOLOCATION,
				   &hashtbl_cross_dim_geolocation);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_CROSS_DIM_PROJECT,
				   &hashtbl_cross_dim_project);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_CROSS_BRIDGE_PROJECT,
				   &hashtbl_cross_bridge_project);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_CROSS_BRIDGE_PROJECT_TMP,
				   &hashtbl_cross_bridge_project_tmp);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_CROSS_DOWNLOAD,
				   &hashtbl_cross_fact_download);
	    }
	  else
	    pmesg(LOG_DEBUG,__FILE__,__LINE__, "Hash tables already in place with the data [%d]\n",create_populate_done);

          time_t realtime;
          realtime = (time_t)timestamp;
          //time(&realtime);
          struct tm * ptm = gmtime(&realtime);
          printf ("The UTC time is: %s", asctime(ptm));
          char buf[30];
          char buf1[30];
          char buf2[30];
          char buf3[30];
          char buf4[30];
         
          strftime (buf, 30, "%Y-%m-%d",  ptm);
          strftime (buf1, 30, "%m",  ptm);
          strftime (buf2, 30, "%Y",  ptm);
          strftime (buf3, 30, "%H",  ptm);
          strftime (buf4, 30, "%M",  ptm);

          printf("buf vale %s, buf1=%s, buf2=%s buf3=%s buf4=%s\n", buf, buf1, buf2, buf3, buf4);

          long int date_id = 0;
          char *hashtbl_result;

          if (hashtbl_result = hashtbl_get (hashtbl_cross_dim_date, buf))
          {
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",buf, hashtbl_result);
             date_id = atol (hashtbl_result);
	     success_lookup[0]++;
          }
          else
	  {		// add host entry in DB (and hashtable too) without geolocation information

             char date_id_str[128] = { '\0' };
             char insert_new_date_query[2048] = { '\0' };

             success_lookup[1]++;
             snprintf (insert_new_date_query, sizeof (insert_new_date_query), QUERY_INSERT_NEW_DATE, buf, buf1, buf2);
	     submit_query (conn, insert_new_date_query);

	     snprintf(select_id_date_query, sizeof (select_id_date_query), QUERY_GET_DATE_ID, buf);
             date_id=get_foreign_key_value(conn, select_id_date_query);
	     // add entry to hash table
	     sprintf (date_id_str, "%ld", date_id);
             hashtbl_insert (hashtbl_cross_dim_date, buf, date_id_str);
	     pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",date_id, date_id_str);
          }
          if(remote_addr)
          {
             int code=0;
             struct geo_output_struct geo_output;
             char select_geo_query[2048] = { '\0' };

             pmesg(LOG_DEBUG,__FILE__,__LINE__,"GeoLocation pieces of info need to be taken from GeoIP library (estimation)\n");
             code=esgf_geolookup (remote_addr, &geo_output);
             //if (!esgf_geolookup (remote_addr, &geo_output))
	     //{
                 PGresult *res_geo;
		 //char select_geo_query[2048] = { '\0' };
	         char insert_geo_query[2048] = { '\0' };
                 char key_geo[2048] = { '\0' };
                 float lat=40.33495040000;
                 float lon=18.11679550000;
              if(code==0)
              {
		 pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_COUNTRY_CODE=%s]\n",geo_output.country_code);
		 pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_LATITUDE=%f]\n",geo_output.latitude);
	         pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_LONGITUDE=%f]\n",geo_output.longitude);
                 sprintf(key_geo, "%14.11f:%14.11f", geo_output.latitude,geo_output.longitude);
                 lat=geo_output.latitude;
                 lon=geo_output.longitude;
              }
              else
                 sprintf(key_geo, "%14.11f:%14.11f",lat,lon);
                
                 if (hashtbl_result = hashtbl_get (hashtbl_cross_dim_geolocation, key_geo))
                 {
                     pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",key_geo, hashtbl_result);
                     geo_id = atol (hashtbl_result);
                     success_lookup[0]++;
                 }
                 else
                 {             // add host entry in DB (and hashtable too) without geolocation information
                     char geo_id_str[128] = { '\0' };
                     char select_id_geo_query[2048] = { '\0' };

                     success_lookup[1]++;
                     char *country_name=NULL;
                     char *country_code=NULL;
                     if(code==0)
                         country_code=strdup(geo_output.country_code);
                     else
                         country_code=strdup("IT");
		     snprintf (select_geo_query, sizeof (select_geo_query),QUERY_SELECT_GEOLOCATION_INFO,country_code);
	             pmesg(LOG_DEBUG,__FILE__,__LINE__,"I got the geolocation info from the DB (estimation)\n");
		     pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s\n",select_geo_query);
		     submit_query_res (conn, select_geo_query, &res_geo);
                     country_name=PQgetvalue(res_geo, 0, 0);
                     PQclear (res_geo);
                     printf("country_name %ls\n", country_name);
                     if(country_name)
		       snprintf (insert_geo_query, sizeof (insert_geo_query),QUERY_INSERT_CROSS_DIM_GEOLOCATION, lat, lon, country_code, country_name);
                     submit_query (conn, insert_geo_query);
                     free(country_code);
	             snprintf(select_id_geo_query, sizeof (select_id_geo_query), QUERY_GET_GEO_ID, lat, lon);
                     geo_id=get_foreign_key_value(conn, select_id_geo_query);
	             // add entry to hash table
	             sprintf (geo_id_str, "%ld", geo_id);
                     hashtbl_insert (hashtbl_cross_dim_geolocation, key_geo, geo_id_str);
	             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_geo, geo_id_str);
                  }
	      //}
	      //else
		//pmesg(LOG_ERROR,__FILE__,__LINE__,"Esgf-lookup error\n");

              free(remote_addr);
              
          }


         int miss_proj=0;
         int size_idproj=(*datasetproj)[cnt]->size;
         int *array_idproj=calloc(size_idproj, sizeof(int));
         for(size2=0; (*datasetproj)[cnt]->first[size2]!=NULL; size2++)
         {  
            int proj_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_cross_dim_project, (*datasetproj)[cnt]->first[size2]->project))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",(*datasetproj)[cnt]->first[size2]->project, hashtbl_result);
               proj_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_idproj[size2]=proj_id;
            }
            else
            {
               miss_proj=1;
               char proj_id_str[128] = { '\0' };
               char select_id_proj_query[2048] = { '\0' };
               char insert_proj_query[2048] = { '\0' };
               char select_proj_query[2048] = { '\0' };
               success_lookup[1]++;
	       snprintf (insert_proj_query, sizeof (insert_proj_query),QUERY_INSERT_CROSS_DIM_PROJECT,(*datasetproj)[cnt]->first[size2]->project);
               submit_query (conn, insert_proj_query);
	       snprintf(select_id_proj_query, sizeof (select_id_proj_query), QUERY_GET_PROJ_ID,(*datasetproj)[cnt]->first[size2]->project);
               proj_id=get_foreign_key_value(conn, select_id_proj_query);
               array_idproj[size2]=proj_id;
	       // add entry to hash table
	       sprintf (proj_id_str, "%ld", proj_id);
               hashtbl_insert (hashtbl_cross_dim_project,(*datasetproj)[cnt]->first[size2]->project,proj_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",(*datasetproj)[cnt]->first[size2]->project, proj_id_str);
            }  
         }


         quickSort(array_idproj,0,size_idproj-1); 
         char proj_key[2048] = { '\0' };
         char *ap = proj_key;
         for(i=0; i<(*datasetproj)[cnt]->size;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==(*datasetproj)[cnt]->size-1)
                   ap += sprintf(ap, "%d", array_idproj[i]);
                else
                   ap += sprintf(ap, ",%d,", array_idproj[i]);
             
         }
         printf("proj_key %s\n", proj_key);
         if(miss_proj==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_proj_query[2048] = { '\0' };
            char array_idproj_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            resp=submit_query_res (conn, QUERY_GET_MAX_GROUP_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<(*datasetproj)[cnt]->size;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                snprintf (insert_group_proj_query, sizeof (insert_group_proj_query),QUERY_INSERT_CROSS_BRIDGE_PROJECT,array_idproj[i],max_group_id+1);
                submit_query (conn, insert_group_proj_query);

                sprintf(array_idproj_str, "%d", array_idproj[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_cross_bridge_project,array_idproj_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_idproj_str,max_group_id_str);
            } 
            project_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_cross_bridge_project_tmp,max_group_id_str,proj_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,proj_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_cross_bridge_project_tmp, proj_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",proj_key, hashtbl_result);
               project_group_key = atol(hashtbl_result);
               printf("project_group_key %d\n", project_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_proj_query[2048] = { '\0' };
                resp=submit_query_res (conn, QUERY_GET_MAX_GROUP_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                project_group_key= max_group_id+1;
                  char array_idproj_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<(*datasetproj)[cnt]->size;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  snprintf (insert_group_proj_query, sizeof (insert_group_proj_query),QUERY_INSERT_CROSS_BRIDGE_PROJECT,array_idproj[i],max_group_id+1);
                  submit_query (conn, insert_group_proj_query);
                  sprintf(array_idproj_str, "%d", array_idproj[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_cross_bridge_project,array_idproj_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_idproj_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_cross_bridge_project_tmp,max_group_id_str,proj_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,proj_key);
            }

             //devo controllare se esiste già il gruppo
         }

/*
         long int proj_group_id=0; 
         if (hashtbl_result = hashtbl_get (hashtbl_cross_bridge_project, proj_group_id)) 
         {
                 pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",proj_group_id, hashtbl_result);
                 proj_group_id = atol (hashtbl_result);
                 success_lookup[0]++;
         }
         else
         {
               
         }   */  
#if 0
          if (hashtbl_result = hashtbl_get (hashtbl_hosts, node_ip))
          {
		      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",node_ip, hashtbl_result);
		      host_id = atol (hashtbl_result);
		      success_lookup[0]++;
		    }
		  else
		    {		// add host entry in DB (and hashtable too) without geolocation information
                    }
           }
#endif


	
  // end parser call
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Hashtables: Hits [%lld] Failure [%lld]\n", success_lookup[0], success_lookup[1]);

          //DA CONTROLLARE
          char fact_row[2048] = { '\0' };
          int fact_id=0;
          sprintf(fact_row, "%.0f:%d:%d:%s:%s:%s:%d:%d:%ld:%d:%d", size_row, duration_row, atoi(replica), user_id_hash_row, esgf_node, user_idp_row, atoi(buf3),atoi(buf4), project_group_key, geo_id, date_id);
          printf("fact_row %s\n", fact_row);
          if (hashtbl_result = hashtbl_get (hashtbl_cross_fact_download, fact_row))
          {
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",fact_row, hashtbl_result);
             fact_id = atol (hashtbl_result);
	     success_lookup[0]++;
          }
          else
	  {		// add host entry in DB (and hashtable too) without geolocation information

             char cross_bridge_project_id_str[128] = { '\0' };
             char fact_id_str[128] = { '\0' };
             char insert_cross_bridge_project[2048] = { '\0' };
             char select_id_cross_bridge_project[2048] = { '\0' };

             success_lookup[1]++;
             snprintf (insert_cross_bridge_project, sizeof (insert_cross_bridge_project), QUERY_INSERT_CROSS_FACT_DOWNLOAD,size_row, success_row, duration_row, replica, user_id_hash_row, esgf_node, user_idp_row, buf3, buf4, project_group_key, geo_id, date_id);


	     submit_query (conn, insert_cross_bridge_project);

	     //snprintf(select_id_cross_bridge_project, sizeof (select_id_cross_bridge_project), QUERY_GET_CROSS_FACT_DOWNLOAD, size_row, success_row, duration_row, replica, user_id_hash_row, esgf_node, user_idp_row, buf3, buf4, project_group_key, geo_id, date_id);
             //fact_id=get_foreign_key_value(conn, select_id_cross_bridge_project);
	     // add entry to hash table
	     //sprintf (fact_id_str, "%d", fact_id);
             //hashtbl_insert (hashtbl_cross_fact_download, fact_row, fact_id_str);
	     //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%d] [%s]\n",fact_id, fact_id_str);
          }
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"*********** End parsing routine  ************\n");
     free(array_idproj);
     free(user_id_hash_row);
     free(user_idp_row);
     free(success_row);
     
     char update_dashboard_queue[2048] = { '\0' };
     
     snprintf (update_dashboard_queue, sizeof (update_dashboard_queue), QUERY_UPDATE_DASHBOARD_QUEUE,(*datasetproj)[cnt]->id_query);
     submit_query (conn, update_dashboard_queue);
    
          //Put processed=1 in dashboard_queue
	  //QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST

   
   }
  // closing database connection
  PQfinish (conn);
  // releasing memory for hashtables
  if (create_populate_done)
    {
      hashtbl_destroy (hashtbl_cross_dim_date);
      hashtbl_destroy (hashtbl_cross_dim_geolocation);
      hashtbl_destroy (hashtbl_cross_dim_project);
      hashtbl_destroy (hashtbl_cross_bridge_project);
      hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
      hashtbl_destroy (hashtbl_cross_fact_download);
      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Releasing memory for hashtables [%d] \n",create_populate_done);
    }

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Closing transaction and releasing the database tables lock...\n");
  /*if (manage_database_open_close_transaction(conn,QUERY_CLOSE_TRANSACTION)) {
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"closing transaction FAILED\n");
  	  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Hashtables: Hits [%lld] Failure [%lld]\n", success_lookup[0], success_lookup[1]);
  	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"*********** End parsing routine  ************\n");
      	  PQfinish (conn);
	  return -1;
	 }*/
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"closing transaction OK. Database tables locks released!\n");


  return 0;
}
int insert_dmart_cross_project()
{
  PGconn *conn;
  PGresult *res;
  char conninfo[1024] = { '\0' };
  char conninfo_printf[1024] = { '\0' };
  snprintf (conninfo, sizeof (conninfo),
	    "dbname=%s user=%s password=abcdef",POSTGRES_DB_NAME, POSTGRES_USER);

  snprintf (conninfo_printf, sizeof (conninfo_printf),
	    "host=%s port=%d dbname=%s user=%s password=******", POSTGRES_HOST,
	    POSTGRES_PORT_NUMBER, POSTGRES_DB_NAME, POSTGRES_USER);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"*********** Starting parsing routine  ************\n");
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Open connection to: %s\n", conninfo_printf);

  conn = PQconnectdb ((const char *) conninfo);
  if (PQstatus (conn) != CONNECTION_OK)
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s",
      	       PQerrorMessage (conn));
      PQfinish (conn);
      return -1;
    }
     char insert_dmart_project_host[2048] = { '\0' };
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST);
     submit_query (conn, insert_dmart_project_host);
     PQfinish (conn);
   
}

//da eliminare quando si farà l'integrazione
int manage_database_open_close_transaction(PGconn *conn, char* QUERY)
{
  char query_transaction[2048] = { '\0' };

  snprintf (query_transaction,sizeof (query_transaction),QUERY);
  if (submit_query (conn, query_transaction))
          return -1;
   
  return 0;
}
int
submit_query (PGconn * conn, char *query)
{
  PGresult *res;
  fprintf (stderr, "Query: %s\n", query);

  res = PQexec (conn, query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Submit query failed\n");
      PQclear (res);
      return -1;
    }
  PQclear (res);
  return 0;
}
int
submit_query_res (PGconn * conn, char *query, PGresult **res)
{
  fprintf (stderr, "Query**: %s\n", query);

  //PGresult *res=NULL;

  *res = PQexec (conn, query);

  if ((!(*res)) || (PQresultStatus (*res) != PGRES_TUPLES_OK))
    {
      //pmesg(LOG_ERROR,__FILE__,__LINE__,"Submit query failed\n");
      PQclear (*res);
      return -1;
    }

  return 0;
}
int
populate_hash_table (PGconn * conn, char *query, HASHTBL ** pointer)
{

  PGresult *res;
  int i, num_records;

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Retrieving data for hashtable\n");
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[QUERY=%s]\n", query);
  res = PQexec (conn, query);
  if ((!res) || (PQresultStatus (res) != PGRES_TUPLES_OK))
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Populate_hash_table error running query [%s]\n", query);
      PQclear (res);
      return -1;
    }

  num_records = PQntuples (res);

  for (i = 0; i < num_records; i++)
    {
      hashtbl_insert (*pointer, (char *) PQgetvalue (res, i, 0),
		      (char *) PQgetvalue (res, i, 1));
      pmesg(LOG_DEBUG,__FILE__,__LINE__,"To be stored into the hashtable [%s]#[%s]\n",
	       (char *) PQgetvalue (res, i, 0), (char *) PQgetvalue (res, i,
								     1));
    }
  PQclear (res);

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Hashtable properly created!\n", query);
  return 0;
}

long 
int get_foreign_key_value (PGconn * conn, char *query)
{
  PGresult *res;
  long int fk;
  //fprintf (stderr, "Query: %s\n", query);

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
      return -2;
    }

  fk = atol (PQgetvalue (res, 0, 0));

  PQclear (res);
  //fprintf (stderr, "The fk is %ld\n", fk);
  return fk;
}

void quickSort( int *a, int l, int r)
{
   int j;

   if( l < r )
   {
        // divide and conquer
       j = partition( a, l, r);
       quickSort( a, l, j-1);
       quickSort( a, j+1, r);
   }

}



int partition( int *a, int l, int r) {
   int pivot, i, j, t;
   pivot = a[l];
   i = l; j = r+1;

   while( 1)
   {
        do ++i; while( a[i] <= pivot && i <= r );
        do --j; while( a[j] > pivot );
        if( i >= j ) break;
        t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}
