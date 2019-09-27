#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int ipToHostname(char *,struct in_addr);
int hostnameToIp(char *);

int main(int argc, char *argv[])
{
    struct in_addr ip;

    if (argc != 2) {
        printf("error: wrong form\n");
        return 1;
    }

    if (!inet_pton(AF_INET,argv[1],&ip)) {
        return hostnameToIp(argv[1]);
    }else return ipToHostname(argv[1],ip);
}

int hostnameToIp(char *param){
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(param)) == NULL) {  
        herror("gethostbyname");
        return 1;
    }
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

int ipToHostname(char *param, struct in_addr ip){
    
    struct hostent *hp;
    if ((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL) {
        herror("gethostbyaddr");
        return 1;
    }
    printf("IP: %s\n",param);
    printf("    Official name: %s\n",hp->h_name);
    printf("    Alias name:\n");
    for(int i =0; hp->h_aliases[i]!=NULL;i++){
        printf("    %s\n", hp->h_aliases[i]);

    }
    return 0;
}