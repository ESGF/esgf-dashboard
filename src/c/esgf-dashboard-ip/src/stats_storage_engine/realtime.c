#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REALTIME_CPU_1M "realtime_cpu_1m.dat"
#define REALTIME_CPU_5M "realtime_cpu_5m.dat"
#define REALTIME_CPU_15M "realtime_cpu_15m.dat"
#define REALTIME_CPU_1M_TEMP "realtime_cpu_1m.dat.temp"
#define REALTIME_CPU_5M_TEMP "realtime_cpu_5m.dat.temp"
#define REALTIME_CPU_15M_TEMP "realtime_cpu_15m.dat.temp"

struct stats_struct 
{
 long long int intervals; // number of 5min intervals from the time0 (first run) 
 double metrics; // metrics value to be stored 
};

struct start_stats_struct 
{
 time_t start_time; // number of 5min intervals from the time0 (first run) 
};
    
int realtime_cpu_get_stats(void);
int initialize_stats_file(char* filename);

int main(void)
{
    time_t current_time;
    time_t start_time;
    double diff_time;
    char* c_time_string;
    char* c2_time_string;
    struct tm *info;
    int cur_sec;	
    int cur_min;
    int cur_hour;
    int cur_day;
    int cur_month;
    int cur_year;
    //int counter=10512;		  
    int counter=24;		  
    long long int num_interval,pointer;
    long long int num_interval_1h;
    FILE *cur_record;
    struct stats_struct availability_struct,stats;	
    struct stats_struct availability_struct_1h;	
    struct start_stats_struct start_time_struct;	
    //struct stats_pointer null_rec_pointer;	
    FILE* pointer_1h;	
    double av_5m;


    // creates the raw stats files if not existing 
    initialize_stats_file(REALTIME_CPU_1M);
    initialize_stats_file(REALTIME_CPU_5M);
    initialize_stats_file(REALTIME_CPU_15M);
    // file exists now!	
    realtime_cpu_get_stats();
    return 0;
}


int realtime_cpu_get_stats(void )
{
  char esgf_nodetype_filename[256] = { '\0' };
  char loadavg1[256] = { '\0' };
  char loadavg5[256] = { '\0' };
  char loadavg15[256] = { '\0' };
  int counter;

 counter=20000;
 while (counter--)
 {
  sprintf (esgf_nodetype_filename, "/proc/loadavg");
  FILE *file = fopen (esgf_nodetype_filename, "r");

  if (file == NULL)
    return -1;

  if ((fscanf (file, "%s", loadavg1)) == EOF)
    return -1;
  if ((fscanf (file, "%s", loadavg5)) == EOF)
    return -1;
  if ((fscanf (file, "%s", loadavg15)) == EOF)
    return -1;
  fprintf(stdout,"Cpu load average metrics [%s] [%s] [%s]\n", loadavg1, loadavg5,loadavg15);
  fclose (file);
  rotate_realtime_stats(REALTIME_CPU_1M, REALTIME_CPU_1M_TEMP, loadavg1);
  rotate_realtime_stats(REALTIME_CPU_5M, REALTIME_CPU_5M_TEMP, loadavg5);
  rotate_realtime_stats(REALTIME_CPU_15M, REALTIME_CPU_15M_TEMP, loadavg15);
  
  sleep(1); 
 } 

}

int rotate_realtime_stats(char* filestats, char* filestats_tmp, char* metric)
 {
  FILE *fromFile;
  time_t current_time;
  FILE *toFile;
  char* c2_time_string;
  char last_value[256] = { '\0' };
  char c2_copy[256] = { '\0' };
  char str[256] = { '\0' };
  int i;

  open_create_file(&toFile,filestats_tmp, "w+"); //overwrite file or create it if it does not exists.
  time(&(current_time));
  c2_time_string = ctime(&(current_time));
  snprintf(c2_copy,sizeof(c2_copy),"%s",c2_time_string);
  c2_time_string[strlen(c2_time_string)-1]='\0';
  c2_copy[strlen(c2_copy)-6]='\0';
  printf("Time is %s [%s]\n", c2_time_string,(c2_copy+11) );

  // writing the new stats on top
   //snprintf(last_value,sizeof(last_value),"%s&%s\n",metric,c2_time_string);
   snprintf(last_value,sizeof(last_value),"%s&%s\n",metric,(c2_copy+11));
   if (fputs(last_value,toFile)==EOF)
	fprintf(stdout,"Error writing the file\n");

  // now copy the remaining ones from the prod file
  open_create_file(&fromFile , filestats,"r"); // r  - open for reading 
  //snprintf(last_value, sizeof(last_value),metric);

  // iterate on prod file and temp file
  i=1;
  while (i<30)
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
  rename(filestats_tmp,filestats); 
  // display new file content
  open_create_file(&fromFile , filestats,"r"); // r  - open for reading 
  while (fgets(str,256,fromFile)!=NULL)
  	{
  	str[strlen(str)-1]='\0';
	fprintf(stdout,"[%s] ",str); 
      	}
  fprintf(stdout,"\n"); 
  fclose(fromFile);
   
  return 0;
}


// this function initialize all the stats file! It makes sure the files are there.
int initialize_stats_file(char* filename)
{
    FILE *binaryFile;
    if (open_create_file(&binaryFile , filename,"r")) // r  - open for reading 
        open_create_file(&binaryFile ,filename, "w+");
    close_file(binaryFile);
    return 0;
}

