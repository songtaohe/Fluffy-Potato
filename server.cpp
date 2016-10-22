#include "server.h"

Server::Server(int port, char* configfile)
{
    this->s_port = port;
}

int Server::Start(void)
{
    pthread_create(&(this->serverThread), NULL, &server_thread, (void*)this);   
}

int Server::Wait(void)
{
    pthread_join(this->serverThread, NULL);
}

int Server::MessageHandler(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize)
{
    buf[count] = 0;
    printf("%s\n",buf);
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
        int n = recvfrom(_this_->s_sockfd, _this_->recvBuffer, MAX_BUFFER_SIZE, 0,
         (struct sockaddr *) &clientaddr, &clientlen);

        _this_->MessageHandler(_this_->recvBuffer, n, clientaddr, clientlen);
    }
    
}
