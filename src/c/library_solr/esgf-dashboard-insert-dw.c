#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "hashtbl.h"
#include "ftpget.h"
#include <libpq-fe.h>
#include <time.h>
#include "GeoIP.h"
#include "GeoIPCity.h"
#include "config.h"

// QUERY_INSERT_CROSS_DIM_DATE adds a new date of the downloaded file in the database
#define QUERY_INSERT_CROSS_DIM_DATE  "INSERT into esgf_dashboard.cross_dim_date(download_date,month,year) values('%s',%d,%d);"
// QUERY_INSERT_CROSS_DIM_GEOLOCATION adds a new geolocation of the downloaded file in the database
#define QUERY_INSERT_CROSS_DIM_GEOLOCATION "INSERT into esgf_dashboard.cross_dim_geolocation(latitude,longitude,country_id) values(%14.11f,%14.11f,%d);"
// QUERY_INSERT_CROSS_DIM_PROJECT adds a new project related to the downloaded file in the database
#define QUERY_INSERT_CROSS_DIM_PROJECT "INSERT into esgf_dashboard.cross_dim_project(project_name) values('%s');"
// QUERY_INSERT_CROSS_BRIDGE_PROJECT adds a new project related to the downloaded file in the database
#define QUERY_INSERT_CROSS_BRIDGE_PROJECT "INSERT into esgf_dashboard.cross_bridge_project(project_key,project_group_key) values(%d,%d);"
// QUERY_INSERT_CROSS_FACT_DOWNLOAD adds a new fact related to the downloaded file in the database
#define QUERY_INSERT_CROSS_FACT_DOWNLOAD "INSERT into esgf_dashboard.cross_fact_download(size,success,duration,replica,user_id_hash,host_name, user_idp,hour, minute, project_group_key, geolocation_key,date_key) values(%.0f,'%s',%d,'%s','%s','%s','%s','%s','%s',%d,%d,%d);"
// Get list of CROSS DIM DATE
#define QUERY_GET_LIST_OF_CROSS_DIM_DATE "SELECT download_date,date_key from esgf_dashboard.cross_dim_date;"
// Get list of CROSS DIM GEOLOCATION 
#define QUERY_GET_LIST_OF_CROSS_DIM_GEOLOCATION "SELECT (CAST(latitude as numeric(14,11)))||':'||(CAST(longitude as numeric(14,11))) AS name,geolocation_key from esgf_dashboard.cross_dim_geolocation;"
// Get list of CROSS DIM PROJECT 
#define QUERY_GET_LIST_OF_CROSS_DIM_PROJECT "SELECT project_name,project_key from esgf_dashboard.cross_dim_project;"
// Get list of CROSS BRIDGE PROJECT 
#define QUERY_GET_LIST_OF_CROSS_BRIDGE_PROJECT "SELECT project_group_key, project_key from esgf_dashboard.cross_bridge_project;"
// Get list of CROSS BRIDGE PROJECT 
#define QUERY_GET_LIST_OF_CROSS_BRIDGE_PROJECT_TMP "SELECT array_to_string(array(SELECT s.project_key FROM esgf_dashboard.cross_bridge_project s where s.project_group_key=b.project_group_key order by s.project_key ASC), ',') AS project_key,b.project_group_key FROM esgf_dashboard.cross_bridge_project b GROUP BY project_group_key order BY b.project_group_key ASC;"
// Get list of CROSS DOWNLOAD 
#define QUERY_GET_LIST_OF_CROSS_DOWNLOAD "SELECT (CAST(size as bigint)||':'||CAST(duration as integer)||':'||CAST(replica as integer)||':'||user_id_hash||':'||host_name||':'||user_idp||':'||CAST(hour as integer)||':'||CAST(minute as integer)||':'||CAST(project_group_key as integer)||':'||CAST(geolocation_key as bigint)||':'||CAST(date_key as integer)) AS name, download_key from esgf_dashboard.cross_fact_download;"
#define QUERY_SELECT_DASHBOARD_QUEUE "SELECT timestamp,remote_addr,size,success, duration, user_id_hash, user_idp from esgf_dashboard.dashboard_queue where id=%d;"
// Insert new date adds a new date in the database
#define QUERY_INSERT_NEW_DATE "INSERT into esgf_dashboard.cross_dim_date(download_date,month,year) values('%s','%s','%s');"
// Retrieve the id value of a specific date
#define QUERY_GET_DATE_ID "SELECT date_key from esgf_dashboard.cross_dim_date where download_date='%s';"

//Select the continent starting from country_code of the remote_ip_address 
//#define QUERY_SELECT_GEOLOCATION_INFO "SELECT continent_name from esgf_dashboard.continent, esgf_dashboard.country where esgf_dashboard.country.continent_id=esgf_dashboard.continent.continent_id and esgf_dashboard.country.country_code='%s';" 
#define QUERY_SELECT_GEOLOCATION_INFO "SELECT country_id from esgf_dashboard.country where esgf_dashboard.country.country_code='%s';" 
//Select the geolocation_id starting from latitude and longitude
#define QUERY_GET_GEO_ID "SELECT geolocation_key from esgf_dashboard.cross_dim_geolocation where latitude=%14.11f and longitude=%14.11f;"
//Select the project_id starting from its name 
#define QUERY_GET_PROJ_ID "SELECT project_key from esgf_dashboard.cross_dim_project where project_name='%s';"
//Select the max group id from cross_bridge_project 
//#define QUERY_GET_MAX_GROUP_ID "SELECT COALESCE(max(project_group_key),0) from cross_bridge_project;"
#define QUERY_GET_MAX_GROUP_ID "SELECT MAX(project_group_key) AS max_bridge_proj from esgf_dashboard.cross_bridge_project;"
//Retrieve the id of fact download 
#define QUERY_GET_CROSS_FACT_DOWNLOAD "SELECT download_key from esgf_dashboard.cross_fact_download where size=%.0f and success='%s' and duration=%d and replica='%s' and user_id_hash='%s' and host_name='%s' and user_idp='%s' and hour='%s' and minute='%s' and project_group_key=%d and geolocation_key=%d and date_key=%d;"
//insert into cross_dmart_project_host
//#define QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST "INSERT INTO esgf_dashboard.cross_dmart_project_host(total_size, number_of_downloads, number_of_successful_downloads, number_of_replica_downloads, average_duration, number_of_files, number_of_users, host_name, project_name, date_key) SELECT sum(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(*) as number_of_successful_downloads, COUNT(*) as number_of_replica_downloads, ROUND(avg(cast(duration as integer)),0) as average_duration,COUNT(*) AS number_of_files, COUNT(distinct user_id_hash) AS number_of_users, host_name, project_name, esgf_dashboard.cross_fact_download.date_key from esgf_dashboard.cross_fact_download, esgf_dashboard.cross_bridge_project, esgf_dashboard.cross_dim_project, esgf_dashboard.cross_dim_date where success='true' and replica='false' and cross_fact_download.project_group_key=cross_bridge_project.project_group_key and esgf_dashboard.cross_bridge_project.project_key=esgf_dashboard.cross_dim_project.project_key and cross_fact_download.date_key=esgf_dashboard.cross_dim_date.date_key group by project_name, esgf_dashboard.cross_fact_download.date_key, host_name;" 

//#define QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST "INSERT INTO esgf_dashboard.cross_dmart_project_host_time(total_size, number_of_downloads, number_of_successful_downloads, number_of_replica_downloads, average_duration, number_of_users, host_name, project_name, month, year) SELECT (select sum(size) from esgf_dashboard.cross_fact_download where size<>-1)AS total_size, COUNT(*) AS number_of_downloads, (select count(*)  from  esgf_dashboard.cross_fact_download where success) AS number_of_successful_downloads, (select count(*)  from  esgf_dashboard.cross_fact_download where replica) AS number_of_replica_downloads, (select ROUND(avg(cast(duration as integer)),0) from esgf_dashboard.cross_fact_download where success) AS average_duration, (select COUNT(distinct user_id_hash) from  esgf_dashboard.cross_fact_download) AS number_of_users, host_name, project_name, esgf_dashboard.cross_dim_date.month, esgf_dashboard.cross_dim_date.year FROM esgf_dashboard.cross_fact_download, esgf_dashboard.cross_bridge_project, esgf_dashboard.cross_dim_project, esgf_dashboard.cross_dim_date where cross_fact_download.project_group_key=cross_bridge_project.project_group_key and esgf_dashboard.cross_bridge_project.project_key=esgf_dashboard.cross_dim_project.project_key and cross_fact_download.date_key=esgf_dashboard.cross_dim_date.date_key group by project_name, esgf_dashboard.cross_dim_date.month, esgf_dashboard.cross_dim_date.year, host_name ORDER BY esgf_dashboard.cross_dim_date.month, esgf_dashboard.cross_dim_date.year ASC;"
#define QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST "DELETE FROM esgf_dashboard.cross_dmart_project_host_time;INSERT INTO esgf_dashboard.cross_dmart_project_host_time(total_size, number_of_downloads, number_of_successful_downloads, number_of_replica_downloads, average_duration, number_of_users, host_name, project_name, month, year) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, COUNT(CASE WHEN replica THEN 1 END) AS number_of_replica_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, fact.host_name, project.project_name, date.month, date.year FROM esgf_dashboard.cross_fact_download AS fact JOIN esgf_dashboard.cross_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.cross_bridge_project AS bridgeproj ON fact.project_group_key = bridgeproj.project_group_key JOIN esgf_dashboard.cross_dim_project AS project ON bridgeproj.project_key = project.project_key WHERE size <> -1 GROUP BY host_name, project_name, month, year ORDER BY date.year, date.month ASC;"

