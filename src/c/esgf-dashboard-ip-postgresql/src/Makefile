


all:
	gcc -c main.c ping.c dbAccess.c -I/usr/local/pgsql/include/
 
	#gcc -o main main.o ping.o dbAccess.o -L/usr/local/pgsql/lib -lpq -L/usr/lib64 -lz -lm -lpthread -lssl -lcrypto  
	gcc -o esgf-dashboard-ip main.o ping.o dbAccess.o -L/usr/local/pgsql/lib -lpq -lz -lpthread -lm -lpthread -L/usr/lib -lssl -lcrypto

clean:
	#rm *.o main
	rm *.o esgf-dashboard-ip
