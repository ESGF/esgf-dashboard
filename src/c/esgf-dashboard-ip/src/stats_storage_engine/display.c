#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILE_NAME_STATS "raw_stats.dat"
#define FILE_NAME_START_STATS "start_stats_time.dat"


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

int main(void)
{
    FILE *binaryFile;
    char* c_time_string;
    struct start_stats_struct start_time_struct;
    
    if (!open_create_file(&binaryFile , FILE_NAME_START_STATS,"r")) 
	{
    	read_start_from_file(binaryFile,&start_time_struct);
    	close_file(binaryFile);
    	c_time_string = ctime(&(start_time_struct.start_time));
    	printf("Starting time for stats is: %s \n", c_time_string);
	}
	else
	{
    	printf("No starting time file is available for stats\n");
	return 0;
	}

    fprintf(stdout, "START - Displaying file content for %s\n",FILE_NAME_STATS);
    open_create_file(&binaryFile , FILE_NAME_STATS,"r+"); 
    display_file(binaryFile,start_time_struct.start_time);
    close_file(binaryFile);
    fprintf(stdout, "END   - Displaying file content for %s\n",FILE_NAME_STATS);

    return 0;
}

int display_file(FILE* file, time_t start_time)
{
   struct stats_struct stats;
   time_t next_time; 
   char* c_t_string;

   while (1)
   {
    if (!fread(&stats, sizeof(struct stats_struct), 1, file))
	break;
    next_time = start_time + stats.intervals*300;  
    c_t_string = ctime(&(next_time));
    fprintf(stdout,"Reading file %lld=%4.2f - %s",stats.intervals, stats.metrics,c_t_string);
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