#define QUERY_UPDATE_DASHBOARD_QUEUE "update esgf_dashboard.dashboard_queue set processed=1 where id=%d;"

#define QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST_POS "DELETE FROM esgf_dashboard.cross_dmart_project_host_geolocation;INSERT INTO esgf_dashboard.cross_dmart_project_host_geolocation(total_size, number_of_downloads, number_of_successful_downloads, number_of_replica_downloads, average_duration, number_of_users, host_name, project_name, latitude, longitude) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, COUNT(CASE WHEN replica THEN 1 END) AS number_of_replica_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, fact.host_name, project.project_name, geo.latitude, geo.longitude FROM esgf_dashboard.cross_fact_download AS fact JOIN esgf_dashboard.cross_dim_geolocation AS geo ON fact.geolocation_key = geo.geolocation_key JOIN esgf_dashboard.cross_bridge_project AS bridgeproj ON fact.project_group_key = bridgeproj.project_group_key JOIN esgf_dashboard.cross_dim_project AS project ON bridgeproj.project_key = project.project_key WHERE size <> -1 GROUP BY host_name, project_name, geo.latitude, geo.longitude;"
//SELECT (select sum(size) from esgf_dashboard.cross_fact_download where size<>-1)AS total_size, COUNT(*) AS number_of_downloads, (select count(*)  from  esgf_dashboard.cross_fact_download where success) AS number_of_successful_downloads, (select count(*)  from  esgf_dashboard.cross_fact_download where replica) AS number_of_replica_downloads, (select ROUND(avg(cast(duration as integer)),0) from esgf_dashboard.cross_fact_download where success) AS average_duration, (select COUNT(distinct user_id_hash) from  esgf_dashboard.cross_fact_download) AS number_of_users, host_name, project_name, esgf_dashboard.cross_dim_geolocation.latitude, esgf_dashboard.cross_dim_geolocation.longitude FROM esgf_dashboard.cross_fact_download, esgf_dashboard.cross_bridge_project, esgf_dashboard.cross_dim_project, esgf_dashboard.cross_dim_geolocation where cross_fact_download.project_group_key=cross_bridge_project.project_group_key and esgf_dashboard.cross_bridge_project.project_key=esgf_dashboard.cross_dim_project.project_key and cross_dim_geolocation.geolocation_key=cross_dim_geolocation.geolocation_key group by project_name, esgf_dashboard.cross_dim_geolocation.latitude, esgf_dashboard.cross_dim_geolocation.longitude, host_name;"

//Queries about Obs4mips
#define QUERY_GET_LIST_OF_OBS_DIM_FILE "SELECT file_name||':'||file_size as name, file_key from esgf_dashboard.obs4mips_dim_file;"
#define QUERY_GET_LIST_OF_OBS_DIM_DATASET "SELECT dataset_name, dataset_key from esgf_dashboard.obs4mips_dim_dataset;"
#define QUERY_GET_LIST_OF_OBS_DIM_INSTITUTE "SELECT institute_name, institute_key from esgf_dashboard.obs4mips_dim_institute;"
#define QUERY_GET_LIST_OF_OBS_DIM_VARIABLE "SELECT cf_standard_name||':'||variable_long_name||':'||variable_code as name, variable_key from esgf_dashboard.obs4mips_dim_variable;"
#define QUERY_GET_LIST_OF_OBS_DIM_TIME_FREQUENCY "SELECT time_frequency_value, time_frequency_key from esgf_dashboard.obs4mips_dim_time_frequency;"
#define QUERY_GET_LIST_OF_OBS_DIM_INDEX_NODE "SELECT index_node_name, index_node_key from esgf_dashboard.obs4mips_dim_index_node;"
#define QUERY_GET_LIST_OF_OBS_DIM_PROCESSING_LEVEL "SELECT processing_level_name, processing_level_key from esgf_dashboard.obs4mips_dim_processing_level;"
#define QUERY_GET_LIST_OF_OBS_DIM_SOURCE_ID "SELECT source_id_name, source_id_key from esgf_dashboard.obs4mips_dim_source_id;"
#define QUERY_GET_LIST_OF_OBS_DIM_REALM "SELECT realm_name, realm_key from esgf_dashboard.obs4mips_dim_realm;"
#define QUERY_GET_LIST_OF_OBS_DIM_INDEX "SELECT index_node_name, index_node_key from esgf_dashboard.obs4mips_dim_index_node;"
// Insert new file adds a new file in the database
#define QUERY_INSERT_NEW_FILE "INSERT into esgf_dashboard.obs4mips_dim_file(file_name,file_size) values('%s',%.0f);"
// Retrieve the id value of a specific file
#define QUERY_GET_FILE_ID "SELECT file_key from esgf_dashboard.obs4mips_dim_file where file_name='%s' and file_size=%.0f;"
// Insert new dataset adds a new dataset in the database
#define QUERY_INSERT_NEW_DATASET_OBS "INSERT into esgf_dashboard.obs4mips_dim_dataset(dataset_name,dataset_version,datetime_start,datetime_stop) values('%s',%d,'%s','%s');"
// Retrieve the id value of a specific dataset
#define QUERY_GET_DATASET_OBS_ID "SELECT dataset_key from esgf_dashboard.obs4mips_dim_dataset where dataset_name='%s';"
// Insert new institute adds a new institute in the database
#define QUERY_INSERT_OBS_DIM_INSTITUTE "INSERT into esgf_dashboard.obs4mips_dim_institute(institute_name) values('%s');"
// Retrieve the id value of a specific institute
#define QUERY_GET_INSTITUTE_OBS_ID "SELECT institute_key from esgf_dashboard.obs4mips_dim_institute where institute_name='%s';"
// Retrieve the max group id of institute
#define QUERY_INST_GET_MAX_GROUP_OBS_ID "SELECT MAX(institute_group_key) AS max_bridge_inst from esgf_dashboard.obs4mips_bridge_institute;"
// QUERY_INSERT_OBS_BRIDGE_INSTITUTE adds a new institute related to the downloaded file in the database
#define QUERY_INSERT_OBS_BRIDGE_INSTITUTE "INSERT into esgf_dashboard.obs4mips_bridge_institute(institute_key,institute_group_key) values(%d,%d);"
// Get list of OBS BRIDGE INSTITUTE 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_INSTITUTE "SELECT institute_group_key, institute_key from esgf_dashboard.obs4mips_bridge_institute;"
// Get list of OBS BRIDGE INSTITUTE TMP 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_INSTITUTE_TMP "SELECT array_to_string(array(SELECT s.institute_key FROM esgf_dashboard.obs4mips_bridge_institute s where s.institute_group_key=b.institute_group_key order by s.institute_key ASC), ',') AS institute_key,b.institute_group_key FROM esgf_dashboard.obs4mips_bridge_institute b GROUP BY institute_group_key order BY b.institute_group_key ASC;"
// Insert new variable adds a new variable in the database
#define QUERY_INSERT_OBS_DIM_VARIABLE "INSERT into esgf_dashboard.obs4mips_dim_variable(cf_standard_name,variable_long_name,variable_code) values('%s','%s','%s');"
// Retrieve the id value of a specific variable
#define QUERY_GET_VARIABLE_OBS_ID "SELECT variable_key from esgf_dashboard.obs4mips_dim_variable where cf_standard_name='%s' and variable_long_name='%s' and variable_code='%s';"
// Retrieve the max group id of variable
#define QUERY_VAR_GET_MAX_GROUP_OBS_ID "SELECT MAX(variable_group_key) AS max_bridge_var from esgf_dashboard.obs4mips_bridge_variable;"
// QUERY_INSERT_OBS_BRIDGE_VARIABLE adds a new variable related to the downloaded file in the database
#define QUERY_INSERT_OBS_BRIDGE_VARIABLE "INSERT into esgf_dashboard.obs4mips_bridge_variable(variable_key,variable_group_key) values(%d,%d);"
// Get list of OBS BRIDGE VARIABLE 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_VARIABLE "SELECT variable_group_key, variable_key from esgf_dashboard.obs4mips_bridge_variable;"
// Get list of OBS BRIDGE INSTITUTE TMP 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_VARIABLE_TMP "SELECT array_to_string(array(SELECT s.variable_key FROM esgf_dashboard.obs4mips_bridge_variable s where s.variable_group_key=b.variable_group_key order by s.variable_key ASC), ',') AS variable_key,b.variable_group_key FROM esgf_dashboard.obs4mips_bridge_variable b GROUP BY variable_group_key order BY b.variable_group_key ASC;"
// Insert new time_frequency adds a new time_frequency in the database
#define QUERY_INSERT_OBS_DIM_TIME_FREQUENCY "INSERT into esgf_dashboard.obs4mips_dim_time_frequency(time_frequency_value) values('%s');"
// Retrieve the id value of a specific time_frequency 
#define QUERY_GET_TIME_FREQ_OBS_ID "SELECT time_frequency_key from esgf_dashboard.obs4mips_dim_time_frequency where time_frequency_value='%s';"
// Retrieve the max group id of time_frequency
#define QUERY_TIME_FREQ_GET_MAX_GROUP_OBS_ID "SELECT MAX(time_frequency_group_key) AS max_bridge_time_freq from esgf_dashboard.obs4mips_bridge_time_frequency;"
// QUERY_INSERT_OBS_BRIDGE_TIME_FREQUENCY adds a new time_frequency related to the downloaded file in the database
#define QUERY_INSERT_OBS_BRIDGE_TIME_FREQUENCY "INSERT into esgf_dashboard.obs4mips_bridge_time_frequency(time_frequency_key,time_frequency_group_key) values(%d,%d);"
// Get list of OBS BRIDGE TIME_FREQUENCY 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_TIME_FREQUENCY "SELECT time_frequency_group_key, time_frequency_key from esgf_dashboard.obs4mips_bridge_time_frequency;"
// Get list of OBS BRIDGE TIME_FREQUENCY TMP 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_TIME_FREQUENCY_TMP "SELECT array_to_string(array(SELECT s.time_frequency_key FROM esgf_dashboard.obs4mips_bridge_time_frequency s where s.time_frequency_group_key=b.time_frequency_group_key order by s.time_frequency_key ASC), ',') AS time_frequency_key,b.time_frequency_group_key FROM esgf_dashboard.obs4mips_bridge_time_frequency b GROUP BY time_frequency_group_key order BY b.time_frequency_group_key ASC;"
// Get list of OBS BRIDGE PROCESSING_LEVEL 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_PROCESSING_LEVEL "SELECT processing_level_group_key, processing_level_key from esgf_dashboard.obs4mips_bridge_processing_level;"
// Get list of OBS BRIDGE PROCESSING LEVEL TMP 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_PROCESSING_LEVEL_TMP "SELECT array_to_string(array(SELECT s.processing_level_key FROM esgf_dashboard.obs4mips_bridge_processing_level s where s.processing_level_group_key=b.processing_level_group_key order by s.processing_level_key ASC), ',') AS processing_level_key,b.processing_level_group_key FROM esgf_dashboard.obs4mips_bridge_processing_level b GROUP BY processing_level_group_key order BY b.processing_level_group_key ASC;"
// Insert new processing_level adds a new processing level in the database
#define QUERY_INSERT_OBS_DIM_PROCESSING_LEVEL "INSERT into esgf_dashboard.obs4mips_dim_processing_level(processing_level_name) values('%s');"
// Retrieve the id value of a specific processing level 
#define QUERY_GET_PROC_LEV_OBS_ID "SELECT processing_level_key from esgf_dashboard.obs4mips_dim_processing_level where processing_level_name='%s';"
// Retrieve the max group id of processing level
#define QUERY_PROC_LEV_GET_MAX_GROUP_OBS_ID "SELECT MAX(processing_level_group_key) AS max_bridge_proc_lev from esgf_dashboard.obs4mips_bridge_processing_level;"
// QUERY_INSERT_OBS_BRIDGE_PROCESSING_LEVEL adds a new processing_level related to the downloaded file in the database
#define QUERY_INSERT_OBS_BRIDGE_PROCESSING_LEVEL "INSERT into esgf_dashboard.obs4mips_bridge_processing_level(processing_level_key,processing_level_group_key) values(%d,%d);"

