CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: server client

server: tcp-server protocol.o authenticate.o status.o validate.o
	${CC} -pthread server.o protocol.o authenticate.o status.o validate.o -o server

client: tcp-client protocol.o  status.o validate.o
	${CC} -pthread client.o protocol.o status.o validate.o -o client

tcp-server: server.c
	${CC} ${CFLAGS} -o server.o server.c

tcp-client: client.c
	${CC} ${CFLAGS} -o client.o client.c

protocol: protocol.c
	${CC} ${CFLAGS} protocol.c

authenticate: authenticate.c
	${CC} ${CFLAGS} authenticate.c

status: status.c
	${CC} ${CFLAGS} status.c

validate: validate.c
	${CC} ${CFLAGS} validate.c

clean:
	rm -f *.o *~
	rm server
	rm client