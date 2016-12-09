/*
 *  esgf-dashboard-insert-dw.c
 *  
 *  Author: University of Salento and CMCC 
 *      
 */

#include <stdio.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/dbAccess.h"
#include "../include/ftpget.h"
#include <libpq-fe.h>
#include <time.h>
#include "GeoIP.h"
#include "GeoIPCity.h"
#include "../include/config.h"
#include "../include/hashtbl.h"

// HashTables dimensions - Cross Project
#define HAST_TABLE_CROSS_DIM_DATE		64
#define HAST_TABLE_CROSS_DIM_GEOLOCATION	64		
#define HAST_TABLE_CROSS_DIM_PROJECT		16
#define HAST_TABLE_CROSS_BRIDGE_PROJECT		64	
#define HAST_TABLE_CROSS_BRIDGE_PROJECT_TMP	64	
#define HAST_TABLE_CROSS_DOWNLOAD		300
#define HAST_TABLE_OBS4MIPS_DOWNLOAD		300
#define HAST_TABLE_CMIP5_DOWNLOAD		300
#define HAST_TABLE_CROSS_DMART_PROJECT_HOST_GEOLOCATION	64

// HashTables dimensions - Obs4MIPS Project
#define HAST_TABLE_OBS_DIM_FILE		        	64
#define HAST_TABLE_OBS_DIM_DATASET	        	64
#define HAST_TABLE_OBS_DIM_INSTITUTE			64
#define HAST_TABLE_OBS_DIM_VARIABLE			64
#define HAST_TABLE_OBS_DIM_TIME_FREQ			64
#define HAST_TABLE_OBS_DIM_INDEX			64
#define HAST_TABLE_OBS_DIM_PROC_LEV			64
#define HAST_TABLE_OBS_DIM_SOURCE_ID			64
#define HAST_TABLE_OBS_DIM_REALM			64
#define HAST_TABLE_OBS_BRIDGE_INSTITUTE			64	
#define HAST_TABLE_OBS_BRIDGE_INSTITUTE_TMP		64	
#define HAST_TABLE_OBS_BRIDGE_VARIABLE			64	
#define HAST_TABLE_OBS_BRIDGE_VARIABLE_TMP		64	
#define HAST_TABLE_OBS_BRIDGE_TIME_FREQUENCY		64	
#define HAST_TABLE_OBS_BRIDGE_TIME_FREQUENCY_TMP	64	
#define HAST_TABLE_OBS_BRIDGE_PROCESSING_LEVEL		64	
#define HAST_TABLE_OBS_BRIDGE_PROCESSING_LEVEL_TMP	64	
#define HAST_TABLE_OBS_BRIDGE_SOURCE_ID		64	
#define HAST_TABLE_OBS_BRIDGE_SOURCE_ID_TMP	64	
#define HAST_TABLE_OBS_BRIDGE_REALM		64	
#define HAST_TABLE_OBS_BRIDGE_REALM_TMP	        64	
#define HAST_TABLE_OBS_DIM_DATE		64
#define HAST_TABLE_OBS_DIM_GEOLOCATION	64		

// HashTables dimensions - CMIP5 Project
#define HAST_TABLE_CMIP5_DIM_MODEL	        	64
#define HAST_TABLE_CMIP5_DIM_EXPERIMENT	        	64
#define HAST_TABLE_CMIP5_BRIDGE_MODEL		64	
#define HAST_TABLE_CMIP5_BRIDGE_MODEL_TMP	        64	
#define HAST_TABLE_CMIP5_BRIDGE_EXPERIMENT		64	
#define HAST_TABLE_CMIP5_BRIDGE_EXPERIMENT_TMP	        64	



#if 0
#define POSTGRES_HOST "localhost"
#define POSTGRES_PORT_NUMBER 5432
#define POSTGRES_DB_NAME "esgf2"
#define POSTGRES_USER "postgres"
#define POSTGRES_PASSWD ""
#define POSTGRES_HOST "esgf-test.cmcc.it"
#define POSTGRES_PORT_NUMBER 5432
#define POSTGRES_DB_NAME "esgcet"
#define POSTGRES_USER "dbsuper"
#define POSTGRES_PASSWD "abcdef"
#endif 

// Start transaction and lock tables 
#define QUERY_OPEN_TRANSACTION  "start transaction; lock esgf_dashboard.cross_dim_date ; lock esgf_dashboard.cross_dim_geolocation ; lock esgf_dashboard.cross_dim_project ; lock esgf_dashboard.cross_bridge_project ; lock esgf_dashboard.cross_fact_download ; "

// End transaction and release locks 
#define QUERY_CLOSE_TRANSACTION  "end transaction;" 

long
int get_foreign_key_value (PGconn * conn, char *query);

