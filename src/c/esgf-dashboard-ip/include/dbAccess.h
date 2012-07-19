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
//#define QUERY7  "drop table if exists esgf_dashboard.analytics2; create table esgf_dashboard.analytics2 as (select d.project, d.model, d.experiment, s.name, count(*) as downloadcount, sum(size/1024/1024) as downloadsize from (select fv.file_id, fv.size, fv.url from (select file_id, max(version) as mv from file_version group by file_id order by mv desc) as mver, file_version as fv where fv.file_id=mver.file_id and fv.version=mver.mv) as lver, esgf_node_manager.access_logging as dl, file as f, dataset as d, standard_name as s, variable as v  where dl.url=lver.url and dl.success='t' and lver.file_id=f.id and s.name=v.standard_name and v.dataset_id=d.id and f.dataset_id=d.id group by project, model, experiment, s.name);"

//#define QUERY8 	"start transaction"
#define QUERY8 	"start transaction;"

//#define QUERY9 	"drop table if exists esgf_dashboard.analytics2; create table esgf_dashboard.analytics2 (project varchar(255), model varchar(255), experiment varchar(255), name varchar(255), downloadsize bigint default 0, downloadcount numeric default 0);"

// GLOBAL METRICS for DATA and USERS

#define GET_DOWNLOADED_DATA_SIZE "select sum (size) from esgf_dashboard.finaldw where success=1;"

#define GET_DOWNLOADED_DATA_COUNT "select count(*) from esgf_dashboard.finaldw where success=1;"

#define GET_REGISTERED_USERS_COUNT "select count(distinct(username)) from esgf_security.user where openid like 'https://%s%';"

#define GET_LAST_IMPORT_ID "select lastprocessed_id from esgf_dashboard.reconciliation_process;"

// CPU QUERIES
#define START_TRANSACTION_CPU_METRICS "start transaction; lock esgf_dashboard.cpu_metrics;"
#define STORE_CPU_METRICS "INSERT into esgf_dashboard.cpu_metrics(loadavg1,loadavg5,loadavg15,time_stamp) values(%f,%f,%f,now());"
#define REMOVE_OLD_CPU_METRICS "DELETE from esgf_dashboard.cpu_metrics where time_stamp < (now() - interval '%d months' - interval '%d day');"
#define END_TRANSACTION_CPU_METRICS "end transaction;"

// MEMORY QUERIES
#define START_TRANSACTION_MEMORY_METRICS "start transaction; lock esgf_dashboard.memory_metrics;"
#define STORE_MEMORY_METRICS "INSERT into esgf_dashboard.memory_metrics(freeram,usedram,freeswap,usedswap,time_stamp) values(%ld,%ld,%ld,%ld,now());"
#define REMOVE_OLD_MEMORY_METRICS "DELETE from esgf_dashboard.memory_metrics where time_stamp < (now() - interval '%d months' - interval '%d day');"
#define END_TRANSACTION_MEMORY_METRICS "end transaction;"

