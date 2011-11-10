/* 
esgf-geoiplookup.c 
*/

#include "GeoIP.h"
#include "GeoIPCity.h"
#include "GeoIP_internal.h"

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

//#define GEOIPINSTALLDIR "@geoipinstalldir@"
//#define GEOIPIDATABASE "@geoipinstallfile@" 
#define GEOIPINSTALLDIR "/export/fiore2/workspace/esgf-dashboard/src/c/GeoIP-1.4.8/data/"
#define GEOIPIDATABASE "GeoLiteCity.dat"

#if defined(_WIN32)
# ifndef uint32_t
typedef unsigned int uint32_t;
# endif
#endif

static const char *
_mk_NA (const char *p)
{
  return p ? p : "N/A";
}

struct geo_output_struct
{
  char country_code[256];
  char region[256];
  char city[256];
  char postal_code[256];
  double latitude;
  double longitude;
  long int metro_code;
  long int area_code;
};

int geoiplookup (GeoIP * gi, char *hostname, int i, long int geo_outputmask,
		 struct geo_output_struct *geo_output);
int esgf_geolookup (char *hostname, long int geo_outputmask,
		    struct geo_output_struct *geo_output);

/* extra info used in _say_range_ip */
int info_flag = 0;

      /*printf ("%s: %s, %s, %s, %s, %f, %f, %d, %d\n",
         GeoIPDBDescription[i], gir->country_code,
         _mk_NA (gir->region), _mk_NA (gir->city),
         _mk_NA (gir->postal_code), gir->latitude,
         gir->longitude, gir->metro_code, gir->area_code);
         _say_range_by_ip (gi, ipnum); */


int
main (int argc, char *argv[])
{
  int code;
  struct geo_output_struct geo_output;
  int geo_outputmask;
  char hostname[1024]={'\0'};
  int iterator = 1000;

  geo_outputmask = atol (argv[2]);	 
  sprintf(hostname,"%s",argv[1]);

while (iterator--)
{
  if (code = esgf_geolookup (hostname,geo_outputmask, &geo_output))
     continue;	
  if (geo_outputmask & OUTPUT_COUNTRY_CODE)
    fprintf (stdout, "[OUTPUT_COUNTRY_CODE=%s]\n", geo_output.country_code);
  if (geo_outputmask & OUTPUT_REGION)
    fprintf (stdout, "[OUTPUT_REGION=%s]\n", _mk_NA (geo_output.region));
  if (geo_outputmask & OUTPUT_CITY)
    fprintf (stdout, "[OUTPUT_CITY=%s]\n", _mk_NA (geo_output.city));
  if (geo_outputmask & OUTPUT_POSTAL_CODE)
    fprintf (stdout, "[OUTPUT_POSTAL_CODE=%s]\n", _mk_NA (geo_output.postal_code));
  if (geo_outputmask & OUTPUT_LATITUDE)
    fprintf (stdout, "[OUTPUT_LATITUDE=%f]\n", geo_output.latitude);
  if (geo_outputmask & OUTPUT_LONGITUDE)
    fprintf (stdout, "[OUTPUT_LONGITUDE=%f]\n", geo_output.longitude);
  if (geo_outputmask & OUTPUT_METROCODE)
    fprintf (stdout, "[OUTPUT_METROCODE=%d]\n", geo_output.metro_code);
  if (geo_outputmask & OUTPUT_AREACODE)
    fprintf (stdout, "[OUTPUT_AREACODE=%d]\n", geo_output.area_code);

  //if (geo_outputmask & OUTPUT_NETWORK)
  //  _say_range_by_ip (gi, ipnum);

  fprintf (stderr, "Exit code for esgf-lookup [%d]\n", code);
}

  return 0;
}

int
esgf_geolookup (char *hostname, long int geo_outputmask,
		struct geo_output_struct *geo_output)
{
  char *db_info;
  GeoIP *gi;
  int i, ret_code;
  char *custom_directory = NULL;
  char *custom_file = NULL;
  char geoipdat[1024] = { '\0' };

  sprintf (geoipdat, "%s/%s", GEOIPINSTALLDIR, GEOIPIDATABASE);

  fprintf (stderr, "Loading GeoLiteCity...");
  if (gi = GeoIP_open (geoipdat, GEOIP_STANDARD))
    {
      i = GeoIP_database_edition (gi);
      fprintf (stderr, " GeoIP database found [Ok]\n");
      ret_code = geoiplookup (gi, hostname, i, geo_outputmask, geo_output);
    }
  else
    {
      fprintf (stderr, " %s not available, skipping.Please check ! [Exit]\n",
	       geoipdat);
      ret_code = -4;
    }

  GeoIP_delete (gi);
  fprintf (stderr, "Exit code for geoiplookup [%d]\n", ret_code);
  return 0;
}


