

all:	test display realtime iplookup 


test:
	cc -o test -I/usr/include/libxml2 test.c -lpthread -lxml2 -lz -lm -L/root/Desktop/installLLNL/cUrl/lib -lcurl 

display:
	cc -o display display.c

realtime:
	cc -o realtime realtime.c

iplookup:
	cc -o iplookup -I/root/Desktop/installLLNL/geoIP/include iplookup.c debug.c libiplookup.c -L/root/Desktop/installLLNL/geoIP/lib -lGeoIP

clean:
	rm test display realtime iplookup 
