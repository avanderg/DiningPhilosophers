CC = gcc
CFLAGS = -Wall -g -pthread

all: dine

dine: dine.o dine.h
	$(CC) $(CFLAGS) -lrt -o dine dine.o dine.h

dine.o: dine.c 
	$(CC) $(CFLAGS) -c dine.c

clean : 
	rm -f *.o
