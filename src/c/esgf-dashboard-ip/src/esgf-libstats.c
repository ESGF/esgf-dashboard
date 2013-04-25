#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "../include/stats.h"
#include "../include/debug.h"
#include "../include/config.h"

/*#define FILE_NAME_STATS "raw_%s_%s_stats.dat"
#define TEMP_SEARCH_STATS_FILE "search_%s_stats.xml"
#define FILE_NAME_START_STATS "start_stats_time.dat"
#define MAX_WINDOWS 10
#define BUFCHAR_MAX 1024
#define BUFCHAR_EXEC_MAX 1024
#define THREAD_SENSOR_OPEN_MAX 10 


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
    // todo0: to be added both the current_time and num_interval che saranno specifici di ogni sensore 
    long long int time_interval;	
    long long int num_interval;
    time_t current_time;
    int windows_number;
    int reset_onstart;			// reset raw_file removing the history every time the ip boots default 0 which means keeps the history 
    int ext_sensor;  			// external sensor 1=yes, which means of out of the box sensor ; 0=no, which means core sensor 
    int aggregation;  			// it relates to the aggregation mechanism. Default 0=no, 1=yes 
    char sensor_type[BUFCHAR_MAX];		// entry among squared brackets in the config file
    char sensor_name[BUFCHAR_MAX];		// entry among squared brackets in the config file
    char file_name_sensor_stats[BUFCHAR_MAX];	// filename for the raw stats 
    char sensor_executable[BUFCHAR_EXEC_MAX];	// executable for sensors
    char sensor_args[BUFCHAR_MAX];	// args for executable for sensors
    long long int windows_pointers[MAX_WINDOWS]; // windows pointers for 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    long long int windows_length[MAX_WINDOWS];  // current windows length for 5m, 1h, 1d, 1w, 30days, 365days, ALL
    long long int windows_limits[MAX_WINDOWS];  // time windows dimension for 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    double aggregated_values[MAX_WINDOWS]; 	// aggregated values last 5m, 1h, 1d, 1w, 30days, 365days, ALL 
    double window_avg_values_p[MAX_WINDOWS];  // pessimistic last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    double window_avg_values_o[MAX_WINDOWS];  // optimistic last 5m, 1h, 1d, 1w, 30days, 365days, ALL
    FILE *windows_FILE_pointer[MAX_WINDOWS];
    struct stats_struct stats_array[MAX_WINDOWS];	
};*/

/*int main(void)
{
   struct sensor_struct sens_struct[MAX_SENSORS];
   pthread_t threads[MAX_SENSORS]; 
   unsigned int num_sensors;  // real number of sensors in the config file

   // at the beginning of the information provider	
   num_sensors = read_sensors_list_from_file(&sens_struct[0]);
   display_sensor_structures_info(num_sensors,&sens_struct[0]);

   thread_manager_start (&threads[0],&sens_struct,num_sensors);
	
   // information provider stuff
   // information provider stuff

   // at the end of the information provider	
   thread_manager_stop (&threads[0],&sens_struct,num_sensors);

   return 0;
}*/

int display_sensor_structures_info(int num_sensors, struct sensor_struct *sens_struct)
{
   int i,j;
   for (i=0; i<num_sensors;i++)
	{
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor name [%s])\n",i,(sens_struct[i]).sensor_name);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor type [%s])\n",i,(sens_struct[i]).sensor_type);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor executable [%s])\n",i,(sens_struct[i]).sensor_executable);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor args [%s])\n",i,(sens_struct[i]).sensor_args);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"File name [%s])\n",(sens_struct[i]).file_name_sensor_stats);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Reset on start [%d]\n",(sens_struct[i]).reset_onstart);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"External sensor [%d]\n",(sens_struct[i]).ext_sensor);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Aggregation enabled [%d]\n",(sens_struct[i]).aggregation);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Time interval [%d]\n",(sens_struct[i]).time_interval);
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Windows number [%d] \n",(sens_struct[i]).windows_number);
   	for (j=0; j<(sens_struct[i]).windows_number;j++)
		pmesg(LOG_DEBUG,__FILE__,__LINE__,"|-> window [%d] Limit [%d]\n",j,(sens_struct[i]).windows_limits[j]);
	}	
}

