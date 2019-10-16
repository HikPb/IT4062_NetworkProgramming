#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFSIZE 1024
#define MAX_LINE 1024

void writefile(int sockfd, FILE *fp);
ssize_t total=0;
int main(int argc, char *argv[]) 
{
    int connfd;
	if(argc!=2){
		printf("Wrong number of parameters.\n");
		exit(EXIT_FAILURE);
	}

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        perror("[-]Error in connection.\n");
		exit(1);
    }
    
    struct sockaddr_in clientaddr, serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) 
    {
        perror("[-]Bind Error");
        exit(1);
    }
    printf("[+]Bind to port %s\n", argv[1]);
    if (listen(sockfd, 10) == -1) 
    {
        perror("[-]Listen Error");
        exit(1);
    }
    printf("[+]Listening....\n");
    while(1){
        socklen_t addrlen = sizeof(clientaddr);
        connfd = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
        if (connfd == -1) 
        {
            perror("Connect Error");
            exit(1);
        }
    	printf("You got a connection from %s\n", inet_ntoa(clientaddr.sin_addr) ); /* prints client's IP */
        while (1){
        char filename[BUFFSIZE] = {0}; 
        if (recv(connfd, filename, BUFFSIZE, 0) == -1) 
        {
            perror("Can't receive filename");
            exit(1);
        }

        FILE *fp = fopen(filename, "wb");
        if (fp == NULL) 
        {
            perror("Can't open file");
            exit(1);
        }
        
        char addr[INET_ADDRSTRLEN];
        printf("Start receive file: %s from %s\n", filename, inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
        writefile(connfd, fp);
        printf("Receive Success, NumBytes = %ld\n", total);
        fclose(fp);
        // int bytes_sent = send(connfd,"Da nhan",30,0);
        // if (bytes_sent <= 0){
        //     printf("\nConnection closed");
        //     exit(1);
        // }
        printf("Sent to client\n");
        }
        close(connfd);
    }
    close(sockfd); 
    return 0;
}


void writefile(int sockfd, FILE *fp)
{
    ssize_t n;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0) 
    {
	    total+=n;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }
        
        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE);
    }

}