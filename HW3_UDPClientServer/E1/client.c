// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define BUFF_SIZE 1024 

int main(int argc, char* argv[]) { 
	int sockfd; 
	char buffer[BUFF_SIZE]; 
	struct sockaddr_in server_addr; 
	int bytes_received, sin_size;

	//Check parameter
	if(argc!=3){
		printf("Wrong number of parameters!\n");
		exit(EXIT_FAILURE);
	}
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("[+]Client Socket Created Sucessfully.\n");
	
	// Filling server information 
	memset(&server_addr, 0, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = inet_addr(argv[1]); 
	server_addr.sin_port = htons(atoi(argv[2])); 

	//Communicate with server
	while (1){
		printf("\nInsert string:");
		memset(buffer,'\0',(strlen(buffer)+1));
		fgets(buffer, BUFF_SIZE, stdin);
		if(buffer[0]=='\n') break;
		buffer[strlen(buffer)-1]='\0';
		sendto(sockfd, buffer, strlen(buffer), 
			MSG_CONFIRM, (const struct sockaddr *) &server_addr, 
				sizeof(server_addr)); 
		printf("[+]Successfully sent to server...\n"); 
			
		bytes_received = recvfrom(sockfd, (char *)buffer, BUFF_SIZE, 
					MSG_WAITALL, (struct sockaddr *) &server_addr, 
					&sin_size); 
		buffer[bytes_received] = '\0'; 
		printf("[+]Server reply: %s\n",buffer);
	}
	close(sockfd); 
	return 0; 
} 