void *
thread_serve (void *arg)
{
   int counter=1; // iterations per sensor		  
   struct stats_struct availability_struct; 
   struct sensor_struct *sens_struct = (struct sensor_struct *) arg;
   //fprintf(stdout,"Calling thread server\n");
   //fprintf(stdout,"Parameter %s\n",sens_struct->file_name_sensor_stats);
   
   if (sens_struct->reset_onstart)
	remove(sens_struct->file_name_sensor_stats);
    
   // initializing pointers
   reset_sensor_struct_and_raw_stats_file(sens_struct);

   // setup start time and synchronize current_time 
   setup_and_synchronize_start_time(sens_struct);

   // setting up pointers	
   setup_time_windows(sens_struct);

   //1_todo: create database cache table -- table name= sensor_<metric>	
	
   //display_windows_metrics(&sens_struct);

   //while (1) TEST_ 
   while (counter--) 
    {
    // next serial timestamp 
    increment_num_interval(sens_struct);
	
    compute_and_display_current_time_stamp(sens_struct);
	
    produce_and_append_next_sample_to_raw_file(&availability_struct,sens_struct);
     
    shift_windows_set(&availability_struct,sens_struct); 
    //2_todo: store local metrics in the database cache table -- recuperare dall'hostname l'IDhost 

    //3_todo: conditional external metrics aggregation in the database cache table curl-based	
    sleep(sens_struct->time_interval);
    }
    
   display_windows_pointers(sens_struct);
   close_windows_FILE_pointers(sens_struct);

 return 0;
}

int sensor_specific_thread()
{
   return 0;
}

int increment_num_interval(struct sensor_struct *sens_struct)
{
    sens_struct->num_interval= sens_struct->num_interval + 1;	
   return 0;
}

int compute_and_display_current_time_stamp(struct sensor_struct *sens_struct)
{
    char* c_time_string;

    sens_struct->current_time = sens_struct->current_time + sens_struct->time_interval;
    c_time_string = ctime(&(sens_struct->current_time));
    pmesg(LOG_DEBUG,__FILE__,__LINE__,"***************************************************\n");
    pmesg(LOG_DEBUG,__FILE__,__LINE__,"Regular current time for stats is %s", c_time_string);
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
    	pmesg(LOG_DEBUG,__FILE__,__LINE__,"|C-> [%d] Window pointer [%lld]\n",i,sens_struct->windows_pointers[i]);
   pmesg(LOG_DEBUG,__FILE__,__LINE__,"|D-> sensor name [%s] sensor executable [%s] sensor type [%s] sensor filename [%s]\n",sens_struct->sensor_name,sens_struct->sensor_executable,sens_struct->sensor_type, sens_struct->file_name_sensor_stats);
   pmesg(LOG_DEBUG,__FILE__,__LINE__,"end process\n");
    
   return 0;
}

