#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int ipToHostname(char *param);
int hostnameToIp(char *param, struct in_addr ip);

int main(int argc, char *argv[])
{
    struct in_addr ip;

    if (argc != 2) {
        fprintf(stderr,"usage: ghbn hostname\n");
        return 1;
    }

    if (!inet_pton(AF_INET,argv[1],&ip)) {
        ipToHostname(argv[1]);
    }else hostnameToIp(argv[1],ip);
    return 0;
}

int ipToHostname(char *param){
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(param)) == NULL) {  // get the host info
        herror("gethostbyname");
        return 1;
    }
    // print information about this host:
    printf("Hostname: %s\n", he->h_name);
    addr_list = (struct in_addr **)he->h_addr_list;
    printf("    Official IP: %s\n",inet_ntoa(*addr_list[0]));
    printf("    Alias IP: ");
    for(int i = 1; addr_list[i] != NULL; i++) {
        printf("%s ", inet_ntoa(*addr_list[i]));
    }
    printf("\n");   
    return 0;
}

int hostnameToIp(char *param, struct in_addr ip){
    
    struct hostent *hp;
    //struct in_addr **host_list;

    if ((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL) {
        herror("gethostbyaddr");
        return 1;
    }
    //host_list = (struct in_addr**)hp->h_aliases;
    printf("IP: %s\n",param);
    printf("    Official name: %s\n",hp->h_name);
    printf("    Alias name: %s\n", hp->h_aliases[0]);
    return 0;
}