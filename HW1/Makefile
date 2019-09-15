CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: hw1

hw1: main.o linkList.o
	${CC} main.o  linkList.o -o hw1

main.o: main.c
	${CC} ${CFLAGS} main.c

linkList.o: linkList.c
	${CC} ${CFLAGS} linkList.c

clean:
	rm -f *.o *~
