#define FILE_NAME_STATS "%s/raw_%s_%s_stats.dat"
#define TEMP_SEARCH_STATS_FILE "search_%s_stats.xml"
#define FILE_NAME_START_STATS "%s/start_stats_time.dat"
#define MAX_WINDOWS 10
#define MAX_SENSORS 100
#define BUFCHAR_MAX 1024
#define BUFCHAR_EXEC_MAX 1024
#define THREAD_SENSOR_OPEN_MAX 100 
#define REGISTRATION_XML_URL "http://dist.ceda.ac.uk/esgf/dist/devel/lists/registration.xml" //29-01-2016 

struct stats_struct 
{
 long long int intervals; // number of 5min intervals from the time0 (first run) 
 double metrics; // metrics value to be stored 
};

struct start_stats_struct 
{
 time_t start_time; 
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
    int federation;  			// it relates to the federation mechanism. Default 0=no, 1=point to point (1 row per host), 2 single point (all row from single host) 
    char sensor_type[BUFCHAR_MAX];		// type of the sensor (cpu, memory, availability) 
    char sensor_name[BUFCHAR_MAX];		// name of the sensor (it is the key in the info properties file)
    char sensor_table[BUFCHAR_MAX];		// table name in the database for the sensor information 
    char sensor_result[BUFCHAR_MAX];		// specific output of the sensor we are interested in 
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