// Query CORRETTE
//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP1 "drop table if exists esgf_dashboard.dwstep1; create table esgf_dashboard.dwstep1 as (select file_id, max(version) as mv from file_version group by file_id order by mv desc);"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP1 "drop table esgf_dashboard.dwstep1; create table esgf_dashboard.dwstep1 as (select file_id, max(version) as mv from file_version group by file_id order by mv desc);"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP2 "drop table if exists esgf_dashboard.dwstep2; create table esgf_dashboard.dwstep2 as (select fv.file_id, fv.size, fv.url, substr(f.base, 0, position('_' in f.base)) as var, dwstep1.mv from file_version as fv, esgf_dashboard.dwstep1, file as f where fv.file_id=dwstep1.file_id and fv.version=dwstep1.mv and f.id=fv.file_id);"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP2 "drop table esgf_dashboard.dwstep2; create table esgf_dashboard.dwstep2 as (select fv.file_id, fv.size, fv.url, substr(f.base, 0, position('_' in f.base)) as var, dwstep1.mv from file_version as fv, esgf_dashboard.dwstep1, file as f where fv.file_id=dwstep1.file_id and fv.version=dwstep1.mv and f.id=fv.file_id);"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP3 "drop table if exists esgf_dashboard.dwstep3; create table esgf_dashboard.dwstep3 as (select file.dataset_id, dwstep2.size,dwstep2.url, dwstep2.mv,dwstep2.file_id, dwstep2.var from file, esgf_dashboard.dwstep2 where dwstep2.file_id=file.id);"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP3 "drop table esgf_dashboard.dwstep3; create table esgf_dashboard.dwstep3 as (select file.dataset_id, dwstep2.size,dwstep2.url, dwstep2.mv,dwstep2.file_id, dwstep2.var from file, esgf_dashboard.dwstep2 where dwstep2.file_id=file.id);"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP4 "drop table if exists esgf_dashboard.dwstep4; create table esgf_dashboard.dwstep4 as (select d.project, d.model, d.experiment, d.name as datasetname, d.id as datasetid, dwstep3.size, dwstep3.url, dwstep3.mv,dwstep3.file_id,dwstep3.var from dataset d, esgf_dashboard.dwstep3 where d.id=dwstep3.dataset_id);"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP4 "drop table esgf_dashboard.dwstep4; create table esgf_dashboard.dwstep4 as (select d.project, d.model, d.experiment, d.name as datasetname, d.id as datasetid, dwstep3.size, dwstep3.url, dwstep3.mv,dwstep3.file_id,dwstep3.var from dataset d, esgf_dashboard.dwstep3 where d.id=dwstep3.dataset_id);"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5 "drop table if exists esgf_dashboard.dwstep5; create table esgf_dashboard.dwstep5 as (select * from dataset_attr as da where da.name='realm');"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5 "drop table esgf_dashboard.dwstep5; create table esgf_dashboard.dwstep5 as (select * from dataset_attr as da where da.name='realm');"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5B "drop table if exists esgf_dashboard.dwstep5b; create table esgf_dashboard.dwstep5b as (select * from dataset_attr as da where da.name='time_frequency');"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5B "drop table esgf_dashboard.dwstep5b; create table esgf_dashboard.dwstep5b as (select * from dataset_attr as da where da.name='time_frequency');"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5C "drop table if exists esgf_dashboard.dwstep5c; create table esgf_dashboard.dwstep5c as (select * from dataset_attr as da where da.name='institute');"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5C "drop table esgf_dashboard.dwstep5c; create table esgf_dashboard.dwstep5c as (select * from dataset_attr as da where da.name='institute');"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5D "drop table if exists esgf_dashboard.dwstep5d; create table esgf_dashboard.dwstep5d as (select * from dataset_attr as da where da.name='product');"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5D "drop table esgf_dashboard.dwstep5d; create table esgf_dashboard.dwstep5d as (select * from dataset_attr as da where da.name='product');"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5E "drop table if exists esgf_dashboard.dwstep5e; create table esgf_dashboard.dwstep5e as (select * from dataset_attr as da where da.name='ensemble');"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5E "drop table esgf_dashboard.dwstep5e; create table esgf_dashboard.dwstep5e as (select * from dataset_attr as da where da.name='ensemble');"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5F "drop table if exists esgf_dashboard.dwstep5f; create table esgf_dashboard.dwstep5f as (select * from dataset_attr as da where da.name='cmor_table');"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP5F "drop table esgf_dashboard.dwstep5f; create table esgf_dashboard.dwstep5f as (select * from dataset_attr as da where da.name='cmor_table');"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6 "drop table if exists esgf_dashboard.dwstep6; create table esgf_dashboard.dwstep6 as (select dwstep4.project, dwstep4.model, dwstep4.experiment, dwstep4.datasetname, dwstep4.datasetid, dwstep4.size, dwstep4.url, dwstep4.mv, dwstep4.file_id,dwstep4.var, dwstep5.value as realm from esgf_dashboard.dwstep4 LEFT OUTER JOIN esgf_dashboard.dwstep5 ON (dwstep4.datasetid = dwstep5.dataset_id));"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6 "drop table esgf_dashboard.dwstep6; create table esgf_dashboard.dwstep6 as (select dwstep4.project, dwstep4.model, dwstep4.experiment, dwstep4.datasetname, dwstep4.datasetid, dwstep4.size, dwstep4.url, dwstep4.mv, dwstep4.file_id,dwstep4.var, dwstep5.value as realm from esgf_dashboard.dwstep4 LEFT OUTER JOIN esgf_dashboard.dwstep5 ON (dwstep4.datasetid = dwstep5.dataset_id));"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6B "drop table if exists esgf_dashboard.dwstep6b; create table esgf_dashboard.dwstep6b as (select dwstep6.project, dwstep6.model, dwstep6.experiment, dwstep6.datasetname, dwstep6.datasetid, dwstep6.size, dwstep6.url, dwstep6.mv, dwstep6.file_id,dwstep6.var,dwstep6.realm, dwstep5b.value as time_frequency from esgf_dashboard.dwstep6 LEFT OUTER JOIN esgf_dashboard.dwstep5b ON (dwstep6.datasetid = dwstep5b.dataset_id));"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6B "drop table esgf_dashboard.dwstep6b; create table esgf_dashboard.dwstep6b as (select dwstep6.project, dwstep6.model, dwstep6.experiment, dwstep6.datasetname, dwstep6.datasetid, dwstep6.size, dwstep6.url, dwstep6.mv, dwstep6.file_id,dwstep6.var,dwstep6.realm, dwstep5b.value as time_frequency from esgf_dashboard.dwstep6 LEFT OUTER JOIN esgf_dashboard.dwstep5b ON (dwstep6.datasetid = dwstep5b.dataset_id));"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6C "drop table if exists esgf_dashboard.dwstep6c; create table esgf_dashboard.dwstep6c as (select dwstep6b.project, dwstep6b.model, dwstep6b.experiment, dwstep6b.datasetname, dwstep6b.datasetid, dwstep6b.size, dwstep6b.url, dwstep6b.mv, dwstep6b.file_id,dwstep6b.var,dwstep6b.realm, dwstep6b.time_frequency, dwstep5c.value as institute from esgf_dashboard.dwstep6b LEFT OUTER JOIN esgf_dashboard.dwstep5c ON (dwstep6b.datasetid = dwstep5c.dataset_id));"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6C "drop table esgf_dashboard.dwstep6c; create table esgf_dashboard.dwstep6c as (select dwstep6b.project, dwstep6b.model, dwstep6b.experiment, dwstep6b.datasetname, dwstep6b.datasetid, dwstep6b.size, dwstep6b.url, dwstep6b.mv, dwstep6b.file_id,dwstep6b.var,dwstep6b.realm, dwstep6b.time_frequency, dwstep5c.value as institute from esgf_dashboard.dwstep6b LEFT OUTER JOIN esgf_dashboard.dwstep5c ON (dwstep6b.datasetid = dwstep5c.dataset_id));"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6D "drop table if exists esgf_dashboard.dwstep6d; create table esgf_dashboard.dwstep6d as (select dwstep6c.project, dwstep6c.model, dwstep6c.experiment, dwstep6c.datasetname, dwstep6c.datasetid, dwstep6c.size, dwstep6c.url, dwstep6c.mv, dwstep6c.file_id,dwstep6c.var,dwstep6c.realm, dwstep6c.time_frequency, dwstep6c.institute, dwstep5d.value as product from esgf_dashboard.dwstep6c LEFT OUTER JOIN esgf_dashboard.dwstep5d ON (dwstep6c.datasetid = dwstep5d.dataset_id));"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6D "drop table esgf_dashboard.dwstep6d; create table esgf_dashboard.dwstep6d as (select dwstep6c.project, dwstep6c.model, dwstep6c.experiment, dwstep6c.datasetname, dwstep6c.datasetid, dwstep6c.size, dwstep6c.url, dwstep6c.mv, dwstep6c.file_id,dwstep6c.var,dwstep6c.realm, dwstep6c.time_frequency, dwstep6c.institute, dwstep5d.value as product from esgf_dashboard.dwstep6c LEFT OUTER JOIN esgf_dashboard.dwstep5d ON (dwstep6c.datasetid = dwstep5d.dataset_id));"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6E "drop table if exists esgf_dashboard.dwstep6e; create table esgf_dashboard.dwstep6e as (select dwstep6d.project, dwstep6d.model, dwstep6d.experiment, dwstep6d.datasetname, dwstep6d.datasetid, dwstep6d.size, dwstep6d.url, dwstep6d.mv, dwstep6d.file_id,dwstep6d.var,dwstep6d.realm, dwstep6d.time_frequency, dwstep6d.institute, dwstep6d.product, dwstep5e.value as ensemble from esgf_dashboard.dwstep6d LEFT OUTER JOIN esgf_dashboard.dwstep5e ON (dwstep6d.datasetid = dwstep5e.dataset_id));"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6E "drop table esgf_dashboard.dwstep6e; create table esgf_dashboard.dwstep6e as (select dwstep6d.project, dwstep6d.model, dwstep6d.experiment, dwstep6d.datasetname, dwstep6d.datasetid, dwstep6d.size, dwstep6d.url, dwstep6d.mv, dwstep6d.file_id,dwstep6d.var,dwstep6d.realm, dwstep6d.time_frequency, dwstep6d.institute, dwstep6d.product, dwstep5e.value as ensemble from esgf_dashboard.dwstep6d LEFT OUTER JOIN esgf_dashboard.dwstep5e ON (dwstep6d.datasetid = dwstep5e.dataset_id));"

