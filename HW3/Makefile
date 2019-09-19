CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

client: client.c
	${CC} client.c  -o client

server: server.c
	${CC} server.c  -o server

clean:
	rm -f *.o *~