//int shift_windows_set(struct stats_struct *availability_struct,struct sensor_struct *sens_struct, long long int num_interval)
int shift_windows_set(struct stats_struct *availability_struct,struct sensor_struct *sens_struct)
{
    int i;

    for (i=0 ; i<sens_struct->windows_number; i++)
    	if (!(sens_struct->windows_pointers[i])) // set pointer if undefined (this means it does not exist an older last5 min sample)
    		{
		fread(&(sens_struct->stats_array[i]), sizeof(struct stats_struct), 1, sens_struct->windows_FILE_pointer[i]);
		sens_struct->windows_pointers[i]=sens_struct->num_interval; // now the window exists!!!
		sens_struct->windows_length[i]++;
		sens_struct->aggregated_values[i]+=(availability_struct->metrics);	
   		sens_struct->window_avg_values_p[i]=sens_struct->aggregated_values[i]/sens_struct->windows_limits[i];
   		sens_struct->window_avg_values_o[i]=sens_struct->aggregated_values[i]/sens_struct->windows_length[i];
    		pmesg(LOG_DEBUG,__FILE__,__LINE__,"|A->i=[%d] Pointer=[%lld] Metrics=[%4.2f] Aggreg=[%4.2f] Length=[%lld] Optim=[%4.2f] Pessim=[%4.2f]\n", i,sens_struct->windows_pointers[i],availability_struct->metrics,sens_struct->aggregated_values[i],sens_struct->windows_length[i], sens_struct->window_avg_values_o[i],sens_struct->window_avg_values_p[i]);
		}
		else 
    		{ 
		 if (sens_struct->windows_length[i] == sens_struct->windows_limits[i] || ((sens_struct->num_interval-sens_struct->windows_pointers[i])==(sens_struct->windows_limits[i]))) 
    			{
			sens_struct->aggregated_values[i]-=sens_struct->stats_array[i].metrics;
			fread(&(sens_struct->stats_array[i]), sizeof(struct stats_struct), 1, sens_struct->windows_FILE_pointer[i]);
			if (sens_struct->windows_length[i] == sens_struct->windows_limits[i])   
				{
 			   	sens_struct->windows_pointers[i]++;
				pmesg(LOG_DEBUG,__FILE__,__LINE__,"Regular window\n");
				}
			else 
				if ((sens_struct->num_interval-sens_struct->windows_pointers[i])==sens_struct->windows_limits[i])
					{
					pmesg(LOG_DEBUG,__FILE__,__LINE__, "Non regular window!\n");	
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
			
    		pmesg(LOG_DEBUG,__FILE__,__LINE__,"|B->i=[%d] Pointer=[%lld] Metrics=[%4.2f] Aggreg=[%4.2f] Length=[%lld] Optim=[%4.2f] Pessim=[%4.2f]\n", i,sens_struct->windows_pointers[i],availability_struct->metrics,sens_struct->aggregated_values[i],sens_struct->windows_length[i], sens_struct->window_avg_values_o[i],sens_struct->window_avg_values_p[i]);

   		}
    return 0;
}

int display_windows_metrics(struct sensor_struct *sens_struct)
{ 
   int i=0;

   for (i=0; i<sens_struct->windows_number; i++) 
    	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Last [%d] minutes window pointer [%d]\n",i,sens_struct->windows_pointers[i]);
   for (i=0 ; i<sens_struct->windows_number; i++) // to be replaced with the line above (7 instead of 3)
	if (sens_struct->windows_length[i])
   		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Average_o[%d] on [%d] values = [%4.2f]\n", i,sens_struct->windows_length[i], sens_struct->window_avg_values_o[i]);
	else
   		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Average_o[%d] is undefined\n",i);
   for (i=0 ; i<sens_struct->windows_number; i++) // to be replaced with the line above (7 instead of 3)
   		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Average_p[%d]  = %4.2f\n", i,sens_struct->window_avg_values_p[i]);
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

int setup_time_windows(struct sensor_struct *sens_struct)
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
		if (time_counter<=(sens_struct->num_interval+1) &&  time_counter>=(sens_struct->num_interval+1-sens_struct->windows_limits[i]+1))
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
    		pmesg(LOG_DEBUG,__FILE__,__LINE__,"|A->i=[%d] FilePointer=[%lld] WinPointer=[%lld]\n", i,sens_struct->stats_array[i].intervals,sens_struct->windows_pointers[i]);
    	
   for (i=0 ; i<sens_struct->windows_number; i++) 
   	sens_struct->window_avg_values_p[i]=sens_struct->aggregated_values[i]/sens_struct->windows_limits[i];
   for (i=0 ; i<sens_struct->windows_number; i++) 
	if (sens_struct->windows_length[i])
   		sens_struct->window_avg_values_o[i]=sens_struct->aggregated_values[i]/sens_struct->windows_length[i];
	else
		sens_struct->window_avg_values_o[i]=-1; // -1 means undefined
    return 0;
}

int setup_and_synchronize_start_time(struct sensor_struct *sens_struct)
{
    FILE *binaryFile;
    struct start_stats_struct start_time_struct;	
    double diff_time;
    char* c_string;
    char* c2_time_string;
    char file_name_start_stats[1024];

    // ***** START setup start time only at the beginning of the information provider ******
    snprintf(file_name_start_stats,sizeof(file_name_start_stats),FILE_NAME_START_STATS,DASHBOARD_SERVICE_PATH); 
    
    if (open_create_file(&binaryFile , file_name_start_stats,"r")) // r  - open for reading 
    {
    	time(&(start_time_struct.start_time));
    	time(&(sens_struct->current_time));
        open_create_file(&binaryFile , file_name_start_stats,"w+");
    	write_start_time_to_file(binaryFile , &start_time_struct);
    	close_file(binaryFile);
    	sens_struct->num_interval = 0;
        diff_time = 0; // by default in this branch start_time=current_time 
    } 
    else
    {
      // starting time file already exists! Read the start time value
      read_start_from_file(binaryFile,&start_time_struct);
      close_file(binaryFile);
      time(&(sens_struct->current_time));
      sens_struct->num_interval=(long long int) ((sens_struct->current_time-start_time_struct.start_time)/sens_struct->time_interval);
      diff_time = difftime(sens_struct->current_time,start_time_struct.start_time);
    } 
    
    c_string = ctime(&(start_time_struct.start_time));
    pmesg(LOG_DEBUG,__FILE__,__LINE__,"Starting time for stats is: %s Number of past T[i] (time intervals) [%lld]\n", c_string,sens_struct->num_interval);
    sens_struct->current_time = sens_struct->current_time - sens_struct->time_interval;
    c2_time_string = ctime(&(sens_struct->current_time));
    pmesg(LOG_DEBUG,__FILE__,__LINE__,"Raw (current time -1) for stats is: %s\n", c2_time_string);
    if ((int)diff_time % sens_struct->time_interval)
    {
	// the line below will be commented ones the sleep one will be uncommented
    	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Synchronize (%d) seconds\n", sens_struct->time_interval- ((int)diff_time % sens_struct->time_interval));	
	sleep(sens_struct->time_interval- ((int)diff_time % sens_struct->time_interval));
    	sens_struct->current_time = sens_struct->current_time + sens_struct->time_interval - ((int)diff_time % sens_struct->time_interval);
    } 
    c2_time_string = ctime(&(sens_struct->current_time));
    pmesg(LOG_DEBUG,__FILE__,__LINE__,"Synchronized (current time -1) for stats is: %s\n", c2_time_string);
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

// todo: dovrebbe restituire il numero di sensori rilevati dal file di configurazione 
int setup_sens_struct_from_config_file(struct sensor_struct *sens_struct)
{
    

    // TEST_
    //snprintf(query_memory_metric_insert,sizeof(query_memory_metric_insert),STORE_MEMORY_METRICS,fram,uram,fswap,uswap);
    //snprintf(sens_struct->sensor_name,sizeof(sens_struct->sensor_name),"availability"); 
    //snprintf(sens_struct->sensor_executable,sizeof(sens_struct->sensor_executable),"executable availability"); 
    //snprintf(sens_struct->file_name_sensor_stats,sizeof(sens_struct->file_name_sensor_stats),FILE_NAME_STATS,sens_struct->sensor_name); 
    snprintf(sens_struct->sensor_args,sizeof(sens_struct->sensor_args), ""); 
    snprintf(sens_struct->sensor_type,sizeof(sens_struct->sensor_type), ""); 
    snprintf(sens_struct->sensor_executable,sizeof(sens_struct->sensor_executable), ""); 
    sens_struct->reset_onstart=0;			// reset raw_file removing the history every time the ip boots default 0 which means keeps the history 
    sens_struct->ext_sensor=0;  			// external sensor 1=yes, which means of out of the box sensor ; 0=no, which means core sensor 
    sens_struct->aggregation=0;  			// it relates to the aggregation mechanism. Default 0=no, 1=yes 
    sens_struct->time_interval=300;			// default 5 minutes	
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
    pmesg(LOG_DEBUG,__FILE__,__LINE__,"Reading file %lld=%4.2f\n",stats.intervals, stats.metrics);
   }
   return 0;
}

/*int open_create_file(FILE** file , char* filename, char* mode)
{
    *file = fopen(filename,mode);
    if ((*file) == NULL)
	{
	 fprintf(stdout,"Can't open binary file [%s] in [%s] mode\n",filename,mode);
	 return -1; 
	}
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
	 pmesg(LOG_DEBUG,__FILE__,__LINE__,"Can't write on binary output file\n");
	 return -2; 
	}
    return 0;
}

int write_stats_to_file(FILE* file , struct stats_struct *stats )
{
    if ( fwrite( stats, sizeof(struct stats_struct), 1, file ) != 1)
	{
	 pmesg(LOG_DEBUG,__FILE__,__LINE__,"Can't write on binary output file\n");
	 return -2; 
	}
    return 0;
}

/*int close_file(FILE* file)
{
    fclose( file );
    return 0;
}*/

long long int print_element_names(xmlNode * a_node)
{    
    char *num_rec;
    xmlNode *cur_node = NULL;
    long long int num_rec_d;

    
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
            //printf("node type: Element, name: %s\n", cur_node->name);
	    if (cur_node->type == XML_ELEMENT_NODE && !strcmp(cur_node->name,"response")) 
	    	cur_node= cur_node->children;
	    if (cur_node->type == XML_ELEMENT_NODE && !strcmp(cur_node->name,"result")) 
	     	num_rec = xmlGetProp (cur_node, "numFound");	
        }
   	
  num_rec_d=atoll(num_rec); 
  free(num_rec);	
  return num_rec_d; 
}
	
long long int parse_xml_search_file(char* tmp_file)
{
  int while_end;
  long long int num_rec_d=0;
  char *position;
  char *position2;

  FILE *file = fopen (tmp_file, "r");

  if (file == NULL)             // /esg/config/infoprovider.properties not found
    return -1;

  while_end = 1;

  while (while_end)
    {
      char buffer[256] = { '\0' };
      char value_buffer[256] = { '\0' };
      int strlen_buf,j;

      if (!(fgets (buffer,256, file))) // now reading ATTRIBUTE=VALUE
        {
	  //fprintf(stdout,"end of file\n");
	  while_end = 0;
	  break;	
        }
      //fprintf(stdout,"line = %s",buffer);
      j=0;
      strlen_buf = strlen(buffer);
      while (j<strlen_buf)
        {
	  if (buffer[j]=='n')
	    if (buffer[j+1]=='u')
  	      if (buffer[j+2]=='m')
  	      	if (buffer[j+3]=='F')
  	      	   if (buffer[j+4]=='o')
  	      	      if (buffer[j+5]=='u')
  	      	      	if (buffer[j+6]=='n')
  	      	      	   if (buffer[j+7]=='d')
        			{
          			 position = strchr(&(buffer[j+7]),'"');  
          			 position2 = strchr(position+1,'"');  
				 *position2='\0';
				 //fprintf(stdout,"trovato %s\n",position+1);
  				 num_rec_d=atoll(position+1); 
				 while_end = 0; 	
			     	 break; 
        			}
	  j++;	
        }
 	

      
      /*if (position != NULL)     // the '=' has been found, which means the line is properly written as attribute=value
        {
          strcpy (value_buffer, position + 1);  
          *position = '\0';    // now value_buffer stores the VALUE 
          position = strchr(value_buffer,'\n');  
	  *position = '\0';     // now buffer stores the ATTRIBUTE       
          //fprintf(stdout,"Attribute=[%s] Value=[%s]\n",buffer,value_buffer);
	}*/
   }
  fclose (file);
  return num_rec_d; 
}


//int get_search_metrics(struct sensor_struct *sens_struct)
int get_search_metrics(char* name, char* query)
{
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  CURL *curl;
  CURLcode curl_res;
  CURLINFO info;
  long http_code;
  double c_length;
  FILE *tmp;
  FILE *file;
  char buffer[10024];
  char url_action[10024];
  char tmp_file[1024];
  long int i;
  long long int num_rec;
  int right_url;

  snprintf (url_action, sizeof (url_action),query);
  snprintf (tmp_file, sizeof (tmp_file),TEMP_SEARCH_STATS_FILE,name);

  // filename to be stats_<host>_<processed_id>.tmp

  tmp=fopen(tmp_file, "w");
  if(tmp==NULL)
        {
         //pmesg(LOG_ERROR,__FILE__,__LINE__,"ERROR to open file stats.tmp\n");
         return -2;
        }

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url_action);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,  tmp);
  curl_res = curl_easy_perform(curl);

  if(curl_res)
        {
        //pmesg(LOG_ERROR,__FILE__,__LINE__,"ERROR contatting [%s] or downloading stats\n",peername);
        remove(tmp_file);
        fclose(tmp);
        curl_easy_cleanup(curl);
        return -1;
        }
  fclose(tmp);
  curl_easy_cleanup(curl);

  num_rec = parse_xml_search_file(tmp_file);

  remove(tmp_file);

 return(num_rec);
}

