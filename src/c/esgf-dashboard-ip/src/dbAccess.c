/*
 * dbAccess.c
 *
 *      Author: University of Salento and CMCC 
 */
#include <stdlib.h>
#include <stdio.h>
#include <mysql.h>
#include <string.h>
#include "ping.h"
#include "dbAccess.h"
#include "config.h"

struct host * loadHosts(unsigned *numHosts) {
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, MYSQL_HOST, MYSQL_USER, MYSQL_PASSWD, MYSQL_DB_NAME,
			MYSQL_PORT, MYSQL_UNIX_SOCKET, MYSQL_CLIENT_FLAG)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		return NULL;
	}

	/* send SQL query */
	if (mysql_query(conn, QUERY1)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		return NULL;
	}

	res = mysql_store_result(conn);
	if(res == NULL) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		return NULL;
	}
	const unsigned numTuples = mysql_num_rows(res);

	struct host* hosts = (struct host *) malloc(sizeof(struct host) * numTuples);
	unsigned t;
	for(t = 0; t < numTuples; t ++) {
		if((row = mysql_fetch_row(res)) == NULL)
			; // error
		hosts[t].id = atoi(row[0]);
		strcpy(hosts[t].hostName, row[1]);
		hosts[t].portNumber = atoi(row[2]);
	}
	mysql_free_result(res);
	mysql_close(conn);

	*numHosts = numTuples;
	return hosts;
}

void writeResults(struct host *hosts, const unsigned numHosts) {
	MYSQL *conn;
	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, MYSQL_HOST, MYSQL_USER, MYSQL_PASSWD, MYSQL_DB_NAME,
			MYSQL_PORT, MYSQL_UNIX_SOCKET, MYSQL_CLIENT_FLAG)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		return;
	}

	MYSQL_STMT * stmt = mysql_stmt_init(conn);
	mysql_stmt_prepare(stmt, QUERY2, strlen(QUERY2));

	unsigned index;
	for(index = 0; index < numHosts; index ++) {
		MYSQL_BIND bind[3];
		memset(bind, 0, sizeof(bind));
//		char *status[] = { "", "Active", "Inactive", "Timeout", "Error" };

		unsigned position = hosts[index].status;
		bind[0].buffer = &position;
		bind[0].buffer_type = MYSQL_TYPE_LONG;
		bind[0].buffer_length = sizeof(unsigned);
		bind[0].is_null = 0;

		bind[1].buffer = &hosts[index].elapsedTime;
		bind[1].buffer_type = MYSQL_TYPE_LONG;
		bind[1].buffer_length = sizeof(unsigned);
		bind[1].is_null = 0;

		bind[2].buffer = &hosts[index].id;
		bind[2].buffer_type = MYSQL_TYPE_LONG;
		bind[2].buffer_length = sizeof(unsigned);
		bind[2].is_null = 0;

		if(mysql_stmt_bind_param(stmt, bind)) {
			fprintf(stderr, " mysql_stmt_bind_param() failed\n");
			fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
			break;
		}
		if(mysql_stmt_execute(stmt)) {
			fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
			fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
			break;
		}
	}
	mysql_stmt_close(stmt);
	mysql_close(conn);

/*
	for(index = 0; index < numHosts; index ++) {
		printf("%s '%d', '%u', %s, '%u'\n", hosts[index].hostName, hosts[index].status, hosts[index].elapsedTime,
		hosts[index].details, hosts[index].id);
	}*/
}
