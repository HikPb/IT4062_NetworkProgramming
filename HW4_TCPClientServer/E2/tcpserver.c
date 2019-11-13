
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
#define EXISTEDFILE "Error: File da ton tai "
#define SEND_SIZE 1024

int main(int argc, char **argv)
{
 	int port_number;
 	if(argc != 2) {
 		perror(" Error Parameter! Please input only port number\n ");
 		exit(0);
 	}
 	if((port_number = atoi(argv[1])) == 0) {
 		perror(" Please input port number\n");
 		exit(0);
 	}
	int listen_sock, conn_sock; /* file descriptors */
	char fileName[BUFF_SIZE];
	char *ndFile;
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	FILE *fileptr;
	long filelen;

	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(port_number);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	struct stat st = {0};

	if (stat(STORAGE, &st) == -1) { //create storage if it not exist

	    mkdir(STORAGE, 0755);
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, (unsigned int*)&sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1){
			//receives message from client
			if (stat(STORAGE, &st) == -1) { //create storage if it not exist
			    mkdir(STORAGE, 0755);
			}
			char number[BUFF_SIZE];
 			char character[BUFF_SIZE];
			bytes_received = recv(conn_sock, fileName, BUFF_SIZE-1, 0); //blocking
			
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				fileName[bytes_received] = '\0';

				char name[100];
				strcpy(name, STORAGE); // save file in storage
				strcat(name, fileName); // file name
				printf("Ten file : %s\n", name);
				if((fileptr = fopen(name, "rb")) != NULL) { // check if file exist
					printf("%s\n", EXISTEDFILE);
					bytes_sent = send(conn_sock, EXISTEDFILE, strlen(EXISTEDFILE), 0);
					if (bytes_sent <= 0){
						printf("\nConnection closed");
						break;
					}
					fclose(fileptr);
					continue;
				}
				else {
					
					// fclose(fileptr);
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
					printf("Uploaded file name: %s, Size of file: %ld\n\n", fileName, filelen);

					int sumByte = 0;
					fileptr = fopen(name, "wb");
					ndFile = (char*) malloc(SEND_SIZE * sizeof(char));
					while(1) {
						bytes_received = recv(conn_sock, ndFile, SEND_SIZE, 0);
						if(bytes_received == 0) {
							printf("Error: Truyen File that bai \n");
						}
						sumByte += bytes_received;
						fwrite(ndFile, bytes_received, 1, fileptr);
						free(ndFile);
						ndFile = (char*) malloc(SEND_SIZE * sizeof(char));
						if(sumByte >= filelen) {
							break;
						}
					}  // file content
					if (bytes_received <= 0){
						printf("\nConnection closed");
						break;
					}
					
					bytes_sent = send(conn_sock, "Upload success \n", 30, 0);
					if (bytes_sent <= 0){
						printf("\nConnection closed");
						break;
					}

					fclose(fileptr);
					free(ndFile);
				}
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}