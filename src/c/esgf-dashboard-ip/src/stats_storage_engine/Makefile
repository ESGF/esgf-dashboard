

all:	test display realtime 


test:
	cc -o test -I/usr/include/libxml2 test.c -lpthread -lcurl -lxml2 -lz -lm 

display:
	cc -o display display.c

realtime:
	cc -o realtime realtime.c

clean:
	rm test display realtime 
