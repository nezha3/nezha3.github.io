.c.o:
	gcc -c -std=gnu99 -I /usr/local/include  $<

LIBS :=  -L /usr/local/lib -lCscNet -lpthread

all: jsonDemo

jsonDemo: jsonDemo.o
	gcc jsonDemo.o $(LIBS) -o jsonDemo

clean:
	rm  jsonDemo *.o test.log
