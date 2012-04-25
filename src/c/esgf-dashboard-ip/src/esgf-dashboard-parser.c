/*fare il merge di questa funzione nel file dbAccess.c, dove c'e' gia' il collegamento con il DB e tutte le variabili sono ok */

#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <netdb.h>
#include <netinet/in.h>

#include "libpq-fe.h"
//#include "../include/ping.h"
//#include "../include/dbAccess.h"
#include "../include/config.h"
#include "../include/hashtbl.h"
#include "../include/debug.h"

#define APPLICATION_SERVER_NAME		"Application Server"

// "Node" ELEMENTS 
#define REG_ELEMENT_REGISTRATION	"Registration"
#define REG_ELEMENT_NODE		"Node"
#define REG_ELEMENT_GEOLOCATION		"GeoLocation"
#define REG_ELEMENT_RSSFEEDS		"RSSFeeds"
#define REG_ELEMENT_SINGLE_RSSFEED	"RSSFeed"
#define REG_ELEMENT_NODEMANAGER		"NodeManager"
#define REG_ELEMENT_GRIDFTPSERVICE	"GridFTPService"
#define REG_ELEMENT_METRICS		"Metrics"
#define REG_ELEMENT_CONFIGURATION	"Configuration"
#define REG_ELEMENT_DOWNLOADEDDATA	"DownloadedData"
#define REG_ELEMENT_REGISTEREDUSERS	"RegisteredUsers"

// "Node" ATTRIBUTES
#define REG_ATTR_REGISTRATION_TIMESTAMP		 	"timeStamp"
#define REG_ATTR_NODE_ORGANIZATION		 	"organization"
#define REG_ATTR_NODE_NAMESPACE 			"namespace"
#define REG_ATTR_NODE_NODEPEERGROUP			"nodePeerGroup"
#define REG_ATTR_NODE_SUPPORTEMAIL 			"supportEmail"
#define REG_ATTR_NODE_NODEHOSTNAME			"hostname"
#define REG_ATTR_NODE_NODEIP				"ip"
#define REG_ATTR_NODE_DN				"dn"
#define REG_ATTR_NODE_SHORTNAME				"shortName"
#define REG_ATTR_NODE_LONGNAME				"longName"
#define REG_ATTR_NODE_TIMESTAMP				"timeStamp"
#define REG_ATTR_NODE_VERSION				"version"
#define REG_ATTR_NODE_RELEASE				"release"
#define REG_ATTR_NODE_NODETYPE				"nodeType"
#define REG_ATTR_NODE_ADMINPEER				"adminPeer"
#define REG_ATTR_NODE_DEFAULTPEER			"defaultPeer"
#define REG_ELEMENT_GEOLOCATION_ATTR_LAT		"lat"
#define REG_ELEMENT_GEOLOCATION_ATTR_LON		"lon"
#define REG_ELEMENT_GEOLOCATION_ATTR_CITY		"city"
#define REG_ELEMENT_CONFIGURATION_ATTR_TYPE		"serviceType"
#define REG_ELEMENT_CONFIGURATION_ATTR_PORT		"port"
#define REG_ELEMENT_NODEMANAGER_ATTR_ENDPOINT		"endpoint"
#define REG_ELEMENT_SINGLE_RSSFEED_ATTR_URL		"url"
#define REG_ELEMENT_DOWNLOADEDDATA_ATTR_COUNT		"count"
#define REG_ELEMENT_DOWNLOADEDDATA_ATTR_SIZE		"size"
#define REG_ELEMENT_REGISTEREDUSERS_ATTR_COUNT		"count"

// Start transaction and lock tables 
#define QUERY_OPEN_TRANSACTION  "start transaction; lock esgf_dashboard.project_dash ; lock esgf_dashboard.host ; lock esgf_dashboard.uses ; lock esgf_dashboard.join1 ; lock esgf_dashboard.service_instance ;"

// End transaction and release locks 
#define QUERY_CLOSE_TRANSACTION  "end transaction;" 

// Get list of PROJECTS
#define QUERY_GET_LIST_OF_PROJECTS  "SELECT name,id from esgf_dashboard.project_dash;"
// Get list of SERVICES 
#define QUERY_GET_LIST_OF_SERVICES  "select (CAST(idhost as varchar) || ':' || CAST(port as varchar)) as key,id from esgf_dashboard.service_instance;"
// Get list of HOST
#define QUERY_GET_LIST_OF_HOSTS  "SELECT ip,id from esgf_dashboard.host;"
// Get list of "USES" 
#define QUERY_GET_LIST_OF_USES  "SELECT (CAST(idproject as varchar ) || ':' || CAST(idserviceinstance as varchar)) as key, idproject from esgf_dashboard.uses;"
// Get list of "RSSFEEDS" 
#define QUERY_GET_LIST_OF_RSSFEEDS  "SELECT rssfeed, idrssfeed from esgf_dashboard.rssfeed;"

// Get list of "HASRSSFEEDS" 
#define QUERY_GET_LIST_OF_HASRSSFEEDS  "SELECT (CAST(idrssfeed as varchar) || ':' || CAST(idhost as varchar)) as key,idrssfeed from esgf_dashboard.hasfeed;"

// QUERY_INSERT_PROJECT adds a new project (peer group) in the database
#define QUERY_INSERT_PROJECT  "INSERT into esgf_dashboard.project_dash(name,description) values('%s','%s');"

// QUERY QUERY_UPDATE_GEOLOCATION_INFO update geolocation info to an existing host (P2P node) in the database
#define QUERY_UPDATE_GEOLOCATION_INFO  "UPDATE esgf_dashboard.host set city='%s',latitude=%f,longitude=%f where id=%ld;"

// QUERY QUERY_INSERT_HOST adds a new host (P2P node) in the database
#define QUERY_INSERT_NEW_HOST  "INSERT into esgf_dashboard.host(name,ip) values('%s','%s');"

