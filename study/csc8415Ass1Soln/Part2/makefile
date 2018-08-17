.c.o:
	gcc -c -std=gnu99 -I /usr/local/include  $<

LIBS :=  -L /usr/local/lib -lCscNet -lpthread

all: server 

server: server.o
	gcc server.o $(LIBS) -o server

clean:
	rm server *.o server.log