// Get list of OBS BRIDGE SOURCE ID 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_SOURCE_ID "SELECT source_id_group_key, source_id_key from esgf_dashboard.obs4mips_bridge_source_id;"
// Get list of OBS BRIDGE SOURCE ID TMP 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_SOURCE_ID_TMP "SELECT array_to_string(array(SELECT s.source_id_key FROM esgf_dashboard.obs4mips_bridge_source_id s where s.source_id_group_key=b.source_id_group_key order by s.source_id_key ASC), ',') AS source_id_key,b.source_id_group_key FROM esgf_dashboard.obs4mips_bridge_source_id b GROUP BY source_id_group_key order BY b.source_id_group_key ASC;"
// Insert new source id adds a new source id in the database
#define QUERY_INSERT_OBS_DIM_SOURCE_ID "INSERT into esgf_dashboard.obs4mips_dim_source_id(source_id_name) values('%s');"
// Retrieve the id value of a specific source id 
#define QUERY_GET_SOURCE_ID_ID "SELECT source_id_key from esgf_dashboard.obs4mips_dim_source_id where source_id_name='%s';"
// Retrieve the max group id of the source id
#define QUERY_SOURCE_ID_GET_MAX_GROUP_ID "SELECT MAX(source_id_group_key) AS max_bridge_source_id from esgf_dashboard.obs4mips_bridge_source_id;"
// QUERY_INSERT_OBS_BRIDGE_SOURCE_ID adds a new source_id related to the downloaded file in the database
#define QUERY_INSERT_OBS_BRIDGE_SOURCE_ID "INSERT into esgf_dashboard.obs4mips_bridge_source_id(source_id_key,source_id_group_key) values(%d,%d);"

