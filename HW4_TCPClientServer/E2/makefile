all : client server
.PHONY : all
client : tcpclient.c
	gcc tcpclient.c -o client
server : tcpserver.c
	gcc tcpserver.c -o server
clean :
	rm client server