int check_cross_project (PGconn *conn, struct dataset_project ***datasetproj, char* esgf_node, int res_rep)
{

  //PGconn *conn;
  //PGresult *res;
  //char conninfo[1024] = { '\0' };
  //char conninfo_printf[1024] = { '\0' };
  //char open_transaction[2048] = { '\0' };
  //char close_transaction[2048] = { '\0' };
  int create_populate_done = 0;
  char select_query[2048] = { '\0' };
  char select_id_date_query[2048] = { '\0' };
  static long long int success_lookup[2] = { 0 };	// [0] success [1] missing
  int project_group_key=0;
  int institute_group_key=0;
  int var_group_key=0;
  int time_freq_group_key=0;
  int proc_lev_group_key=0;
  int source_id_group_key=0;
  int realm_group_key=0;
  int exp_group_key=0;
  int model_group_key=0;
  int geo_id = 0;
  int geo_obs_id = 0;
  char *proj_name = NULL;

  /* cross project */
  HASHTBL *hashtbl_cross_dim_date;
  HASHTBL *hashtbl_cross_dim_geolocation;
  HASHTBL *hashtbl_cross_dim_project;
  HASHTBL *hashtbl_cross_bridge_project;
  HASHTBL *hashtbl_cross_bridge_project_tmp;
  HASHTBL *hashtbl_cross_fact_download;
  HASHTBL *hashtbl_obs4mips_fact_download;
  HASHTBL *hashtbl_cmip5_fact_download;

  /* obs4mips project */
  HASHTBL *hashtbl_obs_dim_file;
  HASHTBL *hashtbl_obs_dim_dataset;
  HASHTBL *hashtbl_obs_dim_institute;
  HASHTBL *hashtbl_obs_dim_variable;
  HASHTBL *hashtbl_obs_dim_time_frequency;
  HASHTBL *hashtbl_obs_dim_processing_level;
  HASHTBL *hashtbl_obs_dim_source_id;
  HASHTBL *hashtbl_obs_dim_realm;
  HASHTBL *hashtbl_obs_dim_index;
  HASHTBL *hashtbl_obs_bridge_institute;
  HASHTBL *hashtbl_obs_bridge_institute_tmp;
  HASHTBL *hashtbl_obs_bridge_variable;
  HASHTBL *hashtbl_obs_bridge_variable_tmp;
  HASHTBL *hashtbl_obs_bridge_time_frequency;
  HASHTBL *hashtbl_obs_bridge_time_frequency_tmp;
  HASHTBL *hashtbl_obs_bridge_processing_level;
  HASHTBL *hashtbl_obs_bridge_processing_level_tmp;
  HASHTBL *hashtbl_obs_bridge_source_id;
  HASHTBL *hashtbl_obs_bridge_source_id_tmp;
  HASHTBL *hashtbl_obs_bridge_realm;
  HASHTBL *hashtbl_obs_bridge_realm_tmp;
  HASHTBL *hashtbl_obs_dim_date;
  HASHTBL *hashtbl_obs_dim_geolocation;
  HASHTBL *hashtbl_cmip5_dim_model;
  HASHTBL *hashtbl_cmip5_bridge_model;
  HASHTBL *hashtbl_cmip5_bridge_model_tmp;
  HASHTBL *hashtbl_cmip5_dim_experiment;
  HASHTBL *hashtbl_cmip5_bridge_experiment;
  HASHTBL *hashtbl_cmip5_bridge_experiment_tmp;

/*
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
*/

        //PGconn *conn;
        PGresult *res1;
        int numTuples;
        /*char conninfo[1024] = {'\0'};
        printf("host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);

        snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
        conn = PQconnectdb ((const char *) conninfo);

        if (PQstatus(conn) != CONNECTION_OK)
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"Connection to database failed: %s\n", PQerrorMessage(conn));
                PQfinish(conn);
                return -2;
        }
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying open transaction\n");
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY3);
        res1 = PQexec(conn, QUERY3);
        if ((!res1) || (PQresultStatus (res1) != PGRES_COMMAND_OK))
        {
           pmesg(LOG_ERROR,__FILE__,__LINE__,"Open transaction failed\n");
           PQclear(res1);
           PQfinish(conn);
           return -2;
        }
        PQclear(res1);
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction opened\n");
    */
    int i,cnt=0;
    int size2=0;
    int timestamp;
    char *remote_addr=NULL;
    //float size_row;
    int size_row;
    int duration_row=0;
    char *user_id_hash_row=NULL;
    char *user_idp_row=NULL;
    char *success_row=NULL;

    //printf("%s\n", "sono qui");
    for(cnt=0; (*datasetproj)[cnt]!=NULL; cnt++)
    {
    //printf("%s\n", "sono qui2");
                char* replica=NULL; //cross_project
                char* index_node=NULL; //obs4mips
                //char* version=NULL; //obs4mips
                char* dataset_id=NULL; //obs4mips
                char* size_file=NULL; //obs4mips
                char* datetime_start=NULL; //obs4mips
                char* datetime_stop=NULL; //obs4mips
                char** institute=NULL; //obs4mips
                char** cf_standard_name=NULL; //obs4mips
                char** variable=NULL; //obs4mips
                char** variable_long_name=NULL; //obs4mips
                char** time_frequency=NULL; //obs4mips
                char** processing_level=NULL; //obs4mips
                char** source_id=NULL; //obs4mips
                char** realm=NULL; //obs4mips
                char** model=NULL; //cmip5
                char** experiment=NULL; //cmip5
                int size_institute=0;
                int size_cf=0;
                int size_variable=0;
                int size_variable_long=0;
                int size_time=0;
                int size_processing=0;
                int size_realm=0;
                int size_source=0;
                int size_model=0;
                int size_exp=0;
    
        // SELECT START 

            
                snprintf (select_query, sizeof (select_query), QUERY_SELECT_DASHBOARD_QUEUE, (*datasetproj)[cnt]->id_query);
        res1 = PQexec(conn,select_query);

        if ((!res1) || (PQresultStatus(res1) != PGRES_TUPLES_OK))
        {
                pmesg(LOG_ERROR,__FILE__,__LINE__,"SELECT remote clients data from access_logging FAILED\n");
                PQclear(res1);
                //PQfinish(conn);
                return -2;
        }

        //submit_query_res (conn, select_query,&res1);
                for (i = 0; i < PQntuples(res1); i++)
                {
                   //printf("timestamp %ld\n", atoi(PQgetvalue(res1, i, 0)));
                   timestamp=atoi(PQgetvalue(res1, i, 0));
                   //printf("timestamp %s\n", PQgetvalue(res1, i, 0));
                   //printf("remote_addr %s\n", PQgetvalue(res1, i, 1));
                   remote_addr=strdup(PQgetvalue(res1, i, 1));

                   //printf("size %s\n", PQgetvalue(res1, i, 2));
                   //printf("size %.0f\n", atof(PQgetvalue(res1, i, 2)));
                   size_row= atoi(PQgetvalue(res1, i, 2));
                   if(size_row>0)
                      size_row=size_row/(1024*1024*1024);
                   //printf("size %d\n", size_row);
                   //printf("success %s\n", PQgetvalue(res1, i, 3));
                   success_row=strdup(PQgetvalue(res1, i, 3));

                   //printf("duration %ld\n", atoi(PQgetvalue(res1, i, 4)));
                   duration_row=atoi(PQgetvalue(res1, i, 4));
                   //printf("duration %s\n", PQgetvalue(res1, i, 4));
                   //printf("user_id_hash %s\n", PQgetvalue(res1, i, 5));
                   user_id_hash_row=strdup(PQgetvalue(res1, i, 5));
                 
                   //printf("user_idp %s\n", PQgetvalue(res1, i, 6));
                   user_idp_row=strdup(PQgetvalue(res1, i, 6));
                }

                //PQclear (res1);

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
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {
                      replica=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "institute")==0)
                  {
                    size_institute=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    institute=(char **)calloc(size_institute+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      institute[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    institute[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "cf_standard_name")==0)
                  {
                    size_cf=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    cf_standard_name=(char **)calloc(size_cf+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      cf_standard_name[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    cf_standard_name[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "variable")==0)
                  {
                    size_variable=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    variable=(char **)calloc(size_variable+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      variable[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    variable[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "variable_long_name")==0)
                  {
                    size_variable_long=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    variable_long_name=(char **)calloc(size_variable_long+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      variable_long_name[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    variable_long_name[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "time_frequency")==0)
                  {
                    size_time=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    time_frequency=(char **)calloc(size_time+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      time_frequency[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    time_frequency[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "index_node")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      index_node=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "processing_level")==0)
                  {
                    size_processing=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    processing_level=(char **)calloc(size_processing+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      processing_level[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    processing_level[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "source_id")==0)
                  {
                    size_source=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    source_id=(char **)calloc(size_source+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      source_id[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    source_id[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "realm")==0)
                  {
                    size_realm=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    realm=(char **)calloc(size_realm+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      realm[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    realm[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "model")==0)
                  {
                    size_model=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    model=(char **)calloc(size_model+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      model[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    model[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "experiment")==0)
                  {
                    size_exp=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    experiment=(char **)calloc(size_exp+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      experiment[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                    }
                    experiment[size4]=NULL;
                  }
                  /*if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "version")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      version=(*datasetproj)[cnt]->first[size2]->first[size3]->value[size4];
                      printf("version vale %s\n", version);
                  }*/
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "dataset_id")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      dataset_id=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "size")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      size_file=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "datetime_start")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      datetime_start=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "datetime_stop")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      datetime_stop=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                  }
                }
              }
         }
       }

// create and populate hashtables if not yet
	  if (!create_populate_done)
	    {
             create_populate_done=1;
	     pmesg(LOG_DEBUG,__FILE__,__LINE__,"Create and populate Hash tables (first iteration)\n");
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
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS4MIPS DOWNLOAD\n");
	      if (!(hashtbl_obs4mips_fact_download = hashtbl_create (HAST_TABLE_OBS4MIPS_DOWNLOAD, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CMIP5 DOWNLOAD\n");
	      if (!(hashtbl_cmip5_fact_download = hashtbl_create (HAST_TABLE_CMIP5_DOWNLOAD, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  hashtbl_destroy (hashtbl_cmip5_fact_download);
		  //continue;
		}

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


                     time_t realtime;
          realtime = (time_t)timestamp;
          //time(&realtime);
          struct tm * ptm = gmtime(&realtime);
          //printf ("The UTC time is: %s", asctime(ptm));
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

             pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query %s\n", insert_new_date_query);

	     snprintf(select_id_date_query, sizeof (select_id_date_query), QUERY_GET_DATE_ID, buf);
             date_id=get_foreign_key_value(conn, select_id_date_query);
               
	     // add entry to hash table
	     sprintf (date_id_str, "%ld", date_id);
             hashtbl_insert (hashtbl_cross_dim_date, buf, date_id_str);
	     pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%d] [%s]\n",date_id, date_id_str);
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
                 char str_geo[2048] = { '\0' };
                 char str1_geo[2048] = { '\0' };
                 float lat=40.33495040000;
                 float lon=18.11679550000;
              if(code==0)
              {
		 pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_COUNTRY_CODE=%s]\n",geo_output.country_code);
		 pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_LATITUDE=%f]\n",geo_output.latitude);
	         pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_LONGITUDE=%f]\n",geo_output.longitude);
                 sprintf(str1_geo, "%14.11f",geo_output.latitude);
                 sprintf(str_geo, "%14.11f",geo_output.longitude);
                 if (*str1_geo==' ')
                     sprintf(str1_geo, "%s",str1_geo+1);
                 if (*str_geo==' ')
                     sprintf(str_geo, "%s",str_geo+1);
                 
                     
                 lat=atof(str1_geo);
                 lon=atof(str_geo);
                 //sprintf(key_geo, "%14.11f:%14.11f", geo_output.latitude,geo_output.longitude);
                 sprintf(key_geo, "%s:%s", str1_geo,str_geo);
                 
                 //int i, j=0;
                 //while(str_geo[key_geo[i]!=' ' ? j++ : j] = str_geo[i++]);
                 //char *str2; 
                 //str2=key_geo;  
                 //while (*str2==' ') str2++;  
                 //if (str2!=key_geo) memmove(key_geo,str2,strlen(str2)+1); 

                 //printf("str_geo vale %s\n",key_geo);
                 //sprintf(key_geo,"%s", str_geo);
                 //lat=geo_output.latitude;
                 //lon=geo_output.longitude;
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
                     char *country_id=NULL;
                     char *country_code=NULL;
                     if(code==0)
                         country_code=strdup(geo_output.country_code);
                     else
                         country_code=strdup("IT");
		     snprintf (select_geo_query, sizeof (select_geo_query),QUERY_SELECT_GEOLOCATION_INFO,country_code);
	             pmesg(LOG_DEBUG,__FILE__,__LINE__,"I got the geolocation info from the DB (estimation)\n");
		     pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s\n",select_geo_query);

		     submit_query_res (conn, select_geo_query, &res_geo);
                     country_id=PQgetvalue(res_geo, 0, 0);
                     PQclear (res_geo);
                     //printf("country_id %ls\n", country_id);
                     if(country_id)
		       snprintf (insert_geo_query, sizeof (insert_geo_query),QUERY_INSERT_CROSS_DIM_GEOLOCATION, lat, lon, atoi(country_id));
                     submit_query (conn, insert_geo_query);
                     free(country_code);
                     
	             snprintf(select_id_geo_query, sizeof (select_id_geo_query), QUERY_GET_GEO_ID, lat, lon);
                     geo_id=get_foreign_key_value(conn, select_id_geo_query);
	             sprintf (geo_id_str, "%ld", geo_id);
                     hashtbl_insert (hashtbl_cross_dim_geolocation, key_geo, geo_id_str);
	             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_geo, geo_id_str);
                  }
	      //}
	      //else
		//pmesg(LOG_ERROR,__FILE__,__LINE__,"Esgf-lookup error\n");

              //free(remote_addr);
              
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
                if(i==0 || i==(*datasetproj)[cnt]->size-1)
                   ap += sprintf(ap, "%d", array_idproj[i]);
                else
                   ap += sprintf(ap, ",%d,", array_idproj[i]);
             
         }
         //printf("proj_key %s\n", proj_key);
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
               //printf("project_group_key %d\n", project_group_key);
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
         free(array_idproj);
             if(!replica)
             {
                if(res_rep==0)
                  replica=strdup("true");
                else
                {
                  replica=strdup("false");
                  //printf("replica is %s\n", replica);
                }
             }
             char *str_succ=NULL;
             if(strcmp(success_row,"t")==0)
                  str_succ=strdup("true");
             else
                  str_succ=("false");

          char fact_row[2048] = { '\0' };
          long int fact_id=0;
          sprintf(fact_row, "%d:%s:%d:%s:%s:%s:%s:%s:%s:%d:%d:%d", size_row, str_succ, duration_row, replica, user_id_hash_row, esgf_node, user_idp_row, buf3,buf4, project_group_key, geo_id, date_id);
          //printf("fact_row %s\n", fact_row);
          free(str_succ);
          str_succ=NULL;
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
             char select_id_fact_query[2048] = { '\0' };

             success_lookup[1]++;
             snprintf (insert_cross_bridge_project, sizeof (insert_cross_bridge_project), QUERY_INSERT_CROSS_FACT_DOWNLOAD,size_row, success_row, duration_row, replica, user_id_hash_row, esgf_node, user_idp_row, buf3, buf4, project_group_key, geo_id, date_id);


	     submit_query (conn, insert_cross_bridge_project);
	     snprintf(select_id_fact_query, sizeof (select_id_fact_query), QUERY_GET_FACT_ROW,  size_row, success_row, duration_row, replica, user_id_hash_row, esgf_node, user_idp_row, buf3,buf4, project_group_key, geo_id, date_id);

             //printf("select_id_fact_query vale %s\n", select_id_fact_query);
             fact_id=get_foreign_key_value(conn, select_id_fact_query);
             sprintf (fact_id_str, "%ld", fact_id);
             //printf("FACT ID %ld", fact_id);
             hashtbl_insert (hashtbl_cross_fact_download,fact_row,fact_id_str);
             //printf("hashtable [%s]*****[%s]\n",fact_row,fact_id_str);
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",fact_row,fact_id_str);
          }


         for(size2=0; (*datasetproj)[cnt]->first[size2]!=NULL; size2++)
         {
           proj_name=strdup((*datasetproj)[cnt]->first[size2]->project);
           uppercase(proj_name);
      
           if (create_populate_done==1)
	   {
            if(strcmp(proj_name, "OBS4MIPS")==0)
            {
              create_populate_done=2;       
	      //pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
	      if (!(hashtbl_obs_dim_file = hashtbl_create (HAST_TABLE_OBS_DIM_FILE, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  //continue;
		}
             }
            
	      if (!(hashtbl_obs_dim_institute = hashtbl_create (HAST_TABLE_OBS_DIM_INSTITUTE, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  //continue;
		}
	      if (!(hashtbl_obs_dim_variable = hashtbl_create (HAST_TABLE_OBS_DIM_VARIABLE, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  //continue;
		}
	      if (!(hashtbl_obs_dim_time_frequency = hashtbl_create (HAST_TABLE_OBS_DIM_TIME_FREQ, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  //continue;
		}

	      if (!(hashtbl_obs_dim_index = hashtbl_create (HAST_TABLE_OBS_DIM_INDEX, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  //continue;
		}
	      if (!(hashtbl_obs_dim_processing_level = hashtbl_create (HAST_TABLE_OBS_DIM_PROC_LEV, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  //continue;
		}
             if(strcmp(proj_name, "OBS4MIPS")==0)
             {
            	if (!(hashtbl_obs_dim_source_id = hashtbl_create (HAST_TABLE_OBS_DIM_SOURCE_ID, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  //continue;
		}
             }
              
	      if (!(hashtbl_obs_dim_realm = hashtbl_create (HAST_TABLE_OBS_DIM_REALM, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  //continue;
		}
	      if (!(hashtbl_obs_dim_dataset = hashtbl_create (HAST_TABLE_OBS_DIM_DATASET, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS4MIPS BRIDGE INSTITUTE\n");
	      if (!(hashtbl_obs_bridge_institute = hashtbl_create (HAST_TABLE_OBS_BRIDGE_INSTITUTE, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable TMP for OBS BRIDGE INSTITUTE\n");
	      if (!(hashtbl_obs_bridge_institute_tmp = hashtbl_create (HAST_TABLE_OBS_BRIDGE_INSTITUTE_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS BRIDGE VARIABLE\n");
	      if (!(hashtbl_obs_bridge_variable = hashtbl_create (HAST_TABLE_OBS_BRIDGE_VARIABLE, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable TMP for OBS BRIDGE VARIABLE\n");
	      if (!(hashtbl_obs_bridge_variable_tmp = hashtbl_create (HAST_TABLE_OBS_BRIDGE_VARIABLE_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS BRIDGE TIME FREQUENCY\n");
	      if (!(hashtbl_obs_bridge_time_frequency = hashtbl_create (HAST_TABLE_OBS_BRIDGE_TIME_FREQUENCY, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable TMP for OBS BRIDGE TIME FREQUENCY\n");
	      if (!(hashtbl_obs_bridge_time_frequency_tmp = hashtbl_create (HAST_TABLE_OBS_BRIDGE_TIME_FREQUENCY_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS BRIDGE PROCESSING LEVEL\n");
	      if (!(hashtbl_obs_bridge_processing_level = hashtbl_create (HAST_TABLE_OBS_BRIDGE_PROCESSING_LEVEL, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable TMP for OBS BRIDGE PROCESSING LEVEL\n");
	      if (!(hashtbl_obs_bridge_processing_level_tmp = hashtbl_create (HAST_TABLE_OBS_BRIDGE_PROCESSING_LEVEL_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  //continue;
		}
             if(strcmp(proj_name, "OBS4MIPS")==0)
             {
              pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS BRIDGE SOURCE ID\n");
	      if (!(hashtbl_obs_bridge_source_id = hashtbl_create (HAST_TABLE_OBS_BRIDGE_SOURCE_ID, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the TMP hashtable for OBS BRIDGE SOURCE ID\n");
	      if (!(hashtbl_obs_bridge_source_id_tmp = hashtbl_create (HAST_TABLE_OBS_BRIDGE_SOURCE_ID_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  //continue;
		}
              }
              
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS BRIDGE REALM\n");
	      if (!(hashtbl_obs_bridge_realm = hashtbl_create (HAST_TABLE_OBS_BRIDGE_REALM, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the TMP hashtable for OBS BRIDGE REALM\n");
	      if (!(hashtbl_obs_bridge_realm_tmp = hashtbl_create (HAST_TABLE_OBS_BRIDGE_REALM_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS DIM DATE\n");
	      if (!(hashtbl_obs_dim_date = hashtbl_create (HAST_TABLE_OBS_DIM_DATE, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for OBS DIM GEOLOCATION\n");
	      if (!(hashtbl_obs_dim_geolocation = hashtbl_create (HAST_TABLE_OBS_DIM_GEOLOCATION, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  //continue;
		}
	      if (!(hashtbl_obs4mips_fact_download = hashtbl_create (HAST_TABLE_OBS4MIPS_DOWNLOAD, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
             }

            if(strcmp(proj_name, "CMIP5")==0)
            {
              create_populate_done=3;
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CMIP5 DIM MODEL\n");
	      if (!(hashtbl_cmip5_dim_model = hashtbl_create (HAST_TABLE_CMIP5_DIM_MODEL, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  hashtbl_destroy (hashtbl_cmip5_dim_model);
		  //continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CMIP5 DIM EXPERIMENT\n");
	      if (!(hashtbl_cmip5_dim_experiment = hashtbl_create (HAST_TABLE_CMIP5_DIM_EXPERIMENT, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  hashtbl_destroy (hashtbl_cmip5_dim_model);
		  hashtbl_destroy (hashtbl_cmip5_dim_experiment);
		  //continue;
		}	       
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CMIP5 BRIDGE MODEL\n");
	      if (!(hashtbl_cmip5_bridge_model = hashtbl_create (HAST_TABLE_CMIP5_BRIDGE_MODEL, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  hashtbl_destroy (hashtbl_cmip5_dim_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model);
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the TMP hashtable for CMIP5 BRIDGE MODEL\n");
	      if (!(hashtbl_cmip5_bridge_model_tmp = hashtbl_create (HAST_TABLE_CMIP5_BRIDGE_MODEL_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  hashtbl_destroy (hashtbl_cmip5_dim_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model_tmp);
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for CMIP5 BRIDGE EXPERIMENT\n");
	      if (!(hashtbl_cmip5_bridge_experiment = hashtbl_create (HAST_TABLE_CMIP5_BRIDGE_EXPERIMENT, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  hashtbl_destroy (hashtbl_cmip5_dim_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model_tmp);
		  hashtbl_destroy (hashtbl_cmip5_bridge_experiment);
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the TMP hashtable for CMIP5 BRIDGE EXPERIMENT\n");
	      if (!(hashtbl_cmip5_bridge_experiment_tmp = hashtbl_create (HAST_TABLE_CMIP5_BRIDGE_EXPERIMENT_TMP, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  hashtbl_destroy (hashtbl_cmip5_dim_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model_tmp);
		  hashtbl_destroy (hashtbl_cmip5_bridge_experiment);
		  hashtbl_destroy (hashtbl_cmip5_bridge_experiment_tmp);
		}
	      if (!(hashtbl_cmip5_fact_download = hashtbl_create (HAST_TABLE_CMIP5_DOWNLOAD, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_cross_dim_date);
		  hashtbl_destroy (hashtbl_cross_dim_geolocation);
		  hashtbl_destroy (hashtbl_cross_dim_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project);
                  hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
		  hashtbl_destroy (hashtbl_cross_fact_download);
		  hashtbl_destroy (hashtbl_obs_dim_file);
		  hashtbl_destroy (hashtbl_obs_dim_institute);
		  hashtbl_destroy (hashtbl_obs_dim_variable);
		  hashtbl_destroy (hashtbl_obs_dim_time_frequency);
		  hashtbl_destroy (hashtbl_obs_dim_index);
		  hashtbl_destroy (hashtbl_obs_dim_processing_level);
		  hashtbl_destroy (hashtbl_obs_dim_source_id);
		  hashtbl_destroy (hashtbl_obs_dim_realm);
		  hashtbl_destroy (hashtbl_obs_dim_dataset);
		  hashtbl_destroy (hashtbl_obs_bridge_institute);
		  hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_variable);
		  hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
		  hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level);
		  hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id);
		  hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
		  hashtbl_destroy (hashtbl_obs_bridge_realm);
		  hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
		  hashtbl_destroy (hashtbl_obs_dim_date);
		  hashtbl_destroy (hashtbl_obs_dim_geolocation);
		  hashtbl_destroy (hashtbl_obs4mips_fact_download);
		  hashtbl_destroy (hashtbl_cmip5_dim_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model_tmp);
		  hashtbl_destroy (hashtbl_cmip5_fact_download);
             }
           }


              if(strcmp(proj_name, "OBS4MIPS")==0)
              {
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_FILE,
				   &hashtbl_obs_dim_file);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_DATASET,
				   &hashtbl_obs_dim_dataset);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_INSTITUTE,
				   &hashtbl_obs_dim_institute);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_VARIABLE,
				   &hashtbl_obs_dim_variable);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_TIME_FREQUENCY,
				   &hashtbl_obs_dim_time_frequency);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_INDEX,
				   &hashtbl_obs_dim_index);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_PROCESSING_LEVEL,
				   &hashtbl_obs_dim_processing_level);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_SOURCE_ID,
				   &hashtbl_obs_dim_source_id);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_REALM,
				   &hashtbl_obs_dim_realm);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_INSTITUTE,
				   &hashtbl_obs_bridge_institute);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_INSTITUTE_TMP,
				   &hashtbl_obs_bridge_institute_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_VARIABLE,
				   &hashtbl_obs_bridge_variable);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_VARIABLE_TMP,
				   &hashtbl_obs_bridge_variable_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_TIME_FREQUENCY,
				   &hashtbl_obs_bridge_time_frequency);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_TIME_FREQUENCY_TMP,
				   &hashtbl_obs_bridge_time_frequency_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_PROCESSING_LEVEL,
				   &hashtbl_obs_bridge_processing_level);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_PROCESSING_LEVEL_TMP,
				   &hashtbl_obs_bridge_processing_level_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_SOURCE_ID,
				   &hashtbl_obs_bridge_source_id);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_SOURCE_ID_TMP,
				   &hashtbl_obs_bridge_source_id_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_REALM,
				   &hashtbl_obs_bridge_realm);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_BRIDGE_REALM_TMP,
				   &hashtbl_obs_bridge_realm_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_DATE,
				   &hashtbl_obs_dim_date);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS_DIM_GEOLOCATION,
				   &hashtbl_obs_dim_geolocation);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_OBS4MIPS_DOWNLOAD,
				   &hashtbl_obs4mips_fact_download);
	        }
              if(strcmp(proj_name, "CMIP5")==0)
              {
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_DATASET,
				   &hashtbl_obs_dim_dataset);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_INSTITUTE,
				   &hashtbl_obs_dim_institute);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_VARIABLE,
				   &hashtbl_obs_dim_variable);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_TIME_FREQUENCY,
				   &hashtbl_obs_dim_time_frequency);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_REALM,
				   &hashtbl_obs_dim_realm);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_INSTITUTE,
				   &hashtbl_obs_bridge_institute);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_INSTITUTE_TMP,
				   &hashtbl_obs_bridge_institute_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_VARIABLE,
				   &hashtbl_obs_bridge_variable);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_VARIABLE_TMP,
				   &hashtbl_obs_bridge_variable_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_TIME_FREQUENCY,
				   &hashtbl_obs_bridge_time_frequency);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_TIME_FREQUENCY_TMP,
				   &hashtbl_obs_bridge_time_frequency_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_REALM,
				   &hashtbl_obs_bridge_realm);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_REALM_TMP,
				   &hashtbl_obs_bridge_realm_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_DATE,
				   &hashtbl_obs_dim_date);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_GEOLOCATION,
				   &hashtbl_obs_dim_geolocation);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_MODEL,
				   &hashtbl_cmip5_dim_model);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_MODEL,
				   &hashtbl_cmip5_bridge_model);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_MODEL_TMP,
				   &hashtbl_cmip5_bridge_model_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DIM_EXPERIMENT,
				   &hashtbl_cmip5_dim_experiment);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_EXPERIMENT,
				   &hashtbl_cmip5_bridge_experiment);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_BRIDGE_EXPERIMENT_TMP,
				   &hashtbl_cmip5_bridge_experiment_tmp);
	          populate_hash_table (conn, QUERY_GET_LIST_OF_CMIP5_DOWNLOAD,
				   &hashtbl_cmip5_fact_download);
	        }
          }
	  else
	    pmesg(LOG_DEBUG,__FILE__,__LINE__, "Hash tables already in place with the data [%d]\n",create_populate_done);

            long int date_obs_id = 0;
 
          if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_date, buf))
          {
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",buf, hashtbl_result);
             date_obs_id = atol (hashtbl_result);
	     success_lookup[0]++;
          }
          else
	  {		// add host entry in DB (and hashtable too) without geolocation information

             char date_obs_id_str[128] = { '\0' };
             char insert_new_date_query[2048] = { '\0' };
             char select_id_date_query[2048] = { '\0' };

             success_lookup[1]++;
             if(strcmp(proj_name, "OBS4MIPS")==0)
                snprintf (insert_new_date_query, sizeof (insert_new_date_query), QUERY_INSERT_NEW_OBS_DATE, buf, buf1, buf2);
             else
                snprintf (insert_new_date_query, sizeof (insert_new_date_query), QUERY_INSERT_NEW_CMIP5_DATE, buf, buf1, buf2);
             
	     submit_query (conn, insert_new_date_query);

             if(strcmp(proj_name, "OBS4MIPS")==0)
	         snprintf(select_id_date_query, sizeof (select_id_date_query), QUERY_GET_OBS_DATE_ID, buf);
             else
	         snprintf(select_id_date_query, sizeof (select_id_date_query), QUERY_GET_CMIP5_DATE_ID, buf);

             date_obs_id=get_foreign_key_value(conn, select_id_date_query);
	     // add entry to hash table
	     sprintf (date_obs_id_str, "%ld", date_obs_id);
             hashtbl_insert (hashtbl_obs_dim_date, buf, date_obs_id_str);
	     pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable query=%s [%d] [%s]\n",select_id_date_query, date_obs_id, date_obs_id_str);
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
                 char str_geo[2048] = { '\0' };
                 char str1_geo[2048] = { '\0' };
                 float lat=40.33495040000;
                 float lon=18.11679550000;
              if(code==0)
              {
		 pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_COUNTRY_CODE=%s]\n",geo_output.country_code);
		 pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_LATITUDE=%f]\n",geo_output.latitude);
	         pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_LONGITUDE=%f]\n",geo_output.longitude);
                 sprintf(str1_geo, "%14.11f",geo_output.latitude);
                 sprintf(str_geo, "%14.11f",geo_output.longitude);
                 if (*str1_geo==' ')
                     sprintf(str1_geo, "%s",str1_geo+1);
                 if (*str_geo==' ')
                     sprintf(str_geo, "%s",str_geo+1);
                 
                     
                 lat=atof(str1_geo);
                 lon=atof(str_geo);
                 //sprintf(key_geo, "%14.11f:%14.11f", geo_output.latitude,geo_output.longitude);
                 sprintf(key_geo, "%s:%s", str1_geo,str_geo);
                 
                 //int i, j=0;
                 //while(str_geo[key_geo[i]!=' ' ? j++ : j] = str_geo[i++]);
                 //char *str2; 
                 //str2=key_geo;  
                 //while (*str2==' ') str2++;  
                 //if (str2!=key_geo) memmove(key_geo,str2,strlen(str2)+1); 

                 //printf("str_geo vale %s\n",key_geo);
                 //sprintf(key_geo,"%s", str_geo);
                 //lat=geo_output.latitude;
                 //lon=geo_output.longitude;
              }
              else
                 sprintf(key_geo, "%14.11f:%14.11f",lat,lon);
                
                 if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_geolocation, key_geo))
                 {
                     pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",key_geo, hashtbl_result);
                     geo_obs_id = atol (hashtbl_result);
                     success_lookup[0]++;
                 }
                 else
                 {             // add host entry in DB (and hashtable too) without geolocation information
                     char geo_id_str[128] = { '\0' };
                     char select_id_geo_query[2048] = { '\0' };

                     success_lookup[1]++;
                     char *country_id=NULL;
                     char *country_code=NULL;
                     if(code==0)
                         country_code=strdup(geo_output.country_code);
                     else
                         country_code=strdup("IT");
		     snprintf (select_geo_query, sizeof (select_geo_query),QUERY_SELECT_GEOLOCATION_INFO,country_code);
	             pmesg(LOG_DEBUG,__FILE__,__LINE__,"I got the geolocation info from the DB (estimation)\n");
		     pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s\n",select_geo_query);
		     submit_query_res (conn, select_geo_query, &res_geo);
                     country_id=PQgetvalue(res_geo, 0, 0);
                     PQclear (res_geo);
                     //printf("country_id %ls\n", country_id);
                     if((country_id) && (strcmp(proj_name, "OBS4MIPS")==0))
		       snprintf (insert_geo_query, sizeof (insert_geo_query),QUERY_INSERT_OBS_DIM_GEOLOCATION, lat, lon, atoi(country_id));
                     else
		       snprintf (insert_geo_query, sizeof (insert_geo_query),QUERY_INSERT_CMIP5_DIM_GEOLOCATION, lat, lon, atoi(country_id));

                     submit_query (conn, insert_geo_query);
                     free(country_code);
                     if(strcmp(proj_name, "OBS4MIPS")==0)
	                snprintf(select_id_geo_query, sizeof (select_id_geo_query), QUERY_GET_GEO_OBS_ID, lat, lon);
                     else
	                snprintf(select_id_geo_query, sizeof (select_id_geo_query), QUERY_GET_GEO_CMIP5_ID, lat, lon);
                     geo_obs_id=get_foreign_key_value(conn, select_id_geo_query);
	             // add entry to hash table
	             sprintf (geo_id_str, "%ld", geo_obs_id);
                     hashtbl_insert (hashtbl_obs_dim_geolocation, key_geo, geo_id_str);
	             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_geo, geo_id_str);
                  }
	      //}
	      //else
		//pmesg(LOG_ERROR,__FILE__,__LINE__,"Esgf-lookup error\n");

              free(remote_addr);
              remote_addr=NULL;
          }
              




         int miss_inst=0;
         int *array_id_inst=calloc(size_institute, sizeof(int));
         for(i=0; i<size_institute; i++)
         {  
            int inst_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_institute, institute[i]))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",institute[i], hashtbl_result);
               inst_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_id_inst[i]=inst_id;
            }
            else
            {
               miss_inst=1;
               char inst_id_str[128] = { '\0' };
               char select_id_inst_query[2048] = { '\0' };
               char insert_inst_query[2048] = { '\0' };
               char select_inst_query[2048] = { '\0' };
               success_lookup[1]++;
               if(strcmp(proj_name, "OBS4MIPS")==0)
	           snprintf (insert_inst_query, sizeof (insert_inst_query),QUERY_INSERT_OBS_DIM_INSTITUTE,institute[i]);
               else
	           snprintf (insert_inst_query, sizeof (insert_inst_query),QUERY_INSERT_CMIP5_DIM_INSTITUTE,institute[i]);
               submit_query (conn, insert_inst_query);
               
               if(strcmp(proj_name, "OBS4MIPS")==0)
	           snprintf(select_id_inst_query, sizeof (select_id_inst_query), QUERY_GET_INSTITUTE_OBS_ID,institute[i]);
               else
	           snprintf(select_id_inst_query, sizeof (select_id_inst_query), QUERY_GET_INSTITUTE_CMIP5_ID,institute[i]);
               inst_id=get_foreign_key_value(conn, select_id_inst_query);
               array_id_inst[i]=inst_id;
	       // add entry to hash table
	       sprintf (inst_id_str, "%ld", inst_id);
               hashtbl_insert (hashtbl_obs_dim_institute,institute[i],inst_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",institute[i], inst_id_str);
            }  
         }

         quickSort(array_id_inst,0,size_institute-1); 
         char inst_key[2048] = { '\0' };
         ap = inst_key;
         for(i=0; i<size_institute;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==size_institute-1)
                   ap += sprintf(ap, "%d", array_id_inst[i]);
                else
                   ap += sprintf(ap, ",%d,", array_id_inst[i]);
             
         }
         printf("inst_key %s\n", inst_key);
         if(miss_inst==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_inst_query[2048] = { '\0' };
            char array_id_inst_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            if(strcmp(proj_name, "OBS4MIPS")==0)
                 resp=submit_query_res (conn, QUERY_INST_GET_MAX_GROUP_OBS_ID,&res2);
            else
                 resp=submit_query_res (conn, QUERY_INST_GET_MAX_GROUP_CMIP5_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<size_institute;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                if(strcmp(proj_name, "OBS4MIPS")==0)
                    snprintf (insert_group_inst_query, sizeof (insert_group_inst_query),QUERY_INSERT_OBS_BRIDGE_INSTITUTE,array_id_inst[i],max_group_id+1);
                else
                    snprintf (insert_group_inst_query, sizeof (insert_group_inst_query),QUERY_INSERT_CMIP5_BRIDGE_INSTITUTE,array_id_inst[i],max_group_id+1);
                submit_query (conn, insert_group_inst_query);

                sprintf(array_id_inst_str, "%d", array_id_inst[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_obs_bridge_institute,array_id_inst_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_inst_str,max_group_id_str);
            } 
            institute_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_obs_bridge_institute_tmp,max_group_id_str,inst_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,inst_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_obs_bridge_institute_tmp, inst_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",inst_key, hashtbl_result);
               institute_group_key = atol(hashtbl_result);
               //printf("institute_group_key %d\n", institute_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_inst_query[2048] = { '\0' };
                if(strcmp(proj_name, "OBS4MIPS")==0)
                   resp=submit_query_res (conn, QUERY_INST_GET_MAX_GROUP_OBS_ID,&res2);
                else
                   resp=submit_query_res (conn, QUERY_INST_GET_MAX_GROUP_CMIP5_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                institute_group_key= max_group_id+1;
                  char array_id_inst_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<size_institute;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  if(strcmp(proj_name, "OBS4MIPS")==0)
                      snprintf (insert_group_inst_query, sizeof (insert_group_inst_query),QUERY_INSERT_OBS_BRIDGE_INSTITUTE,array_id_inst[i],max_group_id+1);
                  else
                      snprintf (insert_group_inst_query, sizeof (insert_group_inst_query),QUERY_INSERT_CMIP5_BRIDGE_INSTITUTE,array_id_inst[i],max_group_id+1);
                  submit_query (conn, insert_group_inst_query);
                  sprintf(array_id_inst_str, "%d", array_id_inst[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_obs_bridge_institute,array_id_inst_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_inst_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_obs_bridge_institute_tmp,max_group_id_str,inst_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,inst_key);
            }
        }
        free(array_id_inst);

         /* ob4mips_dim_variable and bridge */
         int miss_var=0;
         int *array_id_var=calloc(size_variable, sizeof(int));

         for(i=0; i<size_variable; i++)
         {  
            char var_hash[1028] = { '\0' };
            sprintf(var_hash,"%s:%s:%s", cf_standard_name[i], variable_long_name[i], variable[i]);
            int var_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_variable, var_hash))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n", var_hash, hashtbl_result);
               var_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_id_var[i]=var_id;
            }
            else
            {
               miss_var=1;
               char var_id_str[128] = { '\0' };
               char select_id_var_query[2048] = { '\0' };
               char insert_var_query[2048] = { '\0' };
               char select_var_query[2048] = { '\0' };
               success_lookup[1]++;
               if(strcmp(proj_name, "OBS4MIPS")==0)
	          snprintf (insert_var_query, sizeof (insert_var_query),QUERY_INSERT_OBS_DIM_VARIABLE, cf_standard_name[i], variable_long_name[i], variable[i]);
               else
	          snprintf (insert_var_query, sizeof (insert_var_query),QUERY_INSERT_CMIP5_DIM_VARIABLE, cf_standard_name[i], variable_long_name[i], variable[i]);

               submit_query (conn, insert_var_query);
               if(strcmp(proj_name, "OBS4MIPS")==0)
	          snprintf(select_id_var_query, sizeof (select_id_var_query), QUERY_GET_VARIABLE_OBS_ID,cf_standard_name[i], variable_long_name[i], variable[i]);
               else
	          snprintf(select_id_var_query, sizeof (select_id_var_query), QUERY_GET_VARIABLE_CMIP5_ID,cf_standard_name[i], variable_long_name[i], variable[i]);

               var_id=get_foreign_key_value(conn, select_id_var_query);
               array_id_var[i]=var_id;
	       // add entry to hash table
	       sprintf (var_id_str, "%ld", var_id);
               hashtbl_insert (hashtbl_obs_dim_variable,var_hash,var_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",var_hash, var_id_str);
            }  
         }


         quickSort(array_id_var,0,size_variable-1); 
         char var_key[2048] = { '\0' };
         ap = var_key;
         for(i=0; i<size_variable;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==size_variable-1)
                   ap += sprintf(ap, "%d", array_id_var[i]);
                else
                   ap += sprintf(ap, ",%d,", array_id_var[i]);
             
         }
         //printf("var_key %s\n", var_key);
         if(miss_var==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_var_query[2048] = { '\0' };
            char array_id_var_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            if(strcmp(proj_name, "OBS4MIPS")==0)
                resp=submit_query_res (conn, QUERY_VAR_GET_MAX_GROUP_OBS_ID,&res2);
            else
                resp=submit_query_res (conn, QUERY_VAR_GET_MAX_GROUP_CMIP5_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<size_variable;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                if(strcmp(proj_name, "OBS4MIPS")==0)
                   snprintf (insert_group_var_query, sizeof (insert_group_var_query),QUERY_INSERT_OBS_BRIDGE_VARIABLE,array_id_var[i],max_group_id+1);
                else
                   snprintf (insert_group_var_query, sizeof (insert_group_var_query),QUERY_INSERT_CMIP5_BRIDGE_VARIABLE,array_id_var[i],max_group_id+1);
                submit_query (conn, insert_group_var_query);

                sprintf(array_id_var_str, "%d", array_id_var[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_obs_bridge_variable,array_id_var_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_var_str,max_group_id_str);
            } 
            var_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_obs_bridge_variable_tmp,max_group_id_str,var_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,var_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_obs_bridge_variable_tmp, var_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",var_key, hashtbl_result);
               var_group_key = atol(hashtbl_result);
               //printf("variable_group_key %d\n", var_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_var_query[2048] = { '\0' };
                if(strcmp(proj_name, "OBS4MIPS")==0)
                    resp=submit_query_res (conn, QUERY_VAR_GET_MAX_GROUP_OBS_ID,&res2);
                else
                    resp=submit_query_res (conn, QUERY_VAR_GET_MAX_GROUP_CMIP5_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                var_group_key= max_group_id+1;
                  char array_id_var_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<size_variable;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  if(strcmp(proj_name, "OBS4MIPS")==0)
                      snprintf (insert_group_var_query, sizeof (insert_group_var_query),QUERY_INSERT_OBS_BRIDGE_VARIABLE,array_id_var[i],max_group_id+1);
                  else
                      snprintf (insert_group_var_query, sizeof (insert_group_var_query),QUERY_INSERT_CMIP5_BRIDGE_VARIABLE,array_id_var[i],max_group_id+1);
                  submit_query (conn, insert_group_var_query);
                  sprintf(array_id_var_str, "%d", array_id_var[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_obs_bridge_variable,array_id_var_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_var_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_obs_bridge_variable_tmp,max_group_id_str,var_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,var_key);
            }
             //devo controllare se esiste già il gruppo
         }
         free(array_id_var);

         /* ob4mips_dim_time_frequency and bridge */
         int miss_time_freq=0;
         int *array_id_time_freq=calloc(size_time, sizeof(int));

         for(i=0; i<size_time; i++)
         {  
            int time_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_time_frequency, time_frequency[i]))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n", time_frequency[i], hashtbl_result);
               time_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_id_time_freq[i]=time_id;
            }
            else
            {
               miss_time_freq=1;
               char time_freq_id_str[128] = { '\0' };
               char select_id_time_freq_query[2048] = { '\0' };
               char insert_time_freq_query[2048] = { '\0' };
               char select_time_freq_query[2048] = { '\0' };
               success_lookup[1]++;
               if(strcmp(proj_name, "OBS4MIPS")==0)
	         snprintf (insert_time_freq_query, sizeof (insert_time_freq_query),QUERY_INSERT_OBS_DIM_TIME_FREQUENCY, time_frequency[i]);
               else
	         snprintf (insert_time_freq_query, sizeof (insert_time_freq_query),QUERY_INSERT_CMIP5_DIM_TIME_FREQUENCY, time_frequency[i]);
               submit_query (conn, insert_time_freq_query);
               if(strcmp(proj_name, "OBS4MIPS")==0)
	         snprintf(select_id_time_freq_query, sizeof (select_id_time_freq_query), QUERY_GET_TIME_FREQ_OBS_ID,time_frequency[i]);
               else
	         snprintf(select_id_time_freq_query, sizeof (select_id_time_freq_query), QUERY_GET_TIME_FREQ_CMIP5_ID,time_frequency[i]);
               time_id=get_foreign_key_value(conn, select_id_time_freq_query);
               array_id_time_freq[i]=time_id;
	       // add entry to hash table
	       sprintf (time_freq_id_str, "%ld", time_id);
               hashtbl_insert (hashtbl_obs_dim_time_frequency,time_frequency[i],time_freq_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",time_frequency[i], time_freq_id_str);
            }  
         }


         quickSort(array_id_time_freq,0,size_time-1); 
         char time_freq_key[2048] = { '\0' };
         ap = time_freq_key;
         for(i=0; i<size_time;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==size_time-1)
                   ap += sprintf(ap, "%d", array_id_time_freq[i]);
                else
                   ap += sprintf(ap, ",%d,", array_id_time_freq[i]);
             
         }
         //printf("var_key %s\n", time_freq_key);
         if(miss_time_freq==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_time_freq_query[2048] = { '\0' };
            char array_id_time_freq_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            if(strcmp(proj_name, "OBS4MIPS")==0)
               resp=submit_query_res (conn, QUERY_TIME_FREQ_GET_MAX_GROUP_OBS_ID,&res2);
            else
               resp=submit_query_res (conn, QUERY_TIME_FREQ_GET_MAX_GROUP_CMIP5_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<size_time;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                if(strcmp(proj_name, "OBS4MIPS")==0)
                   snprintf (insert_group_time_freq_query, sizeof (insert_group_time_freq_query),QUERY_INSERT_OBS_BRIDGE_TIME_FREQUENCY,array_id_time_freq[i],max_group_id+1);
                else
                   snprintf (insert_group_time_freq_query, sizeof (insert_group_time_freq_query),QUERY_INSERT_CMIP5_BRIDGE_TIME_FREQUENCY,array_id_time_freq[i],max_group_id+1);
                submit_query (conn, insert_group_time_freq_query);

                sprintf(array_id_time_freq_str, "%d", array_id_time_freq[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_obs_bridge_time_frequency,array_id_time_freq_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_time_freq_str,max_group_id_str);
            } 
            time_freq_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_obs_bridge_time_frequency_tmp,max_group_id_str,time_freq_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,time_freq_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_obs_bridge_time_frequency_tmp, time_freq_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",time_freq_key, hashtbl_result);
               time_freq_group_key = atol(hashtbl_result);
               //printf("variable_group_key %d\n", time_freq_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_time_freq_query[2048] = { '\0' };
                if(strcmp(proj_name, "OBS4MIPS")==0)
                   resp=submit_query_res (conn, QUERY_TIME_FREQ_GET_MAX_GROUP_OBS_ID,&res2);
                else
                   resp=submit_query_res (conn, QUERY_TIME_FREQ_GET_MAX_GROUP_CMIP5_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                time_freq_group_key= max_group_id+1;
                  char array_id_time_freq_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<size_time;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  if(strcmp(proj_name, "OBS4MIPS")==0)
                     snprintf (insert_group_time_freq_query, sizeof (insert_group_time_freq_query),QUERY_INSERT_OBS_BRIDGE_TIME_FREQUENCY,array_id_time_freq[i],max_group_id+1);
                  else
                     snprintf (insert_group_time_freq_query, sizeof (insert_group_time_freq_query),QUERY_INSERT_CMIP5_BRIDGE_TIME_FREQUENCY,array_id_time_freq[i],max_group_id+1);
                  submit_query (conn, insert_group_time_freq_query);
                  sprintf(array_id_time_freq_str, "%d", array_id_time_freq[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_obs_bridge_time_frequency,array_id_time_freq_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_time_freq_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_obs_bridge_time_frequency_tmp,max_group_id_str,time_freq_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,time_freq_key);
            }
             //devo controllare se esiste già il gruppo
         }
        free(array_id_time_freq);
        if(strcmp(proj_name, "OBS4MIPS")==0)
        { 
         /* ob4mips_dim_processing level and bridge */
         int miss_proc_lev=0;
         int *array_id_proc_lev=calloc(size_processing, sizeof(int));

         for(i=0; i<size_processing; i++)
         {  
            int processing_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_processing_level, processing_level[i]))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n", processing_level[i], hashtbl_result);
               processing_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_id_proc_lev[i]=processing_id;
            }
            else
            {
               miss_proc_lev=1;
               char proc_lev_id_str[128] = { '\0' };
               char select_id_proc_lev[2048] = { '\0' };
               char insert_proc_lev_query[2048] = { '\0' };
               char select_id_proc_lev_query[2048] = { '\0' };
               success_lookup[1]++;
	       snprintf (insert_proc_lev_query, sizeof (insert_proc_lev_query),QUERY_INSERT_OBS_DIM_PROCESSING_LEVEL, processing_level[i]);
               submit_query (conn, insert_proc_lev_query);
               if(strcmp(proj_name, "OBS4MIPS")==0)
	          snprintf(select_id_proc_lev_query, sizeof (select_id_proc_lev_query), QUERY_GET_PROC_LEV_OBS_ID,processing_level[i]);
               else
	          snprintf(select_id_proc_lev_query, sizeof (select_id_proc_lev_query), QUERY_GET_PROC_LEV_CMIP5_ID,processing_level[i]);
               processing_id=get_foreign_key_value(conn, select_id_proc_lev_query);
               array_id_proc_lev[i]=processing_id;
	       // add entry to hash table
	       sprintf (proc_lev_id_str, "%ld", processing_id);
               hashtbl_insert (hashtbl_obs_dim_processing_level,processing_level[i],proc_lev_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",processing_level[i], proc_lev_id_str);
            }  
         }


         quickSort(array_id_proc_lev,0,size_processing-1); 
         char proc_lev_key[2048] = { '\0' };
         ap = proc_lev_key;
         for(i=0; i<size_processing;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==size_processing-1)
                   ap += sprintf(ap, "%d", array_id_proc_lev[i]);
                else
                   ap += sprintf(ap, ",%d,", array_id_proc_lev[i]);
             
         }
         //printf("proc_lev_key %s\n", proc_lev_key);
         if(miss_proc_lev==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_proc_lev_query[2048] = { '\0' };
            char array_id_proc_lev_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            if(strcmp(proj_name, "OBS4MIPS")==0)
               resp=submit_query_res (conn, QUERY_PROC_LEV_GET_MAX_GROUP_OBS_ID,&res2);
            else
               resp=submit_query_res (conn, QUERY_PROC_LEV_GET_MAX_GROUP_CMIP5_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<size_processing;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                snprintf (insert_group_proc_lev_query, sizeof (insert_group_proc_lev_query),QUERY_INSERT_OBS_BRIDGE_PROCESSING_LEVEL,array_id_proc_lev[i],max_group_id+1);
                submit_query (conn, insert_group_proc_lev_query);

                sprintf(array_id_proc_lev_str, "%d", array_id_proc_lev[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_obs_bridge_processing_level,array_id_proc_lev_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_proc_lev_str,max_group_id_str);
            } 
            proc_lev_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_obs_bridge_processing_level_tmp,max_group_id_str,proc_lev_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,proc_lev_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_obs_bridge_processing_level_tmp, proc_lev_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",proc_lev_key, hashtbl_result);
               proc_lev_group_key = atol(hashtbl_result);
               //printf("proc level_group_key %d\n", proc_lev_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_proc_lev_query[2048] = { '\0' };
                if(strcmp(proj_name, "OBS4MIPS")==0)
                   resp=submit_query_res (conn, QUERY_PROC_LEV_GET_MAX_GROUP_OBS_ID,&res2);
                else
                   resp=submit_query_res (conn, QUERY_PROC_LEV_GET_MAX_GROUP_CMIP5_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                proc_lev_group_key= max_group_id+1;
                  char array_id_proc_lev_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<size_processing;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  snprintf (insert_group_proc_lev_query, sizeof (insert_group_proc_lev_query),QUERY_INSERT_OBS_BRIDGE_PROCESSING_LEVEL,array_id_proc_lev[i],max_group_id+1);
                  submit_query (conn, insert_group_proc_lev_query);
                  sprintf(array_id_proc_lev_str, "%d", array_id_proc_lev[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_obs_bridge_processing_level,array_id_proc_lev_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_proc_lev_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_obs_bridge_processing_level_tmp,max_group_id_str,proc_lev_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,proc_lev_key);
            }
             //devo controllare se esiste già il gruppo
         }
        free(array_id_proc_lev);
      }

         /* ob4mips_dim_source_id and bridge */
      if(strcmp(proj_name, "OBS4MIPS")==0)
      {
         int miss_source_id=0;
         int *array_id_source_id=calloc(size_source, sizeof(int));

         for(i=0; i<size_source; i++)
         {  
            int source_id_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_source_id, source_id[i]))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n", source_id[i], hashtbl_result);
               source_id_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_id_source_id[i]=source_id_id;
            }
            else
            {
               miss_source_id=1;
               char source_id_id_str[128] = { '\0' };
               char select_id_source_id[2048] = { '\0' };
               char insert_source_id_query[2048] = { '\0' };
               char select_id_source_id_query[2048] = { '\0' };
               success_lookup[1]++;
	       snprintf (insert_source_id_query, sizeof (insert_source_id_query),QUERY_INSERT_OBS_DIM_SOURCE_ID, source_id[i]);
               submit_query (conn, insert_source_id_query);
	       snprintf(select_id_source_id_query, sizeof (select_id_source_id_query), QUERY_GET_SOURCE_ID_ID,source_id[i]);
               source_id_id=get_foreign_key_value(conn, select_id_source_id_query);
               array_id_source_id[i]=source_id_id;
	       // add entry to hash table
	       sprintf (source_id_id_str, "%ld", source_id_id);
               hashtbl_insert (hashtbl_obs_dim_source_id,source_id[i],source_id_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",source_id[i], source_id_id_str);
            }  
         }


         quickSort(array_id_source_id,0,size_source-1); 
         char source_id_key[2048] = { '\0' };
         ap = source_id_key;
         for(i=0; i<size_source;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==size_source-1)
                   ap += sprintf(ap, "%d", array_id_source_id[i]);
                else
                   ap += sprintf(ap, ",%d,", array_id_source_id[i]);
             
         }
         //printf("source_id_key %s\n", source_id_key);
         if(miss_source_id==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_source_id_query[2048] = { '\0' };
            char array_id_source_id_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            resp=submit_query_res (conn, QUERY_SOURCE_ID_GET_MAX_GROUP_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<size_source;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                snprintf (insert_group_source_id_query, sizeof (insert_group_source_id_query),QUERY_INSERT_OBS_BRIDGE_SOURCE_ID,array_id_source_id[i],max_group_id+1);
                submit_query (conn, insert_group_source_id_query);

                sprintf(array_id_source_id_str, "%d", array_id_source_id[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_obs_bridge_source_id,array_id_source_id_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_source_id_str,max_group_id_str);
            } 
            source_id_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_obs_bridge_source_id_tmp,max_group_id_str,source_id_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,source_id_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_obs_bridge_source_id_tmp, source_id_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",source_id_key, hashtbl_result);
               source_id_group_key = atol(hashtbl_result);
               //printf("source id_group_key %d\n", source_id_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_source_id_query[2048] = { '\0' };
                resp=submit_query_res (conn, QUERY_SOURCE_ID_GET_MAX_GROUP_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                source_id_group_key= max_group_id+1;
                  char array_id_source_id_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<size_source;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  snprintf (insert_group_source_id_query, sizeof (insert_group_source_id_query),QUERY_INSERT_OBS_BRIDGE_SOURCE_ID,array_id_source_id[i],max_group_id+1);
                  submit_query (conn, insert_group_source_id_query);
                  sprintf(array_id_source_id_str, "%d", array_id_source_id[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_obs_bridge_source_id,array_id_source_id_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_source_id_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_obs_bridge_source_id_tmp,max_group_id_str,source_id_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,source_id_key);
            }
             //devo controllare se esiste già il gruppo
         }
        free(array_id_source_id);
       }

         /* ob4mips_dim_realm and bridge */
         int miss_realm=0;
         int *array_id_realm=calloc(size_realm, sizeof(int));

         for(i=0; i<size_realm; i++)
         {  
            int realm_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_realm, realm[i]))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n", realm[i], hashtbl_result);
               realm_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_id_realm[i]=realm_id;
            }
            else
            {
               miss_realm=1;
               char realm_id_str[128] = { '\0' };
               char select_id_realm[2048] = { '\0' };
               char insert_realm_query[2048] = { '\0' };
               char select_id_realm_query[2048] = { '\0' };
               success_lookup[1]++;
               if(strcmp(proj_name, "OBS4MIPS")==0)
	          snprintf (insert_realm_query, sizeof (insert_realm_query),QUERY_INSERT_OBS_DIM_REALM, realm[i]);
               else
	          snprintf (insert_realm_query, sizeof (insert_realm_query),QUERY_INSERT_CMIP5_DIM_REALM, realm[i]);
               submit_query (conn, insert_realm_query);
               if(strcmp(proj_name, "OBS4MIPS")==0)
	          snprintf(select_id_realm_query, sizeof (select_id_realm_query), QUERY_GET_REALM_OBS_ID,realm[i]);
               else
	          snprintf(select_id_realm_query, sizeof (select_id_realm_query), QUERY_GET_REALM_CMIP5_ID,realm[i]);
               realm_id=get_foreign_key_value(conn, select_id_realm_query);
               array_id_realm[i]=realm_id;
	       // add entry to hash table
	       sprintf (realm_id_str, "%ld", realm_id);
               hashtbl_insert (hashtbl_obs_dim_realm,realm[i],realm_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",realm[i], realm_id_str);
            }  
         }


         quickSort(array_id_realm,0,size_realm-1); 
         char realm_key[2048] = { '\0' };
         ap = realm_key;
         for(i=0; i<size_realm;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==size_realm-1)
                   ap += sprintf(ap, "%d", array_id_realm[i]);
                else
                   ap += sprintf(ap, ",%d,", array_id_realm[i]);
             
         }
         //printf("realm_key %s\n", realm_key);
         if(miss_realm==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_realm_query[2048] = { '\0' };
            char array_id_realm_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            if(strcmp(proj_name, "OBS4MIPS")==0)
               resp=submit_query_res (conn, QUERY_REALM_GET_MAX_GROUP_OBS_ID,&res2);
            else
               resp=submit_query_res (conn, QUERY_REALM_GET_MAX_GROUP_CMIP5_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<size_realm;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                if(strcmp(proj_name, "OBS4MIPS")==0)
                    snprintf (insert_group_realm_query, sizeof (insert_group_realm_query),QUERY_INSERT_OBS_BRIDGE_REALM,array_id_realm[i],max_group_id+1);
                else
                    snprintf (insert_group_realm_query, sizeof (insert_group_realm_query),QUERY_INSERT_CMIP5_BRIDGE_REALM,array_id_realm[i],max_group_id+1);
                submit_query (conn, insert_group_realm_query);

                sprintf(array_id_realm_str, "%d", array_id_realm[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_obs_bridge_realm,array_id_realm_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_realm_str,max_group_id_str);
            } 
            realm_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_obs_bridge_realm_tmp,max_group_id_str,realm_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,realm_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_obs_bridge_realm_tmp, realm_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",realm_key, hashtbl_result);
               realm_group_key = atol(hashtbl_result);
               //printf("realm_group_key %d\n", realm_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_realm_query[2048] = { '\0' };
                if(strcmp(proj_name, "OBS4MIPS")==0)
                    resp=submit_query_res (conn, QUERY_REALM_GET_MAX_GROUP_OBS_ID,&res2);
                else
                    resp=submit_query_res (conn, QUERY_REALM_GET_MAX_GROUP_CMIP5_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                realm_group_key= max_group_id+1;
                  char array_id_realm_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<size_realm;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  if(strcmp(proj_name, "OBS4MIPS")==0)
                     snprintf (insert_group_realm_query, sizeof (insert_group_realm_query),QUERY_INSERT_OBS_BRIDGE_REALM,array_id_realm[i],max_group_id+1);
                  else
                     snprintf (insert_group_realm_query, sizeof (insert_group_realm_query),QUERY_INSERT_CMIP5_BRIDGE_REALM,array_id_realm[i],max_group_id+1);

                  submit_query (conn, insert_group_realm_query);
                  sprintf(array_id_realm_str, "%d", array_id_realm[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_obs_bridge_realm,array_id_realm_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_realm_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_obs_bridge_realm_tmp,max_group_id_str,realm_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,realm_key);
            }
             //devo controllare se esiste già il gruppo
         }
        free(array_id_realm);
         /* cmip5_dim_model and bridge */
      if(strcmp(proj_name, "CMIP5")==0)
      {
         int miss_model=0;
         int *array_model_id=calloc(size_model, sizeof(int));

         for(i=0; i<size_model; i++)
         {  
            int model_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_cmip5_dim_model, model[i]))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n", model[i], hashtbl_result);
               model_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_model_id[i]=model_id;
            }
            else
            {
               miss_model=1;
               char model_id_str[128] = { '\0' };
               char select_model_id[2048] = { '\0' };
               char insert_model_query[2048] = { '\0' };
               char select_model_id_query[2048] = { '\0' };
               success_lookup[1]++;
	       snprintf (insert_model_query, sizeof (insert_model_query),QUERY_INSERT_CMIP5_DIM_MODEL, model[i]);
               submit_query (conn, insert_model_query);
	       snprintf(select_model_id_query, sizeof (select_model_id_query), QUERY_GET_MODEL_CMIP5_ID,model[i]);
               model_id=get_foreign_key_value(conn, select_model_id_query);
               array_model_id[i]=model_id;
	       // add entry to hash table
	       sprintf (model_id_str, "%ld", model_id);
               hashtbl_insert (hashtbl_cmip5_dim_model,model[i],model_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",model[i], model_id_str);
            }  
         }


         quickSort(array_model_id,0,size_model-1); 
         char model_key[2048] = { '\0' };
         ap = model_key;
         for(i=0; i<size_model;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==size_model-1)
                   ap += sprintf(ap, "%d", array_model_id[i]);
                else
                   ap += sprintf(ap, ",%d,", array_model_id[i]);
             
         }
         printf("model_key %s\n", model_key);
         if(miss_model==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_model_query[2048] = { '\0' };
            char array_model_id_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            resp=submit_query_res (conn, QUERY_MODEL_GET_MAX_GROUP_CMIP5_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<size_model;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                snprintf (insert_group_model_query, sizeof (insert_group_model_query),QUERY_INSERT_CMIP5_BRIDGE_MODEL,array_model_id[i],max_group_id+1);
                submit_query (conn, insert_group_model_query);

                sprintf(array_model_id_str, "%d", array_model_id[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_cmip5_bridge_model,array_model_id_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_model_id_str,max_group_id_str);
            } 
            model_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_cmip5_bridge_model_tmp,max_group_id_str,model_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,model_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_cmip5_bridge_model_tmp, model_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",model_key, hashtbl_result);
               model_group_key = atol(hashtbl_result);
               //printf("source id_group_key %d\n", model_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_model_query[2048] = { '\0' };
                resp=submit_query_res (conn, QUERY_MODEL_GET_MAX_GROUP_CMIP5_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                model_group_key= max_group_id+1;
                  char array_model_id_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<size_model;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  snprintf (insert_group_model_query, sizeof (insert_group_model_query),QUERY_INSERT_CMIP5_BRIDGE_MODEL,array_model_id[i],max_group_id+1);
                  submit_query (conn, insert_group_model_query);
                  sprintf(array_model_id_str, "%d", array_model_id[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_cmip5_bridge_model,array_model_id_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_model_id_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_cmip5_bridge_model_tmp,max_group_id_str,model_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,model_key);
            }
             //devo controllare se esiste già il gruppo
         }
        free(array_model_id);
      }
         /* cmip5_dim_experiment and bridge */
      if(strcmp(proj_name, "CMIP5")==0)
      {
         int miss_exp=0;
         int *array_exp_id=calloc(size_exp, sizeof(int));

         for(i=0; i<size_exp; i++)
         {  
            int exp_id=0;
            if (hashtbl_result = hashtbl_get (hashtbl_cmip5_dim_experiment, experiment[i]))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n", experiment[i], hashtbl_result);
               exp_id = atoi (hashtbl_result);
               success_lookup[0]++;
               array_exp_id[i]=exp_id;
            }
            else
            {
               miss_exp=1;
               char exp_id_str[128] = { '\0' };
               char select_exp_id[2048] = { '\0' };
               char insert_exp_query[2048] = { '\0' };
               char select_exp_id_query[2048] = { '\0' };
               success_lookup[1]++;
	       snprintf (insert_exp_query, sizeof (insert_exp_query),QUERY_INSERT_CMIP5_DIM_EXPERIMENT, experiment[i]);
               submit_query (conn, insert_exp_query);
	       snprintf(select_exp_id_query, sizeof (select_exp_id_query), QUERY_GET_EXPERIMENT_CMIP5_ID,experiment[i]);
               exp_id=get_foreign_key_value(conn, select_exp_id_query);
               array_exp_id[i]=exp_id;
	       // add entry to hash table
	       sprintf (exp_id_str, "%ld", exp_id);
               hashtbl_insert (hashtbl_cmip5_dim_experiment,experiment[i],exp_id_str);
	       pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",experiment[i], exp_id_str);
            }  
         }


         quickSort(array_exp_id,0,size_exp-1); 
         char exp_key[2048] = { '\0' };
         ap = exp_key;
         for(i=0; i<size_exp;i++)
         {
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==size_exp-1)
                   ap += sprintf(ap, "%d", array_exp_id[i]);
                else
                   ap += sprintf(ap, ",%d,", array_exp_id[i]);
             
         }
         //printf("exp_key %s\n", exp_key);
         if(miss_exp==1)
         {
            PGresult   *res2;
            int resp=0;
            int max_group_id=0;
            char insert_group_exp_query[2048] = { '\0' };
            char array_exp_id_str[2048] = { '\0' };
            char max_group_id_str[2048] = { '\0' };
            resp=submit_query_res (conn, QUERY_EXPERIMENT_GET_MAX_GROUP_CMIP5_ID,&res2);
            if(resp!=-1)
            {
               for (i = 0; i < PQntuples(res2); i++)
               {
                  max_group_id=atol(PQgetvalue(res2, i, 0));
               }
               PQclear (res2);
            }
            for(i=0; i<size_exp;i++)
            {
                //sicuramente devo aggiungere un nuovo gruppo
                snprintf (insert_group_exp_query, sizeof (insert_group_exp_query),QUERY_INSERT_CMIP5_BRIDGE_EXPERIMENT,array_exp_id[i],max_group_id+1);
                submit_query (conn, insert_group_exp_query);

                sprintf(array_exp_id_str, "%d", array_exp_id[i]);
                sprintf(max_group_id_str, "%d", max_group_id+1);
                hashtbl_insert (hashtbl_cmip5_bridge_experiment,array_exp_id_str,max_group_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_exp_id_str,max_group_id_str);
            } 
            exp_group_key= max_group_id+1;
            hashtbl_insert (hashtbl_cmip5_bridge_experiment_tmp,max_group_id_str,exp_key);
            pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,exp_key);
         }
         else
         {
            if (hashtbl_result = hashtbl_get (hashtbl_cmip5_bridge_experiment_tmp, exp_key))
            {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%d] [%s]\n",exp_key, hashtbl_result);
               exp_group_key = atol(hashtbl_result);
               //printf("experiment_group_key %d\n", exp_group_key);
               success_lookup[0]++;
            }
            else
            {
                PGresult   *res2;
                int max_group_id=0;
                int resp=0;
                char insert_group_exp_query[2048] = { '\0' };
                resp=submit_query_res (conn, QUERY_EXPERIMENT_GET_MAX_GROUP_CMIP5_ID,&res2);
                if(resp!=-1)
                {
                   for (i = 0; i < PQntuples(res2); i++)
                   {
                      max_group_id=atoi(PQgetvalue(res2, i, 0));
                   }
                   PQclear (res2);
                
                }
                exp_group_key= max_group_id+1;
                  char array_id_exp_str[2048] = { '\0' };
                  char max_group_id_str[2048] = { '\0' };
                for(i=0; i<size_exp;i++)
                {
                  //sicuramente devo aggiungere un nuovo gruppo
                  snprintf (insert_group_exp_query, sizeof (insert_group_exp_query),QUERY_INSERT_CMIP5_BRIDGE_EXPERIMENT,array_exp_id[i],max_group_id+1);
                  submit_query (conn, insert_group_exp_query);
                  sprintf(array_id_exp_str, "%d", array_exp_id[i]);
                  sprintf(max_group_id_str, "%d", max_group_id+1);
                  hashtbl_insert (hashtbl_cmip5_bridge_experiment,array_id_exp_str,max_group_id_str);
                  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",array_id_exp_str,max_group_id_str);
                }
                hashtbl_insert (hashtbl_cmip5_bridge_experiment_tmp,max_group_id_str,exp_key);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",max_group_id_str,exp_key);
            }
             //devo controllare se esiste già il gruppo
         }
         free(array_exp_id);
       }


         char dataset_name[2048]={ '\0' };
         long int file_id = 0;              
          //
          //Obs4MIPS project
          //
          char filen[2048]={ '\0' };
          char filename[2048]={ '\0' };
          char ver[2048]={ '\0' };
          char *str=NULL;
          char *str1=NULL;
          str=strdup((*datasetproj)[cnt]->id);
          str1=strrchr(str, '|');
          if(str1!=NULL)
            *str1='\0';
          str1=strstr(str, ".nc");
          if(str1!=NULL)
            *str1='\0';
          str1=strrchr(str, '.');
          if(str1!=NULL)
          {
            sprintf(filen, "%s.nc:%d",str1+1, size_row);
            sprintf(filename, "%s.nc", str1+1);
          }
          if(str1!=NULL)
            *str1='\0';
          str1=strrchr(str, '.');
          if(str1!=NULL)
          {
            sprintf(ver, "%s", str1+1);
            *str1='\0';
            sprintf(dataset_name, "%s", str);
          }
 
          free(str);
         if(strcmp(proj_name, "OBS4MIPS")==0)
         {
          if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_file, filen))
          {
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",buf, hashtbl_result);
             file_id = atol (hashtbl_result);
             success_lookup[0]++;
          }
          else
          {             // add host entry in DB (and hashtable too) without geolocation information

             char file_id_str[128] = { '\0' };
             char insert_new_file_query[2048] = { '\0' };
             char select_id_file_query[2048] = { '\0' };

             success_lookup[1]++;
             snprintf (insert_new_file_query, sizeof (insert_new_file_query), QUERY_INSERT_NEW_FILE, filename, size_row);
             submit_query (conn, insert_new_file_query);

             snprintf(select_id_file_query, sizeof (select_id_file_query), QUERY_GET_FILE_ID, filename, size_row);
             file_id=get_foreign_key_value(conn, select_id_file_query);
             // add entry to hash table
              sprintf (file_id_str, "%ld", file_id);
             hashtbl_insert (hashtbl_obs_dim_file, filen, file_id_str);
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%d] [%s]\n",file_id, file_id_str);
          }

         }
          long int dataset_id_key = 0;              
         if((strcmp(proj_name, "OBS4MIPS")==0) || (strcmp(proj_name, "CMIP5")==0))
         {

          if (hashtbl_result = hashtbl_get (hashtbl_obs_dim_dataset, dataset_name))
          {
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",dataset_name, hashtbl_result);
             dataset_id_key = atol (hashtbl_result);
             success_lookup[0]++;
          }
          else
          {             // add host entry in DB (and hashtable too) without geolocation information

             char dataset_id_str[128] = { '\0' };
             char insert_new_dataset_query[2048] = { '\0' };
             char select_id_dataset_query[2048] = { '\0' };

             success_lookup[1]++;
             if(strcmp(proj_name, "OBS4MIPS")==0)
                 snprintf (insert_new_dataset_query, sizeof (insert_new_dataset_query), QUERY_INSERT_NEW_DATASET_OBS, dataset_name, atoi((*datasetproj)[cnt]->version), datetime_start, datetime_stop);
             else
                 snprintf (insert_new_dataset_query, sizeof (insert_new_dataset_query), QUERY_INSERT_NEW_DATASET_CMIP5, dataset_name, atoi((*datasetproj)[cnt]->version), datetime_start, datetime_stop);
             submit_query (conn, insert_new_dataset_query);

             if(strcmp(proj_name, "OBS4MIPS")==0)
                snprintf(select_id_dataset_query, sizeof (select_id_dataset_query), QUERY_GET_DATASET_OBS_ID, dataset_name);
             else
                snprintf(select_id_dataset_query, sizeof (select_id_dataset_query), QUERY_GET_DATASET_CMIP5_ID, dataset_name);
              dataset_id_key=get_foreign_key_value(conn, select_id_dataset_query);
             // add entry to hash table
              sprintf (dataset_id_str, "%ld", dataset_id_key);
             hashtbl_insert (hashtbl_obs_dim_dataset, dataset_name, dataset_id_str);
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%d] [%s]\n",dataset_id_key, dataset_id_str);
          }
        }
        if((strcmp(proj_name, "OBS4MIPS")==0) || (strcmp(proj_name, "CMIP5")==0))
        {
          if(strcmp(proj_name, "OBS4MIPS")==0)
          {
             char *str_succ=NULL;
             if(strcmp(success_row,"t")==0)
                  str_succ=strdup("true");
             else
                 str_succ=strdup("false");
             char fact_obs_row[2048] = { '\0' };
             long int fact_obs_id=0;
             sprintf(fact_obs_row, "%d:%s:%d:%s:%s:%s:%s:%s:%s:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d", size_row, str_succ, duration_row, user_id_hash_row, esgf_node, user_idp_row, buf3, buf4, index_node, dataset_id_key, file_id, geo_obs_id, date_obs_id, institute_group_key, var_group_key, time_freq_group_key, proc_lev_group_key, source_id_group_key, realm_group_key);
             free(str_succ);
             //printf("*************fact_obs_row*****%s\n", fact_obs_row); 
             if (hashtbl_result = hashtbl_get (hashtbl_obs4mips_fact_download, fact_obs_row))
             {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",fact_row, hashtbl_result);
               fact_obs_id = atol (hashtbl_result);
	        success_lookup[0]++;
             }
             else
	     {		// add host entry in DB (and hashtable too) without geolocation information

                char insert_obs4mips_fact_download[2048] = { '\0' };
                char select_id_fact_query[2048] = { '\0' };
                char fact_obs_id_str[256] = { '\0' };
                snprintf (insert_obs4mips_fact_download, sizeof (insert_obs4mips_fact_download), QUERY_INSERT_OBS_FACT_DOWNLOAD,size_row, success_row, duration_row, user_id_hash_row, esgf_node, user_idp_row, buf3, buf4, index_node, dataset_id_key, file_id, geo_obs_id, date_obs_id, institute_group_key, var_group_key, time_freq_group_key, proc_lev_group_key, source_id_group_key, realm_group_key);
	        submit_query (conn, insert_obs4mips_fact_download);

                snprintf(select_id_fact_query, sizeof (select_id_fact_query), QUERY_GET_OBS_ROW, size_row, success_row, duration_row, user_id_hash_row, esgf_node, user_idp_row, buf3, buf4, index_node, dataset_id_key, file_id, geo_obs_id, date_obs_id, institute_group_key, var_group_key, time_freq_group_key, proc_lev_group_key, source_id_group_key, realm_group_key);

                //printf("select_id_fact_query vale %s\n", select_id_fact_query);
                fact_obs_id=get_foreign_key_value(conn, select_id_fact_query);
                sprintf (fact_obs_id_str, "%ld", fact_obs_id);
                hashtbl_insert (hashtbl_obs4mips_fact_download,fact_obs_row,fact_obs_id_str);
                //printf("hashtable [%s]*****[%s]\n",fact_obs_row,fact_obs_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",fact_obs_row,fact_obs_id_str);
             }
          }
          else
          {
             char *str_succ=NULL;
             if(strcmp(success_row,"t")==0)
                  str_succ=strdup("true");
             else
                 str_succ=strdup("false");
             char fact_cmip_row[2048] = { '\0' };
             long int fact_cmip_id=0;
             sprintf(fact_cmip_row,"%d,%s,%d,%s,%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d", size_row, str_succ, duration_row, replica, esgf_node, buf3, buf4, user_id_hash_row, user_idp_row, date_obs_id, geo_obs_id, dataset_id_key, time_freq_group_key, var_group_key, exp_group_key, model_group_key, realm_group_key, institute_group_key); 
             free(str_succ);
             if (hashtbl_result = hashtbl_get (hashtbl_cmip5_fact_download, fact_cmip_row))
             {
               pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",fact_row, hashtbl_result);
               fact_cmip_id = atol (hashtbl_result);
	        success_lookup[0]++;
             }
             else
	     {		// add host entry in DB (and hashtable too) without geolocation information
                 char insert_cmip5_fact_download[2048] = { '\0' };
                 char select_id_fact_query[2048] = { '\0' };
                 char fact_cmip_id_str[256] = { '\0' };
                 char fact1_row[2048] = { '\0' };
                 sprintf(fact1_row, "%d,%s,%d,%s,%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d", size_row, success_row, duration_row, replica, esgf_node, buf3, buf4, user_id_hash_row, user_idp_row, date_obs_id, geo_obs_id, dataset_id_key, time_freq_group_key, var_group_key, exp_group_key, model_group_key, realm_group_key, institute_group_key);
                  //printf("fact1_row %s\n", fact1_row);
                  snprintf (insert_cmip5_fact_download, sizeof (insert_cmip5_fact_download), QUERY_INSERT_CMIP5_FACT_DOWNLOAD,size_row, success_row, duration_row, replica, esgf_node, buf3, buf4, user_id_hash_row, user_idp_row, date_obs_id, geo_obs_id, dataset_id_key, time_freq_group_key, var_group_key, exp_group_key, model_group_key, realm_group_key, institute_group_key);
                 //printf("insert_cmip5_fact_download vale %s\n", insert_cmip5_fact_download);
	         submit_query (conn, insert_cmip5_fact_download);
                 snprintf(select_id_fact_query, sizeof (select_id_fact_query), QUERY_GET_CMIP5_ROW, size_row, success_row, duration_row, replica, esgf_node, buf3, buf4, user_id_hash_row, user_idp_row, date_obs_id, geo_obs_id, dataset_id_key, time_freq_group_key, var_group_key, exp_group_key, model_group_key, realm_group_key, institute_group_key);

                //printf("select_id_fact_query vale %s\n", select_id_fact_query);
                fact_cmip_id=get_foreign_key_value(conn, select_id_fact_query);
                sprintf (fact_cmip_id_str, "%ld", fact_cmip_id);
                hashtbl_insert (hashtbl_cmip5_fact_download,fact_cmip_row,fact_cmip_id_str);
                printf("hashtable [%s]*****[%s]\n",fact_cmip_row,fact_cmip_id_str);
                pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",fact_cmip_row,fact_cmip_id_str);
              }

          }
        }

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"*********** End parsing routine  ************\n");
     free(user_id_hash_row);
     user_id_hash_row=NULL;
     free(user_idp_row);
     user_idp_row=NULL;
     free(success_row);
     success_row=NULL;
     
     char update_dashboard_queue[2048] = { '\0' };
     
     snprintf (update_dashboard_queue, sizeof (update_dashboard_queue), QUERY_UPDATE_DASHBOARD_QUEUE,(*datasetproj)[cnt]->id_query);
     submit_query (conn, update_dashboard_queue);
     myfree_array(institute,size_institute+1);
     myfree_array(cf_standard_name,size_cf+1);
     myfree_array(variable,size_variable+1);
     myfree_array(variable_long_name,size_variable_long+1);
     myfree_array(time_frequency,size_time+1);
     myfree_array(processing_level,size_processing+1);
     myfree_array(realm,size_realm+1);
     myfree_array(source_id,size_source+1);
     myfree_array(model,size_model+1);
     myfree_array(experiment,size_exp+1);
     free(proj_name);
     free(replica);
     free(index_node);
     free(dataset_id);
     free(size_file);
     free(datetime_start);
     free(datetime_stop);
     proj_name=NULL;
     replica=NULL;
     index_node=NULL;
     dataset_id=NULL;
     size_file=NULL;
     datetime_start=NULL;
     datetime_stop=NULL; 
          //Put processed=1 in dashboard_queue
	  //QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST
   
  }//end of the OBS4MIPS
  }//end of the OBS4MIPS

  // closing database connection
        res1 = PQexec(conn, QUERY4);
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Trying to close the transaction\n");
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Query: [%s]\n",QUERY4);
        if ((!res1) || (PQresultStatus (res1) != PGRES_COMMAND_OK))
        {
          pmesg(LOG_ERROR,__FILE__,__LINE__,"Close transaction failed\n");
          PQclear(res1);
          //PQfinish(conn);
          return -2;
        }
        PQclear(res1);
        //PQfinish(conn);
        pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction closed\n");                
  //PQfinish (conn);
  // releasing memory for hashtables
  if (create_populate_done)
    {
      hashtbl_destroy (hashtbl_cross_dim_date);
      hashtbl_destroy (hashtbl_cross_dim_geolocation);
      hashtbl_destroy (hashtbl_cross_dim_project);
      hashtbl_destroy (hashtbl_cross_bridge_project);
      hashtbl_destroy (hashtbl_cross_bridge_project_tmp);
      hashtbl_destroy (hashtbl_cross_fact_download);
     if(create_populate_done==2)
     {
      hashtbl_destroy (hashtbl_obs_dim_file);
      hashtbl_destroy (hashtbl_obs_dim_dataset);
      hashtbl_destroy (hashtbl_obs_dim_institute);
      hashtbl_destroy (hashtbl_obs_dim_variable);
      hashtbl_destroy (hashtbl_obs_dim_time_frequency);
      hashtbl_destroy (hashtbl_obs_dim_processing_level);
      hashtbl_destroy (hashtbl_obs_dim_source_id);
      hashtbl_destroy (hashtbl_obs_dim_realm);
      hashtbl_destroy (hashtbl_obs_dim_index);
      hashtbl_destroy (hashtbl_obs_bridge_institute);
      hashtbl_destroy (hashtbl_obs_bridge_institute_tmp);
      hashtbl_destroy (hashtbl_obs_bridge_variable);
      hashtbl_destroy (hashtbl_obs_bridge_variable_tmp);
      hashtbl_destroy (hashtbl_obs_bridge_time_frequency);
      hashtbl_destroy (hashtbl_obs_bridge_time_frequency_tmp);
      hashtbl_destroy (hashtbl_obs_bridge_processing_level);
      hashtbl_destroy (hashtbl_obs_bridge_processing_level_tmp);
      hashtbl_destroy (hashtbl_obs_bridge_source_id);
      hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
      hashtbl_destroy (hashtbl_obs_bridge_realm);
      hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
      hashtbl_destroy (hashtbl_obs_dim_date);
      hashtbl_destroy (hashtbl_obs_dim_geolocation);
      hashtbl_destroy (hashtbl_obs4mips_fact_download);
      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Releasing memory for hashtables [%d] \n",create_populate_done); 
      }
      else
      {
        if(proj_name!=NULL)
        {
           hashtbl_destroy (hashtbl_cmip5_dim_model);
           hashtbl_destroy (hashtbl_cmip5_bridge_model);
           hashtbl_destroy (hashtbl_cmip5_bridge_model_tmp);
           hashtbl_destroy (hashtbl_cmip5_dim_experiment);
           hashtbl_destroy (hashtbl_cmip5_bridge_experiment);
           hashtbl_destroy (hashtbl_cmip5_bridge_experiment_tmp);
           hashtbl_destroy (hashtbl_cmip5_fact_download);
        }
      }
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

void uppercase ( char *sPtr )
{
    while ( *sPtr != '\0' )
    {
    *sPtr = toupper ( ( unsigned char ) *sPtr );
    ++sPtr;
    }
}

int insert_dmart_cross_project(PGconn *conn)
{
  HASHTBL *hashtbl_cross_dmart_project_host_geolocation;
  //PGconn *conn;
  PGresult *res;
  /*char conninfo[1024] = { '\0' };
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
    */
    PGresult   *res1;
    res1 = PQexec(conn, "select datmart, dmart_key from esgf_dashboard.registry;");
      if (PQresultStatus(res1) != PGRES_TUPLES_OK)
      {
        fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(conn));
        PQclear(res1);
        PQfinish(conn);
        exit(1);
        //exit_nicely(conn);
      }
    int i=0;
    char *datamart=NULL;
    int dmart_key=0;
    if (!( hashtbl_cross_dmart_project_host_geolocation = hashtbl_create (HAST_TABLE_CROSS_DMART_PROJECT_HOST_GEOLOCATION, NULL)))
    {
	pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for CROSS DMART_PROJECT_HOST_POS [skip parsing]\n");
    }
    populate_hash_table (conn, QUERY_GET_LIST_OF_CROSS_DMART_GEOLOCATION,
                                   &hashtbl_cross_dmart_project_host_geolocation);

    
    for (i = 0; i < PQntuples(res1); i++)
    {
        datamart=strdup(PQgetvalue(res1, i, 0)); 
        dmart_key=atoi(PQgetvalue(res1, i, 1));
        update_dmart(conn, &res1, hashtbl_cross_dmart_project_host_geolocation, datamart, dmart_key);
        free(datamart);
        datamart=NULL;
    }
    PQclear(res1);
    hashtbl_destroy (hashtbl_cross_dmart_project_host_geolocation);
    //PQfinish (conn);
    return 0;
}

int update_dmart(PGconn *conn, PGresult   *res1, HASHTBL *hashtbl_cross_dmart_project_host_geolocation, char *datamart, int dmart_key)
{
     char insert_dmart_project_host[2048] = { '\0' };
     char update_dmart_project_host[2048] = { '\0' };
     char select_query[2048] = { '\0' };
     char select_id_dmart_query[2048] = { '\0' };
     char update_registry[2048] = { '\0' };
     PGresult   *res2;
     int month=0;
     int year=0;
     int i=0;
     int dmart_id=0;
     time_t realtime;
     //char *timestamp=NULL;
     char timestamp[2048] = { '\0' };
     //timestamp=(char *)calloc (17, sizeof(char ));
     //timestamp = (char *)malloc(sizeof(char) * 16);
     //time_stamp(&timestamp);
     
     time_t ltime;
     ltime=time(NULL);
     struct tm *tm;
     tm=localtime(&ltime);

    sprintf(timestamp,"%04d%02d%02d%02d%02d%02d", tm->tm_year+1900, tm->tm_mon,
    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);


     realtime = (time_t)timestamp;
     time(&realtime);
     //free(timestamp);
     //timestamp=NULL;
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

     char dmart_id_str[128] = { '\0' };
     
     if(strcmp(datamart, "esgf_dashboard.cross_dmart_project_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CROSS_DMART_PROJECT_HOST, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           //for (int i=0; i < PQntuples(res1); i++)
           //{
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
             //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
             //sprintf(key_dmart_cros_proj_hos_time_id, "%d:%d", month, year);
             snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_CROSS_DMART_PROJECT_HOST, month, year, month, year);
             submit_query(conn,insert_dmart_project_host);
             
             
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST_UPDATE, month, year);
             submit_query (conn,update_dmart_project_host);


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
           PQclear (res1);
        }
     }
        char str_geo[2048] = { '\0' };
        char str1_geo[2048] = { '\0' }; 
              
     if(strcmp(datamart, "esgf_dashboard.cross_dmart_project_host_geolocation")==0)
     { 
        float lon=0.0;
        float lat=0.0;

        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST_POS);
           submit_query(conn,insert_dmart_project_host);
           //devo prendere, lat, lon e dmart_key

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_POS, datamart);
           int res_query=submit_query_res (conn, select_id_dmart_query, &res1);
           if(PQntuples(res1)!=0)
           {
             dmart_id=atoi(PQgetvalue(res1, 0, 0));
             lat=atof(PQgetvalue(res1, 0, 1));
             lon=atof(PQgetvalue(res1, 0, 2));
           }
           PQclear (res1);

           sprintf(str1_geo, "%14.11f",lat);
           sprintf(str_geo, "%14.11f",lon);
           if (*str1_geo==' ')
             sprintf(str1_geo, "%s",str1_geo+1);
           if (*str_geo==' ')
             sprintf(str_geo, "%s",str_geo+1);


           //dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
           char key_dmart_cros_proj_hos_pos_id[2048]= { '\0' };
           sprintf(key_dmart_cros_proj_hos_pos_id, "%s:%s", str1_geo, str_geo);
           
           hashtbl_insert (hashtbl_cross_dmart_project_host_geolocation,key_dmart_cros_proj_hos_pos_id,dmart_id_str);
           pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_pos_id, dmart_id_str);
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CROSS_DMART_HOST_POS);
           //printf("select_query %s\n", select_query);

           submit_query_res (conn, select_query,&res1);
           char *hashtbl_result;
         for (i = 0; i < PQntuples(res1); i++)
         {
           lat=atof(PQgetvalue(res1, i, 0));
           lon=atof(PQgetvalue(res1, i, 1));
           sprintf(str1_geo, "%14.11f",lat);
           sprintf(str_geo, "%14.11f",lon);
           if (*str1_geo==' ')
             sprintf(str1_geo, "%s",str1_geo+1);
           if (*str_geo==' ')
             sprintf(str_geo, "%s",str_geo+1);
           char key_geo[2048]= { '\0' };
           //devo controllare se esiste già nella hash, se esiste faccio update del dmart. Se non esiste faccio l'inserimento

           sprintf(key_geo, "%s:%s",str1_geo,str_geo);
           int geo_id=0;

           if (hashtbl_result = hashtbl_get (hashtbl_cross_dmart_project_host_geolocation, key_geo))
           {
              pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",key_geo, hashtbl_result);
              geo_id = atol (hashtbl_result);
              snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_CROSS_DMART_PROJECT_HOST_POS, str1_geo, str_geo, str1_geo, str_geo);
              //printf("QUERY_UPDATE_INSERT_NEW_CROSS_DMART_PROJECT_HOST_POS %s\n", insert_dmart_project_host);
              submit_query (conn,insert_dmart_project_host);
           }
           else
           {
             snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST_POS_UP, str1_geo, str_geo);
             submit_query (conn,insert_dmart_project_host);
              //printf("QUERY_UPDATE_INSERT_NEW_CROSS_DMART_PROJECT_HOST_POS_UP %s\n", insert_dmart_project_host);

             //PQclear (res1);
             /* update registry */
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             char key_dmart_cros_proj_hos_pos_id[2048]= { '\0' };
             sprintf(key_dmart_cros_proj_hos_pos_id, "%s:%s", str1_geo, str_geo);
           
             hashtbl_insert (hashtbl_cross_dmart_project_host_geolocation,key_dmart_cros_proj_hos_pos_id,dmart_id_str);
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_pos_id, dmart_id_str);
          }
         }
         PQclear (res1);
        }//chiude else
     }
     if(strcmp(datamart, "esgf_dashboard.obs4mips_dmart_clients_host_time_geolocation")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_CLIENTS_HOST_TIME_GEOLOCATION);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_PROJECT_HOST_POS, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           float lat=0.0;
           float lon=0.0;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_POS_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             lat=atof(PQgetvalue(res1, i, 2));
	     lon=atof(PQgetvalue(res1, i, 3));

             sprintf(str1_geo, "%14.11f",lat);
             sprintf(str_geo, "%14.11f",lon);
             if (*str1_geo==' ')
                     sprintf(str1_geo, "%s",str1_geo+1);
             if (*str_geo==' ')
                     sprintf(str_geo, "%s",str_geo+1);
           
             snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_EXIST_POS_DATE, month, year, str1_geo, str_geo);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_OBS4MIPS_DMART_CLIENTS_HOST_TIME_GEOLOCATION, month, year, str1_geo, str_geo, month, year, str1_geo, str_geo);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_OBS4MIPS_DMART_CLIENTS_HOST_TIME_GEOLOCATION, month, year, str1_geo, str_geo);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_OBS4MIPS_DMART_CLIENTS_HOST_TIME_GEOLOCATION, month,year);
             submit_query (conn,update_dmart_project_host);


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.obs4mips_dmart_variable_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_VARIABLE_HOST_TIME);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_VARIABLE_HOST_TIME, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           char *var_code=NULL;
           char *var_name=NULL;
           char *cf_st=NULL;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_VAR_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             var_code=PQgetvalue(res1, i, 2);
             var_name=PQgetvalue(res1, i, 3);
             cf_st=PQgetvalue(res1, i, 4);

             snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_EXIST_VAR_DATE, month, year, var_code, var_name,cf_st);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_OBS4MIPS_DMART_VARIABLE_HOST_TIME, month, year, var_code, var_name,cf_st, month, year, var_code, var_name,cf_st);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_OBS4MIPS_DMART_VARIABLE_HOST_TIME, month, year, var_code, var_name,cf_st);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_OBS4MIPS_VARIABLE_HOST_TIME, month,year);
             submit_query (conn,update_dmart_project_host);
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.obs4mips_dmart_source_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_SOURCE_HOST_TIME);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_SOURCE_HOST_TIME, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           char *source_id_name=NULL;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_SRC_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             source_id_name=PQgetvalue(res1, i, 2);

             snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_EXIST_SRC_DATE, month, year, source_id_name);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_OBS4MIPS_DMART_SOURCE_HOST_TIME, month, year, source_id_name, month, year, source_id_name);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_OBS4MIPS_DMART_SOURCE_HOST_TIME, month, year, source_id_name);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_OBS4MIPS_SOURCE_HOST_TIME, month,year);
             submit_query (conn,update_dmart_project_host);
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.obs4mips_dmart_realm_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_REALM_HOST_TIME);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_REALM_HOST_TIME, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           char *realm_name=NULL;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_RLM_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             realm_name=PQgetvalue(res1, i, 2);

             snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_EXIST_RLM_DATE, month, year, realm_name);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_OBS4MIPS_DMART_REALM_HOST_TIME, month, year, realm_name, month, year, realm_name);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_OBS4MIPS_DMART_REALM_HOST_TIME, month, year, realm_name);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_OBS4MIPS_REALM_HOST_TIME, month,year);
             submit_query (conn,update_dmart_project_host);
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.obs4mips_dmart_dataset_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_DATASET_HOST_TIME);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_DATASET_HOST_TIME, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           char *dataset_name=NULL;
           int dataset_version=0;
           char *datetime_start=NULL;
           char *datetime_stop=NULL;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_DAT_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             dataset_name=PQgetvalue(res1, i, 2);
             dataset_version=atoi(PQgetvalue(res1, i, 3));
             datetime_start=PQgetvalue(res1, i, 4);
             datetime_stop=PQgetvalue(res1, i, 5);

             snprintf (select_query, sizeof (select_query), QUERY_SELECT_OBS4MIPS_DMART_EXIST_DAT_DATE, month, year, dataset_name, dataset_version, datetime_start, datetime_stop);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_OBS4MIPS_DMART_DATASET_HOST_TIME, month, year, dataset_name, dataset_version, datetime_start, datetime_stop, month, year, dataset_name, dataset_version, datetime_start, datetime_stop);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_OBS4MIPS_DMART_DATASET_HOST_TIME, month, year, dataset_name, dataset_version, datetime_start, datetime_stop);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_OBS4MIPS_DATASET_HOST_TIME, month,year);
             submit_query (conn,update_dmart_project_host);
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.cmip5_dmart_clients_host_time_geolocation")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_CLIENTS_HOST_TIME_GEOLOCATION);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_PROJECT_HOST_POS, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           float lat=0.0;
           float lon=0.0;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_POS_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             lat=atof(PQgetvalue(res1, i, 2));
	     lon=atof(PQgetvalue(res1, i, 3));

             sprintf(str1_geo, "%14.11f",lat);
             sprintf(str_geo, "%14.11f",lon);
             if (*str1_geo==' ')
                     sprintf(str1_geo, "%s",str1_geo+1);
             if (*str_geo==' ')
                     sprintf(str_geo, "%s",str_geo+1);
           
             snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_EXIST_POS_DATE, month, year, str1_geo, str_geo);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_CMIP5_DMART_CLIENTS_HOST_TIME_GEOLOCATION, month, year, str1_geo, str_geo, month, year, str1_geo, str_geo);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_CMIP5_DMART_CLIENTS_HOST_TIME_GEOLOCATION, month, year, str1_geo, str_geo);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_CMIP5_DMART_CLIENTS_HOST_TIME_GEOLOCATION, month,year);
             submit_query (conn,update_dmart_project_host);


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.cmip5_dmart_dataset_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_DATASET_HOST_TIME);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_DATASET_HOST_TIME, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           char *dataset_name=NULL;
           int dataset_version=0;
           char *datetime_start=NULL;
           char *datetime_stop=NULL;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_DAT_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             dataset_name=PQgetvalue(res1, i, 2);
             dataset_version=atoi(PQgetvalue(res1, i, 3));
             datetime_start=PQgetvalue(res1, i, 4);
             datetime_stop=PQgetvalue(res1, i, 5);

             snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_EXIST_DAT_DATE, month, year, dataset_name, dataset_version, datetime_start, datetime_stop);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_CMIP5_DMART_DATASET_HOST_TIME, month, year, dataset_name, dataset_version, datetime_start, datetime_stop, month, year, dataset_name, dataset_version, datetime_start, datetime_stop);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_CMIP5_DMART_DATASET_HOST_TIME, month, year, dataset_name, dataset_version, datetime_start, datetime_stop);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_CMIP5_DATASET_HOST_TIME, month,year);
             submit_query (conn,update_dmart_project_host);
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.cmip5_dmart_variable_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_VARIABLE_HOST_TIME);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_VARIABLE_HOST_TIME, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           char *var_code=NULL;
           char *var_name=NULL;
           char *cf_st=NULL;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_VAR_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             var_code=PQgetvalue(res1, i, 2);
             var_name=PQgetvalue(res1, i, 3);
             cf_st=PQgetvalue(res1, i, 4);

             snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_EXIST_VAR_DATE, month, year, var_code, var_name,cf_st);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_CMIP5_DMART_VARIABLE_HOST_TIME, month, year, var_code, var_name,cf_st, month, year, var_code, var_name,cf_st);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_CMIP5_DMART_VARIABLE_HOST_TIME, month, year, var_code, var_name,cf_st);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_CMIP5_VARIABLE_HOST_TIME, month,year);
             submit_query (conn,update_dmart_project_host);
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.cmip5_dmart_model_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_MODEL_HOST_TIME);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_MODEL_HOST_TIME, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           char *model_name=NULL;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_MDL_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             model_name=PQgetvalue(res1, i, 2);

             snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_EXIST_MDL_DATE, month, year, model_name);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_CMIP5_DMART_MODEL_HOST_TIME, month, year, model_name, month, year, model_name);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_CMIP5_DMART_MODEL_HOST_TIME, month, year, model_name);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_CMIP5_MODEL_HOST_TIME, month,year);
             submit_query (conn,update_dmart_project_host);
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           PQclear (res1);
        }
      }
     if(strcmp(datamart, "esgf_dashboard.cmip5_dmart_experiment_host_time")==0)
     {  
        if(dmart_key==0)
        {  
           snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_EXPERIMENT_HOST_TIME);
           submit_query_res (conn,insert_dmart_project_host,&res1);

           /* update registry */
	   snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
           dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
           sprintf (dmart_id_str, "%ld", dmart_id);

           //char key_dmart_cros_proj_hos_time_id[2048]= { '\0' };
           //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
           
           //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
           //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           if(dmart_id>=0)
           {  
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           }
        }
        else
        {
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_EXPERIMENT_HOST_TIME, dmart_key);
           int res_query=submit_query_res (conn, select_query,&res1);
           char *experiment_name=NULL;
           if(PQntuples(res1)!=0)
           {
             month=atoi(PQgetvalue(res1, 0, 0));
             year=atoi(PQgetvalue(res1, 0, 1));
           }
           PQclear (res1);
           snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_EXP_DATE, month, year);
           res_query=submit_query_res (conn, select_query,&res1);

           for (i = 0; i < PQntuples(res1); i++)
           {
             month=atoi(PQgetvalue(res1, i, 0));
             year=atoi(PQgetvalue(res1, i, 1));

             experiment_name=PQgetvalue(res1, i, 2);

             snprintf (select_query, sizeof (select_query), QUERY_SELECT_CMIP5_DMART_EXIST_EXP_DATE, month, year, experiment_name);
             res_query=submit_query_res (conn, select_query,&res2);

             if(PQntuples(res2)!=0)    
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_UPDATE_INSERT_NEW_CMIP5_DMART_EXPERIMENT_HOST_TIME, month, year, experiment_name, month, year, experiment_name);
             else
                 snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_NEW_CMIP5_DMART_EXPERIMENT_HOST_TIME, month, year, experiment_name);
             
             submit_query(conn,insert_dmart_project_host);
             PQclear (res2);
             


	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
             //sprintf(key_dmart_cros_proj_hos_time_id, "%s:%s", buf1, buf2);
             //hashtbl_insert (hashtbl_cross_dmart_project_host_time,key_dmart_cros_proj_hos_time_id,dmart_id_str);
             //pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",key_dmart_cros_proj_hos_time_id, dmart_id_str);
           }
             snprintf (update_dmart_project_host, sizeof (update_dmart_project_host), QUERY_SELECT_INSERT_UPDATE_CMIP5_EXPERIMENT_HOST_TIME, month,year);
             submit_query (conn,update_dmart_project_host);
	     snprintf(select_id_dmart_query, sizeof (select_id_dmart_query), QUERY_GET_DMART_ID, datamart);
             dmart_id=get_foreign_key_value(conn, select_id_dmart_query);
             snprintf (update_registry, sizeof (update_registry), QUERY_UPDATE_REGISTRY,realtime, dmart_id,datamart);
             submit_query (conn, update_registry);
           PQclear (res1);
        }
      }

