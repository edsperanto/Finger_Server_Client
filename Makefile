CC = gcc
CFLAGS = -Wall -pthread -c
LFLAGS = -Wall -pthread

all: fingerserver fingerclient

fingerserver: fingerserver.o
	$(CC) $(LFLAGS) fingerserver.o -o fingerserver
	
fingerclient: fingerclient.o
	$(CC) $(LFLAGS) fingerclient.o -o fingerclient

fingerserver.o:
	$(CC) $(CFLAGS) fingerserver.c

fingerclient.o:
	$(CC) $(CFLAGS) fingerclient.c

clean:
	rm fingerserver fingerclient *.o *~
