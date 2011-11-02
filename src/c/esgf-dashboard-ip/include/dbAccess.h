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
//#define QUERY2	"INSERT INTO esgf_dashboard.service_status(status, elapsedTime, idServiceInstance) VALUES(?, ?, ?)"
// --------------------------------------------------------

struct host * loadHosts(unsigned *numHosts);
void writeResults(struct host *hosts, const unsigned numHosts);

#endif /* DBACCESS_H_ */
