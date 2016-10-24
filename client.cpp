#include "client.h"

Client::Client(Cluster * cluster)
{
    this->cluster = cluster;
    this->sendbuf = (char*)malloc(MAX_BUFFER_SIZE);
    this->recvbuf = (char*)malloc(MAX_BUFFER_SIZE);
    this->thisNode = &(this->cluster->nodelist[this->cluster->thisNode]);
}

int Client::CreateType(char* name,int shape_type, int index_type, int flag)
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
    cth->shape_type = shape_type;
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
    printf("CreateType result: %s\n",this->recvbuf);


    return RET_SUCCESS;   
}


int Client::StoreObject(char* t_name, char* sub_name, union Shape shape, void* buf, int length)
{   
    struct Header * h = (struct Header *)(this->sendbuf);
    struct StoreObjectHeader * soh = (struct StoreObjectHeader*)(&(h->cmd_header));
    int size = 0;
    struct sockaddr_in s_addr;
    socklen_t slen = 0;
    int ret;
    char* ptr;

    h->cmd = CMD_STORE_OBJ;
    soh->typeNameLength = strlen(t_name);
    soh->subNameLength = (sub_name == NULL)?0:strlen(sub_name);
    soh->objSize = length;
    soh->shape = shape;
    soh->segmentSize = length;
    soh->segmentId = 1;
    soh->segmentTotal = 1;
    ptr = (char*)&(soh->data);
    ptr += sprintf(ptr,"%s",t_name);
    if(sub_name!=NULL)
    {
        ptr += sprintf(ptr,"@%s",sub_name);
    }

    *ptr=0;
    ptr++;
   
    soh->dataOffset = ptr - (char*)&(soh->data); 
    memcpy(ptr,buf,length);
    
    size = sizeof(struct Header) + sizeof(struct StoreObjectHeader) + soh->typeNameLength + soh->subNameLength + 1 + length - 2;    

    sendto(this->thisNode->node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->thisNode->node_addr), sizeof(this->thisNode->node_addr));

    ret = recvfrom(this->thisNode->node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);

    if(ret<= 0) return RET_ERROR;

    //Test
    this->recvbuf[ret] = 0;
    printf("StoreObject result: %s\n",this->recvbuf);


    return RET_SUCCESS;
}

