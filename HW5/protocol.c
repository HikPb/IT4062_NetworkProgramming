#include "protocol.h"
#include "status.h"

int copyMess(Message* mess, Message temp) {
  mess->type = temp.type;
  mess->requestId = temp.requestId;
  mess->length = temp.length;
  memcpy(mess->payload, temp.payload, temp.length+1);
  return 1;
}

int printMess(Message mess) {
  printf("requestId: %d\n", mess.requestId);
  printf("length: %d\n", mess.length);
  printf("payload: %s\n", mess.payload);
  return 1;
}

int sendMsg(int socket, Message msg){
  int dataLength, nLeft, idx;
  nLeft = sizeof(Message);
  idx = 0;
  while (nLeft > 0){
    dataLength = send(socket, &((char*)&msg)[idx], nLeft, 0);
    if (dataLength < 0) return dataLength;
    nLeft -= dataLength;
    idx += dataLength;
  }

  return sizeof(Message);
}

int recvMsg(int socket, Message *msg){

  char recvBuff[BUFF_SIZE];
  int ret, nLeft, idx, bytes_recv;
  Message recvMessage;
  // init value for variables
  ret = 0;
  idx = 0;
  nLeft = sizeof(Message);

  while (nLeft > 0) {
    // if sizeof message big than buff_size// bytes receive = buffsize else nleft
    bytes_recv = nLeft > BUFF_SIZE ? BUFF_SIZE : nLeft;
    ret = recv(socket, recvBuff, bytes_recv, 0);
    // if recv <= 0 return ret <= 0
    if (ret <= 0) return ret;

    //clone ret to message with idx position
    memcpy(&(((char*)&recvMessage)[idx]), recvBuff, ret); 
    
    idx += ret;
    // set number of bytes of message left 
    nLeft -= ret;
    
  }

  // copy message to msg
  copyMess(&(*msg), recvMessage);
  return sizeof(Message);
}

int sendMessage(int conn_sock, Message msg) {
  int bytes_sent = sendMsg(conn_sock, msg);
  if(bytes_sent <= 0){
    printf("\nConnection closed!\n");
    close(conn_sock);
    return -1;
  }
  return 1;
}

int receiveMessage(int conn_sock, Message *msg) {
  int bytes_received;
  bytes_received = recvMsg(conn_sock, msg);
  if (bytes_received <= 0){
    printf("\nConnection closed\n");
    return -1;
  }
  return 1;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        if((count - 1)) {
          assert(idx == count - 1);
        }
        *(result + idx) = 0;
    }

    return result;
}

char* getHeaderOfPayload(char* payload) {
  if(strlen(payload)) 
    return str_split(payload, '\n')[0];
  return NULL;
}

void sendWithCode(Message mess,StatusCode code, int sockfd) {
    char msgCode[200];
    Message *newMess = (Message*)malloc(sizeof(Message));
    newMess->type = mess.type;
    newMess->requestId = mess.requestId;
    messageCode(code, msgCode);
    sprintf(newMess->payload,"%d : %s", code, msgCode);
    // strcat(newMess->payload, msgCode);
    newMess->length = strlen(newMess->payload);
    sendMessage(sockfd, *newMess);
}
