#include "client.h"

Client::Client(Cluster * cluster)
{
    this->cluster = cluster;
    this->sendbuf = (char*)malloc(MAX_BUFFER_SIZE);
    this->recvbuf = (char*)malloc(MAX_BUFFER_SIZE);
    this->thisNode = &(this->cluster->nodelist[this->cluster->thisNode]);

    //Set timeout of receiver to 1 second.
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(this->thisNode->node_socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
}


//************   API Primitives  *****************//

int Client::__CreateType(char* name,int shape_type, int index_type, int flag, int node)
{
    struct Header * h = (struct Header *)(this->sendbuf);
    struct CreateTypeHeader *cth = (struct CreateTypeHeader*)(&(h->cmd_header));
    int size = 0;
    struct sockaddr_in s_addr;
    socklen_t slen = sizeof(s_addr);
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

    sendto(this->cluster->nodelist[node].node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->cluster->nodelist[node].node_addr), sizeof(this->cluster->nodelist[node].node_addr));
    ret = recvfrom(this->cluster->nodelist[node].node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);
 
    if(ret<= 0) return RET_ERROR;
    if(ret==1) return RET_FAILED;
    //Test
    this->recvbuf[ret] = 0;
    if(DEBUG) printf("CreateType result: %s\n",this->recvbuf);


    return RET_SUCCESS;   
}


int Client::__StoreObject(char* t_name, char* sub_name, union Shape shape, void* buf, int length, int node)
{   
    struct Header * h = (struct Header *)(this->sendbuf);
    struct StoreObjectHeader * soh = (struct StoreObjectHeader*)(&(h->cmd_header));
    int size = 0;
    struct sockaddr_in s_addr;
    socklen_t slen = sizeof(s_addr);
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

    //sendto(this->thisNode->node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->thisNode->node_addr), sizeof(this->thisNode->node_addr));

    //ret = recvfrom(this->thisNode->node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);

    sendto(this->cluster->nodelist[node].node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->cluster->nodelist[node].node_addr), sizeof(this->cluster->nodelist[node].node_addr));
    ret = recvfrom(this->cluster->nodelist[node].node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);



    if(ret<= 0) return RET_ERROR;
    if(ret==1) return RET_FAILED;
    //Test
    this->recvbuf[ret] = 0;
    if(DEBUG) printf("StoreObject result: %s\n",this->recvbuf);


    return RET_SUCCESS;
}


int Client::__QueryObjectRange(char* t_name, struct Rect range, char** result, int node)
{
    struct Header * h = (struct Header *)(this->sendbuf);
    struct QueryObjectRangeHeader *qorh = (struct QueryObjectRangeHeader*)(&(h->cmd_header));
    int size = 0;
    struct sockaddr_in s_addr;
    socklen_t slen = sizeof(s_addr);
    int ret;
    // Create Packet
    h->cmd = CMD_QUERY_OBJECT_RANGE;
    qorh->typeNameLength = strlen(t_name) + 1;
    qorh->queryRect = range;
    memcpy(&(qorh->data),t_name,strlen(t_name));
    ((char*)&(qorh->data))[qorh->typeNameLength - 1] = 0;
    size = sizeof(struct Header) + sizeof(struct QueryObjectRangeHeader) + qorh->typeNameLength - 2;

    //sendto(this->thisNode->node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->thisNode->node_addr), sizeof(this->thisNode->node_addr));
    //ret = recvfrom(this->thisNode->node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);

    sendto(this->cluster->nodelist[node].node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->cluster->nodelist[node].node_addr), sizeof(this->cluster->nodelist[node].node_addr));
    ret = recvfrom(this->cluster->nodelist[node].node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);


    if(ret<=0) return RET_ERROR;
    if(ret==1) return RET_FAILED;
    unsigned int *count = (unsigned int *)(this->recvbuf);
    *result = (char*)malloc(sizeof(char)*(ret-sizeof(unsigned int)));
    memcpy(*result, (this->recvbuf)+sizeof(unsigned int), ret-sizeof(unsigned int));
    if(DEBUG) printf("Query Result %d:\n%s\n",*count, *result);
    return *count;
}

int Client::__LoadObject(char* t_name, char* sub_name, void** result, int* length, int node)
{
    struct Header *h = (struct Header *)(this->sendbuf);
    struct LoadObjectHeader *loh = (struct LoadObjectHeader*)(&(h->cmd_header));
    int size = 0;
    struct sockaddr_in s_addr;
    socklen_t slen = sizeof(s_addr);
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

    if(DEBUG) printf("Client::LoadObject %d %s\n",size, &(loh->data));

    //sendto(this->thisNode->node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->thisNode->node_addr), sizeof(this->thisNode->node_addr));
    //ret = recvfrom(this->thisNode->node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);

    sendto(this->cluster->nodelist[node].node_socket, this->sendbuf, size, 0, (struct sockaddr *)&(this->cluster->nodelist[node].node_addr), sizeof(this->cluster->nodelist[node].node_addr));
    ret = recvfrom(this->cluster->nodelist[node].node_socket, this->recvbuf, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&s_addr, &slen);


    if(ret<=0) return RET_ERROR;
    if(ret==1) return RET_FAILED;
    *result = malloc(ret);
    memcpy(*result, (this->recvbuf), ret);
    *length = ret;

    if(DEBUG) printf("LoadObject %d %s\n",ret,*result);

    return RET_SUCCESS;
}


//*************   API  ***************

int Client::CreateType(char* name, int shape_type, int index_type, int flag)
{
    int ret = RET_SUCCESS;

    for(int node = 0; node < this->cluster->num; node ++)
    {
        int _ret;
        _ret = this->__CreateType(name,shape_type,index_type,flag,node);
        if(_ret==RET_ERROR)
        {
            _ret = this->__CreateType(name,shape_type,index_type,flag,node);
            if(_ret==RET_ERROR)
            {
                _ret = this->__CreateType(name,shape_type,index_type,flag,node);
                if(_ret==RET_ERROR)
                {
                    fprintf(stderr,"Error! Check the Network Connection!\n");
                    ret = RET_ERROR;
                }
            }
        }
    }

    return ret;
}

int Client::StoreObject(char* t_name, char* sub_name, union Shape shape, void* buf, int length)
{
    int ret;
    ret = this->__StoreObject(t_name, sub_name, shape, buf, length, this->cluster->thisNode);

    return ret;
}

int Client::QueryObjectRange(char* t_name, struct Rect range, char** result)
{
    int ret;
    struct Point loc;
    int nodeid;

    //This is Wrong!!!!!!!   TODO  TODO  TODO  FIXME FIXME FIXME
    loc.lat = (range.latmin + range.latmax)/2;
    loc.lon = (range.lonmin + range.lonmax)/2;
    nodeid = this->cluster->Query(loc) - 1;

    ret = this->__QueryObjectRange(t_name, range, result, nodeid);//FIXME Check partition!


    return ret;
}

int Client::LoadObject(char* t_name, char* sub_name, void** result, int* length)
{
    int ret;
    ret = this->__LoadObject(t_name, sub_name, result, length, this->cluster->thisNode);
    if(ret == RET_FAILED)
    {
        for(int node = 0; node < this->cluster->num; node ++)  // Check other node 
        if(node!=this->cluster->thisNode)
        {
            ret = this->__LoadObject(t_name, sub_name, result, length, node);
            if(ret == RET_SUCCESS)
                return ret;
        }
    }

    return ret;
}




