bin_PROGRAMS = esgf-dashboard-ip 

AM_CFLAGS = $(OPT) -I../include -Wall

esgf_dashboard_ip_SOURCES=esgf-dashboard-ip.c dbAccess.c ping.c 
esgf_dashboard_ip_CFLAGS=-O2 $(postgres_CFLAGS) $(CFLAGS) -I. -I../include 
esgf_dashboard_ip_LDFLAGS=-O2 $(pq_LDFLAGS) -lpq -lz -lpthread -lm -lpthread -L/usr/lib -lssl -lcrypto

#libgrelcpostgres_la_LDFLAGS = -module -avoid-version -no-undefined


#EXTRA_DIST=esgf-dashboard-ip.sh
