CC=clang
CFLAGS=-Wall -g

all: webserver

webserver: webserver.o
	$(CC) $(CFLAGS) -o webserver $<

webserver.o: webserver.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm *o webserver
