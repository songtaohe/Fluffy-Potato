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
    cluster = new Cluster("node1","/home/songtao/Mapmaking/Fluffy-Potato/Cluster.cfg");
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

int StoreObjectPoint(char* t_name, char* sub_name, double lat, double lon, char* obj)
{
    init();
    union Shape shape;
    shape.point.lat = lat;
    shape.point.lon = lon;
    
    return C1->StoreObject(t_name, sub_name, shape, obj, strlen(obj));
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
    return C1->LoadObject(t_name,sub_name, (void**)obj, &length);
}



}
