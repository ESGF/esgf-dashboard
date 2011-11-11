/*
 * main.c
 *
 *      Author: University of Salento and CMCC 
 */
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "../include/ping.h"
#include "../include/dbAccess.h"
#include "../include/config.h"

void readConfig (void);
int myfree (char *mystring);


static struct option long_options[] = {
  {"version", 0, 0, 'v'},
  {"help", 0, 0, 'h'},
  {0, 0, 0, 0}
};

static char *USAGE =
  "USAGE:\n"
  "  --v, --version                            Get esgf-dashboard-ip version\n"
  "  --h, --help                               Show usage\n\n"
  "EXAMPLE:\n" "%s -v \n";

#define PRINT_USAGE fprintf(stderr, USAGE, argv[0])
#define VERSION "@version_num@"

int
main (int argc, char **argv)
{

  char *esgf_properties = NULL;
  char esgf_properties_default_path[1024] = { '\0' };
  char esgf_registration_xml_path[1024] = { '\0' };
  int res = 0;
  unsigned numHosts = 0;
  struct host *hosts = NULL;
  int counter = 0;

  int c;
  int option_index = 0;
  int iterator=1;
  int opt_t = 0;
  int mandatory;
  int allprop;

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION
// reading the command line arguments
    while ((c =
	    getopt_long (argc, argv, "v:h", long_options,
			 &option_index)) != -1)
    {
      switch (c)
	{
	case 'v':
	  fprintf (stdout, "%s\n", VERSION);
	  return 0;
	case 'h':
	  PRINT_USAGE;
	  return 0;
	case '?':
	  fprintf (stdout, "Bad Arguments\n");
	  PRINT_USAGE;
	  return 0;
	default:
	  fprintf (stdout, "Bad Arguments\n");
	  PRINT_USAGE;
	  return 0;
	}
    }

  fprintf (stderr,"Starting esgf-dashboard-ip\n");

// reading the ESGF_HOME attribute

  if (ESGF_config_path (&esgf_properties))
    {				// default setting /esg
      //strcpy (esgf_properties_default_path, "/esg/");
      //strcpy (esgf_properties_default_path, "/export/fiore2/esg/");
      strcpy (esgf_properties_default_path, "/root/workspace/install/esg/");
      esgf_properties =
	(char *) malloc (strlen (esgf_properties_default_path) + 1);
      strcpy (esgf_properties, esgf_properties_default_path);
      fprintf (stderr,
	       "ESGF_HOME attribute not found... setting /esg as default");
    }

  fprintf (stderr, "ESGF_HOME = [%s]\n", esgf_properties);

// reading the esgf-dashboard-ip properties

  if (ESGF_properties (esgf_properties, &mandatory, &allprop))
    {
      if (mandatory)
	{
	  fprintf
	    (stderr,
	     "Please note tha %d DB properties are missing in the esgf.properties file. Please check! Exit\n",
	     mandatory);
	  myfree (esgf_properties);
	  myfree (POSTGRES_HOST);
	  myfree (POSTGRES_DB_NAME);
	  myfree (POSTGRES_USER);
	  return 0;
	}
      if (allprop)
	fprintf
	  (stderr,
	   "Please note that %d non-mandatory properties are missing in the esgf.properties file. Default have been loaded\n",
	   allprop);
    }

  fprintf (stderr, "POSTGRES_HOST value = [%s]\n", POSTGRES_HOST);
  fprintf (stderr, "POSTGRES_DB_NAME value = [%s]\n", POSTGRES_DB_NAME);
  fprintf (stderr, "POSTGRES_USER value = [%s]\n", POSTGRES_USER);
  fprintf (stderr, "POSTGRES_PORT_NUMBER value = [%d]\n",
	   POSTGRES_PORT_NUMBER);
  fprintf (stderr, "CONNECTION_TIMEOUT = [%d]\n", CONNECTION_TIMEOUT);
  fprintf (stderr, "THREAD_OPEN_MAX = [%d]\n", THREAD_OPEN_MAX);
  fprintf (stderr, "PING_SPAN = [%d]\n", PING_SPAN);
  fprintf (stderr, "PING_SPAN_NO_HOSTS = [%d]\n", PING_SPAN_NO_HOSTS);
  fprintf (stderr, "HOSTS_LOADING_SPAN = [%d]\n", HOSTS_LOADING_SPAN);
  fprintf (stderr, "REGISTRATION_XML_PATH = [%s]\n", REGISTRATION_XML_PATH);

// reading the postgres password

  if ((ESGF_passwd (esgf_properties)))
    {
      fprintf
	(stderr,
	 "Some error occurred while opening the .esgf_pass file Please check!\n");
      myfree (esgf_properties);
      myfree (POSTGRES_HOST);
      myfree (POSTGRES_DB_NAME);
      myfree (POSTGRES_USER);
      myfree (REGISTRATION_XML_PATH);
      return 0;
    }

  fprintf (stderr, "POSTGRES_PASSWD value = [%s]\n", POSTGRES_PASSWD);

  fprintf (stderr, "All of the properties have been found\n");
  fprintf (stderr, "Information provider startup...\n");

  sprintf (esgf_registration_xml_path, "%s/registration.xml",
	   REGISTRATION_XML_PATH);
  fprintf (stderr, "Feeding %s\n", esgf_registration_xml_path);

  counter = 0;
  while (iterator--)
  //while (1)
    {
      //load information from registration.xml();       
      automatic_registration_xml_feed (esgf_registration_xml_path);
      	
      if (counter == 0)
	{
	  fprintf(stderr,"*** Reloading host configuration ***\n");
	  if (hosts)
	    free (hosts);
	  // reload list of hosts/services
	  hosts = loadHosts (&numHosts);
	}
      if (numHosts != 0 && hosts != NULL)
	{
	  fprintf (stderr, "Host/services found. Let's check them...\n");
	  pingHostList (hosts, numHosts);
	  writeResults (hosts, numHosts);
	  counter = (counter + 1) % HOSTS_LOADING_SPAN;
	  fprintf (stderr, "Metrics have been collected. Now waiting for %d sec\n", PING_SPAN);
	  sleep (PING_SPAN);
	}
      else
	{
	  fprintf (stderr, "Host/services not found...\n");
	  fprintf (stderr, "Waiting for %d sec\n", PING_SPAN_NO_HOSTS);
	  sleep (PING_SPAN_NO_HOSTS);
	}
    }				// forever loop end

  // freeing space

  fprintf(stderr,"Releasing memory\n"); 
  if (hosts)
	free (hosts);

  myfree (esgf_properties);
  myfree (POSTGRES_HOST);
  myfree (POSTGRES_DB_NAME);
  myfree (POSTGRES_USER);
  myfree (POSTGRES_PASSWD);
  myfree (REGISTRATION_XML_PATH);
  fprintf(stderr,"END\n"); 

  return 0;
}

