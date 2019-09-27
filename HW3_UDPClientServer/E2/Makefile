CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client2 server2

client: client2.c
	${CC} client2.c  -o client2

server: server2.c
	${CC} server2.c  -o server2

clean:
	rm -f *.o *~