// Get list of OBS BRIDGE REALM 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_REALM "SELECT realm_group_key, realm_key from esgf_dashboard.obs4mips_bridge_realm;"
// Get list of OBS BRIDGE REALM TMP 
#define QUERY_GET_LIST_OF_OBS_BRIDGE_REALM_TMP "SELECT array_to_string(array(SELECT s.realm_key FROM esgf_dashboard.obs4mips_bridge_realm s where s.realm_group_key=b.realm_group_key order by s.realm_key ASC), ',') AS realm_key,b.realm_group_key FROM esgf_dashboard.obs4mips_bridge_realm b GROUP BY realm_group_key order BY b.realm_group_key ASC;"
// Insert new realm adds a new realm in the database
#define QUERY_INSERT_OBS_DIM_REALM "INSERT into esgf_dashboard.obs4mips_dim_realm(realm_name) values('%s');"
// Retrieve the id value of a specific realm
#define QUERY_GET_REALM_OBS_ID "SELECT realm_key from esgf_dashboard.obs4mips_dim_realm where realm_name='%s';"
// Retrieve the max group id of the realm
#define QUERY_REALM_GET_MAX_GROUP_OBS_ID "SELECT MAX(realm_group_key) AS max_bridge_realm from esgf_dashboard.obs4mips_bridge_realm;"
// QUERY_INSERT_OBS_BRIDGE_REALM adds a new realm related to the downloaded file in the database
#define QUERY_INSERT_OBS_BRIDGE_REALM "INSERT into esgf_dashboard.obs4mips_bridge_realm(realm_key,realm_group_key) values(%d,%d);"
// QUERY_INSERT_OBS_FACT_DOWNLOAD adds a new fact related to the downloaded file in the database
#define QUERY_INSERT_OBS_FACT_DOWNLOAD "INSERT into esgf_dashboard.obs4mips_fact_download(size,success,duration,user_id_hash,host_name, user_idp,hour, minute, index_node_name, dataset_key, file_key, geolocation_key,date_key, institute_group_key, variable_group_key, time_frequency_group_key, processing_level_group_key, source_id_group_key, realm_group_key) values(%.0f,'%s',%d,'%s','%s','%s','%s','%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d);"
// Get list of OBS DIM DATE
#define QUERY_GET_LIST_OF_OBS_DIM_DATE "SELECT download_date,date_key from esgf_dashboard.obs4mips_dim_date;"
// Get list of OBS DIM GEOLOCATION 
#define QUERY_GET_LIST_OF_OBS_DIM_GEOLOCATION "SELECT (CAST(latitude as numeric(14,11)))||':'||(CAST(longitude as numeric(14,11))) AS name,geolocation_key from esgf_dashboard.obs4mips_dim_geolocation;"
// Insert new date adds a new date in the database
#define QUERY_INSERT_NEW_OBS_DATE "INSERT into esgf_dashboard.obs4mips_dim_date(download_date,month,year) values('%s','%s','%s');"
// Retrieve the id value of a specific date
#define QUERY_GET_OBS_DATE_ID "SELECT date_key from esgf_dashboard.obs4mips_dim_date where download_date='%s';"
// QUERY_INSERT_OBS_DIM_GEOLOCATION adds a new geolocation of the downloaded file in the database
#define QUERY_INSERT_OBS_DIM_GEOLOCATION "INSERT into esgf_dashboard.obs4mips_dim_geolocation(latitude,longitude,country_id) values(%14.11f,%14.11f,%d);"
//Select the geolocation_id starting from latitude and longitude
#define QUERY_GET_GEO_OBS_ID "SELECT geolocation_key from esgf_dashboard.obs4mips_dim_geolocation where latitude=%14.11f and longitude=%14.11f;"
//NEW_OBS4MIPS_DMART_CLIENTS_HOST_TIME_GEOLOCATION
#define QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_CLIENTS_HOST_TIME_GEOLOCATION "DELETE FROM esgf_dashboard.obs4mips_dmart_clients_host_time_geolocation; INSERT INTO esgf_dashboard.obs4mips_dmart_clients_host_time_geolocation(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, latitude, longitude, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, geo.latitude, geo.longitude, fact.host_name FROM esgf_dashboard.obs4mips_fact_download AS fact JOIN esgf_dashboard.obs4mips_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.obs4mips_dim_geolocation AS geo ON fact.geolocation_key = geo.geolocation_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, geo.latitude, geo.longitude ORDER BY date.year, date.month ASC;"
//NEW_OBS4MIPS_DMART_CLIENTS_VARIABLE_HOST_TIME
#define QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_CLIENTS_VARIABLE_HOST_TIME "DELETE FROM esgf_dashboard.obs4mips_dmart_variable_host_time;INSERT INTO esgf_dashboard.obs4mips_dmart_variable_host_time(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, variable_code, variable_long_name, cf_standard_name, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, var.variable_code, var.variable_long_name, var.cf_standard_name, fact.host_name FROM esgf_dashboard.obs4mips_fact_download AS fact JOIN esgf_dashboard.obs4mips_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.obs4mips_bridge_variable AS bridgevar ON fact.variable_group_key = bridgevar.variable_group_key JOIN esgf_dashboard.obs4mips_dim_variable AS var ON bridgevar.variable_key=var.variable_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, var.variable_code, var.variable_long_name, var.cf_standard_name ORDER BY date.year, date.month ASC;"
//NEW_OBS4MIPS_DMART_SOURCE_HOST_TIME
#define QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_SOURCE_HOST_TIME "DELETE FROM esgf_dashboard.obs4mips_dmart_source_host_time;INSERT INTO esgf_dashboard.obs4mips_dmart_source_host_time(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, source_id_name, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, src.source_id_name, fact.host_name FROM esgf_dashboard.obs4mips_fact_download AS fact JOIN esgf_dashboard.obs4mips_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.obs4mips_bridge_source_id AS bridgesrc ON fact.source_id_group_key = bridgesrc.source_id_group_key JOIN esgf_dashboard.obs4mips_dim_source_id AS src ON bridgesrc.source_id_key=src.source_id_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, src.source_id_name ORDER BY date.year, date.month ASC;"
//NEW_OBS4MIPS_DMART_REALM_HOST_TIME
#define QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_REALM_HOST_TIME "DELETE FROM esgf_dashboard.obs4mips_dmart_realm_host_time;INSERT INTO esgf_dashboard.obs4mips_dmart_realm_host_time(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, realm_name, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, rlm.realm_name, fact.host_name FROM esgf_dashboard.obs4mips_fact_download AS fact JOIN esgf_dashboard.obs4mips_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.obs4mips_bridge_realm AS bridgerlm ON fact.realm_group_key = bridgerlm.realm_group_key JOIN esgf_dashboard.obs4mips_dim_realm AS rlm ON bridgerlm.realm_key=rlm.realm_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, rlm.realm_name ORDER BY date.year, date.month ASC;"
//NEW_OBS4MIPS_DMART_DATASET_HOST_TIME
#define QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_DATASET_HOST_TIME "DELETE FROM esgf_dashboard.obs4mips_dmart_dataset_host_time;INSERT INTO esgf_dashboard.obs4mips_dmart_dataset_host_time(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, dataset_name,dataset_version,datetime_start, datetime_stop, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, dat.dataset_name, dat.dataset_version,dat.datetime_start,datetime_stop, fact.host_name FROM esgf_dashboard.obs4mips_fact_download AS fact JOIN esgf_dashboard.obs4mips_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.obs4mips_dim_dataset AS dat ON fact.dataset_key = dat.dataset_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, dat.dataset_name, dat.dataset_version, dat.datetime_start, dat.datetime_stop ORDER BY date.year, date.month ASC;"


