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
#define QUERY7  "drop table if exists esgf_dashboard.analytics2; create table esgf_dashboard.analytics2 as (select d.project, d.model, d.experiment, s.name, count(*) as downloadcount, sum(size/1024/1024) as downloadsize from (select fv.file_id, fv.size, fv.url from (select file_id, max(version) as mv from file_version group by file_id order by mv desc) as mver, file_version as fv where fv.file_id=mver.file_id and fv.version=mver.mv) as lver, esgf_node_manager.access_logging as dl, file as f, dataset as d, standard_name as s, variable as v  where dl.url=lver.url and lver.file_id=f.id and s.name=v.standard_name and v.dataset_id=d.id and f.dataset_id=d.id group by project, model, experiment, s.name);"

//#define QUERY8 	"start transaction esgf_dashboard.analytics2"
#define QUERY8 	"start transaction;"

#define QUERY9 	"drop table if exists esgf_dashboard.analytics2; create table esgf_dashboard.analytics2 (project varchar(255), model varchar(255), experiment varchar(255), name varchar(255), downloadsize bigint default 0, downloadcount numeric default 0);"

// --------------------------------------------------------

struct host * loadHosts(unsigned *numHosts);
int writeResults(struct host *hosts, const unsigned numHosts);

#endif /* DBACCESS_H_ */
