// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#define BUFF_SIZE 1024 

int hostnameToIp(char *, char *);
int ipToHostname(char *, struct in_addr,char *);

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
	struct in_addr ip;
	int x;
	while (1)
	{
		memset(result,'\0',100);
		bytes_received = recvfrom(sockfd, (char *)buffer, BUFF_SIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &sin_size); 
		buffer[bytes_received] = '\0'; 
		printf("\tReceive: \"%s\" from client %s:%d\n", buffer, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
		if (!inet_pton(AF_INET,buffer,&ip)) {
			x = hostnameToIp(buffer,result);
		}else x = ipToHostname(buffer,ip,result);
		if(x==1) strcpy(buffer,"Error");
		sendto(sockfd, (char *)result, strlen(result), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr)); 
		printf("[+]Message was sent to client....\n"); 
	}
	return 0; 
} 

int hostnameToIp(char *param, char *result){
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(param)) == NULL) {  
        herror("gethostbyname");
        return 1;
    }
	
    ;
    addr_list = (struct in_addr **)he->h_addr_list;
    strcpy(result,inet_ntoa(*addr_list[0])); 
    return 0;
}

int ipToHostname(char *param, struct in_addr ip, char *result){
    
    struct hostent *hp;
    if ((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL) {
        herror("gethostbyaddr");
        return 1;
    }
    strcpy(result,hp->h_name);
    return 0;
}