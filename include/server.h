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
#include "hashTable.h"
#include "cluster.h"

class Server
{
public:
    Server(int port, char* configfile, ParallelHashTable *pht_obj, ParallelHashTable * pht_type, Cluster * cluster);
    
    int MessageHandler(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize);        
   
    int Start();
    int Wait();

//  Core API Functions
    int CreateType(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize);
    int StoreObject(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize);
    int QueryObjectRange(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize);
    int LoadObject(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize);





//*** Server Info ***     		
    char recvbuf[MAX_BUFFER_SIZE];
    char sendbuf[MAX_BUFFER_SIZE];// Should be changed to sendbuf....TODO
    int s_sockfd;
    int s_port;
    struct sockaddr_in s_addr;

//pthread
    pthread_t serverThread;   

//Hash Table
    ParallelHashTable *pht_obj = NULL;
    ParallelHashTable *pht_type = NULL;

//Cluster Configuration
    Cluster * cluster;


};


void* server_thread(void* context);
