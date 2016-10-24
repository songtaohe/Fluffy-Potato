#include "server.h"
#include "index.h"

Server::Server(int port, char* configfile, ParallelHashTable *pht_obj, ParallelHashTable * pht_type, Cluster * cluster)
{
    this->s_port = port;
    this->pht_obj = pht_obj;
    this->pht_type = pht_type;
    this->cluster = cluster;
}

int Server::Start(void)
{
    pthread_create(&(this->serverThread), NULL, &server_thread, (void*)this);   
}

int Server::Wait(void)
{
    pthread_join(this->serverThread, NULL);
}




int Server::CreateType(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize)
{
    struct Header * h = (struct Header *)(buf);
    struct CreateTypeHeader *cth = (struct CreateTypeHeader*)(&(h->cmd_header));
    
    printf("Server Receive %s\n", &(cth->typeName));
    
    IndexBase * base = NULL;
    int size = 0;

    if(cth->index_type == INDEX_LIST)
    {
        IndexList* index = new IndexList(cth->shape_type);
        base = (IndexBase*)index;
        size = sizeof(index);
    }    
    
    void* hashEntity = this->pht_type->Insert(&(cth->typeName),base,size);

    if(hashEntity == NULL)
    {
        delete base;
        memcpy(this->sendbuf,"Name Existed!\0",13);
    }
    else
    {
        memcpy(this->sendbuf,"Type Created!\0",13);
    }
    sendto(this->s_sockfd, this->sendbuf, 13, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);
}


int Server::StoreObject(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize)
{
    struct Header * h = (struct Header *)(buf);
    struct StoreObjectHeader *soh = (struct StoreObjectHeader*)(&(h->cmd_header));
    
    //TODO
    
    //Check Type  index?  readonly?

    //Insert to obj hash
    //  existed? --> overwrite?
    
    //Insert to index

    //feedback
}


int Server::MessageHandler(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize)
{
    buf[count] = 0;
    printf("%s\n",buf);

    switch(buf[0]){
        case CMD_CREATE_TYPE : this->CreateType(buf, count, clientAddr, clientAddrSize); break;
        case CMD_STORE_OBJ : this->StoreObject(buf, count, clientAddr, clientAddrSize); break;
    



        default:
            break;
    }    

    /*
    if(this->s_port == 8001)
        this->pht_obj->Insert(buf,NULL,0);
    else
    {
        struct HashTableEntry* result = this->pht_obj->Query(buf);
        if(result == NULL) printf("Not found!\n");
        else printf("Found %lu %s\n",result->Key, result->name);
    }

    */
    
}


void* server_thread(void* context)
{
    Server *_this_ = (Server*)context;

    _this_->s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_this_->s_sockfd < 0)
    { 
        fprintf(stderr, "ERROR opening socket\n");
        return NULL;
    }

    memset(&(_this_->s_addr), 0, sizeof(_this_->s_addr));
    _this_->s_addr.sin_family = AF_INET;
    _this_->s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _this_->s_addr.sin_port = htons((unsigned short)(_this_->s_port));

    if (bind(_this_->s_sockfd, (struct sockaddr *) &(_this_->s_addr), 
       sizeof(_this_->s_addr)) < 0)
    { 
        fprintf(stderr, "ERROR on binding");
        return NULL;
    }

    while (1) {
        struct sockaddr_in clientaddr;
        socklen_t clientlen;
        int n = recvfrom(_this_->s_sockfd, _this_->recvbuf, MAX_BUFFER_SIZE, 0,
         (struct sockaddr *) &clientaddr, &clientlen);
        if(n>0)
            _this_->MessageHandler(_this_->recvbuf, n, clientaddr, clientlen);
    }
    
}
