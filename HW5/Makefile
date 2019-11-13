CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: ser cli

cli: client.c
	 ${CC} -o client client.c

ser: server.c linkList.o
	 ${CC} -o server server.c linkList.o

linkList.o: linkList.c
	${CC} -c linkList.c
clean:
	rm -f *.o *~