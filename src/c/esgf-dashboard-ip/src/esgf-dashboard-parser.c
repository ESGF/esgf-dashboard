/*fare il merge di questa funzione nel file dbAccess.c, dove c'e' gia' il collegamento con il DB e tutte le variabili sono ok */

#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "libpq-fe.h"
//#include "../include/ping.h"
//#include "../include/dbAccess.h"
#include "../include/config.h"

// "Node" ELEMENTS 
#define REG_ELEMENT_REGISTRATION	"Registration"
#define REG_ELEMENT_NODE		"Node"
#define REG_ELEMENT_GEOLOCATION		"GeoLocation"
#define REG_ELEMENT_NODEMANAGER		"NodeManager"
#define REG_ELEMENT_GRIDFTPSERVICE	"GridFTPService"
#define REG_ELEMENT_CONFIGURATION	"Configuration"

// "Node" ATTRIBUTES
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

// QUERY_INSERT_PROJECT adds a new project (peer group) in the database
#define QUERY_INSERT_PROJECT  "INSERT into esgf_dashboard.project_dash(name,description) values('%s','%s');"

// QUERY QUERY_INSERT_GEOLOCATION_INFO adds geolocation info to an existing host (P2P node) in the database
#define QUERY_INSERT_GEOLOCATION_INFO  "UPDATE esgf_dashboard.host set city='%s',latitude=%s,longitude=%s where id=%ld;"

// QUERY QUERY_INSERT_HOST adds a new host (P2P node) in the database
#define QUERY_INSERT_NEW_HOST  "INSERT into esgf_dashboard.host(name,ip) values('%s','%s');"

// QUERY QUERY_GET_PROJECT_ID retrieves the id value of a specific project (peer group) 
#define QUERY_GET_PROJECT_ID "SELECT id from esgf_dashboard.project_dash where name='%s';"

// QUERY QUERY_GET_HOST_ID retrieves the id value of a specific host (P2P node) 
#define QUERY_GET_HOST_ID "SELECT id from esgf_dashboard.host where ip='%s';"

// QUERY QUERY_GET_SERVICE_ID retrieves the id value of a specific service 
#define QUERY_GET_SERVICE_ID "SELECT id from esgf_dashboard.service_instance where port=%ld and idhost=%ld;"

// QUERY QUERY_PROJECT_AUTHORIZATION, authorize project (peer group) on guest user in the database
#define QUERY_PROJECT_AUTHORIZATION  "INSERT into esgf_dashboard.join1(iduser,idproject) values(1,%ld);"

// QUERY QUERY_INSERT_SERVICE_INFO, authorize project (peer group) on guest user in the database
#define QUERY_INSERT_SERVICE_INFO  "INSERT into esgf_dashboard.service_instance(port,name,institution,mail_admin,idhost) values(%ld,'GridFTP_%s','%s','%s',%ld);"

// QUERY QUERY_INSERT_SERVICE_TO_PROJECT binds a service to a project (peer group) in the database
#define QUERY_INSERT_SERVICE_TO_PROJECT  "INSERT into esgf_dashboard.uses(idproject,idserviceinstance) values(%ld,%ld);"

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

