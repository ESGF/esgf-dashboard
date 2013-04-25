#define FILE_NAME_STATS "%s/raw_%s_%s_stats.dat"
#define TEMP_SEARCH_STATS_FILE "search_%s_stats.xml"
#define FILE_NAME_START_STATS "%s/start_stats_time.dat"
#define MAX_WINDOWS 10
#define MAX_SENSORS 10
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
};

