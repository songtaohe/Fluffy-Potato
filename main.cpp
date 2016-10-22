#include <cstdio>
#include "server.h"
#include "hashTable.h"
#include "cluster.h"

int main(int argc, char** argv)
{
    if(argc!=3)
    {
        printf("Usage: ./Server NodeName configFile\n"); 
        return 0;
    }

    Cluster *cluster = new Cluster(argv[1],argv[2]);

    ParallelHashTable *PHT_obj = new ParallelHashTable(65536);
    ParallelHashTable *PHT_type = new ParallelHashTable(65536);

    Server* S1 = new Server(8001,NULL,PHT_obj, PHT_type,cluster);
    Server* S2 = new Server(8002,NULL,PHT_obj, PHT_type,cluster);


    S1->Start();
    S2->Start();


    
    S1->Wait();
    S2->Wait();


}
