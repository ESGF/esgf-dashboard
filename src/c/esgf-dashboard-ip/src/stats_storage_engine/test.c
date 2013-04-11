#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILE_NAME_STATS "raw_%s_stats.dat"
#define FILE_NAME_START_STATS "start_stats_time.dat"


struct stats_struct 
{
 long long int intervals; // number of 5min intervals from the time0 (first run) 
 double metrics; // metrics value to be stored 
};

struct start_stats_struct 
{
 time_t start_time; // number of 5min intervals from the time0 (first run) 
};

struct sensor_struct 
{
    long long int time_interval;	
    int windows_number;
    int reset_onstart;			// reset raw_file removing the history every time the ip boots default 0 which means keeps the history 
    int ext_sensor;  			// external sensor 1=yes, which means of out of the box sensor ; 0=no, which means core sensor 
    int aggregation;  			// it relates to the aggregation mechanism. Default 0=no, 1=yes 
    char sensor_name[240];		// entry among squared brackets in the config file
    char file_name_sensor_stats[256];	// filename for the raw stats 
    char sensor_executable[2048];	// executable for sensors
    long long int windows_pointers[10]; // windows pointers for 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    long long int windows_length[10];  // current windows length for 5m, 1h, 1d, 1w, 30days, 365days, ALL
    long long int windows_limits[10];  // time windows dimension for 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    double aggregated_values[10]; 	// aggregated values last 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    double window_avg_values_p[10];  // pessimistic last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    double window_avg_values_o[10];  // optimistic last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    FILE *windows_FILE_pointer[10];
    struct stats_struct stats_array[10];	
};

int main(void)
{
   time_t current_time;
   char* c_time_string;
   int i;
   int counter=3; // iterations		  
   long long int num_interval;
   struct stats_struct availability_struct,stats;	
   //int time_interval=5;   // number of seconds (sleep time_interval)
   //int windows_number=6;  // number of time_windows to be managed (initially 3, then 6)
   struct sensor_struct sens_struct;

   //todo: read config file for all the sensors!!!
   
   // setup sensor struct 
   setup_sens_struct_from_config_file(&sens_struct);

   // initializing pointers
   reset_sensor_struct_and_raw_stats_file(&sens_struct);

   // setup start time and synchronize current_time 
   setup_and_synchronize_start_time(&current_time, &num_interval, sens_struct.time_interval);

   // setting up pointers	
   setup_time_windows(num_interval, &sens_struct);

   //1_todo: create database cache table -- table name= sensor_<metric>	
	
   //display_windows_metrics(&sens_struct);

   // this part should the one in the thread cycle
   while (counter--) 
    {
    // next serial timestamp 
    num_interval++;	
	
    // first version is serial based on the num_interval %% sensor_timespan == 0 ; sleep is sleep(1) 

    compute_and_display_current_time_stamp(&current_time, sens_struct.time_interval);
	
    produce_and_append_next_sample_to_raw_file(&availability_struct,num_interval,sens_struct.file_name_sensor_stats);
     
    shift_windows_set(&availability_struct,&sens_struct, num_interval); 
    //2_todo: store local metrics in the database cache table -- recuperare dall'hostname l'IDhost 

    //3_todo: store external metrics in the database cache table	
    sleep(sens_struct.time_interval);
    }
    
   display_windows_pointers(&sens_struct);
   close_windows_FILE_pointers(&sens_struct);

   return 0;
}

int compute_and_display_current_time_stamp(time_t *current_time, long long int time_interval)
{
    char* c_time_string;

    *current_time = *current_time + time_interval;
    c_time_string = ctime(current_time);
    fprintf(stdout, "***************************************************\n");
    fprintf(stdout, "Regular current time for stats is %s", c_time_string);
   return 0;
}

int close_windows_FILE_pointers(struct sensor_struct *sens_struct)
{
   int i;
   for (i=0 ; i<sens_struct->windows_number; i++) 
   	close_file(sens_struct->windows_FILE_pointer[i]);
    return 0;
}

int display_windows_pointers(struct sensor_struct *sens_struct)
{
   int i;
   for (i=0; i<sens_struct->windows_number; i++) 
    	fprintf(stdout,"|C-> [%d] Window pointer [%lld]\n",i,sens_struct->windows_pointers[i]);
   fprintf(stdout,"|D-> sensor name [%s] sensor executable [%s] sensor filename [%s]\n",sens_struct->sensor_name,sens_struct->sensor_executable,sens_struct->file_name_sensor_stats);
   fprintf(stdout, "end process\n");
    
   return 0;
}