//Queries about cmip5
#define QUERY_GET_LIST_OF_CMIP5_DIM_DATASET "SELECT dataset_name, dataset_key from esgf_dashboard.cmip5_dim_dataset;"
#define QUERY_GET_LIST_OF_CMIP5_DIM_INSTITUTE "SELECT institute_name, institute_key from esgf_dashboard.cmip5_dim_institute;"
#define QUERY_GET_LIST_OF_CMIP5_DIM_VARIABLE "SELECT cf_standard_name||':'||variable_long_name||':'||variable_code as name, variable_key from esgf_dashboard.cmip5_dim_variable;"
#define QUERY_GET_LIST_OF_CMIP5_DIM_TIME_FREQUENCY "SELECT time_frequency_value, time_frequency_key from esgf_dashboard.cmip5_dim_time_frequency;"
//#define QUERY_GET_LIST_OF_CMIP5_DIM_PROCESSING_LEVEL "SELECT processing_level_name, processing_level_key from esgf_dashboard.cmip5_dim_processing_level;"
#define QUERY_GET_LIST_OF_CMIP5_DIM_REALM "SELECT realm_name, realm_key from esgf_dashboard.cmip5_dim_realm;"
#define QUERY_GET_LIST_OF_CMIP5_DIM_EXPERIMENT "SELECT experiment_name, experiment_key from esgf_dashboard.cmip5_dim_experiment;"
#define QUERY_GET_LIST_OF_CMIP5_DIM_MODEL "SELECT model_name, model_key from esgf_dashboard.cmip5_dim_model;"
// Get list of CMIP5 BRIDGE INSTITUTE 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_INSTITUTE "SELECT institute_group_key, institute_key from esgf_dashboard.cmip5_bridge_institute;"
// Get list of CMIP5 BRIDGE INSTITUTE TMP 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_INSTITUTE_TMP "SELECT array_to_string(array(SELECT s.institute_key FROM esgf_dashboard.cmip5_bridge_institute s where s.institute_group_key=b.institute_group_key order by s.institute_key ASC), ',') AS institute_key,b.institute_group_key FROM esgf_dashboard.cmip5_bridge_institute b GROUP BY institute_group_key order BY b.institute_group_key ASC;"
// Get list of CMIP5 BRIDGE VARIABLE 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_VARIABLE "SELECT variable_group_key, variable_key from esgf_dashboard.cmip5_bridge_variable;"
// Get list of CMIP5 BRIDGE INSTITUTE TMP 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_VARIABLE_TMP "SELECT array_to_string(array(SELECT s.variable_key FROM esgf_dashboard.cmip5_bridge_variable s where s.variable_group_key=b.variable_group_key order by s.variable_key ASC), ',') AS variable_key,b.variable_group_key FROM esgf_dashboard.cmip5_bridge_variable b GROUP BY variable_group_key order BY b.variable_group_key ASC;"
// Get list of CMIP5 BRIDGE TIME_FREQUENCY 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_TIME_FREQUENCY "SELECT time_frequency_group_key, time_frequency_key from esgf_dashboard.cmip5_bridge_time_frequency;"
// Get list of CMIP5 BRIDGE TIME_FREQUENCY TMP 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_TIME_FREQUENCY_TMP "SELECT array_to_string(array(SELECT s.time_frequency_key FROM esgf_dashboard.cmip5_bridge_time_frequency s where s.time_frequency_group_key=b.time_frequency_group_key order by s.time_frequency_key ASC), ',') AS time_frequency_key,b.time_frequency_group_key FROM esgf_dashboard.cmip5_bridge_time_frequency b GROUP BY time_frequency_group_key order BY b.time_frequency_group_key ASC;"
// Get list of CMIP5 BRIDGE PROCESSING_LEVEL 
//#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_PROCESSING_LEVEL "SELECT processing_level_group_key, processing_level_key from esgf_dashboard.cmip5_bridge_processing_level;"
// Get list of CMIP5 BRIDGE PROCESSING LEVEL TMP 
//#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_PROCESSING_LEVEL_TMP "SELECT array_to_string(array(SELECT s.processing_level_key FROM esgf_dashboard.cmip5_bridge_processing_level s where s.processing_level_group_key=b.processing_level_group_key order by s.processing_level_key ASC), ',') AS processing_level_key,b.processing_level_group_key FROM esgf_dashboard.cmip5_bridge_processing_level b GROUP BY processing_level_group_key order BY b.processing_level_group_key ASC;"
// Get list of CMIP5 BRIDGE REALM 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_REALM "SELECT realm_group_key, realm_key from esgf_dashboard.cmip5_bridge_realm;"
// Get list of CMIP5 BRIDGE REALM TMP 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_REALM_TMP "SELECT array_to_string(array(SELECT s.realm_key FROM esgf_dashboard.cmip5_bridge_realm s where s.realm_group_key=b.realm_group_key order by s.realm_key ASC), ',') AS realm_key,b.realm_group_key FROM esgf_dashboard.cmip5_bridge_realm b GROUP BY realm_group_key order BY b.realm_group_key ASC;"
// Get list of CMIP5 DIM DATE
#define QUERY_GET_LIST_OF_CMIP5_DIM_DATE "SELECT download_date,date_key from esgf_dashboard.cmip5_dim_date;"
// Get list of CMIP5 DIM GEOLOCATION 
#define QUERY_GET_LIST_OF_CMIP5_DIM_GEOLOCATION "SELECT (CAST(latitude as numeric(14,11)))||':'||(CAST(longitude as numeric(14,11))) AS name,geolocation_key from esgf_dashboard.cmip5_dim_geolocation;"
// Get list of CMIP5 BRIDGE EXPERIMENT 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_EXPERIMENT "SELECT experiment_group_key, experiment_key from esgf_dashboard.cmip5_bridge_experiment;"
// Get list of CMIP5 BRIDGE EXPERIMENT TMP 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_EXPERIMENT_TMP "SELECT array_to_string(array(SELECT s.experiment_key FROM esgf_dashboard.cmip5_bridge_experiment s where s.experiment_group_key=b.experiment_group_key order by s.experiment_key ASC), ',') AS experiment_key,b.experiment_group_key FROM esgf_dashboard.cmip5_bridge_experiment b GROUP BY experiment_group_key order BY b.experiment_group_key ASC;"
// Get list of CMIP5 BRIDGE MODEL 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_MODEL "SELECT model_group_key, model_key from esgf_dashboard.cmip5_bridge_model;"
// Get list of CMIP5 BRIDGE EXPERIMENT TMP 
#define QUERY_GET_LIST_OF_CMIP5_BRIDGE_MODEL_TMP "SELECT array_to_string(array(SELECT s.model_key FROM esgf_dashboard.cmip5_bridge_model s where s.model_group_key=b.model_group_key order by s.model_key ASC), ',') AS model_key,b.model_group_key FROM esgf_dashboard.cmip5_bridge_model b GROUP BY model_group_key order BY b.model_group_key ASC;"
// Insert new date adds a new date in the database
#define QUERY_INSERT_NEW_CMIP5_DATE "INSERT into esgf_dashboard.cmip5_dim_date(download_date,month,year) values('%s','%s','%s');"
// Retrieve the id value of a specific date
#define QUERY_GET_CMIP5_DATE_ID "SELECT date_key from esgf_dashboard.cmip5_dim_date where download_date='%s';"
// QUERY_INSERT CMIP5_DIM_GEOLOCATION adds a new geolocation of the downloaded file in the database
#define QUERY_INSERT_CMIP5_DIM_GEOLOCATION "INSERT into esgf_dashboard.cmip5_dim_geolocation(latitude,longitude,country_id) values(%14.11f,%14.11f,%d);"
//Select the geolocation_id starting from latitude and longitude
#define QUERY_GET_GEO_CMIP5_ID "SELECT geolocation_key from esgf_dashboard.cmip5_dim_geolocation where latitude=%14.11f and longitude=%14.11f;"
// Insert new institute adds a new institute in the database
#define QUERY_INSERT_CMIP5_DIM_INSTITUTE "INSERT into esgf_dashboard.cmip5_dim_institute(institute_name) values('%s');"
// Retrieve the id value of a specific institute
#define QUERY_GET_INSTITUTE_CMIP5_ID "SELECT institute_key from esgf_dashboard.cmip5_dim_institute where institute_name='%s';"
// Retrieve the max group id of institute
#define QUERY_INST_GET_MAX_GROUP_CMIP5_ID "SELECT MAX(institute_group_key) AS max_bridge_inst from esgf_dashboard.cmip5_bridge_institute;"
// QUERY_INSERT_CMIP5_BRIDGE_INSTITUTE adds a new institute related to the downloaded file in the database
#define QUERY_INSERT_CMIP5_BRIDGE_INSTITUTE "INSERT into esgf_dashboard.cmip5_bridge_institute(institute_key,institute_group_key) values(%d,%d);"
// Insert new variable adds a new variable in the database
#define QUERY_INSERT_CMIP5_DIM_VARIABLE "INSERT into esgf_dashboard.cmip5_dim_variable(cf_standard_name,variable_long_name,variable_code) values('%s','%s','%s');"
// Retrieve the id value of a specific variable
#define QUERY_GET_VARIABLE_CMIP5_ID "SELECT variable_key from esgf_dashboard.cmip5_dim_variable where cf_standard_name='%s' and variable_long_name='%s' and variable_code='%s'"
// Retrieve the max group id of variable
#define QUERY_VAR_GET_MAX_GROUP_CMIP5_ID "SELECT MAX(variable_group_key) AS max_bridge_var from esgf_dashboard.cmip5_bridge_variable;"
// QUERY_INSERT_CMIP5_BRIDGE_VARIABLE adds a new variable related to the downloaded file in the database
#define QUERY_INSERT_CMIP5_BRIDGE_VARIABLE "INSERT into esgf_dashboard.cmip5_bridge_variable(variable_key,variable_group_key) values(%d,%d);"
// Insert new time_frequency adds a new time_frequency in the database
#define QUERY_INSERT_CMIP5_DIM_TIME_FREQUENCY "INSERT into esgf_dashboard.cmip5_dim_time_frequency(time_frequency_value) values('%s');"
// Retrieve the id value of a specific time_frequency 
#define QUERY_GET_TIME_FREQ_CMIP5_ID "SELECT time_frequency_key from esgf_dashboard.cmip5_dim_time_frequency where time_frequency_value='%s';"
// Retrieve the max group id of time_frequency
#define QUERY_TIME_FREQ_GET_MAX_GROUP_CMIP5_ID "SELECT MAX(time_frequency_group_key) AS max_bridge_time_freq from esgf_dashboard.cmip5_bridge_time_frequency;"
// QUERY_INSERT_CMIP5_BRIDGE_TIME_FREQUENCY adds a new time_frequency related to the downloaded file in the database
#define QUERY_INSERT_CMIP5_BRIDGE_TIME_FREQUENCY "INSERT into esgf_dashboard.cmip5_bridge_time_frequency(time_frequency_key,time_frequency_group_key) values(%d,%d);"
// Insert new processing_level adds a new processing level in the database
//#define QUERY_INSERT_CMIP5_DIM_PROCESSING_LEVEL "INSERT into esgf_dashboard.cmip5_dim_processing_level(processing_level_name) values('%s');"
// Retrieve the id value of a specific processing level 
#define QUERY_GET_PROC_LEV_CMIP5_ID "SELECT processing_level_key from esgf_dashboard.cmip5_dim_processing_level where processing_level_name='%s';"
// Retrieve the max group id of processing level
#define QUERY_PROC_LEV_GET_MAX_GROUP_CMIP5_ID "SELECT MAX(processing_level_group_key) AS max_bridge_proc_lev from esgf_dashboard.cmip5_bridge_processing_level;"
// QUERY_INSERT_CMIP5_BRIDGE_PROCESSING_LEVEL adds a new processing_level related to the downloaded file in the database
//#define QUERY_INSERT_CMIP5_BRIDGE_PROCESSING_LEVEL "INSERT into esgf_dashboard.cmip5_bridge_processing_level(processing_level_key,processing_level_group_key) values(%d,%d);"
#define QUERY_INSERT_CMIP5_DIM_REALM "INSERT into esgf_dashboard.cmip5_dim_realm(realm_name) values('%s');"
// Retrieve the id value of a specific realm
#define QUERY_GET_REALM_CMIP5_ID "SELECT realm_key from esgf_dashboard.cmip5_dim_realm where realm_name='%s';"
// Retrieve the max group id of the realm
#define QUERY_REALM_GET_MAX_GROUP_CMIP5_ID "SELECT MAX(realm_group_key) AS max_bridge_realm from esgf_dashboard.cmip5_bridge_realm;"
// QUERY_INSERT_CMIP5_BRIDGE_REALM adds a new realm related to the downloaded file in the database
#define QUERY_INSERT_CMIP5_BRIDGE_REALM "INSERT into esgf_dashboard.cmip5_bridge_realm(realm_key,realm_group_key) values(%d,%d);"
//Insert new model adds a new model in the database
#define QUERY_INSERT_CMIP5_DIM_MODEL "INSERT into esgf_dashboard.cmip5_dim_model(model_name) values('%s');"
// Retrieve the id value of a specific model
#define QUERY_GET_MODEL_CMIP5_ID "SELECT model_key from esgf_dashboard.cmip5_dim_model where model_name='%s';"
// Retrieve the max group id of the model
#define QUERY_MODEL_GET_MAX_GROUP_CMIP5_ID "SELECT MAX(model_group_key) AS max_bridge_model from esgf_dashboard.cmip5_bridge_model;"
// QUERY_INSERT_CMIP5_BRIDGE_MODEL adds a new model related to the downloaded file in the database
#define QUERY_INSERT_CMIP5_BRIDGE_MODEL "INSERT into esgf_dashboard.cmip5_bridge_model(model_key,model_group_key) values(%d,%d);"
//Insert new experiment adds a new experiment in the database
#define QUERY_INSERT_CMIP5_DIM_EXPERIMENT "INSERT into esgf_dashboard.cmip5_dim_experiment(experiment_name) values('%s');"
// Retrieve the id value of a specific experiment
#define QUERY_GET_EXPERIMENT_CMIP5_ID "SELECT experiment_key from esgf_dashboard.cmip5_dim_experiment where experiment_name='%s';"
// Retrieve the max group id of the experiment
#define QUERY_EXPERIMENT_GET_MAX_GROUP_CMIP5_ID "SELECT MAX(experiment_group_key) AS max_bridge_experiment from esgf_dashboard.cmip5_bridge_experiment;"
// QUERY_INSERT_CMIP5_BRIDGE_EXPERIMENT adds a new experiment related to the downloaded file in the database
#define QUERY_INSERT_CMIP5_BRIDGE_EXPERIMENT "INSERT into esgf_dashboard.cmip5_bridge_experiment(experiment_key,experiment_group_key) values(%d,%d);"