#if 0
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.obs4mips_dmart_realm_host_time")==0))
     {  
       snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_REALM_HOST_TIME);
       submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.obs4mips_dmart_source_host_time")==0))
     {  
       snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_SOURCE_HOST_TIME);
       submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.obs4mips_dmart_variable_host_time")==0))
     {  
       snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_CLIENTS_VARIABLE_HOST_TIME);
       submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.obs4mips_dmart_dataset_host_time")==0))
     {  
       snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_DATASET_HOST_TIME);
       submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.cmip5_dmart_clients_host_time_geolocation")==0))
     {  
        snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_CLIENTS_HOST_TIME_GEOLOCATION);
        submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.cmip5_dmart_experiment_host_time")==0))
     {  
       snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_EXPERIMENT_HOST_TIME);
       submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.cmip5_dmart_model_host_time")==0))
     {  
       snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_MODEL_HOST_TIME);
       submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.cmip5_dmart_variable_host_time")==0))
     {  
       snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_CLIENTS_VARIABLE_HOST_TIME);
       submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
     if((date_key==0) && strcmp((datamart, "esgf_dashboard.cmip5_dmart_dataset_host_time")==0))
     {  
       snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_DATASET_HOST_TIME);
       submit_query (conn, insert_dmart_project_host);
     }
     else
       XXXX
        submit_query (conn, insert_dmart_project_host);
#endif 
}

#if 0 
int
submit_query (PGconn * conn, char *query)
{
  PGresult *res;
  fprintf (stderr, "Query: %s\n", query);

  res = PQexec (conn, query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    {
      printf("error in the query ++++++++++%s\n", query);
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Submit query failed\n");
      PQclear (res);
      return -1;
    }
  PQclear (res);
  return 0;
}
#endif 
int
submit_query_res (PGconn * conn, char *query, PGresult **res)
{
  //fprintf (stderr, "Query**: %s\n", query);

  //PGresult *res=NULL;

  *res = PQexec (conn, query);

  if ((!(*res)) || (PQresultStatus (*res) != PGRES_TUPLES_OK))
    {
      //printf("error in the query *+*+*+*%s\n", query);
      //pmesg(LOG_ERROR,__FILE__,__LINE__,"Submit query failed\n");
      PQclear (*res);
      return -1;
    }

  return 0;
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

/*
 *  Returns the current time.
 *  */

int time_stamp(char *timestamp){

time_t ltime;
ltime=time(NULL);
struct tm *tm;
tm=localtime(&ltime);

sprintf(timestamp,"%04d%02d%02d%02d%02d%02d", tm->tm_year+1900, tm->tm_mon, 
    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
return 0;
}
