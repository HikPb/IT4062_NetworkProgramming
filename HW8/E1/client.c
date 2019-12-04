#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#define BUFF_SIZE 8192
#define BACKLOG 1 /* Number of allowed connections */

int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if (result != 0)
        return 1;
    else
        return 0;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("error, too many or too few arguments\n");
        printf("Correct format is /.client YourId YourPort\n");
        return 1;
    }
    //check if input id is valid
    if (isValidIpAddress(argv[1]) == 0)
    {
        printf("%s Not a valid ip address\n", argv[1]);
        return 1;
    }

    int client_sock;
    struct sockaddr_in server_addr; /* server's address information */

    //Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    //Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    fcntl(client_sock, F_SETFL, O_NONBLOCK); // set to non-blocking
    fcntl(client_sock, F_SETFL, O_ASYNC);
    //Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("\nError!Can not connect to sever! Client exit immediately! ");
        return 0;
    }

    //Step 4: Communicate with server
    while (1)
    {
        char buff[BUFF_SIZE];
        char receive_msg_1[BUFF_SIZE];
        char receive_msg_2[BUFF_SIZE];
        memset(receive_msg_1, '\0', (strlen(receive_msg_1) + 1));
        memset(receive_msg_2, '\0', (strlen(receive_msg_2) + 1));

        int msg_len;
        ssize_t bytes_sent, bytes_received;
        struct iovec iov_send[1];
        struct iovec iov_recv[2];

        //send message
        printf("Insert string to send:");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        msg_len = strlen(buff);

        iov_send[0].iov_base = buff;
        iov_send[0].iov_len = msg_len;

        char length_1[BUFF_SIZE];
        char length_2[BUFF_SIZE];
        memset(length_1, '\0', (strlen(length_1) + 1));
        memset(length_2, '\0', (strlen(length_2) + 1));

        printf("Send: %s", iov_send[0].iov_base);
        bytes_sent = writev(client_sock, iov_send, 1);
        printf("waiting ...\n");
        //receive echo reply
        iov_recv[0].iov_base = &receive_msg_1;
        recv(client_sock, length_1, BUFF_SIZE - 1, 0);
        iov_recv[0].iov_len = atoi(length_1);
        iov_recv[1].iov_base = &receive_msg_2;
        recv(client_sock, length_2, BUFF_SIZE - 1, 0);
        iov_recv[1].iov_len = atoi(length_2);

        bytes_received = readv(client_sock, iov_recv, 2);
        printf("Reply from server: \n");
        printf(" %s\n %s\n", receive_msg_1, receive_msg_2);
        printf("\n");
        if (strcmp(buff, "\n") == 0)
        {
            printf("Good bye\n");
            char *msg = "All done, rest now\n";
            send(client_sock, msg, strlen(msg), 0);
            sleep(1 / 2);
            close(client_sock);
            exit(1);
        }
        else
        {
            char *msg = "nothing\n";
            send(client_sock, msg, strlen(msg), 0);
            sleep(1 / 2);
        }
        recv(client_sock, buff, BUFF_SIZE - 1, 0);
        printf("------------------------\n");
    }

    //Step 4: Close socket
    close(client_sock);
    return 0;
}
