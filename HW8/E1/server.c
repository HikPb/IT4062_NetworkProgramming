
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#define BACKLOG 2 /* Number of allowed connections */
#define BUFF_SIZE 1024
#define NUM_OF_STRING 1

// max length of a string

void resultMessage(int sockfd, char buffer[], int len)
{
    struct iovec iov[2];
    int pos_1 = 0, pos_2 = 0, isError = 0;
    char number[BUFF_SIZE];
    char alpha[BUFF_SIZE];
    char length_1[BUFF_SIZE];
    char length_2[BUFF_SIZE];
    memset(length_1, '\0', (strlen(length_1) + 1));
    memset(length_2, '\0', (strlen(length_2) + 1));

    if (strcmp(buffer, "\n") == 0)
        isError = 1;
    else
        for (int i = 0; i < len - 1; i++)
        {
            char ch = buffer[i];
            //check if character is number
            if (ch >= '0' && ch <= '9')
            {
                number[pos_1] = ch;
                pos_1++;
            }
            //check if character is alphabet;
            else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
            {
                alpha[pos_2] = ch;
                pos_2++;
            }
            //character is special symbol
            else
            {
                isError = 1;
            }
        }
    number[pos_1] = '\0';
    alpha[pos_2] = '\0';
    //if requested string have any special symbol
    if (isError == 1)
    {
        iov[0].iov_base = "error";
        iov[0].iov_len = sizeof("error");
        sprintf(length_1, "%ld", iov[0].iov_len);
        send(sockfd, length_1, strlen(length_1), 0);
        sleep(1/2);
        iov[1].iov_base = "containt special symbol";
        iov[1].iov_len = sizeof("containt special symbol");
        sprintf(length_2, "%ld\n", iov[1].iov_len);
        send(sockfd, length_2, strlen(length_2), 0);
        sleep(1/2);
    }
    //sent result back
    else
    {

        iov[0].iov_base = number;
        iov[0].iov_len = pos_1;
        sprintf(length_1, "%d", pos_1);
        send(sockfd, length_1, strlen(length_1), 0);
        sleep(1/2);
        iov[1].iov_base = alpha;
        iov[1].iov_len = pos_2;
        sprintf(length_2, "%d\n", pos_2);
        send(sockfd, length_2, strlen(length_2), 0);
        sleep(1/2);
    }
    writev(sockfd, iov, 2);
    sleep(1 / 2);
    printf("number: %s, alpha: %s\n", iov[0].iov_base, iov[1].iov_base);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("error, too many or too few arguments\n");
        printf("Correct format is /.server YourPort\n");
        return 1;
    }
    int i, maxi, max_fd, listen_fd, conn_fd, sockfd;
    int n_ready, client[FD_SETSIZE];
    ssize_t ret;
    fd_set readfds, allset;
    char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    //Step 1: Construct a TCP socket to listen connection request
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { /* calls socket() */
        perror("\nError: ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    { /* calls bind() */
        perror("\nError: ");
        return 0;
    }
    fcntl(listen_fd, F_SETFL, O_NONBLOCK); // set to non-blocking
    fcntl(listen_fd, F_SETFL, O_ASYNC);
    //Step 3: Listen request from client
    if (listen(listen_fd, BACKLOG) == -1)
    { /* calls listen() */
        perror("\nError: ");
        return 0;
    }

    max_fd = listen_fd; /* initialize */
    maxi = -1;          /* index into client[] array */
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1; /* -1 indicates available entry */
    FD_ZERO(&allset);
    FD_SET(listen_fd, &allset);

    //Step 4: Communicate with clients
    while (1)
    {
        readfds = allset; /* structure assignment */
        n_ready = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (n_ready < 0)
        {
            perror("\nError: ");
            return 0;
        }

        if (FD_ISSET(listen_fd, &readfds))
        { /* new client connection */
            clilen = sizeof(cliaddr);
            if ((conn_fd = accept(listen_fd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
                perror("\nError: ");
            else
            {
                printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
                for (i = 0; i < FD_SETSIZE; i++)
                    if (client[i] < 0)
                    {
                        client[i] = conn_fd; /* save descriptor */
                        break;
                    }
                if (i == FD_SETSIZE)
                {
                    printf("\nToo many clients");
                    close(conn_fd);
                }

                FD_SET(conn_fd, &allset); /* add new descriptor to set */
                if (conn_fd > max_fd)
                    max_fd = conn_fd; /* for select */
                if (i > maxi)
                    maxi = i; /* max index in client[] array */

                if (--n_ready <= 0)
                    continue; /* no more readable descriptors */
            }
        }

        for (i = 0; i <= maxi; i++)
        { /* check all clients for data */
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &readfds))
            {
                struct iovec iov_1[1];
                char recv_data[BUFF_SIZE];
                ssize_t bytes_sent, bytes_received;

                memset(recv_data, '\0', (strlen(recv_data) + 1));
                printf("begin\n");
                //receives message from client
                iov_1[0].iov_base = recv_data;
                iov_1[0].iov_len = BUFF_SIZE;
                bytes_received = readv(sockfd, iov_1, 1);
                printf("\nReceive: %s", iov_1[0].iov_base);
                iov_1[0].iov_len = bytes_received;

                //echo to client
                resultMessage(sockfd, iov_1[0].iov_base, iov_1[0].iov_len);

                /* send to the client welcome message */
                printf("-------------------------\n");
                char systemMsg[BUFF_SIZE];
                int systemMsg_bytes_received = recv(sockfd, systemMsg, BUFF_SIZE - 1, 0);
                if (systemMsg_bytes_received <= 0)
                {
                    printf("\nError!Cannot receive data from sever!\n");
                    break;
                }
                systemMsg[systemMsg_bytes_received] = '\0';
                if (strcmp(systemMsg, "All done, rest now\n") == 0)
                {
                    FD_CLR(sockfd, &allset);
                    close(sockfd);
                    client[i] = -1;
                }
                send(sockfd, "done", strlen("done"), 0);
                if (--n_ready <= 0)
                    break; /* no more readable descriptors */
            }
        }
    }
    return 0;
}