int shift_windows_set(struct stats_struct *availability_struct,struct sensor_struct *sens_struct, long long int num_interval)
{
    int i;

    for (i=0 ; i<sens_struct->windows_number; i++)
    	if (!(sens_struct->windows_pointers[i])) // set pointer if undefined (this means it does not exist an older last5 min sample)
    		{
		fread(&(sens_struct->stats_array[i]), sizeof(struct stats_struct), 1, sens_struct->windows_FILE_pointer[i]);
		sens_struct->windows_pointers[i]=num_interval; // now the window exists!!!
		sens_struct->windows_length[i]++;
		sens_struct->aggregated_values[i]+=(availability_struct->metrics);	
   		sens_struct->window_avg_values_p[i]=sens_struct->aggregated_values[i]/sens_struct->windows_limits[i];
   		sens_struct->window_avg_values_o[i]=sens_struct->aggregated_values[i]/sens_struct->windows_length[i];
    		fprintf(stdout, "|A->i=[%d] Pointer=[%lld] Metrics=[%4.2f] Aggreg=[%4.2f] Length=[%lld] Optim=[%4.2f] Pessim=[%4.2f]\n", i,sens_struct->windows_pointers[i],availability_struct->metrics,sens_struct->aggregated_values[i],sens_struct->windows_length[i], sens_struct->window_avg_values_o[i],sens_struct->window_avg_values_p[i]);
		}
		else 
    		{ 
		 if (sens_struct->windows_length[i] == sens_struct->windows_limits[i] || ((num_interval-sens_struct->windows_pointers[i])==(sens_struct->windows_limits[i]))) 
    			{
			sens_struct->aggregated_values[i]-=sens_struct->stats_array[i].metrics;
			fread(&(sens_struct->stats_array[i]), sizeof(struct stats_struct), 1, sens_struct->windows_FILE_pointer[i]);
			if (sens_struct->windows_length[i] == sens_struct->windows_limits[i])   
				{
 			   	sens_struct->windows_pointers[i]++;
				fprintf(stdout, "Regular window\n");
				}
			else 
				if ((num_interval-sens_struct->windows_pointers[i])==sens_struct->windows_limits[i])
					{
					fprintf(stdout, "Non regular window!\n");	
					sens_struct->windows_pointers[i]=sens_struct->stats_array[i].intervals;
					}

			sens_struct->aggregated_values[i]+=(availability_struct->metrics);
   			sens_struct->window_avg_values_p[i]=sens_struct->aggregated_values[i]/sens_struct->windows_limits[i];
   			sens_struct->window_avg_values_o[i]=sens_struct->aggregated_values[i]/sens_struct->windows_length[i];
   			} 
			else 
    			{
			  sens_struct->windows_length[i]++;
			  sens_struct->aggregated_values[i]+=(availability_struct->metrics);
   			  sens_struct->window_avg_values_p[i]=sens_struct->aggregated_values[i]/sens_struct->windows_limits[i];
   			  sens_struct->window_avg_values_o[i]=sens_struct->aggregated_values[i]/sens_struct->windows_length[i];
   			}
			
    		fprintf(stdout, "|B->i=[%d] Pointer=[%lld] Metrics=[%4.2f] Aggreg=[%4.2f] Length=[%lld] Optim=[%4.2f] Pessim=[%4.2f]\n", i,sens_struct->windows_pointers[i],availability_struct->metrics,sens_struct->aggregated_values[i],sens_struct->windows_length[i], sens_struct->window_avg_values_o[i],sens_struct->window_avg_values_p[i]);

   		}
    return 0;
}

int display_windows_metrics(struct sensor_struct *sens_struct)
{ 
   int i=0;

   for (i=0; i<sens_struct->windows_number; i++) 
    	fprintf(stdout,"Last [%d] minutes window pointer [%d]\n",i,sens_struct->windows_pointers[i]);
   for (i=0 ; i<sens_struct->windows_number; i++) // to be replaced with the line above (7 instead of 3)
	if (sens_struct->windows_length[i])
   		fprintf(stdout, "Average_o[%d] on [%d] values = [%4.2f]\n", i,sens_struct->windows_length[i], sens_struct->window_avg_values_o[i]);
	else
   		fprintf(stdout, "Average_o[%d] is undefined\n",i);
   for (i=0 ; i<sens_struct->windows_number; i++) // to be replaced with the line above (7 instead of 3)
   		fprintf(stdout, "Average_p[%d]  = %4.2f\n", i,sens_struct->window_avg_values_p[i]);
    return 0;
}

