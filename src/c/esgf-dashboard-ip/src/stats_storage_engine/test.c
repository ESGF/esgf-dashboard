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
    int i;
    //int counter=10512;		  
    int counter=12;		  
    long long int num_interval,pointer;
    long long int num_interval_1h;
    FILE *binaryFile;
    FILE *cur_record;
    FILE *windows_FILE_pointer[7];
    struct stats_struct availability_struct,stats;	
    struct stats_struct stats_array[7];	
    struct stats_struct availability_struct_1h;	
    struct start_stats_struct start_time_struct;	
    FILE* pointer_1h;	
    double av_5m;
    long long int time_counter;
    long long int windows_pointers[7];  // last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    long long int windows_length[7];  // last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    long long int windows_limits[7];  // time windows dimension 
    double aggregated_values[7];  // last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    double window_avg_values_p[7];  // pessimistic last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    double window_avg_values_o[7];  // optimistic last 5m, 1h, 1d, 1w, 30days, 365days, ALL

	
    // creates the raw stats file if not existing 
    if (open_create_file(&binaryFile , FILE_NAME_STATS,"r")) // r  - open for reading 
        open_create_file(&binaryFile ,FILE_NAME_STATS, "w+");
    close_file(binaryFile);
    windows_limits[0]=1;
    windows_limits[1]=12;
    windows_limits[2]=12*24;
    windows_limits[3]=12*24*7;
    windows_limits[4]=12*24*30;
    windows_limits[5]=12*24*365;
    windows_limits[6]=0;  //never!!! aggregation by ALL
    		
    // file exists now!	
    

    // stats file exists now!	

    //initialize_stats_pointers_files(FILE_NAME_STATS, &pointer[0], &null_rec_pointer);

    // ***** START setup start time only at the beginning of the information provider ******
    if (open_create_file(&binaryFile , FILE_NAME_START_STATS,"r")) // r  - open for reading 
    {
    	time(&(start_time_struct.start_time));
    	time(&(current_time));
        open_create_file(&binaryFile , FILE_NAME_START_STATS,"w+");
    	write_start_time_to_file(binaryFile , &start_time_struct);
    	close_file(binaryFile);
    	num_interval = 0;
        diff_time = 0; // by default in this branch start_time=current_time 
    } 
    else
    {
      // starting time file already exists! Read the start time value
      read_start_from_file(binaryFile,&start_time_struct);
      close_file(binaryFile);
      time(&(current_time));
      num_interval=(long long int) ((current_time-start_time_struct.start_time)/300);
      diff_time = difftime(current_time,start_time_struct.start_time);
    } 
    
    c_time_string = ctime(&(start_time_struct.start_time));
    printf("Starting time for stats is: %s Number of past T[i] (time intervals) [%lld]\n", c_time_string,num_interval);
    current_time = current_time - 300;
    c2_time_string = ctime(&(current_time));
    printf("Raw (current time -1) for stats is: %s\n", c2_time_string);
    if ((int)diff_time % 300)
    {
	// the line below will be commented ones the sleep one will be uncommented
    	printf("sleep(%d) seconds\n", 300- ((int)diff_time % 300));	
	//sleep(300- ((int)diff_time % 300));
    	current_time = current_time + 300 - ((int)diff_time % 300);
    } 
    c2_time_string = ctime(&(current_time));
    printf("Synchronized (current time -1) for stats is: %s\n", c2_time_string);
    // ***** END setup start time only at the beginning of the information provider ******
		
    // defining pointers for stats
    //num_interval_1h=num_interval-12;
    //initialize_stats_file_pointers(&pointer_1h, FILE_NAME_STATS, num_interval_1h);
    //fclose (pointer_1h);
   
    // building pointers and time windows for 1h,1d,1w,1m,1y,all

    // initializing pointers
    for (i=0; i<3; i++) 
    	{
         windows_pointers[i]=0;  // 0 means undefined 
         windows_length[i]=0;  // 0 number of samples in the interval 
         aggregated_values[i]=0;  // 0 sum 
         window_avg_values_p[i]=0;  // 0 average 
         window_avg_values_o[i]=0;  // 0 average 
    	 open_create_file(&(windows_FILE_pointer[i]), FILE_NAME_STATS,"r"); 
   	}
    
    open_create_file(&binaryFile , FILE_NAME_STATS,"r"); 
    time_counter=0;
    while (1)
    	{
    	if (!fread(&stats, sizeof(struct stats_struct), 1, binaryFile))
		break;
	time_counter = stats.intervals;
	if (time_counter<=num_interval &&  time_counter>(num_interval-1))
    		{
    		fprintf(stdout,"Last 5 minutes event [%d]\n",time_counter);
		if (!windows_pointers[0]) // set pointer if undefined
			windows_pointers[0]=time_counter;
		aggregated_values[0]+=stats.metrics;
		windows_length[0]++;
   		}
		else
			fread(&(stats_array[0]), sizeof(struct stats_struct), 1, windows_FILE_pointer[0]);

	if (time_counter<=num_interval &&  time_counter>(num_interval-12)) 
    		{
    		fprintf(stdout,"Last hour event [%d]\n",time_counter);
		if (!windows_pointers[1]) // set pointer if undefined
			windows_pointers[1]=time_counter;
		aggregated_values[1]+=stats.metrics;
		windows_length[1]++;
   		}
		else
			fread(&(stats_array[1]), sizeof(struct stats_struct), 1, windows_FILE_pointer[1]);

	if (time_counter<=num_interval &&  time_counter>(num_interval-12*24)) 
    		{
    		fprintf(stdout,"Last day event [%d]\n",time_counter);
		if (!windows_pointers[2]) // set pointer if undefined
			windows_pointers[2]=time_counter;
		aggregated_values[2]+=stats.metrics;
		windows_length[2]++;
   		}
		else
			fread(&(stats_array[2]), sizeof(struct stats_struct), 1, windows_FILE_pointer[2]);

    	//fprintf(stdout,"Reading file %lld=%4.2f\n",stats.intervals, stats.metrics);
   	}
   close_file(binaryFile);

   for (i=0 ; i<3; i++) // to be replaced with the line above (7 instead of 3)
   	window_avg_values_p[i]=aggregated_values[i]/windows_limits[i];
   	
   	
   //window_avg_values_p[0]=aggregated_values[0]/1;
   //window_avg_values_p[1]=aggregated_values[1]/12;
   //window_avg_values_p[2]=aggregated_values[2]/(12*24);
   /*window_avg_values_p[3]=aggregated_values[3]/(12*24*7);
   window_avg_values_p[4]=aggregated_values[4]/(12*24*30);
   window_avg_values_p[5]=aggregated_values[5]/(12*24*365);
   window_avg_values_p[6]=aggregated_values[6]/num_interval;*/

   //for (i=0 ; i<7; i++)
   for (i=0 ; i<3; i++) // to be replaced with the line above (7 instead of 3)
	if (windows_length[i])
   		window_avg_values_o[i]=aggregated_values[i]/windows_length[i];
	else
		window_avg_values_o[i]=-1; // -1 means undefined
   //window_avg_values_o[6]=aggregated_values[6]/num_interval;

   //for (i=0 ; i<7; i++)
   for (i=0 ; i<3; i++) // to be replaced with the line above (7 instead of 3)
	if (windows_length[i])
   		fprintf(stdout, "Average_o[%d] on [%d] values = [%4.2f]\n", i,windows_length[i], window_avg_values_o[i]);
	else
   		fprintf(stdout, "Average_o[%d] is undefined\n",i);

   //for (i=0 ; i<7; i++)
   for (i=0 ; i<3; i++) // to be replaced with the line above (7 instead of 3)
   		fprintf(stdout, "Average_p[%d]  = %4.2f\n", i,window_avg_values_p[i]);

   // end building pointers and time windows for 1h,1d,1w,1m,1y,all

   //for (i=0 ; i<7; i++)
   //for (i=0 ; i<3; i++) // to be replaced with the line above (7 instead of 3)
   //		fprintf(stdout, "Pointer[%d]=%lld   value=%4.2f\n",i ,stats_array[i].intervals,stats_array[i].metrics);
   
  //return 0;

   while (counter--) 
    {
    num_interval++;	
    current_time = current_time + 300;
    c_time_string = ctime(&(current_time));
    printf("Regular current time for stats is %s", c_time_string);

    // generate the metrics
    availability_struct.intervals=num_interval;
    availability_struct.metrics= 1+(int) (100.0*rand()/(RAND_MAX+1.0)); 

    fprintf(stdout, "Generated metrics [%lld] [%4.2f] \n", availability_struct.intervals, availability_struct.metrics);
	
    event_last5_minutes_occurred(&availability_struct);
    
    for (i=0 ; i<3; i++)
    	if (!windows_pointers[i]) // set pointer if undefined (this means it does not exist an older last5 min sample)
    		{
		windows_pointers[i]=num_interval; // now the window exists!!!
		aggregated_values[i]+=availability_struct.metrics;
   		window_avg_values_p[i]=aggregated_values[i]/windows_limits[i];
		windows_length[i]++;
   		window_avg_values_o[i]=aggregated_values[i]/windows_length[i];
		fread(&(stats_array[i]), sizeof(struct stats_struct), 1, windows_FILE_pointer[i]);
		}
		else 
    		{
		 if (windows_length[i] == windows_limits[i]) // if the window reaches the limit... then move the window!!!
    			{
			windows_pointers[i]++;
			aggregated_values[i]+=availability_struct.metrics;
			aggregated_values[i]-=stats_array[i].metrics;
   			window_avg_values_p[i]=aggregated_values[i]/windows_limits[i];
   			window_avg_values_o[i]=aggregated_values[i]/windows_length[i];
			fread(&(stats_array[i]), sizeof(struct stats_struct), 1, windows_FILE_pointer[i]);
   			} 
			else // this piece of code will be never executed for last5min 
    			{
			  windows_length[i]++;
			  aggregated_values[i]+=availability_struct.metrics;
   			}

   		}

   for (i=0 ; i<3; i++) // to be replaced with the line above (7 instead of 3)
   		fprintf(stdout, "Average_p[%d]  = %4.2f\n", i,window_avg_values_p[i]);
   for (i=0 ; i<3; i++) // to be replaced with the line above (7 instead of 3)
	if (windows_length[i])
   		fprintf(stdout, "Average_o[%d] on [%d] values = [%4.2f]\n", i,windows_length[i], window_avg_values_o[i]);
	else
   		fprintf(stdout, "Average_o[%d] is undefined\n",i);
     
    //read_stats_from_file(pointer_1h,&availability_struct_1h);
    //fprintf(stdout, "********* 1h pointer--> [%lld] [%4.2f] \n", availability_struct_1h.intervals, availability_struct_1h.metrics);
    //av_5m = availability_struct.metrics;
    
    
    /*if (!(num_interval % (12*1)) ) // 12 campioni di 5 minuti = 1h
	event_last_hour_occurred(num_interval);

    if (!(num_interval % (12*24)) ) // 12*24 campioni di 5 minuti = 1day
	event_last_day_occurred(num_interval);

    if (!(num_interval % (12*24*7)) ) // 12*24*7 campioni di 5 minuti = 1week
	event_last_week_occurred(num_interval);

    if (!(num_interval % (12*24*30)) ) // 12*24*30 campioni di 5 minuti = 1month (actually 30days)
	event_last30_days_occurred(num_interval);

    if (!(num_interval % (12*24*365)) ) // 12*24*365 campioni di 5 minuti = 1year
	event_last_year_occurred(num_interval);*/	
    }
    
    //fprintf(stdout, "Displaying file content \n");
    //open_create_file(&binaryFile , FILE_NAME_STATS,"r+"); 
    //display_file(binaryFile);
    //close_file(binaryFile);

    //fclose (pointer_1h);

   //for (i=0 ; i<7; i++)

   for (i=0 ; i<3; i++) // to be replaced with the line above (7 instead of 3)
   	close_file(windows_FILE_pointer[i]);
   		
    fprintf(stdout, "end process\n");
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

int event_last5_minutes_occurred(struct stats_struct *availability_struct)
    {
    // store the metric in the raw file
    // todo: lock file to be added
        FILE *binaryF;
    	open_create_file(&binaryF,FILE_NAME_STATS,"a+"); // a+ - open for reading and writing (append if file exists)
    	write_stats_to_file(binaryF,availability_struct);
    	close_file(binaryF);
    	printf("- Event last 5 minutes occured [5m=%lld]\n\n",availability_struct->intervals);
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
