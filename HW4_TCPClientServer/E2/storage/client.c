#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFSIZE 1024
#define MAX_LINE 1024

void sendfile(FILE *fp, int sockfd);
ssize_t total=0;
int main(int argc, char* argv[])
{
    if(argc!=3){
		printf("Wrong number of parameters!\n");
		exit(EXIT_FAILURE);
	}

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("Can't allocate sockfd");
        exit(1);
    }
    printf("[+]Client Socket is created.\n");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[2], &serveraddr.sin_addr) < 0)
    {
        perror("IPaddress Convert Error");
        exit(1);
    }

    if (connect(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("Connect Error");
        exit(1);
    }
    while (1){
        char path[1024];
        memset(path,'\0',strlen(path)+1);
        printf("\nInsert path:");
        fgets(path, 1024, stdin);
        if (path == NULL)
        {
            perror("Can't get filename");
            exit(1);
        }
        path[strlen(path)-1]='\0';

        char *filename = basename(path); 
        if (filename == NULL)
        {
            perror("Can't get filename");
            break;
        }
        
        char buff[BUFFSIZE] = {0};
        strncpy(buff, filename, strlen(filename));
        if (send(sockfd, buff, BUFFSIZE, 0) == -1)
        {
            perror("Can't send filename");
            exit(1);
        }
        
        FILE *fp = fopen(path, "rb");
        if (fp == NULL) 
        {
            perror("Can't open file");
            exit(1);
        }

        sendfile(fp, sockfd);
        //puts("Send Success");
        printf("Send Success, NumBytes = %ld\n", total);
        fclose(fp);
        char mess[30];
        memset(mess,0,30);
        //if(recv(sockfd,mess,strlen(mess),0)<0) printf("Error\n");
        //printf("%s\n",mess);
    }
    close(sockfd);
    return 0;
}

void sendfile(FILE *fp, int sockfd) 
{
    int n; 
    char sendline[MAX_LINE] = {0}; 
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) 
    {
	    total+=n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
}