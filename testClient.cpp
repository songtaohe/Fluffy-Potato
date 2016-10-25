#include "common.h"
#include "interface.h"
#include "cluster.h"
#include "client.h"

int main(int argc, char** argv)
{
    //if(argc!=5)
    //{
    //    printf("Usage: ./Client NodeName configFile type subname\n");
    //    return 0;
    //}

    Cluster *cluster = new Cluster(argv[1],argv[2]);
    Client* C1 = new Client(cluster);


    if(argv[3][0] == '1')
    {
        C1->CreateType(argv[4],SHAPE_POINT, INDEX_LIST, OBJ_READONLY);
    }

    if(argv[3][0] == '2')
    {
        union Shape shape;
        shape.point.lat = atoi(argv[4]);
        shape.point.lon = atoi(argv[5]);
        C1->StoreObject(argv[6],argv[7],shape,(void*)argv[8],strlen(argv[8])+1);
    }

    if(argv[3][0] == '3')
    {
        struct Rect range;
        char* r;
        range.latmin = atoi(argv[4]);
        range.lonmin = atoi(argv[5]);
        range.latmax = atoi(argv[6]);
        range.lonmax = atoi(argv[7]);

        C1->QueryObjectRange(argv[8],range,&r);
    }

    if(argv[3][0] == '4')
    {
        int length;
        void* buf;
        C1->LoadObject(argv[4],argv[5],&buf, &length);
    }

    return 0;




    C1->CreateType(argv[3],SHAPE_POINT, INDEX_LIST, OBJ_READONLY);
    union Shape shape;
    shape.point.lat = 10;
    shape.point.lon = 10;

    C1->StoreObject(argv[3],argv[4],shape,(void*)"Hello\0",6);

    return 0;
}

