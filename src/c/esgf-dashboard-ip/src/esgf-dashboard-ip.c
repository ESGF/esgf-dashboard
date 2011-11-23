/*
 * main.c
 *
 *      Author: University of Salento and CMCC 
 */
#include <string.h>
#include <pthread.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/inotify.h>

#include "../include/ping.h"
#include "../include/dbAccess.h"
#include "../include/config.h"

static struct option long_options[] = {
  {"version", 0, 0, 'v'},
  {"help", 0, 0, 'h'},
  {0, 0, 0, 0}
};

static char *USAGE =
  "USAGE: %s [-v | -h] \n"
  "  --v, --version                            \n"
  "  --h, --help                               \n\n"
  "EXAMPLE:\n"
  "esgf-dashboard-ip version => %s -v \n"
  "esgf-dashboard-ip start ===> %s \n";

#define PRINT_USAGE fprintf(stderr, USAGE, argv[0],argv[0], argv[0])
#define VERSION "@version_num@"
//#define XMLPARSER_THREAD_FREQ 60  // release value 
//#define XMLPARSER_THREAD_FREQ 3 // test value

//pthread_barrier_t barr;

void readConfig (void);
int myfree (char *mystring);
void print_all_properties (void);
void print_logs_before_starting (char *esgf_registration_xml_path);
//void get_event (int fd, const char *target);
//void handle_error (int error);

void
print_all_properties (void)
{
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
}

void
print_logs_before_starting (char *esgf_registration_xml_path)
{
  //fprintf (stderr, "POSTGRES_PASSWD value = [%s]\n", POSTGRES_PASSWD);
  fprintf (stderr, "All of the properties have been found\n");
  fprintf (stderr, "Information provider startup...\n");
  fprintf (stderr, "Feeding [%s]\n", esgf_registration_xml_path);
}


/* Allow for 1024 simultanious events */
/* #define BUFF_SIZE ((sizeof(struct inotify_event)+FILENAME_MAX)*1024)

void
get_event (int fd, const char *target)
{
  ssize_t len, i = 0;
  char action[81 + FILENAME_MAX] = { 0 };
  char buff[BUFF_SIZE] = { 0 };

  len = read (fd, buff, BUFF_SIZE);

  while (i < len)
    {
      struct inotify_event *pevent = (struct inotify_event *) &buff[i];
      char action[81 + FILENAME_MAX] = { 0 };

      if (pevent->len)
	strcpy (action, pevent->name);
      else
	strcpy (action, target);

      if (pevent->mask & IN_ACCESS)
	strcat (action, " was read");
      if (pevent->mask & IN_ATTRIB)
	strcat (action, " Metadata changed");
      if (pevent->mask & IN_CLOSE_WRITE)
	strcat (action, " opened for writing was closed");
      if (pevent->mask & IN_CLOSE_NOWRITE)
	strcat (action, " not opened for writing was closed");
      if (pevent->mask & IN_CREATE)
	strcat (action, " created in watched directory");
      if (pevent->mask & IN_DELETE)
	strcat (action, " deleted from watched directory");
      if (pevent->mask & IN_DELETE_SELF)
	strcat (action, "Watched file/directory was itself deleted");
      if (pevent->mask & IN_MODIFY)
	strcat (action, " was modified");
      if (pevent->mask & IN_MOVE_SELF)
	strcat (action, "Watched file/directory was itself moved");
      if (pevent->mask & IN_MOVED_FROM)
	strcat (action, " moved out of watched directory");
      if (pevent->mask & IN_MOVED_TO)
	strcat (action, " moved into watched directory");
      if (pevent->mask & IN_OPEN)
	strcat (action, " was opened");
*/
      /*
         printf ("wd=%d mask=%d cookie=%d len=%d dir=%s\n",
         pevent->wd, pevent->mask, pevent->cookie, pevent->len, 
         (pevent->mask & IN_ISDIR)?"yes":"no");

         if (pevent->len) printf ("name=%s\n", pevent->name);
       */

