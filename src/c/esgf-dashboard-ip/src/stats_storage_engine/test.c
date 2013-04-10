#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILE_NAME_STATS "raw_stats.dat"
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

struct windows_struct 
{
    long long int windows_pointers[7]; // windows pointers for 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    long long int windows_length[7];  // current windows length for 5m, 1h, 1d, 1w, 30days, 365days, ALL
    long long int windows_limits[7];  // time windows dimension for 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    double aggregated_values[7]; 	// aggregated values last 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    double window_avg_values_p[7];  // pessimistic last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    double window_avg_values_o[7];  // optimistic last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    FILE *windows_FILE_pointer[7];
    struct stats_struct stats_array[7];	
};

int main(void)
{
   time_t current_time;
   char* c_time_string;
   int i;
   int counter=10; // iterations		  
   long long int num_interval;
   struct stats_struct availability_struct,stats;	
   int time_interval=5;   // number of seconds (sleep time_interval)
   int windows_number=6;  // number of time_windows to be managed (initially 3, then 6)
   struct windows_struct win_struct;

   //todo: read config file for all the sensors!!!
   
   // creates the raw metrics file if not existing 
   create_raw_file_if_not_existing(FILE_NAME_STATS);
	
   // setup start time and synchronize current_time 
   setup_and_synchronize_start_time(&current_time, &num_interval, time_interval);

   // setup win struct 
   setup_win_struct_limits(&win_struct,&windows_number);
    		
   // initializing pointers
   reset_win_struct_pointers(windows_number,&win_struct);

   // setting up pointers	
   setup_time_windows(windows_number, num_interval, &win_struct);

   //todo: create database cache table -- table name= sensor_<metric>	
	
   //display_windows_metrics(windows_number, &win_struct);

   // this part should the one in the thread cycle
   while (counter--) 
    {
    // next serial timestamp 
    num_interval++;	
	
    // first version is serial based on the num_interval %% sensor_timespan == 0 ; sleep is sleep(1) 

    compute_and_display_current_time_stamp(&current_time, time_interval);
	
    produce_and_append_next_sample_to_raw_file(&availability_struct,num_interval);
     
    shift_windows_set(windows_number,&availability_struct,&win_struct, num_interval); 
    //todo: store local metrics in the database cache table -- recuperare dall'hostname l'IDhost 

    //todo: store external metrics in the database cache table	
    sleep(time_interval);
    }
    
   display_windows_pointers(windows_number, &win_struct);
   close_windows_FILE_pointers(windows_number, &win_struct);

   return 0;
}

int compute_and_display_current_time_stamp(time_t *current_time, int time_interval)
{
    char* c_time_string;

    *current_time = *current_time + time_interval;
    c_time_string = ctime(current_time);
    fprintf(stdout, "***************************************************\n");
    fprintf(stdout, "Regular current time for stats is %s", c_time_string);
   return 0;
}

int close_windows_FILE_pointers(int windows_number, struct windows_struct *win_struct)
{
   int i;
   for (i=0 ; i<windows_number; i++) 
   	close_file(win_struct->windows_FILE_pointer[i]);
    return 0;
}

int display_windows_pointers(int windows_number, struct windows_struct *win_struct)
{
   int i;
   for (i=0; i<windows_number; i++) 
    	fprintf(stdout,"|C-> [%d] Window pointer [%lld]\n",i,win_struct->windows_pointers[i]);
   fprintf(stdout, "end process\n");
   return 0;
}