//#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6F "drop table if exists esgf_dashboard.dwstep6f; create table esgf_dashboard.dwstep6f as (select dwstep6e.project, dwstep6e.model, dwstep6e.experiment, dwstep6e.datasetname, dwstep6e.datasetid, dwstep6e.size, dwstep6e.url, dwstep6e.mv, dwstep6e.file_id,dwstep6e.var,dwstep6e.realm, dwstep6e.time_frequency, dwstep6e.institute, dwstep6e.product, dwstep6e.ensemble, dwstep5f.value as cmor_table from esgf_dashboard.dwstep6e LEFT OUTER JOIN esgf_dashboard.dwstep5f ON (dwstep6e.datasetid = dwstep5f.dataset_id));"
#define QUERY_DATA_DOWNLOAD_METRICS_DWSTEP6F "drop table esgf_dashboard.dwstep6f; create table esgf_dashboard.dwstep6f as (select dwstep6e.project, dwstep6e.model, dwstep6e.experiment, dwstep6e.datasetname, dwstep6e.datasetid, dwstep6e.size, dwstep6e.url, dwstep6e.mv, dwstep6e.file_id,dwstep6e.var,dwstep6e.realm, dwstep6e.time_frequency, dwstep6e.institute, dwstep6e.product, dwstep6e.ensemble, dwstep5f.value as cmor_table from esgf_dashboard.dwstep6e LEFT OUTER JOIN esgf_dashboard.dwstep5f ON (dwstep6e.datasetid = dwstep5f.dataset_id));"