/*      printf ("%s\n", action);

      i += sizeof (struct inotify_event) + pevent->len;

    }

}*/				/* get_event */

/* ----------------------------------------------------------------- */
/*
void
handle_error (int error)
{
  fprintf (stderr, "Error: %s\n", strerror (error));

}*/				/* handle_error */

/* ----------------------------------------------------------------- */



/* This is our thread function.  It is like main(), but for a thread*/

//void *
int threadFunc (void *arg)
{
  char *esgf_registration_xml_path;

 char target[FILENAME_MAX];
  int result;
  int fd;

  esgf_registration_xml_path = (char *) arg;
  sprintf (target, "%s", esgf_registration_xml_path);

        fprintf(stderr,"ThreadFunction says... calling: %s\n",target);

         // l_type   l_whence  l_start  l_len  l_pid  
         struct flock fl = {F_WRLCK, SEEK_SET,   0,      0,     0 };
         fl.l_pid = getpid();
         fl.l_type = F_RDLCK;

        if ((fd = open(esgf_registration_xml_path, O_RDWR)) == -1) {
         fprintf(stderr,"Open error... skip parsing\n");
	 return -1;
         }

        fprintf(stderr, "Trying to get lock...");
         if (fcntl(fd, F_SETLKW, &fl) == -1) {
         fprintf(stderr,"Lock error... skip parsing\n");
	 close(fd);
	 return -1;
         }

        fprintf(stderr, "Locked\n%s now parsing\n",target);
       
        automatic_registration_xml_feed (target);

        fprintf(stderr,"Trying to release lock...");
        fl.l_type = F_UNLCK;  // set to unlock same region 

        if (fcntl(fd, F_SETLK, &fl) == -1) {
               	fprintf(stderr,"Unlock error... \n"); 
	 	close(fd);
		return -1;
        }
        close(fd);
        fprintf(stderr,"%s Unlocked.\n",target);

  return 0;
}

