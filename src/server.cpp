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
    struct Type * _type = (struct Type*)malloc(sizeof(struct Type));    

    _type->index_type = cth->index_type;
    _type->shape_type = cth->shape_type;
    _type->flag = cth->flag;
    _type->indexbase = NULL;


    IndexBase * base = NULL;
    int size = 0;

    if(cth->index_type == INDEX_LIST)
    {
        IndexList* index = new IndexList(cth->shape_type);
        base = (IndexBase*)index;
        _type->indexbase = base;
        printf("Set indexbase to %p\n",base);
    }    


    if(cth->index_type == INDEX_RTREE)
    {
        IndexRTree* index = new IndexRTree(cth->shape_type);
        base = (IndexBase*)index;
        _type->indexbase = base;
        printf("Set indexbase to %p\n",base);
    }    

    void* hashEntity = this->pht_type->Insert(&(cth->typeName),_type,sizeof(struct Type));

    if(hashEntity == NULL)
    {
        delete base;
        this->sendbuf[0] = 'e';
        sendto(this->s_sockfd, this->sendbuf, 1, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);
    }
    else
    {
        memcpy(this->sendbuf,"Type Created!\0",13);
        sendto(this->s_sockfd, this->sendbuf, 13, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);
    }
}


int ArrayAction(struct HashTableEntry * input, void* arg)
{
	struct Array2DHeader * h = (struct Array2DHeader*)arg;

	if(h->op1 == ARRAY2D_OP_INIT)
	{
        printf("Initialize Array %d %d\n",h->wx, h->wy);
		if(input->data != NULL)
			free(input->data);

		input->data = (char*)malloc(sizeof(struct Array2DHeader) + (h->wx * h->wy) * sizeof(float));
        input->datasize = sizeof(struct Array2DHeader) + (h->wx * h->wy) * sizeof(float);
        ((struct Array2DHeader *)(input->data))->wx = h->wx;
        ((struct Array2DHeader *)(input->data))->wy = h->wy;
    
		for(int i=0;i<(h->wx * h->wy);i++) ((float*)(&((struct Array2DHeader*)(input->data))->data))[i] = 0.0f;		
	}
	else
	{
        printf("Array Atomic Add\n");
		float* dst = (float*)(&((struct Array2DHeader*)(input->data))->data);
		float* src = (float*)(&(h->data));
		int dim = ((struct Array2DHeader*)(input->data))->wy;
        printf("%d %d %d %d\n",h->sx, h->sy, h->wx, h->wy);
		for(int i = h->sx; i<h->sx + h->wx; i++)
		for(int j = h->sy; j<h->sy + h->wy; j++)
		{
			dst[i*dim + j] += src[(i-h->sx)*(h->wy) + j-h->sy]; //TODO Let's only do add now
            //printf("Result A(%d, %d) = %.2f   += %.2f\n", i,j, dst[i*dim + j],src[(i-h->sx)*(h->wy) + j - h->sy]);
		}
	}

	return 0;
}