int shift_windows_set(int windows_number,struct stats_struct *availability_struct,struct windows_struct *win_struct, long long int num_interval)
{
    int i;

    for (i=0 ; i<windows_number; i++)
    	if (!(win_struct->windows_pointers[i])) // set pointer if undefined (this means it does not exist an older last5 min sample)
    		{
		fread(&(win_struct->stats_array[i]), sizeof(struct stats_struct), 1, win_struct->windows_FILE_pointer[i]);
		win_struct->windows_pointers[i]=num_interval; // now the window exists!!!
		win_struct->windows_length[i]++;
		win_struct->aggregated_values[i]+=(availability_struct->metrics);	
   		win_struct->window_avg_values_p[i]=win_struct->aggregated_values[i]/win_struct->windows_limits[i];
   		win_struct->window_avg_values_o[i]=win_struct->aggregated_values[i]/win_struct->windows_length[i];
    		fprintf(stdout, "|A->i=[%d] Pointer=[%lld] Metrics=[%4.2f] Aggreg=[%4.2f] Length=[%lld] Optim=[%4.2f] Pessim=[%4.2f]\n", i,win_struct->windows_pointers[i],availability_struct->metrics,win_struct->aggregated_values[i],win_struct->windows_length[i], win_struct->window_avg_values_o[i],win_struct->window_avg_values_p[i]);
		}
		else 
    		{ 
		 if (win_struct->windows_length[i] == win_struct->windows_limits[i] || ((num_interval-win_struct->windows_pointers[i])==(win_struct->windows_limits[i]))) 
    			{
			win_struct->aggregated_values[i]-=win_struct->stats_array[i].metrics;
			fread(&(win_struct->stats_array[i]), sizeof(struct stats_struct), 1, win_struct->windows_FILE_pointer[i]);
			if (win_struct->windows_length[i] == win_struct->windows_limits[i])   
				{
 			   	win_struct->windows_pointers[i]++;
				fprintf(stdout, "Regular window\n");
				}
			else 
				if ((num_interval-win_struct->windows_pointers[i])==win_struct->windows_limits[i])
					{
					fprintf(stdout, "Non regular window!\n");	
					win_struct->windows_pointers[i]=win_struct->stats_array[i].intervals;
					}

			win_struct->aggregated_values[i]+=(availability_struct->metrics);
   			win_struct->window_avg_values_p[i]=win_struct->aggregated_values[i]/win_struct->windows_limits[i];
   			win_struct->window_avg_values_o[i]=win_struct->aggregated_values[i]/win_struct->windows_length[i];
   			} 
			else 
    			{
			  win_struct->windows_length[i]++;
			  win_struct->aggregated_values[i]+=(availability_struct->metrics);
   			  win_struct->window_avg_values_p[i]=win_struct->aggregated_values[i]/win_struct->windows_limits[i];
   			  win_struct->window_avg_values_o[i]=win_struct->aggregated_values[i]/win_struct->windows_length[i];
   			}
			
    		fprintf(stdout, "|B->i=[%d] Pointer=[%lld] Metrics=[%4.2f] Aggreg=[%4.2f] Length=[%lld] Optim=[%4.2f] Pessim=[%4.2f]\n", i,win_struct->windows_pointers[i],availability_struct->metrics,win_struct->aggregated_values[i],win_struct->windows_length[i], win_struct->window_avg_values_o[i],win_struct->window_avg_values_p[i]);

   		}
    return 0;
}

