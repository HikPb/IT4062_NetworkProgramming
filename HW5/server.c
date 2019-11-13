#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "linkList.h"
// #include "status.h"

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define LOGIN 1
#define OFFLINE 0
#define BLOCK 0
#define ACTIVE 1
#define ACCOUNT_FILE "account.txt"
#define COMMAND_USER "USER"
#define COMMAND_PASSWORD "PASS"
#define COMMAND_LOGOUT "LOGOUT"
#define MAX 256

void readFile(node **root);
void writeFile(node *root);
int checkLoginInfo(char * username, char * password, node **);

/*
* main function
* @param int argc, char** argv
* @return void
*/
int main(int argc, char* argv[])
{
    node * root =NULL;
 	if(argc != 2) perror("You must run program with port number!");
	int port = atoi(argv[1]);
	if(port == -1) perror("Invalid port number");
	//Step 1: Construct a TCP socket to listen connection request
	readFile(&root);
	int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    int sin_size;
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(port);   /* Remember htons() from "Conversions" section? =) */
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
	int pid;
	//Step 4: Communicate with client
	while(1){
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, (unsigned int*)&sin_size)) == -1) 
		{
			perror("\nError: ");
			break;
		}
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */

		//start conversation
	
		pid = fork();
        if (pid == -1)
        {
            close(conn_sock);
            printf("ERROR in new process creation\n");
            exit(1);
        }
        else if(pid > 0 )
        {
            close(conn_sock);
            continue;
        }
        else if (pid == 0)
        {
            //child process
            //do whatever you want
            while(1){
                char username[BUFF_SIZE];
                char password[BUFF_SIZE];
                memset(username, 0, strlen(username));
                memset(password, 0, strlen(password));
                printf("Receiving data ...\n");
                //receives username from client
                recv(conn_sock, username, BUFF_SIZE, 0);
                printf("Username: %s\n", username);
                //receives password from client
                recv(conn_sock, password, BUFF_SIZE, 0);
                printf("Password: %s\n", password);
                sleep(1);
                node * temp =root;
                int ck = checkLoginInfo(username, password, &temp);
                if(ck == -2){
                    send(conn_sock, "Username incorrect\n", sizeof("Username incorrect\n"), 0);
                }
                if(ck == -1){
                    send(conn_sock, "Account is block\n", sizeof("Account is block\n"), 0);
                }
                if(ck == 0){
                    send(conn_sock, "Pass incorrect\n", sizeof("Pass incorrect\n"), 0);
                    int time=0;
                    int k;
                    while(time!=2){
                        ++time;
                        //send(conn_sock,&time,sizeof(time),0);
                        recv(conn_sock, password, BUFF_SIZE, 0);
                        printf("\n++++%s:%s===%d\n",temp->acc.password,password,temp->acc.status);
                        if(strcmp(password,temp->acc.password)==0){
                            if(temp->acc.status==0) {k=-1;send(conn_sock, "Account is block\n", sizeof("Account is block\n"), 0);}
                            if(temp->acc.status==1) {k=1;send(conn_sock, "Login successfull\n", sizeof("Login successfull\n"), 0);}
                            break;
                        }else {k=0;send(conn_sock, "Pass incorrect\n", sizeof("Pass incorrect\n"), 0);}
                    }
                    if(k==0){
                        temp->acc.status=0;
                        showList(root);
                        writeFile(root);
                    }
                }
                if(ck == 1){
                    send(conn_sock, "Login successfull\n", sizeof("Login successfull\n"), 0);
                }
                printf("-------------------------%d\n",ck);
            }	
		}
        close(conn_sock);
	}
	
	close(listen_sock);
	return 0;
}

void readFile(node **root)
{
    account temp;
    FILE *f=fopen("account.txt","r");
    if(f!=NULL)
    {
        while(fscanf(f,"%s %s %d",temp.userName, temp.password,&temp.status)!=EOF)
        {
            pushToList(root,temp);
        }
    }
    fclose(f);
}

void writeFile(node *root){
    FILE *f=fopen("account.txt","wt");
    while (root!=NULL)
    {
        fprintf(f,"%s %s %d\n", root->acc.userName,root->acc.password,root->acc.status);
        root = root->next;
    }
    fclose(f);   
}
/*
* function checkLoginInfo
* @param char * username, char * password, node *root
* @return -2: ko tồn tại user
* @return -1: đúng user, status: block 
* @return 0: đúng user, status: active, sai pass 
* @return 1: đúng user, status: active, dung pass

*/
int checkLoginInfo(char * username, char * password, node **root){
    int checkuser=0;
    while (*root!=NULL){
        if(strcmp((*root)->acc.userName,username)==0){
            printf("|Ton tai username");
            checkuser=1;
            break;
        }
        *root = (*root)->next;
    }
    if(checkuser==1){
        if((*root)->acc.status==0) return -1;
        else if(strcmp((*root)->acc.password,password)==0) return 1;
        else return 0;
    }else return -2;
    
}
