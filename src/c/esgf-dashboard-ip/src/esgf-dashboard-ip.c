/*
 * main.c
 *
 *      Author: University of Salento and CMCC 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

static char                                   *USAGE =
    "USAGE:\n"
    "  --v, --version                            Get esgf-dashboard-ip version\n"
    "  --h, --help                               Show usage\n\n"
    "EXAMPLE:\n"
    "%s -v \n";

#define PRINT_USAGE fprintf(stderr, USAGE, argv[0])
#define VERSION "0.0.1"

int
main (int argc, char **argv)
{

  char *esgf_properties = NULL;
  char esgf_properties_default_path[256] = { '\0' };
  int res = 0;
  unsigned numHosts = 0;
  struct host *hosts = NULL;
  int counter = 0;

  int c;
  int option_index = 0;
  int opt_t = 0;

// reading the command line arguments

  while ((c = getopt_long(argc, argv, "v:h", long_options, &option_index)) != -1)
  {
    switch (c)
    {
      case 'v':
	fprintf(stdout,"%s\n",VERSION);
	return 0;
      case 'h':
 	PRINT_USAGE;	
	return 0;
      case '?':
	fprintf(stdout,"Bad Arguments\n");
 	PRINT_USAGE;	
	return 0;
      default:
	fprintf(stdout,"Bad Arguments\n");
 	PRINT_USAGE;	
	return 0;
    }
  }

fprintf(stdout,"Starting esgf-dashboard-ip\n");

// reading the ESGF_HOME attribute

  if (ESGF_config_path (&esgf_properties))
    {				// default setting /esg
      strcpy (esgf_properties_default_path, "/esg/");
      //strcpy (esgf_properties_default_path, "/export/fiore2/esg/");
      esgf_properties =
	(char *) malloc (strlen (esgf_properties_default_path) + 1);
      strcpy (esgf_properties, esgf_properties_default_path);
    }

  printf ("ESGF_HOME = [%s]\n", esgf_properties);

// reading the esgf-dashboard-ip properties

  if (ESGF_properties (esgf_properties))
    {
      printf
	("Some DB properties are missing in the esgf.properties file. Please check!");
      myfree (esgf_properties);
      myfree (POSTGRES_HOST);
      myfree (POSTGRES_DB_NAME);
      myfree (POSTGRES_USER);
      return 0;
    }

  printf ("POSTGRES_HOST value = [%s]\n", POSTGRES_HOST);
  printf ("POSTGRES_DB_NAME value = [%s]\n", POSTGRES_DB_NAME);
  printf ("POSTGRES_USER value = [%s]\n", POSTGRES_USER);
  printf ("POSTGRES_PORT_NUMBER value = [%d]\n", POSTGRES_PORT_NUMBER);
  printf ("CONNECTION_TIMEOUT = [%d]\n", CONNECTION_TIMEOUT);
  printf ("THREAD_OPEN_MAX = [%d]\n", THREAD_OPEN_MAX);
  printf ("PING_SPAN = [%d]\n", PING_SPAN);
  printf ("HOSTS_LOADING_SPAN = [%d]\n", HOSTS_LOADING_SPAN);

// reading the postgres password

  if ((ESGF_passwd (esgf_properties)))
    {
      printf
	("Some error occurred while opening the .esg_pg_pass file Please check!");
      myfree (esgf_properties);
      myfree (POSTGRES_HOST);
      myfree (POSTGRES_DB_NAME);
      myfree (POSTGRES_USER);
      return 0;
    }

  printf ("POSTGRES_PASSWD value = [%s]\n", POSTGRES_PASSWD);

  printf ("All of the properties have been found\n");
  printf ("Information provider startup...\n");

  while (1)
    {
      if (counter == 0)
	{
	  free (hosts);
	  hosts = loadHosts (&numHosts);
	  if (numHosts == 0 || hosts == NULL)
	    break;
	}
      pingHostList (hosts, numHosts);
      writeResults (hosts, numHosts);
      counter = (counter + 1) % HOSTS_LOADING_SPAN;
      sleep (PING_SPAN);
    }

  // freeing space

  myfree (esgf_properties);
  myfree (POSTGRES_HOST);
  myfree (POSTGRES_DB_NAME);
  myfree (POSTGRES_USER);
  myfree (POSTGRES_PASSWD);

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

  sprintf (esg_env, "/etc/esg.env");
  //sprintf (esg_env, "/export/fiore2/etc/esg.env");

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
ESGF_properties (char *esgf_properties_path)
{

  char esgf_properties_filename[256] = { '\0' };
  char *position;
  int notfound;

  sprintf (esgf_properties_filename,
	   "/%s/config/esgf.properties", esgf_properties_path);
  //sprintf (esgf_properties_filename,
//	   "/export/fiore2/%s/config/esgf.properties", esgf_properties_path);

  FILE *file = fopen (esgf_properties_filename, "r");

  if (file == NULL)		// /esg/config/esgf.properties not found
    return -1;

  // setting default values for non-mandatory properties 

  CONNECTION_TIMEOUT = 1000000;
  THREAD_OPEN_MAX = 20;
  PING_SPAN = 295;
  HOSTS_LOADING_SPAN = 120;

  notfound = 4; // number of mandatory properties to be retrieved from the esgf.properties file
  while (notfound)
    {
      char buffer[256] = { '\0' };
      char value_buffer[256] = { '\0' };

      if ((fscanf (file, "%s", buffer)) == EOF)	// now reading ATTRIBUTE=VALUE
	return (notfound);	// it provides the number of missing mandatory properties 

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
	      notfound--;
	    }
	  if (!(strcmp (buffer, "db.database")))
	    {
	      strcpy (POSTGRES_DB_NAME =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      notfound--;
	    }
	  if (!(strcmp (buffer, "db.port")))
	    {
	      POSTGRES_PORT_NUMBER = atoi (value_buffer);
	      notfound--;
	    }
	  if (!(strcmp (buffer, "db.user")))
	    {
	      strcpy (POSTGRES_USER =
		      (char *) malloc (strlen (value_buffer) + 1),
		      value_buffer);
	      notfound--;
	    }
	  if (!(strcmp (buffer, "esgf.ip.connection_timeout")))
	    CONNECTION_TIMEOUT = atoi (value_buffer);
	  if (!(strcmp (buffer, "esgf.ip.thread_open_max")))
	    THREAD_OPEN_MAX = atoi (value_buffer);
	  if (!(strcmp (buffer, "esgf.ip.ping_span")))
	    PING_SPAN = atoi (value_buffer);
	  if (!(strcmp (buffer, "esgf.ip.hosts_loading_span")))
	    HOSTS_LOADING_SPAN = atoi (value_buffer);
	}
    }

  fclose (file);
  return 0;
}



int
ESGF_passwd (char *esgf_passwd_path)
{

  char esgf_passwd_filename[256] = { '\0' };
  char *position;
  int notfound;
  char buffer[256] = { '\0' };

  sprintf (esgf_passwd_filename, "/%s/config/.esg_pg_pass",
	   esgf_passwd_path);
 // sprintf (esgf_passwd_filename, "/export/fiore2/%s/config/.esg_pg_pass",
//	   esgf_passwd_path);

  FILE *file = fopen (esgf_passwd_filename, "r");

  if (file == NULL)		// /esg/config/.esg_pg_pass not found
    return -1;

  if ((fscanf (file, "%s", buffer)) == EOF)	// now reading pg_passwd 
    return -1;			// no password found 

  strcpy (POSTGRES_PASSWD = (char *) malloc (strlen (buffer) + 1), buffer);

  fclose (file);
  return 0;
}
