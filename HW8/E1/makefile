all: server client
server: server.c
	gcc	-pthread -o	server	server.c
client: client.c	
	gcc	-o	client	client.c
clean:
	find . -type f | xargs touch
	rm -rf $(OBJS)

	