long int
get_foreign_key_value (PGconn * conn, char *query)
{
  PGresult *res;
  long int fk;
  fprintf (stdout, "Query: %s\n", query);

  res = PQexec (conn, query);
  if ((!res) || (PQresultStatus (res) != PGRES_TUPLES_OK))
    {
      fprintf (stdout, "SELECT command did not return tuples properly\n");
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
  fprintf (stdout, "The fk is %ld\n", fk);
  return fk;
}



int
submit_query (PGconn * conn, char *query)
{
  PGresult *res;
  fprintf (stdout, "Query: %s\n", query);

  res = PQexec (conn, query);

  if ((!res) || (PQresultStatus (res) != PGRES_COMMAND_OK))
    {
      fprintf (stdout, "Insert query failed\n");
      PQclear (res);
      return -1;
    }
  PQclear (res);
  return 0;
}


int
parse_registration_xml_file (xmlNode * a_node)
{
  xmlNode *cur_node = NULL;
  xmlNode *node_node = NULL;
  xmlNode *int_node = NULL;
  xmlNode *gridftp_node = NULL;
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

  snprintf (conninfo, sizeof (conninfo),
	    "host=%s port=%d dbname=%s user=%s password=%s", POSTGRES_HOST,
	    POSTGRES_PORT_NUMBER, POSTGRES_DB_NAME, POSTGRES_USER,
	    POSTGRES_PASSWD);

  fprintf (stdout, "Open connection to: %s\n", conninfo);
  conn = PQconnectdb ((const char *) conninfo);
  if (PQstatus (conn) != CONNECTION_OK)
    {
      fprintf (stdout, "Connection to database failed: %s",
	       PQerrorMessage (conn));
      PQfinish (conn);
      return -1;
    }

  // "Registration" iteration 
  for (cur_node = a_node; cur_node; cur_node = cur_node->next)	// loop on REGISTRATION elements
    {
      if (!strcmp (cur_node->name, "text"))
	continue;
      //printf ("Loop on REGISTRATION: %d\n", ++iter);

      if (cur_node->type == XML_ELEMENT_NODE
	  && (!strcmp (cur_node->name, REG_ELEMENT_REGISTRATION)))
	{
	  fprintf (stdout, "Element->name: %s\n", cur_node->name);

	  // Loop on NODE elements

	  for (node_node = cur_node->children; node_node; node_node = node_node->next)	// loop on NODE elements
	    {
	      char *organization;
	      char *support_email;
	      char *npg_project;
	      char *node_ip;
	      char *node_hostname;
	      int number_of_projects;
	      long int project_ids[1024] = { 0 };
	      long int service_ids[1024] = { 0 };
	      long int host_id = 0;
	      int geolocation_found;
	      int i;

	      if (!strcmp (node_node->name, "text"))
		continue;

	      if (node_node->type == XML_ELEMENT_NODE && (!strcmp (node_node->name, REG_ELEMENT_NODE)))	// if a NODE element 
		{
		  char insert_new_host_query[2048] = { '\0' };
		  char select_id_host_query[2048] = { '\0' };
		  // get NODE attributes values

		  organization =
		    xmlGetProp (node_node, REG_ATTR_NODE_ORGANIZATION);

		  npg_project =
		    xmlGetProp (node_node, REG_ATTR_NODE_NODEPEERGROUP);

		  node_ip = xmlGetProp (node_node, REG_ATTR_NODE_NODEIP);

		  support_email =
		    xmlGetProp (node_node, REG_ATTR_NODE_SUPPORTEMAIL);

		  node_hostname =
		    xmlGetProp (node_node, REG_ATTR_NODE_NODEHOSTNAME);

		  // print attributes values
		  fprintf (stdout, "Organization attribute: %s\n",
			   organization);
		  fprintf (stdout, "PeerGroups attribute: %s\n", npg_project);
		  fprintf (stdout, "Hostname attribute: %s\n", node_hostname);
		  fprintf (stdout, "IP attribute: %s\n", node_ip);

		  // adding host entry in the Host table without geolocation information
		  snprintf (insert_new_host_query,
			    sizeof (insert_new_host_query),
			    QUERY_INSERT_NEW_HOST, node_hostname, node_ip);
		  submit_query (conn, insert_new_host_query);

		  snprintf (select_id_host_query,
			    sizeof (select_id_host_query), QUERY_GET_HOST_ID,
			    node_ip);
		  host_id =
		    get_foreign_key_value (conn, select_id_host_query);

		  fprintf (stdout, "Host %s | id : %ld\n", node_ip, host_id);

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
		      fprintf (stdout, "PeerGroup %s\n", cursor_buf);

		      // add PeerGroup to project_dash table
		      snprintf (insert_query, sizeof (insert_query),
				QUERY_INSERT_PROJECT, cursor_buf, cursor_buf);
		      submit_query (conn, insert_query);

		      // get project ID (servira' successivamente)
		      snprintf (select_query, sizeof (select_query),
				QUERY_GET_PROJECT_ID, cursor_buf);
		      project_ids[number_of_projects++] =
			get_foreign_key_value (conn, select_query);

		      // add user to peer group (authorization) TO BE ADDED
		      snprintf (authorization_query,
				sizeof (authorization_query),
				QUERY_PROJECT_AUTHORIZATION,
				project_ids[number_of_projects - 1]);
		      submit_query (conn, authorization_query);
		    }
		  while (position);

		  for (i = 0; i < number_of_projects; i++)
		    fprintf (stdout, "Valore %d %ld\n", i, project_ids[i]);

		  fprintf (stdout, "Element->name: %s\n", node_node->name);

		  geolocation_found = 0;

		  // loop on internal NODE elements (CA, FrontEnd, etc.)
		  for (int_node = node_node->children; int_node;
		       int_node = int_node->next)
		    {
		      if (!strcmp (int_node->name, "text"))
			continue;

		      //printf ("Loop on INTERNALNODE: %d\n", ++iter3);

		      if (int_node->type == XML_ELEMENT_NODE)	// aggiungere switch all'interno 
			{
			  // Start of "if internal node is GeoLocation ELEMENT"
			  if (!strcmp
			      (int_node->name, REG_ELEMENT_GEOLOCATION))
			    {
			      char insert_query[2048] = { '\0' };
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
			      snprintf (insert_query, sizeof (insert_query),
					QUERY_INSERT_GEOLOCATION_INFO, city,
					latitude, longitude, host_id);
			      submit_query (conn, insert_query);

			      // free XML GEOLOCATION attributes      
			      xmlFree (latitude);
			      xmlFree (longitude);
			      xmlFree (city);
			    }	// end of "if internal node is GeoLocation ELEMENT"

			  // Start of "if internal node is NodeManager ELEMENT"
			  if (!strcmp
			      (int_node->name, REG_ELEMENT_NODEMANAGER))
			    {
			      char insert_query[2048] = { '\0' };
			      char *endpoint;
			      char *slashcursor;
			      char *startcursor;
			      char buffer_endpoint[2048] = { '\0' };
			      long int app_server_port=80;
			      int iteration=0;

			      endpoint =
				xmlGetProp (int_node,
					    REG_ELEMENT_NODEMANAGER_ATTR_ENDPOINT);

			      sprintf (buffer_endpoint, "%s", endpoint);
			   	fprintf(stdout,"%s\n",buffer_endpoint);
			      slashcursor = &buffer_endpoint[0];
			      while (slashcursor =
				     strchr (slashcursor, ':'))
				{
				iteration++;
				fprintf(stdout,"Iteration %d\n",iteration);
				  if ((*(++slashcursor)) == '/') 
					continue;
				  startcursor = slashcursor;
				  if (slashcursor = strchr (startcursor, '/'))
				    {
				      *slashcursor = '\0';
				      app_server_port = atol (startcursor);
				      break;
				    }
				}
				      fprintf (stdout,
					       "Found port for Application server %ld\n",
					       app_server_port);
			      // TO BE added
			      // 1) add service
			      // 2) retrieve service_id
			      // 3) add service to peer_groups

			      // free XML endpoint attribute      
			      xmlFree (endpoint);
			    }	// end of "if internal node is NodeManager ELEMENT"

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

				      snprintf (insert_service_query,
						sizeof (insert_service_query),
						QUERY_INSERT_SERVICE_INFO,
						atol (service_port),
						service_type, organization,
						support_email, host_id);
				      submit_query (conn,
						    insert_service_query);

				      // grab service id servizio from port+host
				      snprintf (select_id_service_query,
						sizeof
						(select_id_service_query),
						QUERY_GET_SERVICE_ID,
						atol (service_port), host_id);
				      service_id =
					get_foreign_key_value (conn,
							       select_id_service_query);

				      fprintf (stdout,
					       "Service %s | id : %ld\n",
					       service_type, service_id);

				      // add services to projects
				      for (i = 0; i < number_of_projects; i++)
					{
					  char
					    insert_service_2_project_query
					    [2048] = { '\0' };
					  snprintf
					    (insert_service_2_project_query,
					     sizeof
					     (insert_service_2_project_query),
					     QUERY_INSERT_SERVICE_TO_PROJECT,
					     project_ids[i], service_id);
					  submit_query (conn,
							insert_service_2_project_query);
					}

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
		      fprintf (stdout,
			       "GeoLocation pieces of info need to be taken from GeoIP library (estimation)");
		      // adding code here related to GeoIP stuff!!!
		    }
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
  // closing database connection
  PQfinish (conn);
  return 0;
}


int
automatic_registration_xml_feed (char *registration_xml_file)
{
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  int result;

  /*parse the file and get the DOM */
  doc = xmlReadFile (registration_xml_file, NULL, 0);

  if (doc == NULL)
    {
      fprintf (stdout, "error: could not parse file %s\n",
	       registration_xml_file);
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

/*int
main (int argc, char **argv)
{

  if (argc != 2)
    return (1);
*/
  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
/*  LIBXML_TEST_VERSION 
  
  fprintf (stdout, "return code %d\n", automatic_registration_xml_feed (argv[1]));

  return 0;
}*/