int create_raw_file_if_not_existing(char* filename)
{
    FILE *binaryFile;
    if (open_create_file(&binaryFile , filename,"r")) // r  - open for reading 
        open_create_file(&binaryFile ,filename, "w+");
    close_file(binaryFile);
    return 0;
}

int setup_time_windows(long long int num_interval, struct sensor_struct *sens_struct)
{
    FILE *binaryFile;
    long long int time_counter;
    struct stats_struct stats;	
    int i;

    open_create_file(&binaryFile , sens_struct->file_name_sensor_stats,"r"); 
    time_counter=0;
    while (1)
    	{
    	if (!fread(&stats, sizeof(struct stats_struct), 1, binaryFile))
		break;
	time_counter = stats.intervals;
    	for (i=0; i<sens_struct->windows_number; i++) 
		if (time_counter<=(num_interval+1) &&  time_counter>=(num_interval+1-sens_struct->windows_limits[i]+1))
    			{
    			//fprintf(stdout,"Last [%d] minutes event [%d]\n",i,time_counter);
			if (!sens_struct->windows_pointers[i]) // set pointer if undefined
				{
				sens_struct->windows_pointers[i]=time_counter;
				fread(&(sens_struct->stats_array[i]), sizeof(struct stats_struct), 1, sens_struct->windows_FILE_pointer[i]);
   				}
			sens_struct->aggregated_values[i]+=stats.metrics;
			sens_struct->windows_length[i]++;
   			}
			else
				fread(&(sens_struct->stats_array[i]), sizeof(struct stats_struct), 1, sens_struct->windows_FILE_pointer[i]);
   	}
   close_file(binaryFile);

   if (time_counter)	
   	for (i=0 ; i<sens_struct->windows_number; i++) 
    		fprintf(stdout, "|A->i=[%d] FilePointer=[%lld] WinPointer=[%lld]\n", i,sens_struct->stats_array[i].intervals,sens_struct->windows_pointers[i]);
    	
   for (i=0 ; i<sens_struct->windows_number; i++) 
   	sens_struct->window_avg_values_p[i]=sens_struct->aggregated_values[i]/sens_struct->windows_limits[i];
   for (i=0 ; i<sens_struct->windows_number; i++) 
	if (sens_struct->windows_length[i])
   		sens_struct->window_avg_values_o[i]=sens_struct->aggregated_values[i]/sens_struct->windows_length[i];
	else
		sens_struct->window_avg_values_o[i]=-1; // -1 means undefined
    return 0;
}


int setup_and_synchronize_start_time(time_t *current_time, long long int *num_interval, long long int time_interval)
{
    FILE *binaryFile;
    struct start_stats_struct start_time_struct;	
    double diff_time;
    char* c_string;
    char* c2_time_string;

    // ***** START setup start time only at the beginning of the information provider ******
    if (open_create_file(&binaryFile , FILE_NAME_START_STATS,"r")) // r  - open for reading 
    {
    	time(&(start_time_struct.start_time));
    	time(current_time);
        open_create_file(&binaryFile , FILE_NAME_START_STATS,"w+");
    	write_start_time_to_file(binaryFile , &start_time_struct);
    	close_file(binaryFile);
    	*num_interval = 0;
        diff_time = 0; // by default in this branch start_time=current_time 
    } 
    else
    {
      // starting time file already exists! Read the start time value
      read_start_from_file(binaryFile,&start_time_struct);
      close_file(binaryFile);
      time(current_time);
      *num_interval=(long long int) ((*current_time-start_time_struct.start_time)/time_interval);
      diff_time = difftime(*current_time,start_time_struct.start_time);
    } 
    
    c_string = ctime(&(start_time_struct.start_time));
    printf("Starting time for stats is: %s Number of past T[i] (time intervals) [%lld]\n", c_string,*num_interval);
    *current_time = *current_time - time_interval;
    c2_time_string = ctime(current_time);
    printf("Raw (current time -1) for stats is: %s\n", c2_time_string);
    if ((int)diff_time % time_interval)
    {
	// the line below will be commented ones the sleep one will be uncommented
    	printf("Synchronize (%d) seconds\n", time_interval- ((int)diff_time % time_interval));	
	sleep(time_interval- ((int)diff_time % time_interval));
    	*current_time = *current_time + time_interval - ((int)diff_time % time_interval);
    } 
    c2_time_string = ctime(current_time);
    printf("Synchronized (current time -1) for stats is: %s\n", c2_time_string);
    // ***** END setup start time only at the beginning of the information provider ******
    return 0; 
} 

