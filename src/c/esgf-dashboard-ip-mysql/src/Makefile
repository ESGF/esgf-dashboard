


all:
	gcc -c main.c ping.c dbAccess.c -I/usr/include/mysql 
 
	gcc -o main main.o ping.o dbAccess.o -rdynamic -L/usr/lib/mysql -lmysqlclient_r -lz -lpthread -lcrypt -lnsl -lm -lpthread -L/usr/lib -lssl -lcrypto 

clean:
	rm *.o *~ main