int
myfree (char *mystring)
{
  if (mystring)
    free (mystring);
  return 0;
}

int
ESGF_config_path (char **esgf_properties_pointer)
{

  char esg_env[256] = { '\0' };
  char *position;
  int notfound;

  //sprintf (esg_env, "/etc/esg.env");
  sprintf (esg_env, "/export/fiore2/etc/esg.env");

  FILE *file = fopen (esg_env, "r");

  if (file == NULL)		// /etc/esg.env not found
    return -1;

  notfound = 1;
  while (notfound)
    {
      char buffer[256] = { '\0' };
      char export_buffer[256] = { '\0' };
      char value_buffer[256] = { '\0' };

      if ((fscanf (file, "%s", export_buffer)) == EOF)	// skip EXPORT
	return -1;

      if ((fscanf (file, "%s", buffer)) == EOF)	// now reading ATTRIBUTE=VALUE
	return -1;

      position = strchr (buffer, '=');
      if (position != NULL)	// the '=' has been found
	{
	  strcpy (value_buffer, position + 1);	// now value_buffer stores the VALUE        
	  *position = '\0';	// now buffer stores the ATTRIBUTE       

	  if (!(strcmp (buffer, "ESGF_HOME")))
	    {
	      *esgf_properties_pointer =
		(char *) malloc (strlen (value_buffer) + 1);
	      strcpy (*esgf_properties_pointer, value_buffer);
	      notfound = 0;
	    }
	}
    }
  fclose (file);
  return (notfound);
}