// Insert new dataset adds a new dataset in the database
#define QUERY_INSERT_NEW_DATASET_CMIP5 "INSERT into esgf_dashboard.cmip5_dim_dataset(dataset_name,dataset_version,datetime_start,datetime_stop) values('%s',%d,'%s','%s');"
// Retrieve the id value of a specific dataset
#define QUERY_GET_DATASET_CMIP5_ID "SELECT dataset_key from esgf_dashboard.cmip5_dim_dataset where dataset_name='%s';"
// QUERY_INSERT_CMIP5_FACT_DOWNLOAD adds a new fact related to the downloaded file in the database
#define QUERY_INSERT_CMIP5_FACT_DOWNLOAD "INSERT into esgf_dashboard.cmip5_fact_download(size,success,duration,replica, host_name, hour, minute, user_id_hash, user_idp, date_key, geolocation_key, dataset_key, time_frequency_group_key, variable_group_key, experiment_group_key, model_group_key, realm_group_key, institute_group_key) values(%.0f,'%s',%d,'%s','%s','%s','%s','%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d);"
//NEW_CMIP5_DMART_CLIENTS_HOST_TIME_GEOLOCATION
#define QUERY_SELECT_INSERT_NEW_CMIP5_DMART_CLIENTS_HOST_TIME_GEOLOCATION "DELETE FROM esgf_dashboard.cmip5_dmart_clients_host_time_geolocation;INSERT INTO esgf_dashboard.cmip5_dmart_clients_host_time_geolocation(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, latitude, longitude, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, geo.latitude, geo.longitude, fact.host_name FROM esgf_dashboard.cmip5_fact_download AS fact JOIN esgf_dashboard.cmip5_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.cmip5_dim_geolocation AS geo ON fact.geolocation_key = geo.geolocation_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, geo.latitude, geo.longitude ORDER BY date.year, date.month ASC;"
//NEW_CMIP5_DMART_CLIENTS_VARIABLE_HOST_TIME
#define QUERY_SELECT_INSERT_NEW_CMIP5_DMART_CLIENTS_VARIABLE_HOST_TIME "DELETE FROM esgf_dashboard.cmip5_dmart_variable_host_time;INSERT INTO esgf_dashboard.cmip5_dmart_variable_host_time(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, variable_code, variable_long_name, cf_standard_name, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, var.variable_code, var.variable_long_name, var.cf_standard_name, fact.host_name FROM esgf_dashboard.cmip5_fact_download AS fact JOIN esgf_dashboard.cmip5_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.cmip5_bridge_variable AS bridgevar ON fact.variable_group_key = bridgevar.variable_group_key JOIN esgf_dashboard.cmip5_dim_variable AS var ON bridgevar.variable_key=var.variable_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, var.variable_code, var.variable_long_name, var.cf_standard_name ORDER BY date.year, date.month ASC;"
//NEW_CMIP5_DMART_MODEL_HOST_TIME
#define QUERY_SELECT_INSERT_NEW_CMIP5_DMART_MODEL_HOST_TIME "DELETE FROM esgf_dashboard.cmip5_dmart_model_host_time; INSERT INTO esgf_dashboard.cmip5_dmart_model_host_time(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, model_name, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, mdl.model_name, fact.host_name FROM esgf_dashboard.cmip5_fact_download AS fact JOIN esgf_dashboard.cmip5_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.cmip5_bridge_model AS bridgemdl ON fact.model_group_key = bridgemdl.model_group_key JOIN esgf_dashboard.cmip5_dim_model AS mdl ON bridgemdl.model_key=mdl.model_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, mdl.model_name ORDER BY date.year, date.month ASC;"
//NEW_CMIP5_DMART_EXPERIMENT_HOST_TIME
#define QUERY_SELECT_INSERT_NEW_CMIP5_DMART_EXPERIMENT_HOST_TIME "DELETE FROM esgf_dashboard.cmip5_dmart_experiment_host_time; INSERT INTO esgf_dashboard.cmip5_dmart_experiment_host_time(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, experiment_name, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, exp.experiment_name, fact.host_name FROM esgf_dashboard.cmip5_fact_download AS fact JOIN esgf_dashboard.cmip5_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.cmip5_bridge_experiment AS bridgeexp ON fact.experiment_group_key = bridgeexp.experiment_group_key JOIN esgf_dashboard.cmip5_dim_experiment AS exp ON bridgeexp.experiment_key=exp.experiment_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, exp.experiment_name ORDER BY date.year, date.month ASC;"
//NEW_CMIP5_DMART_DATASET_HOST_TIME
#define QUERY_SELECT_INSERT_NEW_CMIP5_DMART_DATASET_HOST_TIME "DELETE FROM esgf_dashboard.cmip5_dmart_dataset_host_time;INSERT INTO esgf_dashboard.cmip5_dmart_dataset_host_time(total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, dataset_name,dataset_version,datetime_start, datetime_stop, host_name) SELECT SUM(size) AS total_size, COUNT(*) AS number_of_downloads, COUNT(CASE WHEN success THEN 1 END) AS number_of_successful_downloads, ROUND(AVG(CASE WHEN success THEN duration END)) AS average_duration, COUNT(distinct user_id_hash) AS number_of_users, date.month, date.year, dat.dataset_name, dat.dataset_version,dat.datetime_start,datetime_stop, fact.host_name FROM esgf_dashboard.cmip5_fact_download AS fact JOIN esgf_dashboard.cmip5_dim_date AS date ON fact.date_key = date.date_key JOIN esgf_dashboard.cmip5_dim_dataset AS dat ON fact.dataset_key = dat.dataset_key WHERE size <> -1 GROUP BY host_name, date.month, date.year, dat.dataset_name, dat.dataset_version, dat.datetime_start, dat.datetime_stop ORDER BY date.year, date.month ASC;"


// HashTables dimensions - Cross Project
#define HAST_TABLE_CROSS_DIM_DATE		64
#define HAST_TABLE_CROSS_DIM_GEOLOCATION	64		
#define HAST_TABLE_CROSS_DIM_PROJECT		16
#define HAST_TABLE_CROSS_BRIDGE_PROJECT		64	
#define HAST_TABLE_CROSS_BRIDGE_PROJECT_TMP	64	
#define HAST_TABLE_CROSS_DOWNLOAD		64

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
#endif 
#define POSTGRES_HOST "esgf-node.jpl.nasa.gov"
#define POSTGRES_PORT_NUMBER 5432
#define POSTGRES_DB_NAME "esgcet"
#define POSTGRES_USER "dbsuper"
#define POSTGRES_PASSWD "XXXXX" /* change it */