//#define QUERY_DATA_DOWNLOAD_METRICS_FINALDW_CREATE "drop table if exists esgf_dashboard.finaldw; create table esgf_dashboard.finaldw  (al_id bigint, datasetid integer, file_id integer ,project character varying(1024),model character varying(1024),experiment character varying(512), url character varying(1024),mv integer, var character varying(512), realm character varying(512), user_id_hash character varying(512), user_idp character varying(256), year integer, month integer, day  integer, hour integer, service_type character varying(512), remote_addr character varying(128), datasetname character varying(255),time_frequency character varying(512), institute character varying(512),product character varying(512),ensemble character varying(512),cmor_table character varying(512), size bigint,success integer, duration bigint, peername character varying(1024),UNIQUE(al_id,peername);"
#define QUERY_DATA_DOWNLOAD_METRICS_FINALDW_CREATE "drop table esgf_dashboard.finaldw; create table esgf_dashboard.finaldw  (al_id bigint, datasetid integer, file_id integer ,project character varying(1024),model character varying(1024),experiment character varying(512), url character varying(1024),mv integer, var character varying(512), realm character varying(512), user_id_hash character varying(512), user_idp character varying(256), year integer, month integer, day  integer, hour integer, service_type character varying(512), remote_addr character varying(128), datasetname character varying(255),time_frequency character varying(512), institute character varying(512),product character varying(512),ensemble character varying(512),cmor_table character varying(512), size bigint,success integer, duration bigint, peername character varying(1024), UNIQUE(al_id,peername));"

