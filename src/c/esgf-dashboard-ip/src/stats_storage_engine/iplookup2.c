/* 
esgf-geoiplookup.c 
*/

#include "GeoIP.h"
#include "GeoIPCity.h"
#include "../../include/GeoIP_internal.h"
#include "../../include/config.h"

#if defined(_WIN32)
# ifndef uint32_t
typedef unsigned int uint32_t;
# endif
#endif
	
int
main (int argc, char *argv[])
{
  int code;
  struct geo_output_struct geo_output;
  int geo_outputmask=255;
  char buffer[256] = { '\0' };

  if (code = esgf_geolookup (buffer,&geo_output))
   	fprintf (stdout, "Exit code for esgf-lookup [%d]\n", code);
  if (geo_outputmask & OUTPUT_LATITUDE)
    fprintf (stdout, "%f ", geo_output.latitude);
  if (geo_outputmask & OUTPUT_LONGITUDE)
    fprintf (stdout, "%f ", geo_output.longitude);
  if (geo_outputmask & OUTPUT_COUNTRY_CODE)
    fprintf (stdout, "%s \n", geo_output.country_code);

  return 0;
}

