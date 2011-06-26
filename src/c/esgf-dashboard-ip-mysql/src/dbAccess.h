/*
 * dbAccess.h
 *
 *      Author: University of Salento and CMCC 
 */

#ifndef DBACCESS_H_
#define DBACCESS_H_

// --------------- Connection parameters ------------------
#define MYSQL_UNIX_SOCKET  	NULL
#define MYSQL_CLIENT_FLAG	0
// --------------------------------------------------------

// --------------- Query ----------------------------------
#define QUERY1 	"SELECT s.id, h.ip, s.port FROM service_instance s INNER JOIN host h ON h.id=s.idHost ORDER BY h.ip, s.port;"
#define QUERY2	"INSERT INTO service_status(status, elapsedTime, idServiceInstance) VALUES(?, ?, ?)"
// --------------------------------------------------------

struct host * loadHosts(unsigned *numHosts);
void writeResults(struct host *hosts, const unsigned numHosts);

#endif /* DBACCESS_H_ */
