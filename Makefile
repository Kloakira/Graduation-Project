CC=gcc
 
objs=uart1.o cJSON.o gps.o dht.o main.o -lpthread
srcs=uart1.c cJSON.c gps.c dht.c main.c -lpthread
 
gps_test: $(objs)
        $(CC) -o test $(objs)
        @make clean
 
main.o: $(srcs) gps.h dht.h cJSON.h 
        $(CC) -c  $(srcs) 
 
uart1.o:  uart1.c
        $(CC) -c  uart1.c

cJSON.o:  cJSON.c
		$(CC) -c  cJSON.c

gps.o:  gps.c gps.h
		$(CC) -c gps.c

dht.o:  dht.c dht.h
		$(CC) -c dht.c 

clean:
        rm *.o