// Start transaction and lock tables 
#define QUERY_OPEN_TRANSACTION  "start transaction; lock esgf_dashboard.cross_dim_date ; lock esgf_dashboard.cross_dim_geolocation ; lock esgf_dashboard.cross_dim_project ; lock esgf_dashboard.cross_bridge_project ; lock esgf_dashboard.cross_fact_download ; "

// End transaction and release locks 
#define QUERY_CLOSE_TRANSACTION  "end transaction;" 

long
int get_foreign_key_value (PGconn * conn, char *query);

int check_cross_project (struct dataset_project ***datasetproj, char* esgf_node)
{

  PGconn *conn;
  PGresult *res;
  char conninfo[1024] = { '\0' };
  char conninfo_printf[1024] = { '\0' };
  char open_transaction[2048] = { '\0' };
  char close_transaction[2048] = { '\0' };
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


    int i,cnt=0;
    int size2=0;
    PGresult   *res1;
    int timestamp;
    char *remote_addr=NULL;
    float size_row;
    int duration_row=0;
    char *user_id_hash_row=NULL;
    char *user_idp_row=NULL;
    char *success_row=NULL;
    for(cnt=0; (*datasetproj)[cnt]!=NULL; cnt++)
    {
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
                
                snprintf (select_query, sizeof (select_query), QUERY_SELECT_DASHBOARD_QUEUE, (*datasetproj)[cnt]->id_query);
                submit_query_res (conn, select_query,&res1);
                for (i = 0; i < PQntuples(res1); i++)
                {
                   printf("timestamp %ld\n", atoi(PQgetvalue(res1, i, 0)));
                   timestamp=atoi(PQgetvalue(res1, i, 0));
                   //printf("timestamp %s\n", PQgetvalue(res1, i, 0));
                   printf("remote_addr %s\n", PQgetvalue(res1, i, 1));
                   remote_addr=strdup(PQgetvalue(res1, i, 1));

                   //printf("size %s\n", PQgetvalue(res1, i, 2));
                   //printf("size %.0f\n", atof(PQgetvalue(res1, i, 2)));
                   size_row= atof(PQgetvalue(res1, i, 2));
                   size_row=size_row/1024/1024/1024;
                   printf("size %.0f\n", size_row);
                   printf("success %s\n", PQgetvalue(res1, i, 3));
                   success_row=strdup(PQgetvalue(res1, i, 3));

                   printf("duration %ld\n", atoi(PQgetvalue(res1, i, 4)));
                   duration_row=atoi(PQgetvalue(res1, i, 4));
                   //printf("duration %s\n", PQgetvalue(res1, i, 4));
                   printf("user_id_hash %s\n", PQgetvalue(res1, i, 5));
                   user_id_hash_row=strdup(PQgetvalue(res1, i, 5));
                 
                   printf("user_idp %s\n", PQgetvalue(res1, i, 6));
                   user_idp_row=strdup(PQgetvalue(res1, i, 6));
                }
                PQclear (res1);
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
                  //printf("***numero di metadati vale %d\n", datasetproj[cnt]->first[size2]->first[size3]->size);
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {
                      replica=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                      printf("replica vale %s\n", replica);
                    }
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "institute")==0)
                  {
                    size_institute=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    institute=(char **)calloc(size_institute+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      institute[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                      printf("institute[%d] vale %s\n", size4, institute[size4]);
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
                      printf("cf_standard_name[%d] vale %s\n", size4, cf_standard_name[size4]);
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
                      printf("variable[%d] vale %s\n", size4, variable[size4]);
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
                      printf("variable_long_name[%d] vale %s\n", size4, variable_long_name[size4]);
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
                      printf("time_frequency[%d] vale %s\n", size4, time_frequency[size4]);
                    }
                    time_frequency[size4]=NULL;
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "index_node")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      index_node=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                      printf("index_node vale %s\n", index_node);
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "processing_level")==0)
                  {
                    size_processing=(*datasetproj)[cnt]->first[size2]->first[size3]->size;
                    processing_level=(char **)calloc(size_processing+1, sizeof(char*));                    
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                    {                    
                      processing_level[size4]=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                      printf("processing_level[%d] vale %s\n", size4, processing_level[size4]);
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
                      printf("source_id[%d] vale %s\n", size4, source_id[size4]);
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
                      printf("realm[%d] vale %s\n", size4, realm[size4]);
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
                      printf("model[%d] vale %s\n", size4, model[size4]);
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
                      printf("experiment[%d] vale %s\n", size4, experiment[size4]);
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
                      printf("dataset_id vale %s\n", dataset_id);
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "size")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      size_file=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                      printf("size_file vale %s\n", size_file);
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "datetime_start")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      datetime_start=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                      printf("datetime_start vale %s\n", datetime_start);
                  }
                  if(strcmp((*datasetproj)[cnt]->first[size2]->first[size3]->name, "datetime_stop")==0)
                  {
                    for(size4=0; (*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]!=NULL; size4++)
                      datetime_stop=strdup((*datasetproj)[cnt]->first[size2]->first[size3]->value[size4]);
                      printf("datetime_stop vale %s\n", datetime_stop);
                  }
                }
              }
         }
       }

  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"OPEN Transaction and locking the database tables the parser needs...\n");
  /*if (manage_database_open_close_transaction(conn,QUERY_OPEN_TRANSACTION)) {
	  //pmesg(LOG_ERROR,__FILE__,__LINE__,"Transaction FAILED - Database Tables Lock: Failed. [Recovery action: Skip parsing]\n");
      	  PQfinish (conn);
	  return -1;
	 }*/
  //pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction OK - Database Tables Lock: Ok\n");

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

          printf("buf vale %s, buf1=%s, buf2=%s buf3=%s buf4=%s\n", buf, buf1, buf2, buf3, buf4);

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

	     snprintf(select_id_date_query, sizeof (select_id_date_query), QUERY_GET_DATE_ID, buf);
             date_id=get_foreign_key_value(conn, select_id_date_query);
	     // add entry to hash table
	     sprintf (date_id_str, "%ld", date_id);
             hashtbl_insert (hashtbl_cross_dim_date, buf, date_id_str);
	     pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",date_id, date_id_str);
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
                     printf("country_id %ls\n", country_id);
                     if(country_id)
		       snprintf (insert_geo_query, sizeof (insert_geo_query),QUERY_INSERT_CROSS_DIM_GEOLOCATION, lat, lon, atoi(country_id));
                     submit_query (conn, insert_geo_query);
                     free(country_code);
	             snprintf(select_id_geo_query, sizeof (select_id_geo_query), QUERY_GET_GEO_ID, lat, lon);
                     printf("query select_id_geo_query vale %s\n", select_id_geo_query);
                     geo_id=get_foreign_key_value(conn, select_id_geo_query);
	             // add entry to hash table
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
                //if(i==0) 
                //   sprintf(proj_key, "%d", array_idproj[i]);
                //else
                //{
                //  sprintf(proj_key, ",%s%d", proj_key, array_idproj[i]);
                //}
                if(i==0 || i==(*datasetproj)[cnt]->size-1)
                   ap += sprintf(ap, "%d", array_idproj[i]);
                else
                   ap += sprintf(ap, ",%d,", array_idproj[i]);
             
         }
         printf("proj_key %s\n", proj_key);
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
               printf("project_group_key %d\n", project_group_key);
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

                  char fact_row[2048] = { '\0' };
          int fact_id=0;
          sprintf(fact_row, "%.0f:%d:%s:%s:%s:%s:%d:%d:%ld:%d:%d", size_row, duration_row, replica, user_id_hash_row, esgf_node, user_idp_row, atoi(buf3),atoi(buf4), project_group_key, geo_id, date_id);
          printf("fact_row %s\n", fact_row);

          //if (hashtbl_result = hashtbl_get (hashtbl_cross_fact_download, fact_row))
          //{
          //   pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",fact_row, hashtbl_result);
          //   fact_id = atol (hashtbl_result);
	  //   success_lookup[0]++;
          //}
          //else
	  //{		// add host entry in DB (and hashtable too) without geolocation information

             char cross_bridge_project_id_str[128] = { '\0' };
             char fact_id_str[128] = { '\0' };
             char insert_cross_bridge_project[2048] = { '\0' };
             char select_id_cross_bridge_project[2048] = { '\0' };

             success_lookup[1]++;
             snprintf (insert_cross_bridge_project, sizeof (insert_cross_bridge_project), QUERY_INSERT_CROSS_FACT_DOWNLOAD,size_row, success_row, duration_row, replica, user_id_hash_row, esgf_node, user_idp_row, buf3, buf4, project_group_key, geo_id, date_id);


	     submit_query (conn, insert_cross_bridge_project);


         for(size2=0; (*datasetproj)[cnt]->first[size2]!=NULL; size2++)
         {
           proj_name=strdup((*datasetproj)[cnt]->first[size2]->project);
           uppercase(proj_name);
      
           if (create_populate_done==1)
	   {
            if(strcmp(proj_name, "OBS4MIPS")==0)
            {
              create_populate_done=2;       
	      pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
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
		  hashtbl_destroy (hashtbl_cmip5_dim_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model);
		  hashtbl_destroy (hashtbl_cmip5_bridge_model_tmp);
		  hashtbl_destroy (hashtbl_cmip5_bridge_experiment);
		  hashtbl_destroy (hashtbl_cmip5_bridge_experiment_tmp);
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
	     pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",date_obs_id, date_obs_id_str);
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
                     printf("country_id %ls\n", country_id);
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
                     printf("query select_id_geo_query vale %s\n", select_id_geo_query);
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
      printf("size2+++++++ %d\n", size2);
          }
      printf("size2------ %d\n", size2);
              




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
               printf("institute_group_key %d\n", institute_group_key);
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
      printf("size2************* %d\n", size2);
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
         printf("var_key %s\n", var_key);
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
               printf("variable_group_key %d\n", var_group_key);
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
      printf("size2************* %d\n", size2);
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
         printf("var_key %s\n", time_freq_key);
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
               printf("variable_group_key %d\n", time_freq_group_key);
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
         printf("proc_lev_key %s\n", proc_lev_key);
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
               printf("proc level_group_key %d\n", proc_lev_group_key);
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
      printf("size2 %d\n", size2);
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
         printf("source_id_key %s\n", source_id_key);
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
               printf("source id_group_key %d\n", source_id_group_key);
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
         printf("realm_key %s\n", realm_key);
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
               printf("realm_group_key %d\n", realm_group_key);
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
      printf("size2 %d\n", size2);
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
               printf("source id_group_key %d\n", model_group_key);
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
      printf("size2 %d\n", size2);
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
         printf("exp_key %s\n", exp_key);
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
               printf("experiment_group_key %d\n", exp_group_key);
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
            sprintf(filen, "%s.nc:%.0f",str1+1, size_row);
            sprintf(filename, "%s.nc", str1+1);
          }
          printf("filen vale %s\n", filen);
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
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",file_id, file_id_str);
          }

         }

          long int dataset_id_key = 0;              
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
             pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",dataset_id_key, dataset_id_str);
          }
          
          if(strcmp(proj_name, "OBS4MIPS")==0)
          {
             char insert_obs4mips_fact_download[2048] = { '\0' };
             snprintf (insert_obs4mips_fact_download, sizeof (insert_obs4mips_fact_download), QUERY_INSERT_OBS_FACT_DOWNLOAD,size_row, success_row, duration_row, user_id_hash_row, esgf_node, user_idp_row, buf3, buf4, index_node, dataset_id_key, file_id, geo_obs_id, date_obs_id, institute_group_key, var_group_key, time_freq_group_key, proc_lev_group_key, source_id_group_key, realm_group_key);
	     submit_query (conn, insert_obs4mips_fact_download);
          }
          else
          {
             char insert_cmip5_fact_download[2048] = { '\0' };
             snprintf (insert_cmip5_fact_download, sizeof (insert_cmip5_fact_download), QUERY_INSERT_CMIP5_FACT_DOWNLOAD,size_row, success_row, duration_row, replica, esgf_node, buf3, buf4, user_id_hash_row, user_idp_row, date_obs_id, geo_obs_id, dataset_id_key, time_freq_group_key, var_group_key, exp_group_key, model_group_key, realm_group_key, institute_group_key);
	     submit_query (conn, insert_cmip5_fact_download);

          }

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"*********** End parsing routine  ************\n");
     free(user_id_hash_row);
     free(user_idp_row);
     free(success_row);
     
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
     proj_name=NULL;
     free(replica);
     free(index_node);
     free(dataset_id);
     free(size_file);
     free(datetime_start);
     free(datetime_stop);
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
  PQfinish (conn);
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
      hashtbl_destroy (hashtbl_obs_bridge_source_id);
      hashtbl_destroy (hashtbl_obs_bridge_source_id_tmp);
      hashtbl_destroy (hashtbl_obs_bridge_realm);
      hashtbl_destroy (hashtbl_obs_bridge_realm_tmp);
      hashtbl_destroy (hashtbl_obs_dim_date);
      hashtbl_destroy (hashtbl_obs_dim_geolocation);
      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Releasing memory for hashtables [%d] \n",create_populate_done); 
      }
      else
      {
        hashtbl_destroy (hashtbl_cmip5_dim_model);
        hashtbl_destroy (hashtbl_cmip5_bridge_model);
        hashtbl_destroy (hashtbl_cmip5_bridge_model_tmp);
        hashtbl_destroy (hashtbl_cmip5_dim_experiment);
        hashtbl_destroy (hashtbl_cmip5_bridge_experiment);
        hashtbl_destroy (hashtbl_cmip5_bridge_experiment_tmp);
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

int insert_dmart_cross_project()
{
  PGconn *conn;
  PGresult *res;
  char conninfo[1024] = { '\0' };
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
     char insert_dmart_project_host[2048] = { '\0' };
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CROSS_DMART_PROJECT_HOST_POS);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_CLIENTS_HOST_TIME_GEOLOCATION);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_REALM_HOST_TIME);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_SOURCE_HOST_TIME);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_CLIENTS_VARIABLE_HOST_TIME);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_OBS4MIPS_DMART_DATASET_HOST_TIME);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_CLIENTS_HOST_TIME_GEOLOCATION);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_EXPERIMENT_HOST_TIME);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_MODEL_HOST_TIME);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_CLIENTS_VARIABLE_HOST_TIME);
     submit_query (conn, insert_dmart_project_host);
     snprintf (insert_dmart_project_host, sizeof (insert_dmart_project_host), QUERY_SELECT_INSERT_NEW_CMIP5_DMART_DATASET_HOST_TIME);
     submit_query (conn, insert_dmart_project_host);
     PQfinish (conn);
 
}

