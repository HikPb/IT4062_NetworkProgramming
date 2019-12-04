

void io_handler(int); /*This is the main I/O procedure */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* #include <netdb.h> */
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#define BUFF_SIZE 1024

int sock;
char buff[BUFF_SIZE];
int i;
struct hostent *host;
struct in_addr ipv4addr;

int isValidIpAddress(char ipAddress[])
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if (result != 0)
        return 1;
    else
        return 0;
}

//convert from IP to address
char *fromIpToAddress(char *ip)
{
    char *message = "Official name: ";
    char firstloglocation[BUFF_SIZE];
    inet_pton(AF_INET, ip, &ipv4addr);
    if ((host = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET)) == NULL)
    {
        herror("Error ip");
        return "Error: something wrong\n";
    }
    else
    {
        strcpy(firstloglocation, message);
        message = strcat(firstloglocation, host->h_name);
        strcat(message, "\nAlias name:\n");
        for (i = 0; host->h_aliases[i] != NULL; i++)
        {
            strcpy(firstloglocation, message);
            message = strcat(firstloglocation, host->h_aliases[i]);
            strcat(message, "\n");
        }
        return message;
    }
}

//convert form address to ip
char *fromAddressToIp(char *address)
{
    char *message = "Official IP: \n";
    char firstloglocation[BUFF_SIZE];
    char secondloglocation[BUFF_SIZE];
    if ((host = gethostbyname(address)) == NULL)
    {
        herror("Error address");
        return "Error: something wrong\n";
    }
    else
    {
        for (i = 0; host->h_addr_list[i] != NULL; i++)
        {
            strcpy(firstloglocation, message);
            message = strcat(firstloglocation, inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
            strcat(message, "\n");
        }
        strcat(message, "Alias IP address: \n");

        for (i = 0; host->h_aliases[i] != NULL; i++)
        {
            strcpy(secondloglocation, message);
            message = strcat(secondloglocation, inet_ntoa(*(struct in_addr *)host->h_aliases[i]));
            strcat(message, "\n");
        }
        return message;
    }
}

char *getResult(char *buff)
{
    struct sockaddr_in sa;
    char *output;
    if (strcmp(buff, "") == 0)
        output = "Goodbye";
    else if (isValidIpAddress(buff))
        output = fromIpToAddress(buff);
    else
        output = fromAddressToIp(buff);
    return output;
}

int main(int argc, char *argv[])
{
    int length;
    struct sockaddr_in server;

    if (argc != 2)
    {
        printf("error, too many or too few arguments\n");
        printf("Correct format is /.server YourPort\n");
        return 1;
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("opening datagram socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *)&server, sizeof server) < 0)
    {
        perror("binding datagram socket");
        exit(1);
    }

    length = sizeof(server);
    if (getsockname(sock, (struct sockaddr *)&server, &length) < 0)
    {
        perror("getting socket name");
        exit(1);
    }
    printf("Socket port #%d\n", ntohs(server.sin_port));

    // first: set up a SIGIO signal handler by use of the signal call()
    signal(SIGIO, io_handler);

    // second: set the process id or process group id that is to receive
    // notification of pending input to its own process id or process
    // group id
    if (fcntl(sock, F_SETOWN, getpid()) < 0)
    {
        perror("fcntl F_SETOWN");
        exit(1);
    }

    // third: allow receipt of asynchronous I/O signals
    if (fcntl(sock, F_SETFL, FASYNC) < 0)
    {
        perror("fcntl F_SETFL, FASYNC");
        exit(1);
    }

    for (;;)
        ;
    /* Actually here was suppose to come the rest of the code (not dealing with cothe rest of the code (not dealing with comm) */
}

void io_handler(int signal)
{
    int numbytes;                  /* Number of bytes recieved from client */
    int addr_len;                  /* Address size of the sender		*/
    struct sockaddr_in their_addr; /* connector's address information	*/

    printf("The recvfrom proc. !\n");

    if ((numbytes = recvfrom(sock, buff, BUFF_SIZE, 0,
                             (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    buff[numbytes - 1] = '\0';

    printf("got from %s --->\"%s\" \n", inet_ntoa(their_addr.sin_addr), buff);
    printf("Getting result ... (this may take a while, please wait).\n");
    char *reply = getResult(buff);
    // return result to client
    int bytes_sent = sendto(sock, reply, strlen(reply), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr));
    if (bytes_sent < 0)
        perror("Error 3");
    printf("Done!\n");

    return;
}