int Server::StoreObject(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize)
{
    struct Header * h = (struct Header *)(buf);
    struct StoreObjectHeader *soh = (struct StoreObjectHeader*)(&(h->cmd_header));
    int ret = RET_SUCCESS;

    char t_name[256];
    char full_name[256];

    memcpy(t_name, &(soh->data), soh->typeNameLength);
    t_name[soh->typeNameLength] = 0;

    printf("t_name %s\n",t_name);

    memcpy(full_name, &(soh->data), soh->typeNameLength + soh->subNameLength + 1);
    full_name[soh->typeNameLength + soh->subNameLength + 1] = 0;

    printf("full_name %s\n",full_name);
    printf("data %s\n",&(soh->data) + soh->dataOffset);


    //TODO    
    //Check Type  index?  readonly?
    struct HashTableEntry* h_type = this->pht_type->Query(t_name);
    printf("Mark query type %p\n",h_type);
    if(h_type == NULL) 
    {
        fprintf(stderr, "StoreObject error: Type Undefined\n");
        ret = RET_ERROR;
    }
    else
    {

        struct Type *_type = (struct Type*)(h_type->data);
        struct HashTableEntry* h_obj = NULL;
		
		if(_type->shape_type == SHAPE_ARRAY)
		{
			h_obj = this->pht_obj->QueryAndAction(full_name,ArrayAction, (void*)(&(soh->data) + soh->dataOffset));
		}
		else
		{
			h_obj = this->pht_obj->Query(full_name);
		}

        printf("Query h_obj %p\n",h_obj);

        if(h_obj != NULL)
        {
            if(_type->flag & OBJ_READONLY !=0)
            {
                fprintf(stderr, "StoreObject error: Cannot Overwrite Readonly Object \n");
                ret = RET_ERROR;
            }
            else
            {
				if(_type->shape_type == SHAPE_ARRAY)
				{
					//We have nothing to do here now, due to we have already take the action.					
				}
				else
                {
					free(h_obj->data);
                	h_obj->data = malloc(soh->objSize);
                	memcpy(h_obj->data, &(soh->data) + soh->dataOffset, soh->objSize);
				}
                //TODO change position!!!

            }
        }
        else
        {
            printf("Mark Insert\n");
            void* _hash = this->pht_obj->Insert(full_name, &(soh->data) + soh->dataOffset, soh->objSize);

			if(_type->shape_type == SHAPE_ARRAY)
			{
                void* ptr = (&(soh->data) + soh->dataOffset);
                int op1 = ((struct Array2DHeader *)ptr)->op1;
                unsigned char wx = ((struct Array2DHeader *)ptr)->wx; 
                unsigned char wy = ((struct Array2DHeader *)ptr)->wy; 
            
                ((struct Array2DHeader *)ptr)->op1 = ARRAY2D_OP_INIT;
                ((struct Array2DHeader *)ptr)->wx = 64; // FIXME
                ((struct Array2DHeader *)ptr)->wy = 64; // FIXME
                this->pht_obj->QueryAndAction(full_name,ArrayAction, (void*)(&(soh->data) + soh->dataOffset));
        
                ((struct Array2DHeader *)ptr)->op1 = op1;
                ((struct Array2DHeader *)ptr)->wx = wx;
                ((struct Array2DHeader *)ptr)->wy = wy; 
   
				this->pht_obj->QueryAndAction(full_name,ArrayAction, (void*)(&(soh->data) + soh->dataOffset));
			}


            if(_type->index_type == INDEX_LIST)
            {
                struct IndexEntity IE;
                IE.shape = soh->shape;
                IE.toHash = _hash;
                IE.next = NULL;
                printf("Insert Index %p\n",_type->indexbase);
                
                _type->indexbase->Insert(&IE);
            }            
            if(_type->index_type == INDEX_RTREE)
            {
                struct IndexEntity IE;
                IE.shape = soh->shape;
                IE.toHash = _hash;
                IE.next = NULL;
                printf("Insert Index %p\n",_type->indexbase);
                
                _type->indexbase->Insert(&IE);
            }            
        }
        
    }
    printf("Mark ret\n");
    if(ret == RET_ERROR)
    {
        this->sendbuf[0] = 'e';
        sendto(this->s_sockfd, this->sendbuf, 1, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);

    }
    else
    {
        memcpy(this->sendbuf,"Successful!  \0",13);
        sendto(this->s_sockfd, this->sendbuf, 13, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);
    }



    //Insert to obj hash
    //  existed? --> overwrite?
    
    //Insert to index

    //feedback

}

