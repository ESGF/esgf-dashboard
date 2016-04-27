all: xmlexample
LIBS= -L. -lcurl -lxml2 -lm -L/usr/lib64 -lpq -L/home/esgf/geoip_install/lib -lGeoIP 

INCLUDES= -I. -I/usr/include/libxml2 -I/usr/include/curl/ -I/usr/include/ -I/home/esgf/geoip_install/include 

xmlexample: debug.o hashtbl.o esgf-dashboard-insert-dw.o ftpget.o esgf-libiplookup.o xmlreader.o
	gcc -o xmlexample xmlreader.o ftpget.o esgf-dashboard-insert-dw.o hashtbl.o esgf-libiplookup.o debug.o $(LIBS)

debug.o: debug.c debug.h
	gcc -c -O0 -g3 debug.c $(INCLUDES)

ftpget.o: ftpget.c ftpget.h error.h
	gcc -c -O0 -g3 ftpget.c $(INCLUDES)

esgf-dashboard-insert-dw.o: ftpget.h debug.h config.h debug.c esgf-libiplookup.c esgf-dashboard-insert-dw.c
	gcc -c -O0 -g3 esgf-dashboard-insert-dw.c $(INCLUDES)

esgf-libiplookup.o: esgf-libiplookup.c config.h debug.h
	gcc -c -O0 -g3 esgf-libiplookup.c $(INCLUDES)

hashtbl.o: hashtbl.c hashtbl.h
	gcc -c -O0 -g3 hashtbl.c $(INCLUDES)

xmlreader.o: ftpget.h error.h debug.h hashtbl.h GeoIP_internal.h debug.c esgf-libiplookup.c xmlreader.c
	gcc -c -O0 -g3 xmlreader.c $(INCLUDES)
clean:
	rm -f *.o xmlexample *~