int produce_and_append_next_sample_to_raw_file(struct stats_struct *availability_struct,struct sensor_struct *sens_struct)
    {
    // append the metric in the raw file
    // todo: lock file to be added
        FILE *binaryF;
    	
	// todo: The generate metrics should be a function with a SWITCH based on the type of sensor. Core types: availability, search, exec	
    	// generate the metrics
	
    	availability_struct->intervals=sens_struct->num_interval;
	if (!(strcmp(sens_struct->sensor_type,"search")))	
		availability_struct->metrics= (double) get_search_metrics(sens_struct->sensor_name, sens_struct->sensor_executable);
	if (!(strcmp(sens_struct->sensor_type,"availability")))	
    		availability_struct->metrics= sens_struct->num_interval; 

    	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Generated metrics sensor [%s] [%lld]-[%4.2f] \n", sens_struct->sensor_name,availability_struct->intervals, availability_struct->metrics);

    	open_create_file(&binaryF,sens_struct->file_name_sensor_stats,"a+"); // a+ - open for reading and writing (append if file exists)
    	write_stats_to_file(binaryF,availability_struct);
    	close_file(binaryF);
        return 0;
   }

/*int
thread_manager (struct sensor_struct *sens_struct, const unsigned num_sensors)
{
  const int MIN = num_sensors < THREAD_SENSOR_OPEN_MAX ? num_sensors : THREAD_SENSOR_OPEN_MAX;
  pthread_t *threads = (pthread_t *) malloc (sizeof (pthread_t) * MIN);
  unsigned h = 0;
  int res;

  while (h < num_sensors)
    {
      unsigned c;
      for (c = 0; h < num_sensors && c < MIN; h++, c++)
        pthread_create (threads + c, NULL, thread_serve, sens_struct);

      for (c = 0; c <= (h - 1) % MIN; c++)
        {
          void *ptr = NULL;
          res = pthread_join (threads[c], &ptr);
	  fprintf(stdout,"After joining the thread - res vale [%d] \n",res);
        }
    }

  free (threads);
  return 0;
}*/




