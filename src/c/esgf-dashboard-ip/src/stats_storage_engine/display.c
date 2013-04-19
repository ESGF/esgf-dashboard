#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILE_NAME_STATS "raw_%s_stats.dat"
#define FILE_NAME_START_STATS "start_stats_time.dat"
#define QUERY_PLANB_SUMMARY_DB_TMP "drop table if exists esgf_dashboard.finaldw_planb_tmp; create table esgf_dashboard.finaldw_planb_tmp as (SELECT EXTRACT (YEAR FROM (TIMESTAMP WITH TIME ZONE 'epoch' + fixed_log.date_fetched * INTERVAL '1 second')) as year, EXTRACT (MONTH FROM (TIMESTAMP WITH TIME ZONE 'epoch' + fixed_log.date_fetched * INTERVAL '1 second')) as month, count(*) as downloads, count(distinct url) as files, count(distinct user_id_hash) as users, sum(fixed_log.size)/1024/1024/1024 as gb FROM (SELECT file.url, log.user_id_hash, max(log.date_fetched) as date_fetched, max(file.size) as size FROM esgf_node_manager.access_logging as log join public.file_version as file on (log.url LIKE '%.nc' AND lower(regexp_replace(log.url, E'^.*/(cmip5/.*\\.nc)$', E'\\\\1')) = lower(regexp_replace(file.url, E'^.*/(cmip5/.*\\.nc)$', E'\\\\1'))) where log.success and log.duration > 1000 group by file.url,log.user_id_hash) as fixed_log group by year,month order by year,month);"

struct stats_pointer
{
    FILE *cur_record;  // puntatore al record corrente relativo alla statistica considerata
};


struct stats_struct 
{
 long long int intervals; // number of 5min intervals from the time0 (first run) 
 double metrics; // metrics value to be stored 
};

struct start_stats_struct 
{
 time_t start_time; // number of 5min intervals from the time0 (first run) 
};

int main(int argc, char **argv)
{
    FILE *binaryFile;
    char* c_time_string;
    struct start_stats_struct start_time_struct;
    char file_name_sensor_stats[256];    
    int res;
    int displaytype;
    long long int timeinterval;

    fprintf(stdout, "%s\n",QUERY_PLANB_SUMMARY_DB_TMP );

    return 0;

    if (argc!=4)
	{
	fprintf(stdout, "Please specify the metric (i.e. availability), display type (0=base display  1=csv format) and time interval (in seconds)\n");
	fprintf(stdout, "Examples: ./display availability 0 300\n");
	fprintf(stdout, "          ./display availability 1 300\n");
	return 0;		
	}

    displaytype=atoi(argv[2]);	
    timeinterval=atoi(argv[3]);	
    if (displaytype!=1 && displaytype!=0) 
	{
	fprintf(stdout, "Please specify a valid value for display type (0=default, 1=csv format)\n");
        fprintf(stdout, "Examples: ./display availability 0 300\n");
        fprintf(stdout, "          ./display availability 1 300\n");
	return 0;
        }

    fprintf(stdout,"Display metric: %s\nDisplay type: %d\n", argv[1],displaytype);
    snprintf(file_name_sensor_stats,sizeof(file_name_sensor_stats),FILE_NAME_STATS,argv[1]);
    
    if (!open_create_file(&binaryFile , FILE_NAME_START_STATS,"r")) 
	{
    	read_start_from_file(binaryFile,&start_time_struct);
    	close_file(binaryFile);
    	c_time_string = ctime(&(start_time_struct.start_time));
    	printf("Starting time for stats is: %s\n",c_time_string);
	}
	else
	{
    	printf("No starting time file is available for stats\n");
	return 0;
	}

    //fprintf(stdout, "START - Displaying file content for %s\n",file_name_sensor_stats);
    res=open_create_file(&binaryFile , file_name_sensor_stats,"r+");
    if (res==0)
	{
	display_file(binaryFile,start_time_struct.start_time, displaytype,timeinterval);
    	close_file(binaryFile);
	}
	else
    		fprintf(stdout, "Error opening file %s\n",file_name_sensor_stats);

    return 0;
}

int display_file(FILE* file, time_t start_time,int displaytype, long long int timeinterval)
{
   struct stats_struct stats;
   time_t next_time; 
   char* c_t_string;

   while (1)
   {
    if (!fread(&stats, sizeof(struct stats_struct), 1, file))
	break;
    next_time = start_time + stats.intervals*timeinterval;  
    c_t_string = ctime(&(next_time));
    if (!displaytype)
    	fprintf(stdout,"Metric id: %lld Metric value: %4.2f Metric timestamp: %s",stats.intervals, stats.metrics,c_t_string);
    else
	fprintf(stdout,"%lld|%4.2f|%s",stats.intervals, stats.metrics,c_t_string);
   }
   return 0;
}

int open_create_file(FILE** file , char* filename, char* mode)
{
    *file = fopen(filename,mode);
    if ((*file) == NULL)
	 return -1; 
    return 0;
}

int read_stats_from_file(FILE* file , struct stats_struct *stats_data)
{
    // todo check on return value of fread
    if (!fread(stats_data, sizeof(struct stats_struct), 1, file))
	return -1;	
    return 0;
}

int read_start_from_file(FILE* file , struct start_stats_struct *stats_data)
{
    // todo check on return value of fread
    fread(stats_data, sizeof(struct start_stats_struct), 1, file);
    return 0;
}


int close_file(FILE* file)
{
    fclose( file );
    return 0;
}