int Server::QueryObjectRange(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize)
{
    struct Header * h = (struct Header *)(buf);
    struct QueryObjectRangeHeader *qorh = (struct QueryObjectRangeHeader*)(&(h->cmd_header));
    int ret = RET_SUCCESS;
    char t_name[256];
    int retsize = 0;

    memcpy(t_name, &(qorh->data), qorh->typeNameLength);
    t_name[qorh->typeNameLength] = 0;

    struct HashTableEntry* h_type = this->pht_type->Query(t_name);
    printf("Mark query type %p\n",h_type);
    if(h_type == NULL)
    {
        fprintf(stderr, "QueryObjectRange error: Type Undefined\n");
        ret = RET_ERROR;
    }
    else
    {
        struct Type *_type = (struct Type*)(h_type->data);
        if(_type->index_type != INDEX_NULL)
        {
            struct IndexEntity** IElist = _type->indexbase->QueryRect(qorh->queryRect,0);
            if(IElist != NULL)
            {
                struct IndexEntity* IEcur = *IElist;
                unsigned int *count = (unsigned int*)(this->sendbuf);
                *count = 0;
                char* ptr = (char*)(this->sendbuf)+sizeof(unsigned int);
                while(IEcur != NULL)
                {   
					if(IEcur->toHash == NULL) break;

                    (*count) ++;
                    ptr += sprintf(ptr,"%s\n",((struct HashTableEntry*)(IEcur->toHash))->name);
                    IEcur = IEcur -> next;
                } 
                *ptr = 0;
                ptr++;
                retsize = ptr - (this->sendbuf);
            }
            else
            {
                unsigned int *count = (unsigned int*)(this->sendbuf);
                char* ptr = (char*)(this->sendbuf)+sizeof(unsigned int);
                *count = 0;
                sprintf(ptr,"NULL\n\0");
                retsize = sizeof(unsigned int)+6;
            }
        }
        else
        {
            fprintf(stderr, "QueryObjectRange error: No index for type\n");
            ret = RET_ERROR;
        }
    }

    if(ret == RET_ERROR)
    {
        this->sendbuf[0] = 'e';
        sendto(this->s_sockfd, this->sendbuf, 1, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);
    }
    else
    {
        sendto(this->s_sockfd, this->sendbuf, retsize, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);
    }

    return ret;
}


int Server::LoadObject(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize)
{
    struct Header *h = (struct Header *)(buf);
    struct LoadObjectHeader *loh = (struct LoadObjectHeader*)(&(h->cmd_header));
    int retsize = 0;
    int ret = RET_SUCCESS;
    char full_name[256];

    memcpy(full_name, &(loh->data), loh->typeNameLength + loh->subNameLength + 1);
    full_name[loh->typeNameLength + loh->subNameLength + 1] = 0;

    printf("full_name %d %d %d  %s\n",count, loh->typeNameLength , loh->subNameLength ,full_name);

    struct HashTableEntry* h_obj = this->pht_obj->Query(full_name);
    if(h_obj == NULL)
    {
        fprintf(stderr, "LoadObject error: Object not Found\n");
        ret = RET_ERROR;
    }
    else
    {
        memcpy(this->sendbuf,h_obj->data, h_obj->datasize);
        retsize = h_obj->datasize;
    }

    if(ret == RET_ERROR)
    {
        this->sendbuf[0] = 'e';
        sendto(this->s_sockfd, this->sendbuf, 1, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);
    }
    else
    {
        sendto(this->s_sockfd, this->sendbuf, retsize, 0, (struct sockaddr *)&(clientAddr), clientAddrSize);
    }


    return ret;
}



int Server::MessageHandler(char* buf, int count,  struct sockaddr_in clientAddr, socklen_t clientAddrSize)
{
    buf[count] = 0;
    printf("%s\n",buf);

    switch(buf[0]){
        case CMD_CREATE_TYPE : this->CreateType(buf, count, clientAddr, clientAddrSize); break;
        case CMD_STORE_OBJ : this->StoreObject(buf, count, clientAddr, clientAddrSize); break;
        case CMD_QUERY_OBJECT_RANGE : this->QueryObjectRange(buf, count, clientAddr, clientAddrSize); break;
        case CMD_LOAD_OBJ : this->LoadObject(buf, count, clientAddr, clientAddrSize); break;
    



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

    _this_->s_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
        printf("waiting for data\n");
        clientlen = sizeof(clientaddr);
        int n = recvfrom(_this_->s_sockfd, _this_->recvbuf, MAX_BUFFER_SIZE, 0,
         (struct sockaddr *) &clientaddr, &clientlen);
        if(n>0)
            _this_->MessageHandler(_this_->recvbuf, n, clientaddr, clientlen);
        else
	{
	     printf("Receive %d %s\n",errno, strerror(errno));

        }
    }
    
}