int reset_sensor_struct_and_raw_stats_file(struct sensor_struct *sens_struct)
{
    int i;
    // creates the raw metrics file if not existing 
    create_raw_file_if_not_existing(sens_struct->file_name_sensor_stats);
  
    for (i=0; i<sens_struct->windows_number; i++) 
    	{
         sens_struct->windows_pointers[i]=0;  // 0 means undefined 
         sens_struct->windows_length[i]=0;  // 0 number of samples in the interval 
         sens_struct->aggregated_values[i]=0;  // 0 sum 
         sens_struct->window_avg_values_p[i]=0;  // 0 average 
         sens_struct->window_avg_values_o[i]=0;  // 0 average 
    	 open_create_file(&(sens_struct->windows_FILE_pointer[i]), sens_struct->file_name_sensor_stats,"r"); 
   	}
    return 0; 
} 

/*int setup_host_based_aggregation_table
CREATE TABLE service_instance (
    id integer NOT NULL,
    port bigint NOT NULL,
    name character varying(255),
    institution character varying(255),
    mail_admin character varying(255),
    idhost bigint NOT NULL,
    UNIQUE(port,idhost)
);*/



int setup_sens_struct_from_config_file(struct sensor_struct *sens_struct)
{
    // TEST_
    //snprintf(query_memory_metric_insert,sizeof(query_memory_metric_insert),STORE_MEMORY_METRICS,fram,uram,fswap,uswap);
    snprintf(sens_struct->sensor_name,sizeof(sens_struct->sensor_name),"availability"); 
    snprintf(sens_struct->sensor_executable,sizeof(sens_struct->sensor_executable),"executable"); 
    snprintf(sens_struct->file_name_sensor_stats,sizeof(sens_struct->file_name_sensor_stats),FILE_NAME_STATS,sens_struct->sensor_name); 

    sens_struct->reset_onstart=0;			// reset raw_file removing the history every time the ip boots default 0 which means keeps the history 
    sens_struct->ext_sensor=0;  			// external sensor 1=yes, which means of out of the box sensor ; 0=no, which means core sensor 
    sens_struct->aggregation=0;  			// it relates to the aggregation mechanism. Default 0=no, 1=yes 
    sens_struct->time_interval=5;			
    sens_struct->windows_number=6;  	// number of time_windows to be managed 
    sens_struct->windows_limits[0]=1;
    sens_struct->windows_limits[1]=2;
    sens_struct->windows_limits[2]=4;
    sens_struct->windows_limits[3]=8;
    sens_struct->windows_limits[4]=16;
    sens_struct->windows_limits[5]=32;
	// PRODUCTION_
    /*
    sens_struct->reset_onstart=0;			// reset raw_file removing the history every time the ip boots default 0 which means keeps the history 
    sens_struct->ext_sensor=0;  			// external sensor 1=yes, which means of out of the box sensor ; 0=no, which means core sensor 
    sens_struct->aggregation=0;  			// it relates to the aggregation mechanism. Default 0=no, 1=yes 
    sens_struct->time_interval=300;			// 5 minutes
    sens_struct->windows_number=6;  			// number of time_windows to be managed 
    sens_struct->windows_limits[0]=1;
    sens_struct->windows_limits[1]=12;
    sens_struct->windows_limits[2]=12*24;
    sens_struct->windows_limits[3]=12*24*7;
    sens_struct->windows_limits[4]=12*24*30;
    sens_struct->windows_limits[5]=12*24*365;*/
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


int produce_and_append_next_sample_to_raw_file(struct stats_struct *availability_struct,long long int num_interval, char* file_name_sensor_stats)
    {
    // append the metric in the raw file
    // todo: lock file to be added
        FILE *binaryF;
    	// generate the metrics
    	availability_struct->intervals=num_interval;
    	//availability_struct.metrics= 1+(int) (10.0*rand()/(RAND_MAX+1.0)); 
    	availability_struct->metrics= num_interval; 

    	fprintf(stdout, "Generated metrics [%lld] [%4.2f] \n", availability_struct->intervals, availability_struct->metrics);

    	open_create_file(&binaryF,file_name_sensor_stats,"a+"); // a+ - open for reading and writing (append if file exists)
    	write_stats_to_file(binaryF,availability_struct);
    	close_file(binaryF);
        return 0;
    }
