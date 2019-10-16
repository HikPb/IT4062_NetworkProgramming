#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>


#define PORT 5550   /* Port that will be opened */ 
#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define MAX_SIZE 10e6 * 100
#define STORAGE "./storage/" //default save file place
#define EXISTEDFILE "Error: File is existent on server"
#define BUFF_SEND 1024

/*
* Check valid messages
* @param char* mess
* @return 1 if valid, else return 0
*/

int main(int argc, char** argv)
{
 	if(argc!=2){
		printf("Wrong number of parameters.\n");
		exit(EXIT_FAILURE);
	}

	int sockfd, conn_sock; /* file descriptors */
	char filename[BUFF_SIZE];
	char *fileContent;
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	FILE *fileptr;
	long filelen;

	//Step 1: Construct a TCP socket to listen connection request
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("[-]Error in connection.\n");
		exit(1);
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(sockfd, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("[-]Bind Error");
        exit(1);
	}     
	printf("[+]Bind to port %s\n", argv[1]);
	//Step 3: Listen request from client
	if(listen(sockfd, BACKLOG) == -1){  /* calls listen() */
		perror("[-]Listen Error");
        exit(1);
	}
	printf("[+]Listening....\n");
	struct stat st = {0};

	if (stat(STORAGE, &st) == -1) { //create storage if it not exist

	    mkdir(STORAGE, 0755);
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(sockfd,( struct sockaddr *)&client, (unsigned int*)&sin_size)) == -1){ 
			perror("Connect Error");
            break;
			}
		printf("Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		//start conversation
		while(1){
			//receives message from client
			if (stat(STORAGE, &st) == -1) { //create storage if it not exist
			    mkdir(STORAGE, 0755);
			}
			bytes_received = recv(conn_sock, filename, BUFF_SIZE-1, 0); //blocking
			
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				filename[bytes_received] = '\0';

				char name[100];
				strcpy(name, STORAGE); // save file in storage
				strcat(name, filename); // file name
				printf("File name is: %s\n", name);
				if((fileptr = fopen(name, "rb")) != NULL) { // check if file exist
					bytes_sent = send(conn_sock, EXISTEDFILE, strlen(EXISTEDFILE), 0);
					if (bytes_sent <= 0){
						printf("\nConnection closed");
						break;
					}
					fclose(fileptr);
					continue;
				}
				else {
					bytes_sent = send(conn_sock, "OK", 20, 0); // if file Name valid
					if (bytes_sent <= 0){
						printf("\nConnection closed");
						break;
					}
					bytes_received = recv(conn_sock, &filelen, 20, 0);
					if (bytes_received <= 0){
						printf("\nConnection closed");
						break;
					}
					printf("\tFile \"%s\" is uploading...\n", filename);

					int sumByte = 0;
					fileptr = fopen(name, "wb");
					fileContent = (char*) malloc(BUFF_SEND * sizeof(char));
					while(1) {
						bytes_received = recv(conn_sock, fileContent, BUFF_SEND, 0);
						if(bytes_received == 0) {
							printf("Error: File tranfering is interupted\n\n");
						}
						sumByte += bytes_received;
						fwrite(fileContent, bytes_received, 1, fileptr);
						free(fileContent);
						fileContent = (char*) malloc(BUFF_SEND * sizeof(char));
						if(sumByte >= filelen) {
							break;
						}
					}  // file content
					if (bytes_received <= 0){
						printf("\nConnection closed");
						break;
					}
					printf("\tFile \"%s\" is uploaded, file size: %d bytes\n",filename,sumByte);
					bytes_sent = send(conn_sock, "[Server reply:] Successful transfered!", 40, 0);
					if (bytes_sent <= 0){
						printf("\nConnection closed");
						break;
					}

					fclose(fileptr);
					free(fileContent);
				}
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(sockfd);
	return 0;
}