int
main (int argc, char **argv)
{
  //pthread_t pth;		// this is our thread identifier
  char *esgf_properties = NULL;
  char esgf_properties_default_path[1024] = { '\0' };
  char esgf_registration_xml_path[1024] = { '\0' };
  int res = 0;
  unsigned numHosts = 0;
  struct host *hosts = NULL;
  int counter = 0;
  int c;
  int option_index = 0;
  int iterator = 3;
  int opt_t = 0;
  int mandatory;
  int allprop;

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  //LIBXML_TEST_VERSION

// reading the command line arguments
  while ((c =
	  getopt_long (argc, argv, "v:h", long_options, &option_index)) != -1)
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


  fprintf (stderr, "Starting esgf-dashboard-ip\n");

// reading the ESGF_HOME attribute

  if (ESGF_config_path (&esgf_properties))
    {				// default setting /esg
      strcpy (esgf_properties_default_path, "/esg/"); // setting for release
      esgf_properties =
	(char *) malloc (strlen (esgf_properties_default_path) + 1);
      strcpy (esgf_properties, esgf_properties_default_path);
      fprintf (stderr,
	       "ESGF_HOME attribute not found... setting /esg as default\n");
    }

  fprintf (stderr, "ESGF_HOME = [%s]\n", esgf_properties);

// reading the esgf-dashboard-ip properties

  if (ESGF_properties (esgf_properties, &mandatory, &allprop))
    {
      // check on mandatory properties
      if (mandatory)
	{
	  fprintf
	    (stderr,
	     "Please note that %d DB properties are missing in the esgf.properties file. Please check! Exit\n",
	     mandatory);
	  myfree (esgf_properties);
	  myfree (POSTGRES_HOST);
	  myfree (POSTGRES_DB_NAME);
	  myfree (POSTGRES_USER);
	  return 0;
	}
      // check on non-mandatory properties
      if (allprop)
	fprintf
	  (stderr,
	   "Please note that %d non-mandatory properties are missing in the esgf.properties file. Default have been loaded\n",
	   allprop);
    }

//  print_all_properties ();

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

  sprintf (esgf_registration_xml_path, "%s/registration.xml",
	   REGISTRATION_XML_PATH);

  print_logs_before_starting (esgf_registration_xml_path);

  // start thread 
  //fprintf (stderr, "Creating the registration.xml thread\n");
  //pthread_create (&pth, NULL, threadFunc, esgf_registration_xml_path);
  //sleep(10);

  fprintf (stderr, "Starting the forever loop for the metrics collector\n");

  counter = 0;
  while (iterator--)
  //while (1)
    {
      // Now calling the automatic registration_xml_feed into the parser
      //automatic_registration_xml_feed (esgf_registration_xml_path);
      threadFunc (esgf_registration_xml_path);
      if (counter == 0)
	{
	  if (hosts)
	    free (hosts);
	  hosts = loadHosts (&numHosts);
	}
      if (numHosts != 0 && hosts != NULL)
	{
	  fprintf (stderr, "Host/services found. Let's check them...\n");
	  pingHostList (hosts, numHosts);
	  writeResults (hosts, numHosts);
	  //counter = (counter + 1) % HOSTS_LOADING_SPAN;
	  fprintf (stderr,
		   "Metrics have been collected.\nNow waiting for %d sec\n",
		   PING_SPAN);
	  sleep (PING_SPAN);
	}
      else
	{
	  fprintf (stderr, "Host/services not found...\n");
	  fprintf (stderr, "Waiting for %d sec\n", PING_SPAN_NO_HOSTS);
	  sleep (PING_SPAN_NO_HOSTS);
	}
    }				// forever loop end

// end thread
//  fprintf (stderr,
//	   "Parser thread joins the main program before existing... waiting for it\n");
//  if (pthread_join (pth, NULL))
//      fprintf(stderr,"pthread_join error");

    // freeing space
  fprintf (stderr, "Releasing memory\n");
  if (hosts)
    free (hosts);

  myfree (esgf_properties);
  myfree (POSTGRES_HOST);
  myfree (POSTGRES_DB_NAME);
  myfree (POSTGRES_USER);
  myfree (POSTGRES_PASSWD);
  myfree (REGISTRATION_XML_PATH);

  fprintf (stderr, "esgf-dashboard-ip end\n");

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

  sprintf (esg_env, "/etc/esg.env");			// path for release

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
    {
  	fclose (file);
	return -1;
	}

      if ((fscanf (file, "%s", buffer)) == EOF)	// now reading ATTRIBUTE=VALUE
    {
  	fclose (file);
	return -1;
	}

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

  fprintf (stderr, "%s\n", esgf_properties_filename);
  FILE *file = fopen (esgf_properties_filename, "r");

  if (file == NULL)		// /esg/config/esgf.properties not found
    return -1;

  // setting default values for non-mandatory properties 

  CONNECTION_TIMEOUT = 1000000;
  THREAD_OPEN_MAX = 20;
  PING_SPAN = 295;
  PING_SPAN_NO_HOSTS = 60;
  HOSTS_LOADING_SPAN = 120;
				// TO DO: to add the node.manager.app.home as a mandatory property
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
  sprintf (esgf_passwd_filename, "/%s/config/.esg_pg_pass", esgf_passwd_path);

  fprintf (stderr, "%s\n", esgf_passwd_filename);
  FILE *file = fopen (esgf_passwd_filename, "r");

  if (file == NULL)		// /esg/config/.esg_pg_pass not found
    return -1;

  if ((fscanf (file, "%s", buffer)) == EOF)	// now reading passwd TO DO check on mem leak (closing file) 
    return -1;			// no password found 

  strcpy (POSTGRES_PASSWD = (char *) malloc (strlen (buffer) + 1), buffer);

  fclose (file);
  return 0;
}