// QUERY QUERY_GET_PROJECT_ID retrieves the id value of a specific project (peer group) 
#define QUERY_GET_PROJECT_ID "SELECT id from esgf_dashboard.project_dash where name='%s';"

// QUERY QUERY_GET_HOST_ID retrieves the id value of a specific host (P2P node) 
#define QUERY_GET_HOST_ID "SELECT id from esgf_dashboard.host where ip='%s';"

// QUERY QUERY_GET_SERVICE_ID retrieves the id value of a specific service 
#define QUERY_GET_SERVICE_ID "SELECT id from esgf_dashboard.service_instance where port=%ld and idhost=%ld;"

// QUERY QUERY_GET_RSSFEED_ID retrieves the id value of a specific rssfeed 
#define QUERY_GET_RSSFEED_ID "SELECT idrssfeed from esgf_dashboard.rssfeed where rssfeed='%s';"

// QUERY QUERY_PROJECT_AUTHORIZATION, authorize project (peer group) on guest user in the database
#define QUERY_PROJECT_AUTHORIZATION  "INSERT into esgf_dashboard.join1(iduser,idproject) values(1,%ld);"

// QUERY QUERY_INSERT_SERVICE_INFO, add a service instance in the database
#define QUERY_INSERT_SERVICE_INFO  "INSERT into esgf_dashboard.service_instance(port,name,institution,mail_admin,idhost) values(%ld,'%s','%s','%s',%ld);"

// QUERY QUERY_INSERT_RSSFEED_INFO, add an rssfeed instance in the database
#define QUERY_INSERT_RSSFEED_INFO  "INSERT into esgf_dashboard.rssfeed(rssfeed) values('%s');"

// QUERY QUERY_INSERT_SERVICE_TO_PROJECT binds a service to a project (peer group) in the database
#define QUERY_INSERT_SERVICE_TO_PROJECT  "INSERT into esgf_dashboard.uses(idproject,idserviceinstance) values(%ld,%ld);"

// QUERY QUERY_ADD_RSSFEED_TO_HOST binds a rssfeed to a host in the database
#define QUERY_ADD_RSSFEED_TO_HOST  "INSERT into esgf_dashboard.hasfeed(idrssfeed, idhost) values(%ld,%ld);"

// QUERY TO MANAGE THE METRICS HISTORY 
#define QUERY_DELETE_OLD_METRICS  "DELETE from esgf_dashboard.service_status where timestamp < (now() - interval '%d months' - interval '%d day');"


// HashTables dimensions
#define HAST_TABLE_PROJECT_DIM		16
#define HAST_TABLE_SERVICE_DIM		64		
#define HAST_TABLE_HOST_DIM		16
#define HAST_TABLE_USES_DIM		64	
#define HAST_TABLE_RSSFEED_DIM		16	
#define HAST_TABLE_HASRSSFEED_DIM	32	


/*
 *To compile this file using gcc you can type
 *gcc `xml2-config --cflags --libs` -o esgf-dashboard-parser esgf-dashboard-parser.c
 */


/* 
The get_foreign_key_value function returns:
-1 = SELECT issue
-2 = more than 1 record (UNIQUE constraint issue) 
>0 = fk value
*/

int check_ip_class(char* buffer)
{
    char value_buffer[50];
    char* position;
    int number_dots=0;
    int ip_values[4]={ 0 };
    int i;

    number_dots=0;
    while (number_dots<3)
        {
         position = strrchr (buffer, '.');
         if (!position)
                return -1;
         strcpy (value_buffer, position + 1);  // now value_buffer stores the VALUE        
         *position = '\0';
         ip_values[3-number_dots] = atoi(value_buffer);
         number_dots++;
        }

    ip_values[0] = atoi(buffer);

// Check on private ip classes 

//24-bit Block (/8 prefix, 1 × A)	10.0.0.0	10.255.255.255	16777216
    if (ip_values[0]==10)
        return -2;

//20-bit Block (/12 prefix, 16 × B)	172.16.0.0	172.31.255.255	1048576
    if ((ip_values[0]==172) && (ip_values[1]>=16) && (ip_values[1]<=31))
        return -3;

//16-bit Block (/16 prefix, 256 × C)	192.168.0.0	192.168.255.255	65536
    if ((ip_values[0]==192) && (ip_values[1]==168) )
        return -4;

 return 0;
}



long int
get_foreign_key_value (PGconn * conn, char *query)
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

