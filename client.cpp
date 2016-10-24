#include "client.h"

Client::Client(Cluster * cluster)
{
    this->cluster = cluster;
    this->sendbuf = (char*)malloc(MAX_BUFFER_SIZE);
    this->recvbuf = (char*)malloc(MAX_BUFFER_SIZE);
    this->thisNode = &(this->cluster->nodelist[this->cluster->thisNode]);
}

int Client::CreateType(char* name, int index_type, int flag)
{
    struct Header * h = (struct Header *)(this->sendbuf);
    struct CreateTypeHeader *cth = (struct CreateTypeHeader*)(&(h->cmd_header));
    int size = 0;
    struct sockaddr_in s_addr;
    socklen_t slen = 0;
    int ret;
    // Create Packet
    h->cmd = CMD_CREATE_TYPE;
    cth->index_type = index_type;
    cth->flag = flag;
    cth->typeNameLength = strlen(name) + 1;
    memcpy(&(cth->typeName),name,strlen(name));
    ((char*)&(cth->typeName))[cth->typeNameLength - 1] = 0;

    size = sizeof(struct Header) + sizeof(struct CreateTypeHeader) + cth->typeNameLength - 2;

    sendto(this->thisNode->node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->thisNode->node_addr), sizeof(this->thisNode->node_addr));

    ret = recvfrom(this->thisNode->node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);
 
    if(ret<= 0) return RET_ERROR;

    //Test
    this->recvbuf[ret] = 0;
    printf("Create Type Successful %s\n",this->recvbuf);


    return RET_SUCCESS;   
}


