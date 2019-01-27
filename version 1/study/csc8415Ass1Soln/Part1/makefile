.c.o :
	gcc -c -I /usr/local/include  $<

LIBS :=  -L /usr/local/lib -lCscNet

all: client 

client: client.o
	gcc client.o $(LIBS) -o client

clean:
	rm client *.o
