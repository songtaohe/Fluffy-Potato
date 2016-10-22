#include <cstdio>
#include <pthread.h>
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

class Server
{
public:
    Server(int port, char* configfile);
    
    int MessageHandler(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize);        
   
    int Start();
    int Wait();

//*** Server Info ***     		
    char recvBuffer[MAX_BUFFER_SIZE];
    int s_sockfd;
    int s_port;
    struct sockaddr_in s_addr;

//pthread
    pthread_t serverThread;   


};


void* server_thread(void* context);

