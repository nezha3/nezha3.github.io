.c.o:
	gcc -c -std=gnu99 -I /usr/local/include  $<

LIBS :=  -L /usr/local/lib -lCscNet -lpthread

all: server client

client: client.o
	gcc client.o $(LIBS) -o client

server: server.o
	gcc server.o $(LIBS) -o server

clean:
	rm client server *.o server.log
