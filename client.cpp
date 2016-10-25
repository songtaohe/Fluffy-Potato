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


int Client::QueryObjectRange(char* t_name, struct Rect range, char** result)
{
    struct Header * h = (struct Header *)(this->sendbuf);
    struct QueryObjectRangeHeader *qorh = (struct QueryObjectRangeHeader*)(&(h->cmd_header));
    int size = 0;
    struct sockaddr_in s_addr;
    socklen_t slen = 0;
    int ret;
    // Create Packet
    h->cmd = CMD_QUERY_OBJECT_RANGE;
    qorh->typeNameLength = strlen(t_name) + 1;
    qorh->queryRect = range;
    memcpy(&(qorh->data),t_name,strlen(t_name));
    ((char*)&(qorh->data))[qorh->typeNameLength - 1] = 0;
    size = sizeof(struct Header) + sizeof(struct QueryObjectRangeHeader) + qorh->typeNameLength - 2;

    sendto(this->thisNode->node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->thisNode->node_addr), sizeof(this->thisNode->node_addr));
    ret = recvfrom(this->thisNode->node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);

    if(ret<=0) return RET_ERROR;
    
    unsigned int *count = (unsigned int *)(this->recvbuf);
    *result = (char*)malloc(sizeof(char)*(ret-sizeof(unsigned int)));
    memcpy(*result, (this->recvbuf)+sizeof(unsigned int), ret-sizeof(unsigned int));
    printf("Query Result %d:\n%s\n",*count, *result);
    return *count;
}

int Client::LoadObject(char* t_name, char* sub_name, void** result, int* length)
{
    struct Header *h = (struct Header *)(this->sendbuf);
    struct LoadObjectHeader *loh = (struct LoadObjectHeader*)(&(h->cmd_header));
    int size = 0;
    struct sockaddr_in s_addr;
    socklen_t slen = 0;
    int ret;
    char* ptr;

    h->cmd = CMD_LOAD_OBJ;
    loh->typeNameLength = strlen(t_name);
    loh->subNameLength = (sub_name == NULL)?0:strlen(sub_name);
    ptr = (char*)&(loh->data);
    ptr += sprintf(ptr,"%s",t_name);
    if(sub_name!=NULL)
    {
        ptr += sprintf(ptr,"@%s",sub_name);
    }

    *ptr=0;
    ptr++;


    size = sizeof(struct Header) + sizeof(struct LoadObjectHeader) + loh->typeNameLength + loh->subNameLength + 1 - 2;

    printf("Client::LoadObject %d %s\n",size, &(loh->data));

    sendto(this->thisNode->node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->thisNode->node_addr), sizeof(this->thisNode->node_addr));
    ret = recvfrom(this->thisNode->node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);

    if(ret<=0) return RET_ERROR;

    *result = malloc(ret);
    memcpy(*result, (this->recvbuf), ret);
    *length = ret;

    printf("LoadObject %d %s\n",ret,*result);

    return RET_SUCCESS;
}