int display_file(FILE* file)
{
   struct stats_struct stats;
   while (1)
   {
    if (!fread(&stats, sizeof(struct stats_struct), 1, file))
	break;
    fprintf(stdout,"Reading file %lld=%4.2f\n",stats.intervals, stats.metrics);
   }
   return 0;
}

int open_create_file(FILE** file , char* filename, char* mode)
{
    *file = fopen(filename,mode);
    if ((*file) == NULL)
	{
	 fprintf(stdout,"Can't open binary file [%s] in [%s] mode\n",filename,mode);
	 return -1; 
	}
    return 0;
}

/*int read_pointer_from_file(FILE* file , struct stats_pointer *stats_p)
{
    // todo check on return value of fread
    fread(stats_p, sizeof(struct stats_pointer), 1, file);
    return 0;
}*/

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

int write_start_time_to_file(FILE* file , struct start_stats_struct *stats_p )
{
    if ( fwrite( stats_p, sizeof(struct start_stats_struct), 1, file ) != 1)
	{
	 fprintf(stdout,"Can't write on binary output file\n");
	 return -2; 
	}
    return 0;
}

/*int write_pointer_to_file(FILE* file , struct stats_pointer *stats_p )
{
    if ( fwrite( stats_p, sizeof(struct stats_pointer), 1, file ) != 1)
	{
	 fprintf(stdout,"Can't write on binary output file\n");
	 return -2; 
	}
    return 0;
}*/


int write_stats_to_file(FILE* file , struct stats_struct *stats )
{
    if ( fwrite( stats, sizeof(struct stats_struct), 1, file ) != 1)
	{
	 fprintf(stdout,"Can't write on binary output file\n");
	 return -2; 
	}
    return 0;
}

int close_file(FILE* file)
{
    fclose( file );
    return 0;
}

// rifare una funzione con la gestione della timewindow fatta per bene 
// con fseek all'indietro di 12 elementi quando si tratta di 1h. Poi si legge dal file finchè stats.intervals>=pointer e si tiene traccia di quanti
// cicli sono stati fatti così da poter riposizione facilmente il puntatore dell'1h. Dopo si continua a leggere perchè servono i totali per derivare la media 1h
/*
int initialize_stats_file_pointers(FILE** binaryFile, char* filename, long long int pointer)
{
    int exit_while=1;
    struct stats_struct stats;
    open_create_file(binaryFile , filename,"r"); // r  - open for reading 
    fseek(*binaryFile, (-1)*sizeof(struct stats_struct)*12,SEEK_END);	
    //	... to be continued	
    while (exit_while)
	if (fread(&stats, sizeof(struct stats_struct), 1, *binaryFile))
		{
    		fprintf(stdout,"Reading file %lld=%4.2f\n",stats.intervals, stats.metrics);
		if (stats.intervals >= pointer) {
			fseek(*binaryFile,-1*sizeof(struct stats_struct),SEEK_CUR);
    			fprintf(stdout,"Back to 1 \n");
			exit_while=0;
			}
		}
		else
		   exit_while=0;	 
	
    return 0;
} 

int initialize_stats_file_pointers2(FILE** binaryFile, char* filename, long long int pointer)
{
    int exit_while=1;
    struct stats_struct stats;
    open_create_file(binaryFile , filename,"r"); // r  - open for reading 
    while (exit_while)
	if (fread(&stats, sizeof(struct stats_struct), 1, *binaryFile))
		{
    		fprintf(stdout,"Reading file %lld=%4.2f\n",stats.intervals, stats.metrics);
		if (stats.intervals >= pointer) {
			fseek(*binaryFile,-1*sizeof(struct stats_struct),SEEK_CUR);
    			fprintf(stdout,"Back to 1 \n");
			exit_while=0;
			}
		}
		else
		   exit_while=0;	 
	
    return 0;
} */

/*int event_last5_minutes_occurred(long long int num_interval, struct stats_struct *availability_struct , struct stats_pointer *pointer)
    {
    // store the metric in the raw file
    // todo: lock file to be added
        FILE *binaryF;
    	open_create_file(&binaryF,FILE_NAME_STATS,"a+"); // a+ - open for reading and writing (append if file exists)
    	write_stats_to_file(binaryF,availability_struct);
    	close_file(binaryF);
    	printf("- Event last 5 minutes occured [5m=%lld]\n\n",num_interval);
        return 0;
    }


int event_last_hour_occurred(long long int num_interval)
    {
    	printf("- Event last hour occured [5m=%lld 1h=%lld 1d=%lld]\n",num_interval,num_interval / (12*1),num_interval / (12*24) );
        return 0;
    }

int event_last_week_occurred(long long int num_interval)
    {
    	printf("- Event last week occured [5m=%lld 1h=%lld 1d=%lld 1w=%lld]\n",num_interval,num_interval / (12*1),num_interval / (12*24),num_interval / (12*24*7) );
        return 0;
    }

int event_last_day_occurred(long long int num_interval)
    {
    	printf("- Event last day occurred [5m=%lld 1h=%lld 1d=%lld]\n",num_interval,num_interval / (12*1),num_interval / (12*24) );
        return 0;
    }

int event_last30_days_occurred(long long int num_interval)
    {
    	printf("- Event last 5 minutes occured [%lld]\n",num_interval);
        return 0;
    }

int event_last_year_occurred(long long int num_interval)
    {
    	printf("- Event last year (365 days) occurred [5m=%lld 1h=%lld 1d=%lld 1y=%lld]\n",num_interval,num_interval / (12*1),num_interval / (12*24),num_interval / (12*24*365) );
        return 0;
    }
*/