int
ESGF_properties (char *esgf_properties_path, int *mandatory_properties,
		 int *notfound)
{

  char esgf_properties_filename[256] = { '\0' };
  char *position;

// this line is ok for local and production env
  sprintf (esgf_properties_filename,
	   "/%s/config/esgf.properties", esgf_properties_path);

  fprintf (stderr,"%s\n",esgf_properties_filename);
  FILE *file = fopen (esgf_properties_filename, "r");

  if (file == NULL)		// /esg/config/esgf.properties not found
    return -1;

  // setting default values for non-mandatory properties 

  CONNECTION_TIMEOUT = 1000000;
  THREAD_OPEN_MAX = 20;
  PING_SPAN = 295;
  PING_SPAN_NO_HOSTS = 60;
  HOSTS_LOADING_SPAN = 120;

  *notfound = 10;		// number of total properties to be retrieved from the esgf.properties file
  *mandatory_properties = 4;	// number of mandatory properties to be retrieved from the esgf.properties file

  while (notfound)
    {
      char buffer[256] = { '\0' };
      char value_buffer[256] = { '\0' };

      if ((fscanf (file, "%s", buffer)) == EOF)	// now reading ATTRIBUTE=VALUE
    	{
  	fclose (file);
	return (-1);		// not all of the properties are there 
	    }
      position = strchr (buffer, '=');
      if (position != NULL)	// the '=' has been found
	{
	  strcpy (value_buffer, position + 1);	// now value_buffer stores the VALUE        
	  *position = '\0';	// now buffer stores the ATTRIBUTE       

	  if (!(strcmp (buffer, "db.host")))
	    {
	      strcpy (POSTGRES_HOST =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*mandatory_properties)--;
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "db.database")))
	    {
	      strcpy (POSTGRES_DB_NAME =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "db.port")))
	    {
	      POSTGRES_PORT_NUMBER = atoi (value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "db.user")))
	    {
	      strcpy (POSTGRES_USER =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	      (*mandatory_properties)--;
	    }
	  if (!(strcmp (buffer, "node.manager.app.home")))
	    {
	      strcpy (REGISTRATION_XML_PATH =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.connection_timeout")))
	    {
	      CONNECTION_TIMEOUT = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.thread_open_max")))
	    {
	      THREAD_OPEN_MAX = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.ping_span")))
	    {
	      PING_SPAN = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.ping_span_no_hosts")))
	    {
	      PING_SPAN_NO_HOSTS = atoi (value_buffer);
	      (*notfound)--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.hosts_loading_span")))
	    {
	      HOSTS_LOADING_SPAN = atoi (value_buffer);
	      (*notfound)--;
	    }
	}
    }

  fclose (file);
  return 0;			// all of the properties have been found
}



int
ESGF_passwd (char *esgf_passwd_path)
{

  char esgf_passwd_filename[256] = { '\0' };
  char *position;
  int notfound;
  char buffer[256] = { '\0' };

// this line is ok for local and production env
  sprintf (esgf_passwd_filename, "/%s/config/.esgf_pass", esgf_passwd_path);

  fprintf (stderr,"%s\n",esgf_passwd_filename);
  FILE *file = fopen (esgf_passwd_filename, "r");

  if (file == NULL)		// /esg/config/.esg_pg_pass not found
    return -1;

  if ((fscanf (file, "%s", buffer)) == EOF)	// now reading pg_passwd 
    return -1;			// no password found 

  strcpy (POSTGRES_PASSWD = (char *) malloc (strlen (buffer) + 1), buffer);

  fclose (file);
  return 0;
}
