CC=gcc
CFLAGS=-I.
all : tcpclient tcpserver udpclient udpserver

tcpclient : 
	$(CC) -o tcpclient tcpclient.c $(CFLAGS)

tcpserver : 
	$(CC) -o tcpserver tcpserver.c $(CFLAGS)
	
udpclient : 
	$(CC) -o udpclient udpclient.c $(CFLAGS)

udpserver : 
	$(CC) -o udpserver udpserver.c $(CFLAGS)

clean :
	rm -f tcpclient tcpserver udpclient udpserver