int
submit_query (PGconn * conn, char *query)
{
  PGresult *res;
  //fprintf (stderr, "Query: %s\n", query);

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

/* int history_metrics_management(PGconn *conn, char* QUERY, int hist_month, int hist_day)
{
  char query_history[2048] = { '\0' };

  snprintf (query_history,sizeof (query_history),QUERY,hist_month, hist_day);
  fprintf(stderr,"Query di rimozione delle metriche \n%s",query_history);
  if (submit_query (conn, query_history))
          return -1;
   
  return 0;
}*/

int manage_database_open_close_transaction(PGconn *conn, char* QUERY)
{
  char query_transaction[2048] = { '\0' };

  snprintf (query_transaction,sizeof (query_transaction),QUERY);
  if (submit_query (conn, query_transaction))
          return -1;
   
  return 0;
}

int
parse_registration_xml_file (xmlNode * a_node)
{
  xmlNode *cur_node = NULL;
  xmlNode *node_node = NULL;
  xmlNode *int_node = NULL;
  xmlNode *gridftp_node = NULL;
  xmlNode *metrics_node = NULL;
  xmlNode *rssfeed_node = NULL;
  int iter = 0;
  int iter2 = 0;
  int iter3 = 0;
  char *position;
  char *cursor_buf;
  char buffer[2048] = { '\0' };
  char query[2048] = { '\0' };
  PGconn *conn;
  PGresult *res;
  char conninfo[1024] = { '\0' };
  char conninfo_printf[1024] = { '\0' };
  char open_transaction[2048] = { '\0' };
  char close_transaction[2048] = { '\0' };
  HASHTBL *hashtbl_projects;
  HASHTBL *hashtbl_hosts;
  HASHTBL *hashtbl_services;
  HASHTBL *hashtbl_uses;
  HASHTBL *hashtbl_rssfeed;
  HASHTBL *hashtbl_hasrssfeed;
  char *hashtbl_result;
  int create_populate_done = 0;
  static long long int success_lookup[2] = { 0 };	// [0] success [1] missing
  static long long int last_timestamp = 0;

  snprintf (conninfo, sizeof (conninfo),
	    "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST,
	    POSTGRES_PORT_NUMBER, POSTGRES_DB_NAME, POSTGRES_USER,
	    POSTGRES_PASSWD);

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

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"OPEN Transaction and locking the database tables the parser needs...\n");
  if (manage_database_open_close_transaction(conn,QUERY_OPEN_TRANSACTION)) {
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"Transaction FAILED - Database Tables Lock: Failed. [Recovery action: Skip parsing]\n");
      	  PQfinish (conn);
	  return -1;
	 }
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Transaction OK - Database Tables Lock: Ok\n");

  // "Registration" iteration 
  for (cur_node = a_node; cur_node; cur_node = cur_node->next)	// loop on REGISTRATION elements
    {
      char *registration_timestamp;
      long long int current_timestamp;
      if (!strcmp (cur_node->name, "text"))
	continue;
      //printf ("Loop on REGISTRATION: %d\n", ++iter);

      if (cur_node->type == XML_ELEMENT_NODE
	  && (!strcmp (cur_node->name, REG_ELEMENT_REGISTRATION)))
	{

	  // check on the REGISTRATION timestamp    
	  registration_timestamp =
	    xmlGetProp (cur_node, REG_ATTR_REGISTRATION_TIMESTAMP);

	  if (registration_timestamp == NULL
	      || !strcmp (registration_timestamp, ""))
	    {
	     pmesg(LOG_WARNING,__FILE__,__LINE__, 
		       "Missing/invalid %s [skip current %s element]\n",
		       REG_ATTR_REGISTRATION_TIMESTAMP,
		       REG_ELEMENT_REGISTRATION);
	      xmlFree (registration_timestamp);
	      continue;
	    }
	  current_timestamp = atoll (registration_timestamp);
	  xmlFree (registration_timestamp);

	  if (current_timestamp <= last_timestamp)
	    {
	     pmesg(LOG_WARNING,__FILE__,__LINE__, 
		       "Skipping %s element - [older/equal timestamp]\n",
		       REG_ELEMENT_REGISTRATION);
	      continue;
	    }
	  last_timestamp = current_timestamp;
	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"New timestamp =  %lld\n", last_timestamp);
		
	  // create and populate hashtables if not yet
	  if (!create_populate_done)
	    {
	     pmesg(LOG_DEBUG,__FILE__,__LINE__,"Create and populate Hash tables (first iteration)\n");
	      create_populate_done = 1;
	      // Hash tables creation
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for PROJECTS\n");
	      if (!(hashtbl_projects = hashtbl_create (HAST_TABLE_PROJECT_DIM, NULL)))
		{
		 pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for PROJECTS [skip parsing]\n");
		  continue;
		}

	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for HOST\n");
	      if (!(hashtbl_hosts = hashtbl_create (HAST_TABLE_HOST_DIM, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for HOST [skip parsing]\n");
		  hashtbl_destroy (hashtbl_projects);
		  continue;
		}

	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for SERVICES\n");
	      if (!(hashtbl_services = hashtbl_create (HAST_TABLE_SERVICE_DIM, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for SERVICES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_projects);
		  hashtbl_destroy (hashtbl_hosts);
		  continue;
		}

	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for USES\n");
	      if (!(hashtbl_uses = hashtbl_create (HAST_TABLE_USES_DIM, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for USES [skip parsing]\n");
		  hashtbl_destroy (hashtbl_projects);
		  hashtbl_destroy (hashtbl_hosts);
		  hashtbl_destroy (hashtbl_services);
		  continue;
		}

	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for RSSFEED\n");
	      if (!(hashtbl_rssfeed = hashtbl_create (HAST_TABLE_RSSFEED_DIM, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for RSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_projects);
		  hashtbl_destroy (hashtbl_hosts);
		  hashtbl_destroy (hashtbl_services);
		  hashtbl_destroy (hashtbl_uses);
		  continue;
		}
	      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Creating the hashtable for HASRSSFEED\n");
	      if (!(hashtbl_hasrssfeed = hashtbl_create (HAST_TABLE_HASRSSFEED_DIM, NULL)))
		{
		  pmesg(LOG_WARNING,__FILE__,__LINE__,"ERROR: hashtbl_create() failed for HASRSSFEED [skip parsing]\n");
		  hashtbl_destroy (hashtbl_projects);
		  hashtbl_destroy (hashtbl_hosts);
		  hashtbl_destroy (hashtbl_services);
		  hashtbl_destroy (hashtbl_uses);
		  hashtbl_destroy (hashtbl_rssfeed);
		  continue;
		}
	      populate_hash_table (conn, QUERY_GET_LIST_OF_PROJECTS,
				   &hashtbl_projects);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_HOSTS,
				   &hashtbl_hosts);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_SERVICES,
				   &hashtbl_services);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_USES,
				   &hashtbl_uses);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_RSSFEEDS,
				   &hashtbl_rssfeed);
	      populate_hash_table (conn, QUERY_GET_LIST_OF_HASRSSFEEDS,
				   &hashtbl_hasrssfeed);
	    }
	  else
	    pmesg(LOG_DEBUG,__FILE__,__LINE__, "Hash tables already in place with the data [%d]\n",create_populate_done);


	  // Loop on NODE elements

	  for (node_node = cur_node->children; node_node; node_node = node_node->next)	// loop on NODE elements
	    {
	      char *organization;
	      char *support_email;
	      char *npg_project;
	      char *node_ip;
	      char *node_hostname;
	      int number_of_projects;
	      long int project_ids[1024] = { 0 }; // TO DO: this should be dynamically allocated starting from the total number of projects in the hashtable
	      long int service_ids[1024] = { 0 }; // TO DO: this should be dynamically allocated starting from the total number of services in the hashtable
	      long int host_id = 0;
	      int geolocation_found;
	      int i;

	      if (!strcmp (node_node->name, "text"))
		continue;

	      if (node_node->type == XML_ELEMENT_NODE && (!strcmp (node_node->name, REG_ELEMENT_NODE)))	// if a NODE element 
		{
		  char insert_new_host_query[2048] = { '\0' };
		  char select_id_host_query[2048] = { '\0' };
		  char node_ip_test[64] = { '\0' };

		  // get NODE attributes values

		  organization =
		    xmlGetProp (node_node, REG_ATTR_NODE_ORGANIZATION);

		  if (organization == NULL || !strcmp (organization, ""))
		    {
		      pmesg(LOG_WARNING,__FILE__,__LINE__, 
			       "Missing/invalid %s [skip current NODE element]\n",
			       REG_ATTR_NODE_ORGANIZATION);
		      xmlFree (organization);
		      continue;
		    }

		  npg_project =
		    xmlGetProp (node_node, REG_ATTR_NODE_NODEPEERGROUP);

		  if (npg_project == NULL || !strcmp (npg_project, ""))
		    {
		      pmesg(LOG_WARNING,__FILE__,__LINE__,"Missing/invalid %s [skip current NODE element]\n",REG_ATTR_NODE_NODEPEERGROUP);
		      xmlFree (organization);
		      xmlFree (npg_project);
		      continue;
		    }

		  node_ip = xmlGetProp (node_node, REG_ATTR_NODE_NODEIP);
		   
		  if (node_ip == NULL || !strcmp (node_ip, ""))
		    {

		      pmesg(LOG_WARNING,__FILE__,__LINE__,"Missing/invalid %s [skip current NODE element]\n",REG_ATTR_NODE_NODEIP);
		      xmlFree (organization);
		      xmlFree (npg_project);
		      xmlFree (node_ip);
		      continue;
		    }

 		  sprintf(node_ip_test,"%s",node_ip); 
    		  if (check_ip_class(node_ip_test)) {
		      pmesg(LOG_WARNING,__FILE__,__LINE__,"IP check failed: Is this node [%s] running on a private network? [skip current NODE element]\n",node_ip);		
		      xmlFree (organization);
		      xmlFree (npg_project);
		      xmlFree (node_ip);
		      continue;
		  }
		  pmesg(LOG_DEBUG,__FILE__,__LINE__,"IP check on [%s] OK\n",node_ip);		


		  node_hostname =
		    xmlGetProp (node_node, REG_ATTR_NODE_NODEHOSTNAME);

		  if (node_hostname == NULL || !strcmp (node_hostname, "") )
		    {

		      pmesg(LOG_WARNING,__FILE__,__LINE__,"Missing/invalid %s [skip current NODE element]\n",REG_ATTR_NODE_NODEHOSTNAME);
		      xmlFree (organization);
		      xmlFree (npg_project);
		      xmlFree (node_ip);
		      xmlFree (node_hostname);
		      continue;
		    }
	

		  support_email =
		    xmlGetProp (node_node, REG_ATTR_NODE_SUPPORTEMAIL);

		  if (support_email == NULL || !strcmp (support_email, ""))
		    	pmesg(LOG_WARNING,__FILE__,__LINE__,"Missing/invalid %s [No problem... the attribute is not mandatory]\n",REG_ATTR_NODE_SUPPORTEMAIL);

		  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Organization attribute: %s\n",organization);
		  pmesg(LOG_DEBUG,__FILE__,__LINE__,"PeerGroups attribute: %s\n", npg_project);
		  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Hostname attribute: %s\n", node_hostname);
		  pmesg(LOG_DEBUG,__FILE__,__LINE__,"IP attribute: %s\n", node_ip);


		  if (hashtbl_result = hashtbl_get (hashtbl_hosts, node_ip))
		    {
		      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostTable hit! [%s] [%s]\n",node_ip, hashtbl_result);
		      host_id = atol (hashtbl_result);
		      success_lookup[0]++;
		    }
		  else
		    {		// add host entry in DB (and hashtable too) without geolocation information

		      char host_id_str[128] = { '\0' };
		      success_lookup[1]++;
		      snprintf (insert_new_host_query,
				sizeof (insert_new_host_query),
				QUERY_INSERT_NEW_HOST, node_hostname,
				node_ip);
		      submit_query (conn, insert_new_host_query);

		      snprintf (select_id_host_query,
				sizeof (select_id_host_query),
				QUERY_GET_HOST_ID, node_ip);
		      host_id =
			get_foreign_key_value (conn, select_id_host_query);
		      // add entry to hash table
		      sprintf (host_id_str, "%ld", host_id);
		      hashtbl_insert (hashtbl_hosts, node_ip, host_id_str);
		      pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",node_ip, host_id_str);
		    }

		  //fprintf (stderr, "Host %s | id : %ld\n", node_ip, host_id);

		  // isolate PeerGroups adding them to the DB
		  sprintf (buffer, "%s", npg_project);
		  position = &buffer[0];
		  number_of_projects = 0;
		  do
		    {
		      char insert_query[2048] = { '\0' };
		      char select_query[2048] = { '\0' };
		      char authorization_query[2048] = { '\0' };
		      cursor_buf = position;
		      position = strchr (cursor_buf, ',');
		      if (position)
			{
			  *position = '\0';	// now cursor_buf points to the isolated PeerGroup
			  position++;	// position points to the new string (if any)   
			}
		      pmesg(LOG_DEBUG,__FILE__,__LINE__,"PeerGroup %s\n", cursor_buf);

		      if (hashtbl_result =
			  hashtbl_get (hashtbl_projects, cursor_buf))
			{
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup ProjectTable hit! [%s] [%s]\n",cursor_buf, hashtbl_result);
			  success_lookup[0]++;
			  project_ids[number_of_projects++] =
			    atol (hashtbl_result);
			}
		      else
			{	// add host entry in DB (and hashtable too) without geolocation information

			  char project_id_str[128] = { '\0' };
			  success_lookup[1]++;
			  // add PeerGroup to project_dash table
			  snprintf (insert_query, sizeof (insert_query),
				    QUERY_INSERT_PROJECT, cursor_buf,
				    cursor_buf);
			  submit_query (conn, insert_query);

			  // get project ID 
			  snprintf (select_query, sizeof (select_query),
				    QUERY_GET_PROJECT_ID, cursor_buf);
			  project_ids[number_of_projects++] =
			    get_foreign_key_value (conn, select_query);

			  // added new project into the hashtable
			  sprintf (project_id_str, "%ld",
				   project_ids[number_of_projects - 1]);
			  hashtbl_insert (hashtbl_projects, cursor_buf,
					  project_id_str);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",cursor_buf, project_id_str);

			  // add user to peer group (authorization) 
			  snprintf (authorization_query,
				    sizeof (authorization_query),
				    QUERY_PROJECT_AUTHORIZATION,
				    project_ids[number_of_projects - 1]);
			  submit_query (conn, authorization_query);

			}
		    }
		  while (position);

		  /*for (i = 0; i < number_of_projects; i++)
		    fprintf (stderr, "Valore %d %ld\n", i, project_ids[i]);

		  fprintf (stderr, "Element->name: %s\n", node_node->name);*/

		  geolocation_found = 0;

		  // loop on internal NODE elements (CA, GeoLocation, GridFTPService, etc.)
		  for (int_node = node_node->children; int_node;
		       int_node = int_node->next)
		    {
		      if (!strcmp (int_node->name, "text"))
			continue;

		      //printf ("Loop on INTERNALNODE: %d\n", ++iter3);


		      if (int_node->type == XML_ELEMENT_NODE)	// Internal switch 
			{

		/*******************************************************************/
			  // Start of "if internal node is GeoLocation ELEMENT"
			  if (!strcmp
			      (int_node->name, REG_ELEMENT_GEOLOCATION))
			    {
			      char update_query[2048] = { '\0' };
			      char *latitude;
			      char *longitude;
			      char *city;

			      geolocation_found = 1;
			      latitude =
				xmlGetProp (int_node,
					    REG_ELEMENT_GEOLOCATION_ATTR_LAT);
			      longitude =
				xmlGetProp (int_node,
					    REG_ELEMENT_GEOLOCATION_ATTR_LON);
			      city =
				xmlGetProp (int_node,
					    REG_ELEMENT_GEOLOCATION_ATTR_CITY);

			      // if <GeoLocation> is incomplete or corrupted...
			      if (latitude == NULL || !strcmp (latitude, "")
				  || longitude == NULL
				  || !strcmp (longitude, "") || city == NULL
				  || !strcmp (city, ""))
				{
				  geolocation_found = 0;
				  pmesg(LOG_DEBUG,__FILE__,__LINE__,"The <GeoLocation> element seems to be corrupted or not complete\n");
				}
			      else
				{
				  snprintf (update_query,
					    sizeof (update_query),
					    QUERY_UPDATE_GEOLOCATION_INFO,
					    city, atof (latitude),
					    atof (longitude), host_id);
				  submit_query (conn, update_query);
				}

			      // free XML GEOLOCATION attributes      
			      xmlFree (latitude);
			      xmlFree (longitude);
			      xmlFree (city);
			    }	// end of "if internal node is GeoLocation ELEMENT"
			  
		/*******************************************************************/

			  // Start of "if internal node is an RSSFeed ELEMENT"
			  if (!strcmp
			      (int_node->name, REG_ELEMENT_RSSFEEDS))
			    	{
			      // loop on internal RSSFEEDS children
				pmesg(LOG_DEBUG,__FILE__,__LINE__,"[%s] found\n",REG_ELEMENT_RSSFEEDS);
			      	for (rssfeed_node = int_node->children;
				   rssfeed_node;
				   rssfeed_node = rssfeed_node->next)
				{
				  if (!strcmp (rssfeed_node->name, "text"))
				    continue;
				  // Start of "if SINGLE_RSSFEED element"
				  if (rssfeed_node->type == XML_ELEMENT_NODE
				      &&
				      (!strcmp
				       (rssfeed_node->name,
					REG_ELEMENT_SINGLE_RSSFEED)))
				    {
					pmesg(LOG_DEBUG,__FILE__,__LINE__,"	[%s] found\n",REG_ELEMENT_SINGLE_RSSFEED);
				      	char *rss_feed_url;

				      rss_feed_url =
					xmlGetProp (rssfeed_node,
						    REG_ELEMENT_SINGLE_RSSFEED_ATTR_URL);

				      if (rss_feed_url == NULL
					  || !strcmp (rss_feed_url, ""))
					{
					  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Missing/invalid %s attribute [skip current %s  element]\n",REG_ELEMENT_SINGLE_RSSFEED_ATTR_URL,
						   REG_ELEMENT_SINGLE_RSSFEED);
					} 
					else  
					{  // start "if is a VALID SINGLE_RSSFEED element
					  long int rssfeed_id;
				      	  char hasrssfeed_key[128] = { '\0' };
					  pmesg(LOG_DEBUG,__FILE__,__LINE__,"		Valid [%s] element\n",REG_ELEMENT_SINGLE_RSSFEED);
					  pmesg(LOG_DEBUG,__FILE__,__LINE__,"		Url [%s] \n",rss_feed_url);

		                          if (hashtbl_result = hashtbl_get (hashtbl_rssfeed, rss_feed_url))
                        			{
                          			pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup RSSFeedTable hit! [%s] [%s]\n",rss_feed_url, hashtbl_result);
						rssfeed_id = atol (hashtbl_result);
                          			success_lookup[0]++;
                        			}
						else 
						{ // start "else rssfeed is not in the rssfeed hashtable

				      // 1) add a rssfeed 
				      char rssfeed_id_str[128] = { '\0' };
				      char select_id_rssfeed_query[2048] = { '\0' };
				      char insert_rssfeed_query[2048] =  { '\0' };

				      success_lookup[1]++;
				      snprintf (insert_rssfeed_query,
						sizeof (insert_rssfeed_query),
						QUERY_INSERT_RSSFEED_INFO,
						rss_feed_url);
				      submit_query (conn,
						    insert_rssfeed_query);
				      // 2) retrieve rssfeed_id
				      // grab rssfeed_id from rssfeedurl 
				      snprintf (select_id_rssfeed_query,
						sizeof
						(select_id_rssfeed_query),
						QUERY_GET_RSSFEED_ID,
						rss_feed_url);
				      rssfeed_id =
					get_foreign_key_value (conn,
							       select_id_rssfeed_query);

				      // add new entry into the hashtable
				      sprintf (rssfeed_id_str, "%ld",
					       rssfeed_id);
				      hashtbl_insert (hashtbl_rssfeed,
						      rss_feed_url,
						      rssfeed_id_str);
				      pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",rss_feed_url, rssfeed_id_str);
					} // end "else if not in the rssfeed hashtable 

				      sprintf (hasrssfeed_key, "%ld:%ld",
					       rssfeed_id, host_id);
				      if (hashtbl_result =
					  hashtbl_get (hashtbl_hasrssfeed,
						       hasrssfeed_key))
					{
					  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HasRSSFeed hit! [%s] [%s]\n",hasrssfeed_key, hashtbl_result);
					  success_lookup[0]++;
					}
				      else
					{ // if the key is not in hasfeed then...
				      	  char rssfeed_id_str[128] = { '\0' };
					  char
					    insert_rssfeed_2_host_query
					    [2048] = { '\0' };
					  success_lookup[1]++;
					  char project_ids_str[128] =
					    { '\0' };
					  snprintf
					    (insert_rssfeed_2_host_query,
					     sizeof
					     (insert_rssfeed_2_host_query),
					     QUERY_ADD_RSSFEED_TO_HOST,
					     rssfeed_id, host_id);
					  submit_query (conn,
							insert_rssfeed_2_host_query);
					  // add new entry into the hashtable
					  sprintf (rssfeed_id_str, "%ld",
						   rssfeed_id);
					  hashtbl_insert (hashtbl_hasrssfeed,
							  hasrssfeed_key,
							  rssfeed_id_str);
					  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",hasrssfeed_key, rssfeed_id_str);
					} // end if the key is not in the hasfeed

					}  // end "if is a VALID SINGLE_RSSFEED element 
				
					xmlFree (rss_feed_url);

				    } // end "if SINGLE_RSSFEED element"
				} // end loop on internal RSSFEEDS children
			      }  // end of "if internal node is an RSSFeed ELEMENT"

		/*******************************************************************/
			  // Start of "if internal node is NodeManager ELEMENT"
			  if (!strcmp
			      (int_node->name, REG_ELEMENT_NODEMANAGER))
			    {
			      char insert_query[2048] = { '\0' };
			      char *endpoint;
			      char *slashcursor;
			      char *startcursor;
			      char buffer_endpoint[2048] = { '\0' };
			      long int app_server_port = 80;
			      int iteration = 0;
			      int service_id;
			      char insert_service_query[2048] = { '\0' };
			      char select_id_service_query[2048] = { '\0' };

			      endpoint =
				xmlGetProp (int_node,
					    REG_ELEMENT_NODEMANAGER_ATTR_ENDPOINT);

			      if (endpoint == NULL || !strcmp (endpoint, ""))
				{
				  pmesg(LOG_WARNING,__FILE__,__LINE__,"Attribute [%s] missing/invalid \n",REG_ELEMENT_NODEMANAGER_ATTR_ENDPOINT);
				  pmesg(LOG_WARNING,__FILE__,__LINE__,"No %s registered in the database \n",APPLICATION_SERVER_NAME);
				}
			      else
				{	// "if endpoint is valid"                             

				  char service_id_str[128] = { '\0' };
				  char service_key[128] = { '\0' };
				  sprintf (buffer_endpoint, "%s", endpoint);

				  slashcursor = &buffer_endpoint[0];
				  while (slashcursor =
					 strchr (slashcursor, ':'))
				    {
				      iteration++;
				      if ((*(++slashcursor)) == '/')
					continue;
				      startcursor = slashcursor;
				      if (slashcursor =
					  strchr (startcursor, '/'))
					{
					  *slashcursor = '\0';
					  app_server_port =
					    atol (startcursor);
					  break;
					}
				    }

				  sprintf (service_key, "%ld:%ld", host_id,
					   app_server_port);

				  if (hashtbl_result =
				      hashtbl_get (hashtbl_services,
						   service_key))
				    {
				      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup HostServices hit! [%s] [%s]\n",service_key, hashtbl_result);
				      service_id = atol (hashtbl_result);
				      success_lookup[0]++;
				    }
				  else
				    {	// add service entry in DB (and hashtable too) without geolocation information
				      // 1) add service
				      char service_id_str[128] = { '\0' };
				      success_lookup[1]++;
				      snprintf (insert_service_query,
						sizeof (insert_service_query),
						QUERY_INSERT_SERVICE_INFO,
						app_server_port,
						APPLICATION_SERVER_NAME,
						organization, support_email,
						host_id);
				      submit_query (conn,
						    insert_service_query);
				      // 2) retrieve service_id
				      // grab service id servizio from port+host
				      snprintf (select_id_service_query,
						sizeof
						(select_id_service_query),
						QUERY_GET_SERVICE_ID,
						app_server_port, host_id);
				      service_id =
					get_foreign_key_value (conn,
							       select_id_service_query);

				      // add new entry into the hashtable
				      sprintf (service_id_str, "%ld",
					       service_id);
				      hashtbl_insert (hashtbl_services,
						      service_key,
						      service_id_str);
				      pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",service_key, service_id_str);
				    }

				  // 3) add service to peer_groups
				  for (i = 0; i < number_of_projects; i++)
				    {
				      char uses_key[128] = { '\0' };
				      sprintf (uses_key, "%ld:%ld",
					       project_ids[i], service_id);
				      if (hashtbl_result =
					  hashtbl_get (hashtbl_uses,
						       uses_key))
					{
					  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup Uses hit! [%s] [%s]\n",uses_key, hashtbl_result);
					  success_lookup[0]++;
					}
				      else
					{
					  char
					    insert_service_2_project_query
					    [2048] = { '\0' };
					  success_lookup[1]++;
					  char project_ids_str[128] =
					    { '\0' };
					  snprintf
					    (insert_service_2_project_query,
					     sizeof
					     (insert_service_2_project_query),
					     QUERY_INSERT_SERVICE_TO_PROJECT,
					     project_ids[i], service_id);
					  submit_query (conn,
							insert_service_2_project_query);
					  // add new entry into the hashtable
					  sprintf (project_ids_str, "%ld",
						   project_ids[i]);
					  hashtbl_insert (hashtbl_uses,
							  uses_key,
							  project_ids_str);
					  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",uses_key, project_ids_str);
					}
				    }	// end for add service to peer groups
				}	// end of "if endpoint is valid
			      // free XML endpoint attribute      
			      xmlFree (endpoint);
			    }	// end of "if internal node is NodeManager ELEMENT"

		/***********************************************************************/

			  // Start "if internal node is a METRICS ELEMENT"       
			  if (!strcmp
			      (int_node->name, REG_ELEMENT_METRICS))
			    {
			      pmesg(LOG_DEBUG,__FILE__,__LINE__,"???????????????????????? METRICS FOUND ?????????????????\n");
			      // loop on internal METRICS elements (DOWNLOADCOUNT,DOWNLOADSIZE,REGUSERS)
			      for (metrics_node = int_node->children;
				   metrics_node;
				   metrics_node = metrics_node->next)
				{
				  if (!strcmp (metrics_node->name, "text"))
				    continue;
			      	  pmesg(LOG_DEBUG,__FILE__,__LINE__,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> METRICS CHILD FOUND !!!!!!!!!\n");
			    	}	// end of "loop on internal METRICS elements (DOWNLOADCOUNT,DOWNLOADSIZE,REGUSERS)"
			    }	// end of "if internal node is a METRICS ELEMENT"

		/***********************************************************************/

			  // Start "if internal node is a GridFTPService ELEMENT"       
			  if (!strcmp
			      (int_node->name, REG_ELEMENT_GRIDFTPSERVICE))
			    {

			      // loop on internal GRIFTPSERVICE elements (CONFIGURATION)
			      for (gridftp_node = int_node->children;
				   gridftp_node;
				   gridftp_node = gridftp_node->next)
				{
				  if (!strcmp (gridftp_node->name, "text"))
				    continue;
				  // Start of "if CONFIGURATION element"
				  if (gridftp_node->type == XML_ELEMENT_NODE
				      &&
				      (!strcmp
				       (gridftp_node->name,
					REG_ELEMENT_CONFIGURATION)))
				    {

				      char insert_service_query[2048] =
					{ '\0' };
				      char select_id_service_query[2048] =
					{ '\0' };
				      char *service_type;
				      char *service_port;
				      int service_id;

				      service_type =
					xmlGetProp (gridftp_node,
						    REG_ELEMENT_CONFIGURATION_ATTR_TYPE);
				      service_port =
					xmlGetProp (gridftp_node,
						    REG_ELEMENT_CONFIGURATION_ATTR_PORT);

				      if (service_type == NULL
					  || !strcmp (service_type, "")
					  || service_port == NULL
					  || !strcmp (service_port, ""))
					{
					  pmesg(LOG_WARNING,__FILE__,__LINE__,"Missing/invalid %s-%s attributes [skip current %s  element]\n",
						   REG_ELEMENT_CONFIGURATION_ATTR_TYPE,
						   REG_ELEMENT_CONFIGURATION_ATTR_PORT,
						   REG_ELEMENT_CONFIGURATION);
					}
				      else
					{	// "if valid serviceType and port"

					  char service_key[128] = { '\0' };

					  sprintf (service_key, "%ld:%ld",
						   host_id,
						   atol (service_port));
					  if (hashtbl_result =
					      hashtbl_get (hashtbl_services,
							   service_key))
					    {
					      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup Services hit! [%s] [%s]\n",service_key,hashtbl_result);
					      service_id =
						atol (hashtbl_result);
					      success_lookup[0]++;
					    }
					  else
					    {	// add service entry in DB (and hashtable too) 
					      // 1) add service
					      char service_id_str[128] =
						{ '\0' };
					      char service_type_gridftp[256] =
						{ '\0' };
					      success_lookup[1]++;
					      sprintf(service_type_gridftp,"GridFTP-%s",service_type);

					      snprintf (insert_service_query,
							sizeof
							(insert_service_query),
							QUERY_INSERT_SERVICE_INFO,
							atol (service_port),
							service_type_gridftp,
							organization,
							support_email,
							host_id);
					      submit_query (conn,
							    insert_service_query);

					      // grab service id servizio from port+host
					      snprintf
						(select_id_service_query,
						 sizeof
						 (select_id_service_query),
						 QUERY_GET_SERVICE_ID,
						 atol (service_port),
						 host_id);
					      service_id =
						get_foreign_key_value (conn,
								       select_id_service_query);
					      // add new service into the hashtable
					      sprintf (service_id_str, "%ld",
						       service_id);
					      hashtbl_insert
						(hashtbl_services,
						 service_key, service_id_str);
					      pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] ** Adding new entry in the hashtable [%s] [%s]\n",service_key,service_id_str);
					    }

					  // add services to projects

					  for (i = 0; i < number_of_projects;
					       i++)
					    {
					      char uses_key[128] = { '\0' };
					      sprintf (uses_key, "%ld:%ld",project_ids[i],service_id);
					      if (hashtbl_result = hashtbl_get (hashtbl_uses,uses_key))
						{
						  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Lookup Uses hit! [%s] [%s]\n",uses_key,hashtbl_result);
						  success_lookup[0]++;
						}
					      else
						{
						  success_lookup[1]++;
						  char project_ids_str[128] = { '\0' };
						  char insert_service_2_project_query[2048] = { '\0' };
						  snprintf(insert_service_2_project_query,sizeof(insert_service_2_project_query),QUERY_INSERT_SERVICE_TO_PROJECT,project_ids[i],
						     service_id);
						  submit_query (conn,insert_service_2_project_query);

						  pmesg(LOG_DEBUG,__FILE__,__LINE__,"ADD SERVICES TO PROJECT: %s\n",insert_service_2_project_query);
						  // add new entry into the hashtable
						  sprintf (project_ids_str,"%ld",project_ids[i]);
						  hashtbl_insert(hashtbl_uses, uses_key,project_ids_str);
						  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[LookupFailed] Adding new entry in the hashtable [%s] [%s]\n",uses_key,project_ids_str);
						}
					    }	// end of for "add services to projects"
					}	// end of "if valid serviceType and port"

				      // free XML CONFIGURATION attributes      
				      xmlFree (service_type);
				      xmlFree (service_port);

				    }	// end of "if CONFIGURATION elements "

				}	// end of loop on internal GRIFTPSERVICE elements (CONFIGURATION, etc.)

			    }	// end of "if internal node is a GridFTPService ELEMENT"

			}	// end of "if internal_node is an ELEMENT node 
		    }		// end of loop on INTERNALNODE elements 
		  if (!geolocation_found)
		    {
		      // adding code here related to GeoIP stuff!!!
		      int code;
		      struct geo_output_struct geo_output;

		      pmesg(LOG_DEBUG,__FILE__,__LINE__,"GeoLocation pieces of info need to be taken from GeoIP library (estimation)\n");

		      if (!esgf_geolookup (node_ip, &geo_output))
			{
			  char update_query[2048] = { '\0' };
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_COUNTRY_CODE=%s]\n",geo_output.country_code);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_REGION=%s]\n",geo_output.region);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_CITY=%s]\n",geo_output.city);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_POSTAL_CODE=%s]\n",geo_output.postal_code);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_LATITUDE=%f]\n",geo_output.latitude);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_LONGITUDE=%f]\n",geo_output.longitude);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_METROCODE=%d]\n",geo_output.metro_code);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"[OUTPUT_AREACODE=%d]\n",geo_output.area_code);
			  snprintf (update_query, sizeof (update_query),QUERY_UPDATE_GEOLOCATION_INFO,geo_output.city, geo_output.latitude,geo_output.longitude, host_id);
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"I got the geolocation info from the DB (estimation)\n");
			  pmesg(LOG_DEBUG,__FILE__,__LINE__,"%s\n",update_query);
			  submit_query (conn, update_query);
			}
		      else
			pmesg(LOG_ERROR,__FILE__,__LINE__,"Esgf-lookup error\n");

		    }		// end of "if !geolocation_found" 
		}		// end of "if a NODE element"

	      // free XML NODE attributes
	      xmlFree (organization);
	      xmlFree (npg_project);
	      xmlFree (node_ip);
	      xmlFree (support_email);
	      xmlFree (node_hostname);
	    }			// end of loop on NODE element

	}			// end of "if a REGISTRATION element
    }				// end of loop on REGISTRATION element

  // releasing memory for hashtables
  if (create_populate_done)
    {
      hashtbl_destroy (hashtbl_projects);
      hashtbl_destroy (hashtbl_hosts);
      hashtbl_destroy (hashtbl_services);
      hashtbl_destroy (hashtbl_uses);
      hashtbl_destroy (hashtbl_rssfeed);
      hashtbl_destroy (hashtbl_hasrssfeed);
      pmesg(LOG_DEBUG,__FILE__,__LINE__,"Releasing memory for hashtables [%d] \n",create_populate_done);
    }

  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Closing transaction and releasing the database tables lock...\n");
  if (manage_database_open_close_transaction(conn,QUERY_CLOSE_TRANSACTION)) {
	  pmesg(LOG_ERROR,__FILE__,__LINE__,"closing transaction FAILED\n");
  	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Hashtables: Hits [%lld] Failure [%lld]\n", success_lookup[0], success_lookup[1]);
  	  pmesg(LOG_DEBUG,__FILE__,__LINE__,"*********** End parsing routine  ************\n");
      	  PQfinish (conn);
	  return -1;
	 }
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"closing transaction OK. Database tables locks released!\n");

  // closing database connection
  PQfinish (conn);
	
  // end parser call
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"Hashtables: Hits [%lld] Failure [%lld]\n", success_lookup[0], success_lookup[1]);
  pmesg(LOG_DEBUG,__FILE__,__LINE__,"*********** End parsing routine  ************\n");

  return 0;
}


int
_automatic_registration_xml_feed (char *registration_xml_file)
{
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  int result;

  /*parse the file and get the DOM */
  doc = xmlReadFile (registration_xml_file, NULL, 0);

  if (doc == NULL)
    {
      pmesg(LOG_ERROR,__FILE__,__LINE__,"Could not parse file %s\n",registration_xml_file);
      return -1;		// error parsing the file       
    }

  /*Get the root element node */
  root_element = xmlDocGetRootElement (doc);

  result = parse_registration_xml_file (root_element);

  /*free the document */
  xmlFreeDoc (doc);

  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser ();

  if (result)
    return -2;			// parser failed to connect to the database
  return 0;			// ok
}
