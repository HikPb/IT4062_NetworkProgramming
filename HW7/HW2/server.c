
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BACKLOG 2 /* Number of allowed connections */
#define BUFF_SIZE 1024

// max length of a string
#define MAX 256

enum
{
    user_register = 1,
    sign_in,
    search,
    sign_out
};

enum
{
    false,
    true
};

enum
{
    blocked,
    active
};

//input file
char c_fileName[256] = "account.txt";

typedef struct tagAccount
{
    char m_username[MAX];
    char m_password[MAX];
    int m_status;
    int m_isLogin;
    int m_numOfFalseLogin;
    struct tagAccount *next;
} account;

account *head = NULL;
account *current = NULL;

//print all linked list
void PrintList()
{
    account *ptr = head;
    printf("\n");
    while (ptr != NULL)
    {
        printf("Name: %-15s - Pass: %-10s - Status:%d - IsLogin:%d - NumOfFalseLogin:%d\n", ptr->m_username, ptr->m_password, ptr->m_status, ptr->m_isLogin, ptr->m_numOfFalseLogin);
        ptr = ptr->next;
    }
}

//insert a link to head of linked list
void InsertFirst(account acc)
{
    account *link = (account *)malloc(sizeof(account));
    strcpy(link->m_username, acc.m_username);
    strcpy(link->m_password, acc.m_password);
    link->m_status = acc.m_status;
    link->next = head;
    head = link;
}

//read data from file and add to linked list
//NOTE:: call this one time at the beginning of program
void ReadData()
{
    account acc;
    int i = 0;
    FILE *f;
    //r == read file text
    if (!(f = fopen(c_fileName, "r")))
    {
        printf("\n File not found!! \n\n");
    }
    else
    {
        while (!feof(f))
        {
            fscanf(f, "%s %s %d", acc.m_username, acc.m_password, &acc.m_status);
            acc.m_isLogin = 0;
            acc.m_numOfFalseLogin = 0;
            InsertFirst(acc);
        }
    }
    fclose(f);
}

//rewrite data after any change.
// Only use this if used file is really short
void ReWriteData()
{
    FILE *f;
    if (!(f = fopen(c_fileName, "w")))
    {
        printf("\n File not found!! \n\n");
    }
    else
    {
        account *ptr;
        ptr = head;
        while (ptr != NULL)
        {
            if (ptr->next != NULL)
                fprintf(f, "%s %s %d\n", ptr->m_username, ptr->m_password, ptr->m_status);
            else
                fprintf(f, "%s %s %d", ptr->m_username, ptr->m_password, ptr->m_status);
            ptr = ptr->next;
        }
    }
    fclose(f);
}

//check if usename is in list
int isExisting(char *input)
{
    int isExist = false;
    account *ptr;
    ptr = head;
    while (ptr != NULL)
    {
        if (strcmp(input, ptr->m_username) == 0)
            isExist = true;
        ptr = ptr->next;
    }
    return isExist;
}

//sign in function
void UserSignIn(int conn_sock, char username[], char password[])
{
    int isDone = false;
    printf("---Sign in---\n");
    printf("Username: %s\n", username);
    printf("Password: %s\n", password);

    do
    {
        account *ptr;
        ptr = head;
        int isExist = false;
        while (ptr != NULL)
        {
            //check if username is exist or not
            if (strcmp(username, ptr->m_username) == 0)
            {
                //if username is found in list,
                isExist = true;
                //check user status
                if (ptr->m_status == blocked)
                {
                    char *msg = "This account is blocked\n ";
                    send(conn_sock, msg, strlen(msg), 0);
                    printf("Account \"%s\" is blocked.\n", ptr->m_username);
                }
                /* else if (ptr->m_isLogin == true)
                {
                    char *msg = "This account is allready login\n ";
                    send(conn_sock, msg, strlen(msg), 0);
                    printf("Account \"%s\" is allready login\n", ptr->m_username);
                } */
                else
                {
                    //if this account is active, check password
                    if (strcmp(password, ptr->m_password) != 0)
                    {
                        ptr->m_numOfFalseLogin++;
                        char *msg = "Password is incorrect.\n";
                        //after 3 time wrong pass, set status to "blocked"
                        if (ptr->m_numOfFalseLogin == 3)
                        {
                            ptr->m_status = blocked;
                            isDone = true;
                            ptr->m_numOfFalseLogin = 0;
                            //change data in file txt (status)
                            ReWriteData();
                            char firstloglocation[BUFF_SIZE];
                            strcpy(firstloglocation, msg);
                            msg = strcat(firstloglocation, "Password is incorrect 3 times. You are blocked\n");
                            send(conn_sock, msg, strlen(msg), 0);
                            printf("Password is incorrect 3 times. Account \"%s\" is blocked.\nContract administrator for more information.\n", ptr->m_username);
                        }
                        else
                        {
                            send(conn_sock, msg, strlen(msg), 0);
                            printf("%s", msg);
                        }
                    }
                    //login success
                    else
                    {
                        isDone = true;
                        ptr->m_isLogin = true;
                        ptr->m_numOfFalseLogin = 0;
                        char *msg = "Hello! Successful login.\n";
                        //strcat(msg, ptr->m_username);
                        send(conn_sock, msg, strlen(msg), 0);
                        printf("Done, sign in success\n");
                    }
                }
            }
            ptr = ptr->next;
        }
        if (isExist == false)
        {
            char *msg = "This Account not exist!\n";
            send(conn_sock, msg, strlen(msg), 0);
            printf("Account \"%s\" is not exist.\n", username);
            isDone = true;
        }
    } while (isDone = false);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("error, too many or too few arguments\n");
        printf("Correct format is /.server YourPort\n");
        return 1;
    }
    ReadData();
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

                char username[BUFF_SIZE];
                char password[BUFF_SIZE];
                memset(username, 0, strlen(username));
                memset(password, 0, strlen(password));
                printf("Receiving data ...\n");
                //receives username from client
                recv(sockfd, username, BUFF_SIZE, 0);
                send(sockfd, "Success username\n", sizeof("Success username\n"), 0);
                username[strlen(username) - 1] = '\0';
                //receives password from client
                recv(sockfd, password, BUFF_SIZE, 0);
                send(sockfd, "Success password\n", sizeof("Success password\n"), 0);
                password[strlen(password) - 1] = '\0';
                printf("data is here!\n");
                sleep(1);
                UserSignIn(sockfd, username, password);
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

                if (--n_ready <= 0)
                    break; /* no more readable descriptors */
            }
        }
    }
    return 0;
}
