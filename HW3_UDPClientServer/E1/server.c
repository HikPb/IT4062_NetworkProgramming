// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define BUFF_SIZE 1024 

int separateLetNum(char *, char *,char *);

// Driver code 
int main(int argc, char* argv[]) { 
	int sockfd; 
	char buffer[BUFF_SIZE]; 
	struct sockaddr_in server_addr, client_addr; 
	int bytes_received,sin_size;

	if(argc!=2){
		printf("Wrong number of parameters.\n");
		exit(EXIT_FAILURE);
	}
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("[+]Successful create socket.\n");
	
	memset(&server_addr, 0, sizeof(server_addr)); 
	memset(&client_addr, 0, sizeof(client_addr)); 
	
	// Filling server information 
	server_addr.sin_family = AF_INET; // IPv4 
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	server_addr.sin_port = htons(atoi(argv[1])); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&server_addr, 
			sizeof(server_addr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	char result[100];
	char number[50];
	while (1)
	{
		memset(result,'\0',100);
		memset(number,'\0',50);
		bytes_received = recvfrom(sockfd, (char *)buffer, BUFF_SIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &sin_size); 
		buffer[bytes_received] = '\0'; 
		printf("\tReceive: \"%s\" from client %s:%d\n", buffer, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
		int x = separateLetNum(buffer,result,number);
		if(x==1){
			printf("[Error]: Invalid characters.\n");
			strcpy(result,"Error: Invalid character");
		}
		printf("[+]Server reply: %s %s\n",buffer,number);
		sendto(sockfd, (char *)result, strlen(result), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr)); 
		printf("[+]Message 1 was sent to client....\n"); 
		sendto(sockfd, (char *)number, strlen(number), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr)); 
		printf("[+]Message 2 was sent to client....\n"); 
	}
	return 0; 
} 
int separateLetNum(char *buff, char *result, char *number){
	int l1 = strlen(result);
		int l2 = strlen(number);
	for(int i=0;i<strlen(buff);i++){
		int dec = (int)buff[i];
		if(dec>=48&&dec<=57) {
			*(number+l2)=buff[i];
			l2++;
		}else if((dec>=65&&dec<=90)||(dec>=97&&dec<=122)){
			*(result+l1)=buff[i];
			l1++;
		}else return 1;
	}
	return 0;
}