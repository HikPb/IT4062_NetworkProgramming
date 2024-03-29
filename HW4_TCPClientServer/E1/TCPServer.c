#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int separateLetNum(char *, char *);

int main(int argc, char *argv[]){

	int sockfd, ret;
	struct sockaddr_in serverAddr;
	int newSocket;
	struct sockaddr_in newAddr;
	socklen_t addr_size;
	char buffer[1024];
	pid_t childpid;

	if(argc!=2){
		printf("Wrong number of parameters.\n");
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %s\n", argv[1]);

	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}


	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		char result[100];
		while(1){
			memset(result,'\0',1024);
			recv(newSocket, buffer, 1024, 0);
			//printf("%s",buffer);
			if(buffer[0]=='\0'){
				printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
				break;
			}else{
				buffer[strlen(buffer)-1]='\0';
				printf("Receive: \"%s\" from client.\n", buffer);
				memset(result,'\0',100);
				int x = separateLetNum(buffer,result);
				//printf("%d : %s\n",x,result);
				if(x==1){
					printf("[Error]: Invalid characters.\n");
					strcpy(result,"Error: Invalid character");
				}
				send(newSocket, result, strlen(result), 0);
				printf("[+]Sent to client...\n");
			}

		}
	}
	close(newSocket);
	return 0;
}

int separateLetNum(char *buff, char *result){
	char number[50];
	memset(number,'\0',50);
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
		//if(buff[i]='\n') printf("Co dau xuong dong\n");
	}
	*(result+l1) = ' ';
	strcat(result,number);
	return 0;
}