//da eliminare quando si farà l'integrazione
int manage_database_open_close_transaction(PGconn *conn, char* QUERY)
{
  char query_transaction[2048] = { '\0' };

  snprintf (query_transaction,sizeof (query_transaction),QUERY);
  if (submit_query (conn, query_transaction))
          return -1;
   
  return 0;
}
int
submit_query (PGconn * conn, char *query)
{
  PGresult *res;
  fprintf (stderr, "Query: %s\n", query);

  res = PQexec (conn, query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Submit query failed\n");
      PQclear (res);
      return -1;
    }
  PQclear (res);
  return 0;
}
int
submit_query_res (PGconn * conn, char *query, PGresult **res)
{
  fprintf (stderr, "Query**: %s\n", query);

  //PGresult *res=NULL;

  *res = PQexec (conn, query);

  if ((!(*res)) || (PQresultStatus (*res) != PGRES_TUPLES_OK))
    {
      //pmesg(LOG_ERROR,__FILE__,__LINE__,"Submit query failed\n");
      PQclear (*res);
      return -1;
    }

  return 0;
}
int
populate_hash_table (PGconn * conn, char *query, HASHTBL ** pointer)
{

  PGresult *res;
  int i, num_records;

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Retrieving data for hashtable\n");
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[QUERY=%s]\n", query);
  res = PQexec (conn, query);
  if ((!res) || (PQresultStatus (res) != PGRES_TUPLES_OK))
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Populate_hash_table error running query [%s]\n", query);
      PQclear (res);
      return -1;
    }

  num_records = PQntuples (res);

  for (i = 0; i < num_records; i++)
    {
      hashtbl_insert (*pointer, (char *) PQgetvalue (res, i, 0),
		      (char *) PQgetvalue (res, i, 1));
      pmesg(LOG_DEBUG,__FILE__,__LINE__,"To be stored into the hashtable [%s]#[%s]\n",
	       (char *) PQgetvalue (res, i, 0), (char *) PQgetvalue (res, i,
								     1));
    }
  PQclear (res);

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Hashtable properly created!\n", query);
  return 0;
}

long 
int get_foreign_key_value (PGconn * conn, char *query)
{
  PGresult *res;
  long int fk;
  //fprintf (stderr, "Query: %s\n", query);

  res = PQexec (conn, query);
  if ((!res) || (PQresultStatus (res) != PGRES_TUPLES_OK))
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"SELECT command did not return tuples properly\n");
      PQclear (res);
      return -1;
    }

  if ((PQntuples (res)) != 1)
    {
      PQclear (res);
      return -2;
    }

  fk = atol (PQgetvalue (res, 0, 0));

  PQclear (res);
  //fprintf (stderr, "The fk is %ld\n", fk);
  return fk;
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