int display_windows_metrics(int windows_number, struct windows_struct *win_struct)
{ 
   int i=0;

   for (i=0; i<windows_number; i++) 
    	fprintf(stdout,"Last [%d] minutes window pointer [%d]\n",i,win_struct->windows_pointers[i]);
   for (i=0 ; i<windows_number; i++) // to be replaced with the line above (7 instead of 3)
	if (win_struct->windows_length[i])
   		fprintf(stdout, "Average_o[%d] on [%d] values = [%4.2f]\n", i,win_struct->windows_length[i], win_struct->window_avg_values_o[i]);
	else
   		fprintf(stdout, "Average_o[%d] is undefined\n",i);
   for (i=0 ; i<windows_number; i++) // to be replaced with the line above (7 instead of 3)
   		fprintf(stdout, "Average_p[%d]  = %4.2f\n", i,win_struct->window_avg_values_p[i]);
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

int setup_time_windows(int windows_number, long long int num_interval, struct windows_struct *win_struct)
{
    FILE *binaryFile;
    long long int time_counter;
    struct stats_struct stats;	
    int i;

    open_create_file(&binaryFile , FILE_NAME_STATS,"r"); 
    time_counter=0;
    while (1)
    	{
    	if (!fread(&stats, sizeof(struct stats_struct), 1, binaryFile))
		break;
	time_counter = stats.intervals;
    	for (i=0; i<windows_number; i++) 
		if (time_counter<=(num_interval+1) &&  time_counter>=(num_interval+1-win_struct->windows_limits[i]+1))
    			{
    			//fprintf(stdout,"Last [%d] minutes event [%d]\n",i,time_counter);
			if (!win_struct->windows_pointers[i]) // set pointer if undefined
				{
				win_struct->windows_pointers[i]=time_counter;
				fread(&(win_struct->stats_array[i]), sizeof(struct stats_struct), 1, win_struct->windows_FILE_pointer[i]);
   				}
			win_struct->aggregated_values[i]+=stats.metrics;
			win_struct->windows_length[i]++;
   			}
			else
				fread(&(win_struct->stats_array[i]), sizeof(struct stats_struct), 1, win_struct->windows_FILE_pointer[i]);
   	}
   close_file(binaryFile);

   if (time_counter)	
   	for (i=0 ; i<windows_number; i++) 
    		fprintf(stdout, "|A->i=[%d] FilePointer=[%lld] WinPointer=[%lld]\n", i,win_struct->stats_array[i].intervals,win_struct->windows_pointers[i]);
    	
   for (i=0 ; i<windows_number; i++) 
   	win_struct->window_avg_values_p[i]=win_struct->aggregated_values[i]/win_struct->windows_limits[i];
   for (i=0 ; i<windows_number; i++) 
	if (win_struct->windows_length[i])
   		win_struct->window_avg_values_o[i]=win_struct->aggregated_values[i]/win_struct->windows_length[i];
	else
		win_struct->window_avg_values_o[i]=-1; // -1 means undefined
    return 0;
}


int setup_and_synchronize_start_time(time_t *current_time, long long int *num_interval, int time_interval)
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

int reset_win_struct_pointers(int windows_number,struct windows_struct *win_struct)
{
    int i;
    for (i=0; i<windows_number; i++) 
    	{
         win_struct->windows_pointers[i]=0;  // 0 means undefined 
         win_struct->windows_length[i]=0;  // 0 number of samples in the interval 
         win_struct->aggregated_values[i]=0;  // 0 sum 
         win_struct->window_avg_values_p[i]=0;  // 0 average 
         win_struct->window_avg_values_o[i]=0;  // 0 average 
    	 open_create_file(&(win_struct->windows_FILE_pointer[i]), FILE_NAME_STATS,"r"); 
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



int setup_win_struct_limits(struct windows_struct *win_struct, int *windows_number)
{
    //*windows_number=6;  	// number of time_windows to be managed 
	// TEST_
    win_struct->windows_limits[0]=1;
    win_struct->windows_limits[1]=2;
    win_struct->windows_limits[2]=4;
    win_struct->windows_limits[3]=8;
    win_struct->windows_limits[4]=16;
    win_struct->windows_limits[5]=32;
	// PRODUCTION_
    /*win_struct->windows_limits[0]=1;
    win_struct->windows_limits[1]=12;
    win_struct->windows_limits[2]=12*24;
    win_struct->windows_limits[3]=12*24*7;
    win_struct->windows_limits[4]=12*24*30;
    win_struct->windows_limits[5]=12*24*365;*/
    win_struct->windows_limits[6]=0;  //never!!! aggregation by ALL
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

/*int initialize_stats_file_pointers(FILE** binaryFile, char* filename, long long int pointer)
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
} */

/*int initialize_stats_file_pointers2(FILE** binaryFile, char* filename, long long int pointer)
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
}*/ 

int produce_and_append_next_sample_to_raw_file(struct stats_struct *availability_struct,long long int num_interval)
    {
    // append the metric in the raw file
    // todo: lock file to be added
        FILE *binaryF;
    	// generate the metrics
    	availability_struct->intervals=num_interval;
    	//availability_struct.metrics= 1+(int) (10.0*rand()/(RAND_MAX+1.0)); 
    	availability_struct->metrics= num_interval; 

    	fprintf(stdout, "Generated metrics [%lld] [%4.2f] \n", availability_struct->intervals, availability_struct->metrics);

    	open_create_file(&binaryF,FILE_NAME_STATS,"a+"); // a+ - open for reading and writing (append if file exists)
    	write_stats_to_file(binaryF,availability_struct);
    	close_file(binaryF);
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

    /*pointer = 21;
    while (exit_while)
    {
       fseek(binaryFile, (-i)*sizeof(struct stats_struct), SEEK_END);
       if (fread(&stats, sizeof(struct stats_struct), 1, binaryFile))
		{
    		fprintf(stdout,"Reading file %lld=%4.2f\n",stats.intervals, stats.metrics);
		if (stats.intervals < pointer) {
			fseek(binaryFile,sizeof(struct stats_struct),SEEK_CUR);
    			fprintf(stdout,"Exit condition \n");
			exit_while=0;
			}
		}
		else
		   exit_while=0;	 
    } 
	 

    return 0;*/
