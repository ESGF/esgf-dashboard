/*
 * dbAccess.h
 *
 *      Author: University of Salento and CMCC 
 */

#ifndef DBACCESS_H_
#define DBACCESS_H_

// --------------- Connection parameters ------------------
#define POSTGRES_UNIX_SOCKET  	NULL
#define POSTGRES_CLIENT_FLAG	0
// --------------------------------------------------------

// --------------- Query ----------------------------------
#define QUERY1 	"SELECT s.id, h.ip, s.port FROM esgf_dashboard.service_instance s INNER JOIN esgf_dashboard.host h ON h.id=s.idHost ORDER BY h.ip, s.port;" 

#define QUERY2	"INSERT INTO esgf_dashboard.service_status(status, elapsedTime, idServiceInstance) " 

//#define QUERY3 	"start transaction esgf_dashboard.service_instance AND esgf_dashboard.host"
#define QUERY3 	"start transaction; lock esgf_dashboard.service_instance; lock esgf_dashboard.host;"

//#define QUERY4 	"stop transaction;"
#define QUERY4 	"end transaction;"

// QUERY TO MANAGE THE OLD SERVICE METRICS 
#define QUERY5  "DELETE from esgf_dashboard.service_status where timestamp < (now() - interval '%d months' - interval '%d day');"

//#define QUERY6 	"start transaction on esgf_dashboard.service_status"
#define QUERY6 	"start transaction; lock esgf_dashboard.service_status;"

// #define QUERY7 	"THE PRE-COMPUTED DATA CUBE FOR DATA DOWNLOAD METRICS"
#define QUERY7  "drop table if exists esgf_dashboard.analytics2; create table esgf_dashboard.analytics2 as (select d.project, d.model, d.experiment, s.name, count(*) as downloadcount, sum(size/1024/1024) as downloadsize from (select fv.file_id, fv.size, fv.url from (select file_id, max(version) as mv from file_version group by file_id order by mv desc) as mver, file_version as fv where fv.file_id=mver.file_id and fv.version=mver.mv) as lver, esgf_node_manager.access_logging as dl, file as f, dataset as d, standard_name as s, variable as v  where dl.url=lver.url and dl.success='t' and lver.file_id=f.id and s.name=v.standard_name and v.dataset_id=d.id and f.dataset_id=d.id group by project, model, experiment, s.name);"

//#define QUERY8 	"start transaction esgf_dashboard.analytics2"
#define QUERY8 	"start transaction;"

#define QUERY9 	"drop table if exists esgf_dashboard.analytics2; create table esgf_dashboard.analytics2 (project varchar(255), model varchar(255), experiment varchar(255), name varchar(255), downloadsize bigint default 0, downloadcount numeric default 0);"

// GLOBAL METRICS for DATA and USERS

#define GET_DOWNLOADED_DATA_SIZE "select sum (size) from esgf_dashboard.finaldw;"

#define GET_DOWNLOADED_DATA_COUNT "select count(*) from esgf_dashboard.finaldw;"

#define GET_REGISTERED_USERS_COUNT "select count(distinct(username)) from esgf_security.user where openid like 'https://%s%';"

#define GET_LAST_IMPORT_ID "select lastprocessed_id from esgf_dashboard.reconciliation_process;"

#define START_TRANSACTION_CPU_METRICS "start transaction; lock esgf_dashboard.cpu_metrics;"
#define STORE_CPU_METRICS "INSERT into esgf_dashboard.cpu_metrics(loadavg1,loadavg5,loadavg15,time_stamp) values(%f,%f,%f,now());"
#define REMOVE_OLD_CPU_METRICS "DELETE from esgf_dashboard.cpu_metrics where time_stamp < (now() - interval '%d months' - interval '%d day');"
#define END_TRANSACTION_CPU_METRICS "end transaction;"

#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP1 "drop table if exists esgf_dashboard.dwstep1; create table esgf_dashboard.dwstep1 as (select file_id, max(version) as mv from file_version group by file_id order by mv desc);"

#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP2 "drop table if exists esgf_dashboard.dwstep2; create table esgf_dashboard.dwstep2 as (select fv.file_id, fv.size, fv.url, dwstep1.mv from file_version as fv, esgf_dashboard.dwstep1 where fv.file_id=dwstep1.file_id and fv.version=dwstep1.mv);"

#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP3 "drop table if exists esgf_dashboard.dwstep3; create table esgf_dashboard.dwstep3 as (select file.dataset_id, dwstep2.size,dwstep2.url, dwstep2.mv from file, esgf_dashboard.dwstep2 where dwstep2.file_id=file.id);"

#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP4 "drop table if exists esgf_dashboard.dwstep4; create table esgf_dashboard.dwstep4 as (select d.project, d.model, d.experiment, d.name as datasetname, d.id as datasetid, dwstep3.size, dwstep3.url, dwstep3.mv from dataset d, esgf_dashboard.dwstep3 where d.id=dwstep3.dataset_id);"

#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5 "drop table if exists esgf_dashboard.dwstep5; create table esgf_dashboard.dwstep5 as (select * from dataset_attr as da where da.name='realm');"

#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6 "drop table if exists esgf_dashboard.dwstep6; create table esgf_dashboard.dwstep6 as (select dwstep4.project, dwstep4.model, dwstep4.experiment, dwstep4.datasetname, dwstep4.datasetid, dwstep4.size, dwstep4.url, dwstep4.mv, dwstep5.value as realm from esgf_dashboard.dwstep4 LEFT OUTER JOIN esgf_dashboard.dwstep5 ON (dwstep4.datasetid = dwstep5.dataset_id) );"

#define QUERY_DATA_DOWNLOAD_METRICS_FINALDW_CREATE "drop table if exists esgf_dashboard.finaldw; create table esgf_dashboard.finaldw  (al_id bigint, project character varying(1024),model character varying(1024),experiment character varying(512), url character varying(1024),mv integer, realm character varying(512), user_id_hash character varying(512), user_idp character varying(256), year integer, month integer, day  integer, hour integer, service_type character varying(512), remote_addr character varying(128), datasetname character varying(255),size bigint,success integer, duration bigint);"

#define QUERY_DATA_DOWNLOAD_METRICS_GET_RAW_DATA "select al.id as al_id, dwstep6.project, dwstep6.model, dwstep6.experiment, dwstep6.url, dwstep6.mv, dwstep6.realm, al.user_id_hash, al.user_idp,  (extract(year from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as year, (extract(month from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as month, (extract(day from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as day,  (extract(hour from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as hour, al.service_type, al.remote_addr,  dwstep6.datasetname, dwstep6.size, (case success when 't' then 1 else 0 end) success, al.duration  from esgf_node_manager.access_logging as al, esgf_dashboard.dwstep6 where dwstep6.url=al.url and al.id>(select lastprocessed_id from esgf_dashboard.reconciliation_process) order by al_id;"

#define QUERY_INSERT_DATA_DOWNLOAD_METRICS_FINALDW "insert into esgf_dashboard.finaldw(%s) values(%s);"
#define QUERY_UPDATE_LAST_PROCESSED_AL_ID "update esgf_dashboard.reconciliation_process set lastprocessed_id =%lld, time_stamp=now();"

#define START_TRANSACTION_FINALDW_INGESTION "start transaction; lock esgf_dashboard.finaldw; lock esgf_dashboard.reconciliation_process;"
#define END_TRANSACTION_FINALDW_INGESTION "end transaction;"

// --------------------------------------------------------

struct host * loadHosts(unsigned *numHosts);
int writeResults(struct host *hosts, const unsigned numHosts);

#endif /* DBACCESS_H_ */
