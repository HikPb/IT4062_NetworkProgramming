
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define BUFF_SIZE 1024
int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in their_addr; /* connector's address information */
    struct hostent *he;
    int numbytes;
    char buf[BUFF_SIZE]; /*The buffer that we read / write each time     */
    int addr_len;        /* Address length for the network functions
                                                                 that require that      */

    if (argc != 3)
    {
        printf("error, too many or too few arguments\n");
        printf("Correct format is /.client YourId YourPort\n");
        exit(1);
    }

    if ((he = gethostbyname(argv[1])) == NULL)
    { /* get the host info */
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;            /* host byte order */
    their_addr.sin_port = htons(atoi(argv[2])); /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8); /* zero the rest of the struct */
    while (1)
    {
        int addr_len = sizeof(struct sockaddr_in);
        char buff[BUFF_SIZE];
        char buff_recv[BUFF_SIZE];
        memset(buff_recv, '\0', (strlen(buff_recv) + 1));

        printf("Insert string to send:");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);

        if ((numbytes = sendto(sockfd, buff, strlen(buff), 0,
                               (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)
        {
            perror("sendto");
            exit(1);
        }

        //printf("sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));
        printf("Getting result ... (this may take a while, please wait).\n");

        if ((numbytes = recvfrom(sockfd, buff_recv, BUFF_SIZE, 0,
                                 (struct sockaddr *)&their_addr, &addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }
        buff_recv[numbytes] = '\0';
        // printf("got from %s --->\n", inet_ntoa(their_addr.sin_addr));
        printf("%s \n", buff_recv);
        if (strcmp(buff, "\n") == 0)
        {
            close(sockfd);
            exit(1);
        }
    }

    return 0;
}