static void
_mk_conf_str (unsigned char val, char *to, int size)
{
  if ((val & 0x7f) == 0x7f)
    {
      snprintf (to, 5, "N/A");
      return;
    }
  snprintf (to, 5, "%d", val);
  return;
}

static unsigned long
__addr_to_num (const char *addr)
{
  unsigned int c, octet, t;
  unsigned long ipnum;
  int i = 3;

  octet = ipnum = 0;
  while ((c = *addr++))
    {
      if (c == '.')
	{
	  if (octet > 255)
	    return 0;
	  ipnum <<= 8;
	  ipnum += octet;
	  i--;
	  octet = 0;
	}
      else
	{
	  t = octet;
	  octet <<= 3;
	  octet += t;
	  octet += t;
	  c -= '0';
	  if (c > 9)
	    return 0;
	  octet += c;
	}
    }
  if ((octet > 255) || (i != 0))
    return 0;
  ipnum <<= 8;
  return ipnum + octet;
}



/* ptr must be a memory area with at least 16 bytes */
static char *
__num_to_addr_r (unsigned long ipnum, char *ptr)
{
  char *cur_str;
  int octet[4];
  int num_chars_written, i;

  cur_str = ptr;

  for (i = 0; i < 4; i++)
    {
      octet[3 - i] = ipnum % 256;
      ipnum >>= 8;
    }

  for (i = 0; i < 4; i++)
    {
      num_chars_written = sprintf (cur_str, "%d", octet[i]);
      cur_str += num_chars_written;

      if (i < 3)
	{
	  cur_str[0] = '.';
	  cur_str++;
	}
    }

  return ptr;
}

void
_say_range_by_ip (GeoIP * gi, uint32_t ipnum)
{
  unsigned long last_nm, mask, low, hi;
  char ipaddr[16];
  char tmp[16];
  char **range;

  if (info_flag == 0)
    return;			/* noop unless extra information is requested */

  range = GeoIP_range_by_ip (gi, __num_to_addr_r (ipnum, ipaddr));
  if (range == NULL)
    return;

  printf ("  ipaddr: %s\n", ipaddr);

  printf ("  range_by_ip:  %s - %s\n", range[0], range[1]);
  last_nm = GeoIP_last_netmask (gi);
  mask = 0xffffffff << (32 - last_nm);
  low = ipnum & mask;
  hi = low + (0xffffffff & ~mask);
  printf ("  network:      %s - %s ::%ld\n",
	  __num_to_addr_r (low, ipaddr), __num_to_addr_r (hi, tmp), last_nm);
  printf ("  ipnum: %u\n", ipnum);
  printf ("  range_by_num: %lu - %lu\n", __addr_to_num (range[0]),
	  __addr_to_num (range[1]));
  printf ("  network num:  %lu - %lu ::%lu\n", low, hi, last_nm);

  GeoIP_range_by_ip_delete (range);
}

int
geoiplookup (GeoIP * gi, char *hostname, int i, long int geo_outputmask,
	     struct geo_output_struct *geo_output)
{
  GeoIPRecord *gir;
  uint32_t ipnum;

  ipnum = _GeoIP_lookupaddress (hostname);
  if (ipnum == 0)
    {
      fprintf (stderr,
	       "%s: can't resolve hostname ( %s ) [keep this code for debug -1]\n",
	       GeoIPDBDescription[i], hostname);
      return -1;

    }
  if (GEOIP_CITY_EDITION_REV1 == i)	// it should be always true
    {
      gir = GeoIP_record_by_ipnum (gi, ipnum);
      if (NULL == gir)
	{
	  fprintf (stderr,
		   "%s: IP Address not found [keep this code for debug -3]\n",
		   GeoIPDBDescription[i]);
	  return -3;
	}
      else
	{
	    sprintf (geo_output->country_code, "%s", gir->country_code);
	    sprintf (geo_output->region, "%s",_mk_NA (gir->region));
	    sprintf (geo_output->city,"%s",_mk_NA (gir->city));
	    sprintf (geo_output->postal_code,"%s", _mk_NA (gir->postal_code));
	    geo_output->latitude=gir->latitude;
	    geo_output->longitude=gir->longitude;
	    geo_output->metro_code=gir->metro_code;
	    geo_output->area_code=gir->area_code;
	    //_say_range_by_ip (gi, ipnum);
	  GeoIPRecord_delete (gir);
	}
    }
  else
    {
      fprintf (stderr,
	       "Some problem with the current release [keep this code for debug -2 ver:%d]\n",
	       i);
      return -2;
    }
  return 0;
}


	  /*printf ("%s: %s, %s, %s, %s, %f, %f, %d, %d\n",
	     GeoIPDBDescription[i], gir->country_code,
	     _mk_NA (gir->region), _mk_NA (gir->city),
	     _mk_NA (gir->postal_code), gir->latitude,
	     gir->longitude, gir->metro_code, gir->area_code);
	     _say_range_by_ip (gi, ipnum); */
