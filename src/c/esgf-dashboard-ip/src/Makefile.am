bin_PROGRAMS = esgf-dashboard-ip esgf-iplookup esgf-getrawstats 

AM_CFLAGS = $(OPT) -I../include -Wall

esgf_dashboard_ip_SOURCES=esgf-dashboard-ip.c dbAccess.c ping.c esgf-dashboard-parser.c esgf-libiplookup.c hashtbl.c debug.c esgf-libstats.c xmlreader.c ftpget.c esgf-dashboard-insert-dw.c 
esgf_dashboard_ip_CFLAGS=-O2 $(postgres_CFLAGS) $(CFLAGS) -I. -I../include  `$(XML2_CONFIG) --cflags` -I$(GEOIP_PREFIX_PATH)/include -DGEOIP_DATA_PATH=\"$(GEOIP_PREFIX_PATH)\" 
esgf_dashboard_ip_LDFLAGS=-O2 $(pq_LDFLAGS) -L/usr/local/curl-no-ssl/lib -lcurl -lpq -lz -lpthread -lm -lpthread -L/usr/lib64 -lssl -lcrypto  `$(XML2_CONFIG) --libs` -L$(GEOIP_PREFIX_PATH)/lib -lGeoIP 

esgf_iplookup_SOURCES=esgf-iplookup.c esgf-libiplookup.c debug.c 
esgf_iplookup_CFLAGS=-O2 $(postgres_CFLAGS) $(CFLAGS) -I. -I../include  `$(XML2_CONFIG) --cflags` -I$(GEOIP_PREFIX_PATH)/include -DGEOIP_DATA_PATH=\"$(GEOIP_PREFIX_PATH)\" 
esgf_iplookup_LDFLAGS=-O2 $(pq_LDFLAGS) -lpq -lz -lpthread -lm -lpthread -L/usr/lib64 -lssl -lcrypto  `$(XML2_CONFIG) --libs` -L$(GEOIP_PREFIX_PATH)/lib -lGeoIP 

esgf_getrawstats_SOURCES= esgf-getrawstats.c
esgf_getrawstats_CFLAGS=-O2 $(CFLAGS) -I. -I../include  
esgf_getrawstats_LDFLAGS=-O2

#libgrelcpostgres_la_LDFLAGS = -module -avoid-version -no-undefined
# gcc -o geoiplookup2 -I. -I/export/fiore2/install/GeoIP3/include -L/export/fiore2/install/GeoIP3/lib -lGeoIP geoiplookup2.c
#-DGRELC_SDAIX_LOCATION=\"${prefix}\"
#EXTRA_DIST=esgf-dashboard-ip.sh
#esgf_dashboard_ip_CFLAGS=-O2 $(postgres_CFLAGS) $(CFLAGS) -I. -I../include  `xml2-config --cflags` -I/export/fiore2/install/GeoIP3/include 
#esgf_dashboard_ip_LDFLAGS=-O2 $(pq_LDFLAGS) -lpq -lz -lpthread -lm -lpthread -L/usr/lib -lssl -lcrypto  `xml2-config --libs` -L/export/fiore2/install/GeoIP3/lib -lGeoIP 
