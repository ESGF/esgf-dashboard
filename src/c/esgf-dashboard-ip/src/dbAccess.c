/*
 * dbAccess.c
 *
 *      Author: University of Salento and CMCC 
 */
#include <stdlib.h>
#include <stdio.h>
#include "libpq-fe.h"
#include <string.h>
#include "../include/ping.h"
#include "../include/dbAccess.h"
#include "../include/config.h"

static void
exit_nicely(PGconn *conn)
{
        PQfinish(conn);
        exit(1);
}

struct host * loadHosts(unsigned *numHosts) {
	PGconn *conn;
	PGresult *res;
	int numTuples;

	/* Connect to database */

	char conninfo[1024] = {'\0'};
        /*if (POSTGRES_PASSWD) // da verificare perchè va male come controllo!!!!
             snprintf (conninfo, sizeof (conninfo), "host=%s port=%s dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER, POSTGRES_DB_NAME , POSTGRES_USER, POSTGRES_PASSWD);
        else*/

        snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
	//printf("Conninfo %s\n",conninfo);
	conn = PQconnectdb ((const char *) conninfo);

	if (PQstatus(conn) != CONNECTION_OK)
        {
                fprintf(stderr,"Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		*numHosts = 0;
		struct host* hosts = NULL; 
		return hosts;
        }

	/* send SQL query */
	/* QUERY1 === "SELECT s.id, h.ip, s.port FROM service_instance s INNER JOIN host h ON h.id=s.idHost ORDER BY h.ip, s.port;"*/
	
	//fprintf(stderr,"Query1 = %s \n",QUERY1);

	res = PQexec(conn, QUERY1);

	if ((!res) || (PQresultStatus(res) != PGRES_TUPLES_OK))
    	{
	        fprintf(stderr, "SELECT command did not return tuples properly\n");
	        PQclear(res);
		PQfinish(conn);
		*numHosts = 0;
		struct host* hosts = NULL;
		return hosts;
    	}

	numTuples = PQntuples(res);
	fprintf(stderr,"Number of Host/Services loaded from the DB = %d \n",numTuples);

	struct host* hosts = (struct host *) malloc(sizeof(struct host) * numTuples);
	unsigned t;
	for(t = 0; t < numTuples; t ++) {
		hosts[t].id = atoi(PQgetvalue(res, t, 0));
		strcpy(hosts[t].hostName, PQgetvalue(res, t, 1));
		hosts[t].portNumber = atoi(PQgetvalue(res, t, 2));
	}
	PQclear(res);

    	PQfinish(conn);
	*numHosts = numTuples;
	return hosts;
}

int writeResults(struct host *hosts, const unsigned numHosts) {

	PGconn * conn;
	PGresult *res;
        char conninfo[1024] = {'\0'};

        /* Connect to database */

        /*if (POSTGRES_PASSWD) // da verificare perchè va male come controllo!!!!
             snprintf (conninfo, sizeof (conninfo), "host=%s port=%s dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER, POSTGRES_DB_NAME , POSTG
RES_USER, POSTGRES_PASSWD);
        else*/

        snprintf (conninfo, sizeof (conninfo), "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST, POSTGRES_PORT_NUMBER,POSTGRES_DB_NAME, POSTGRES_USER,POSTGRES_PASSWD);
        //printf("Conninfo %s\n",conninfo);
        conn = PQconnectdb ((const char *) conninfo);

        if (PQstatus(conn) != CONNECTION_OK)
        {
                fprintf(stderr,"Connection to database failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
	  	return -1;	
                //exit_nicely(conn);
        }

	// QUERY2 === "INSERT INTO service_status(status, elapsedTime, idServiceInstance) "

	unsigned index;
	fprintf(stderr,"Number of Hosts = %d\n",numHosts);
	for(index = 0; index < numHosts; index ++) 
	{
		char insert_query[2048]= {'\0'};

		// Query composition //
		snprintf (insert_query, sizeof (insert_query), "%s VALUES(%d,%d,%d);", QUERY2,hosts[index].status, hosts[index].elapsedTime,hosts[index].id);
		fprintf(stderr,"METRICS Query -> index %d query %s\n",index, insert_query);

		res = PQexec(conn, insert_query);

  		if ((!res) || (PQresultStatus(res) != PGRES_COMMAND_OK))
       		 	fprintf(stderr, "Insert query failed\n");

    		PQclear(res);
	}
        PQfinish(conn);
	return 0;
}
