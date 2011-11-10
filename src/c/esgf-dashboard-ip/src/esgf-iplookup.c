/* 
esgf-geoiplookup.c 
*/

#include "GeoIP.h"
#include "GeoIPCity.h"
#include "GeoIP_internal.h"
#include "../include/config.h"

// masks definitions
#define OUTPUT_COUNTRY_CODE	1
#define OUTPUT_REGION    	2
#define OUTPUT_CITY    		4
#define OUTPUT_POSTAL_CODE	8
#define OUTPUT_LATITUDE		16
#define OUTPUT_LONGITUDE	32
#define OUTPUT_METROCODE	64
#define OUTPUT_AREACODE		128

//TO be added 
//#define OUTPUT_NETWORK	256

#if defined(_WIN32)
# ifndef uint32_t
typedef unsigned int uint32_t;
# endif
#endif
	
static char *USAGE =
                "\nUSAGE:\n"
                " %s <ip> [output_mask]\n\n"
                "Useful codes for the ouput_mask\n"
                "COUNTRY_CODE    1\n"
                "REGION          2\n"
                "CITY            4\n"
                "POSTAL_CODE     8\n"
                "LATITUDE        16\n"
                "LONGITUDE       32\n"
                "METROCODE       64\n"
                "AREACODE        128\n\n"
                "Examples: \n"
                " %s 140.208.31.117 48\n"
                " %s 140.208.31.117 255\n"
                " %s 140.208.31.117 \n";

#define PRINT_USAGE fprintf(stdout, USAGE, argv[0],argv[0],argv[0],argv[0]);
	
int
main (int argc, char *argv[])
{
  int code;
  struct geo_output_struct geo_output;
  int geo_outputmask;
  char hostname[1024]={'\0'};
  int iterator = 1000;

	
  if (argc<2)
	{
	 PRINT_USAGE
       	 return 0; 
	}

  fprintf(stderr, "GEOIP_DATA_PATH: %s\n", GEOIP_DATA_PATH);

  if (argv[2])
	geo_outputmask = atol (argv[2]);	
  else
  	geo_outputmask = 255;
  sprintf(hostname,"%s",argv[1]);
  
  if (code = esgf_geolookup (hostname,&geo_output))
   {
     fprintf (stderr, "Exit code for esgf-lookup [%d]\n", code);
     return 0;
   }
     	
  if (geo_outputmask & OUTPUT_COUNTRY_CODE)
    fprintf (stdout, "[OUTPUT_COUNTRY_CODE=%s]\n", geo_output.country_code);
  if (geo_outputmask & OUTPUT_REGION)
    fprintf (stdout, "[OUTPUT_REGION=%s]\n", geo_output.region);
  if (geo_outputmask & OUTPUT_CITY)
    fprintf (stdout, "[OUTPUT_CITY=%s]\n", geo_output.city);
  if (geo_outputmask & OUTPUT_POSTAL_CODE)
    fprintf (stdout, "[OUTPUT_POSTAL_CODE=%s]\n", geo_output.postal_code);
  if (geo_outputmask & OUTPUT_LATITUDE)
    fprintf (stdout, "[OUTPUT_LATITUDE=%f]\n", geo_output.latitude);
  if (geo_outputmask & OUTPUT_LONGITUDE)
    fprintf (stdout, "[OUTPUT_LONGITUDE=%f]\n", geo_output.longitude);
  if (geo_outputmask & OUTPUT_METROCODE)
    fprintf (stdout, "[OUTPUT_METROCODE=%d]\n", geo_output.metro_code);
  if (geo_outputmask & OUTPUT_AREACODE)
    fprintf (stdout, "[OUTPUT_AREACODE=%d]\n", geo_output.area_code);

  return 0;
}

