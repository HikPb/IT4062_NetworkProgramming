#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


#define BUFF_SIZE 8192


int main(int argc,char** argv){
	if(argc != 3) perror("You must run program with port number!");
	int port = atoi(argv[2]);
	if(port == -1) perror("Invalid port number");
	
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
		//send message
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char systemMsg[BUFF_SIZE];
    int systemMsg_bytes_received;
    //send username to server
    printf("\nUsername: ");
    memset(username, '\0', (strlen(username) + 1));
    fgets(username, BUFF_SIZE, stdin);
    username[strlen(username)-1]='\0';
    bytes_sent = send(client_sock, username, strlen(username), 0);
    //send password to server
    printf("\nPassword: ");
    memset(password, '\0', (strlen(password) + 1));
    fgets(password, BUFF_SIZE, stdin);
    password[strlen(password)-1]='\0';
    bytes_sent = send(client_sock, password, strlen(password), 0);

    //receive login status
    systemMsg_bytes_received = recv(client_sock, systemMsg, BUFF_SIZE - 1, 0);
    if (systemMsg_bytes_received <= 0)
    {
        printf("\nError!Cannot receive data from sever!\n");
        return 0;
    }
    systemMsg[systemMsg_bytes_received] = '\0';
    printf("%s\n", systemMsg);
    int time=0;
    while(strcmp(systemMsg,"Pass incorrect\n")==0 && time!=2){
        ++time;
        //bytes_received= recv(client_sock,&time,sizeof(time),0);
        printf("Re-enter password %d: ",time);
        memset(password, '\0', (strlen(password) + 1));
        fgets(password, BUFF_SIZE, stdin);
        password[strlen(password)-1]='\0';
        bytes_sent = send(client_sock, password, strlen(password), 0);
        systemMsg_bytes_received = recv(client_sock, systemMsg, BUFF_SIZE - 1, 0);
        if (systemMsg_bytes_received <= 0)
        {
            printf("\nError!Cannot receive data from sever!\n");
            return 0;
        }
        systemMsg[systemMsg_bytes_received] = '\0';
        printf("%s\n", systemMsg);
        if(strcmp(systemMsg,"Login successfull\n")==0) break;
    }
	
	//Step 4: Close socket
	//close(client_sock);
	return 0;
}
