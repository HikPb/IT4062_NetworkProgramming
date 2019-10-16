#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <libgen.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 8192
#define NUMBEROFDOTSINIPV4 3 //number dots in ipv4
#define NUMBEROFDOTSINIPV6 5 //number dots in ipv6
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define MAXLINE 4096
#define EXISTEDFILE "Error: File is existent on server"
#define BUFF_SEND 1024

// global value
char **tokens;
struct in_addr ipv4addr;
struct hostent *host;
struct in_addr **addr_list;
struct in_addr **alias_list;

/* Main function
* @param int argc, char** argv
* @return 1
*/
int main(int argc, char** argv){

    if(argc!=3){
		printf("Wrong number of parameters!\n");
		exit(EXIT_FAILURE);
	}
	int client_sock;
	char buff[1024];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
    char sendline[MAXLINE], recvline[MAXLINE];
    FILE *fp;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
    if (client_sock < 0) 
    {
        perror("Can't allocate sockfd");
        exit(1);
    }
    printf("[+]Client Socket is created.\n");
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		perror("[-]Connect Error");
        exit(1);
	}
		
	//Step 4: Communicate with server			
	while(1){
		//send message
        char path[1024];
        memset(path,'\0',strlen(path)+1);
        printf("\nInsert path:");
        fgets(path, 1024, stdin);
        if (path[0] == '\n')
        {
            printf("Path is NULL\n");
            break;
        }
        path[strlen(path)-1]='\0';
        printf("Path of send file: %s\n", path); //print file name
        if ((fp = fopen(path, "rb")) == NULL){
            printf("Error: File not found\n");
            fclose(fp);
            continue;
        }
        char *filename = basename(path);
        long filelen;
        fseek(fp, 0, SEEK_END);          // Jump to the end of the file
        filelen = ftell(fp);             // Get the current byte offset in the file       
        rewind(fp);    // pointer to start of file
        
        bytes_sent = send(client_sock, filename, strlen(filename), 0); //send name of file
        if(bytes_sent <= 0){ // check if false
            printf("\nConnection closed!\n");
            break;
        }

        bytes_received = recv(client_sock, buff, MAXLINE, 0); // recv result of check file  name
        if(bytes_received <= 0){
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }
        buff[bytes_received] = '\0';
        printf("[Server reply:] %s\n",buff);
        if(strcmp(buff, EXISTEDFILE) == 0) { // if file is existed
            printf("%s\n", buff);
            continue;
        }

        bytes_sent = send(client_sock, &filelen, 20, 0); //send file size
        if(bytes_sent <= 0){
            printf("\nConnection closed!\n");
            break;
        }

        int sumByte = 0; //count size byte send
        while(1) {
            int numberByteSend = BUFF_SEND;
            if((sumByte + BUFF_SEND) > filelen) {// if over file size
                numberByteSend = filelen - sumByte; 
            }
            char* buffer = (char *) malloc((numberByteSend) * sizeof(char));
            fread(buffer, numberByteSend, 1, fp); // read buffer with size 
            sumByte += numberByteSend; //increase byte send
            bytes_sent = send(client_sock, buffer, numberByteSend, 0);
            if(bytes_sent <= 0){
                printf("\nConnection closed!\n");
                break;
            }
            free(buffer);
            if(sumByte >= filelen) {
                break;
            }
        }
        fclose(fp); //close file
        printf("File \"%s\" is uploading, file size: %d bytes\n",filename,sumByte);
        bytes_received = recv(client_sock, buff, MAXLINE, 0); // recv result
        if(bytes_received <= 0){
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }
        printf("%s \n", buff); //print result
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}