//#define QUERY_DATA_DOWNLOAD_METRICS_GET_RAW_DATA "select al.id as al_id, dwstep6f.datasetid, dwstep6f.file_id, dwstep6f.project, dwstep6f.model, dwstep6f.experiment, dwstep6f.url, dwstep6f.mv,  dwstep6f.var, dwstep6f.realm, al.user_id_hash, al.user_idp,  (extract(year from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as year, (extract(month from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as month, (extract(day from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as day,  (extract(hour from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as hour, al.service_type, al.remote_addr,  dwstep6f.datasetname, dwstep6f.time_frequency,dwstep6f.institute, dwstep6f.product,dwstep6f.ensemble,dwstep6f.cmor_table, (dwstep6f.size)/1024 as size, (case success when 't' then 1 else 0 end) success, al.duration  from esgf_node_manager.access_logging as al, esgf_dashboard.dwstep6f where dwstep6f.url=al.url and al.id>(select lastprocessed_id from esgf_dashboard.reconciliation_process) order by al_id;"
#define QUERY_DATA_DOWNLOAD_METRICS_GET_RAW_DATA "select al.id as al_id, dwstep6f.datasetid, dwstep6f.file_id, dwstep6f.project, dwstep6f.model, dwstep6f.experiment, dwstep6f.url, dwstep6f.mv,  dwstep6f.var, dwstep6f.realm, al.user_id_hash, al.user_idp,  (extract(year from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as year, (extract(month from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as month, (extract(day from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as day,  (extract(hour from (TIMESTAMP WITH TIME ZONE 'epoch' + al.date_fetched * INTERVAL '1 second'))) as hour, al.service_type, al.remote_addr,  dwstep6f.datasetname, dwstep6f.time_frequency,dwstep6f.institute, dwstep6f.product,dwstep6f.ensemble,dwstep6f.cmor_table, (dwstep6f.size)/1024 as size, success, al.duration  from esgf_node_manager.access_logging as al, esgf_dashboard.dwstep6f where dwstep6f.url=al.url and al.id>(select lastprocessed_id from esgf_dashboard.reconciliation_process) order by al_id;"

//#define URL_STATS "http://%s/ESGFNodeDesktop/gridJson/GetStats?al_id=%ld&delta=%ld"
#define URL_STATS "http://%s:8080/ESGFNodeDesktop/gridJson/GetStats?al_id=%ld&delta=%ld"

#define QUERY_INSERT_DATA_DOWNLOAD_METRICS_FINALDW "insert into esgf_dashboard.finaldw(%s,peername) values(%s,'%s');"
#define QUERY_UPDATE_LAST_PROCESSED_AL_ID "update esgf_dashboard.reconciliation_process set lastprocessed_id =%lld, time_stamp=now();"

#define START_TRANSACTION_FINALDW_INGESTION "start transaction; lock esgf_dashboard.finaldw; lock esgf_dashboard.reconciliation_process;"
#define END_TRANSACTION_FINALDW_INGESTION "end transaction;"

#define QUERY_STATS_AGGREGATOR_GET_HOSTLIST "select hostname,lastprocessed_id,time_stamp from esgf_dashboard.aggregation_process;"

#define QUERY_UPDATE_PEER_TIMESTAMP "update esgf_dashboard.aggregation_process set time_stamp=now() where hostname='%s';"
#define QUERY_REMOVE_STATS_FEDDW "delete from esgf_dashboard.federationdw where peername='%s';"
#define START_TRANSACTION_FEDDW "start transaction; lock esgf_dashboard.federationdw;"
#define END_TRANSACTION_FEDDW "end transaction;"

#define INSERT_REMOTE_STAT "insert into esgf_dashboard.federationdw(al_id, datasetid,file_id,project,model,experiment, url,mv, var, realm, user_id_hash, user_idp, year, month, day, hour , service_type, remote_addr , datasetname ,time_frequency , institute , product ,ensemble ,cmor_table , size ,success , duration , peername) values%s" 
#define QUERY_GET_LAST_PROCESSED_ID_FED  "select max(al_id) from esgf_dashboard.federationdw where peername='%s';"
#define QUERY_UPDATE_PEER_LAST_ID "update esgf_dashboard.aggregation_process set lastprocessed_id=%ld where hostname='%s';"


// --------------------------------------------------------

struct host * loadHosts(unsigned *numHosts);
int writeResults(struct host *hosts, const unsigned numHosts);

#endif /* DBACCESS_H_ */
