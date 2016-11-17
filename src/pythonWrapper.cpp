#include "common.h"
#include "client.h"
#include "cluster.h"

Cluster *cluster;
Client* C1;

int isInit = 0;

extern "C"
{

static int init()
{
    if(isInit == 1) return isInit;
    //TODO get the name and config from /usr/local/XXX/X
    char nodename[256];    
    FILE *fp = fopen(PATH_TO_NODEID,"rt");
    printf("We are here !!! %s\n",PATH_TO_NODEID);
    if(fp == NULL)
    {
      printf("Can not open file %s\n",PATH_TO_NODEID);
    }
    fscanf(fp,"%s",nodename);
    fclose(fp);


    printf("This node is %s\n",nodename);

    cluster = new Cluster(nodename,PATH_TO_CONFIG);
    C1 = new Client(cluster);    

    isInit = 1;

    return isInit;
}


int CreateType(char* t_name, int shape, int index, int flag)
{
    init();
    printf("wrapper -> %s\n",t_name);
    return C1->CreateType(t_name,shape,index,flag);
}

int StoreObject(char* t_name, char* sub_name, double lat, double lon, double lat2, double lon2, char* obj)
{
    init();
    union Shape shape;
    shape.point.lat = lat;
    shape.point.lon = lon;
	shape.rect.latmax = lat2;
	shape.rect.lonmax = lon2;
    
    return C1->StoreObject(t_name, sub_name, shape, obj, strlen(obj));
} 

int StoreArray(char* t_name, char* sub_name, int sx, int sy, int wx, int wy, int op1, int op2, float* data, int len)
{
	int size = sizeof(struct Array2DHeader) + len*sizeof(float);
	static char buf[MAX_BUFFER_SIZE];
	struct Array2DHeader * h = (struct Array2DHeader*)buf;
	float* dp = (float*)&(h->data);
	union Shape shape;
	init();

	h->sx = sx;
	h->sy = sy;
	h->wx = wx;
	h->wy = wy;	
	h->op1 = op1;
	h->op2 = op2;

    //data[0] = data[0] + 1;

    printf("StoreArray %d %d %d %d %d %d size %d\n",h->sx,h->sy,h->wx, h->wy, h->op1, h->op2,size);
	memcpy(dp, data, len*sizeof(float));

    //for(int i = 0;i<len;i++) printf("[%d %.2f]\n",i,data[i]);

	return C1->StoreObject(t_name, sub_name, shape, buf, size);
}



int QueryObjectRange(char* t_name, double latmin, double lonmin, double latmax, double lonmax, char** result, int* count)
{
    int ret;
    init();
    union Shape shape;
    shape.rect.latmin = latmin;
    shape.rect.lonmin = lonmin;
    shape.rect.latmax = latmax;
    shape.rect.lonmax = lonmax;
    
    return C1->QueryObjectRange(t_name,shape.rect,result);
}


int LoadObject(char* t_name, char* sub_name, char** obj)
{
    int length;
    init();
    return C1->LoadObject(t_name,sub_name, (void**)obj, &length);
}

int LoadArray(char* t_name, char* sub_name, float* data, int* dimx, int* dimy)
{
	//Use LoadObject and do staff in Python?
    struct Array2DHeader * h = NULL;    
    int length = 0;
    int ret = C1->LoadObject(t_name,sub_name, (void**)(&h), &length);
    printf("ret %d\n",ret);

    if(ret == 0)
    {
        printf("size %d %d\n",h->wx,h->wy);
        memcpy(data,&(h->data),sizeof(float)*h->wx*h->wy);
    }

    return ret;
}


}
