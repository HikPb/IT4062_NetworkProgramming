all : client server
.PHONY : all
client : TCPClient.c
	gcc TCPClient.c -o client
server : TCPServer.c
	gcc TCPServer.c -o server
clean :
	rm client server