int
thread_manager_start (pthread_t *threads, struct sensor_struct *sens_struct, const unsigned num_sensors)
{
//  pthread_t *threads = (pthread_t *) malloc (sizeof (pthread_t) * num_sensors);
  unsigned c = 0;
  //int res;

  for (c = 0; c < num_sensors; c++)
        pthread_create (threads + c, NULL, thread_serve, &sens_struct[c]);
	
/*  for (c = 0; c < num_sensors; c++)
        {
          void *ptr = NULL;
          res = pthread_join (threads[c], &ptr);
	  fprintf(stdout,"After joining the thread - res vale [%d] \n",res);
        }*/

//  free (threads);
  return 0;
}

int
thread_manager_stop (pthread_t *threads, struct sensor_struct *sens_struct, const unsigned num_sensors)
{
  unsigned c = 0;
  int res;

  for (c = 0; c < num_sensors; c++)
        {
          void *ptr = NULL;
          res = pthread_join (threads[c], &ptr);
	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"After joining the thread - res vale [%d] \n",res);
        }
//  free (threads);
  return 0;
}

int read_sensors_list_from_file(struct sensor_struct *sens_struct)
{
  char sensor_file[256] = { '\0' };
  char *position;
  int while_end,found_sensor, curr_sensor;

  snprintf (sensor_file,sizeof(sensor_file),"/esg/config/infoprovider.properties");

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s\n", sensor_file);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[START] %s\n", sensor_file);
 
  FILE *file = fopen (sensor_file, "r");

  if (file == NULL)             // /esg/config/infoprovider.properties not found
    return -1;

  while_end = 1;
  found_sensor=0;
  curr_sensor=-1;

  while (while_end)
    {
      char buffer[256] = { '\0' };
      char value_buffer[256] = { '\0' };

      if (!(fgets (buffer,256,file))) // now reading ATTRIBUTE=VALUE
        {
	  //fprintf(stdout,"end of file\n");
	  while_end = 0;
          fclose (file);
	  break;	
        }
      //fprintf(stdout,"line = %s\n",buffer);

      position = strchr (buffer, '=');
      if (position != NULL)     // the '=' has been found, which means the line is properly written as attribute=value
        {
          strcpy (value_buffer, position + 1);  
          *position = '\0';    // now value_buffer stores the VALUE 
          position = strchr(value_buffer,'\n');  
	  *position = '\0';     // now buffer stores the ATTRIBUTE       
	  	
          //fprintf(stdout,"Attribute=[%s] Value=[%s]\n",buffer,value_buffer);
          if (!(strcmp (buffer, "sensor"))) // IMPO mandatory: first property to be defined
            {
		curr_sensor++; // this index is the right one for this sensor
		snprintf((sens_struct[curr_sensor]).sensor_name,sizeof((sens_struct[curr_sensor]).sensor_name),value_buffer);
		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor name [%s])\n",curr_sensor,(sens_struct[curr_sensor]).sensor_name);
   		setup_sens_struct_from_config_file(&sens_struct[curr_sensor]);

            }
          if (!(strcmp (buffer, "type"))) // mandatory
            {
		snprintf((sens_struct[curr_sensor]).sensor_type,sizeof((sens_struct[curr_sensor]).sensor_type),value_buffer);
		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor type [%s])\n",curr_sensor,(sens_struct[curr_sensor]).sensor_type);
    		snprintf((sens_struct[curr_sensor]).file_name_sensor_stats,sizeof((sens_struct[curr_sensor]).file_name_sensor_stats),FILE_NAME_STATS,DASHBOARD_SERVICE_PATH,sens_struct[curr_sensor].sensor_type,sens_struct[curr_sensor].sensor_name); 
            }
          if (!(strcmp (buffer, "reset")))
            {
		sens_struct[curr_sensor].reset_onstart=atoi(value_buffer);
		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor reset_onstart [%d])\n",curr_sensor,(sens_struct[curr_sensor]).reset_onstart);
            }
          if (!(strcmp (buffer, "interval")))
            {
		sens_struct[curr_sensor].time_interval=atoll(value_buffer);
		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor time_interval [%lld])\n",curr_sensor,(sens_struct[curr_sensor]).time_interval);
            }
          if (!(strcmp (buffer, "exec")))
            {
		snprintf((sens_struct[curr_sensor]).sensor_executable,sizeof((sens_struct[curr_sensor]).sensor_executable),value_buffer);
		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor executable [%s])\n",curr_sensor,(sens_struct[curr_sensor]).sensor_executable);
            }
          if (!(strcmp (buffer, "args")))
            {
		snprintf((sens_struct[curr_sensor]).sensor_args,sizeof((sens_struct[curr_sensor]).sensor_args),value_buffer);
		pmesg(LOG_DEBUG,__FILE__,__LINE__,"Sensor info index [%d] Sensor args [%s])\n",curr_sensor,(sens_struct[curr_sensor]).sensor_args);
            }
         }
	else {
	buffer[strlen(buffer)-1]='\0';
	pmesg(LOG_DEBUG,__FILE__,__LINE__,"Skipping line [%s]\n",buffer);
    	}

    }
  curr_sensor++; // now curr_sensor states the number of sensors. 
  return curr_sensor;
}

