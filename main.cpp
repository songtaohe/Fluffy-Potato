#include <cstdio>
#include "server.h"
#include "hashTable.h"


int main(int argc, char** argv)
{
    ParallelHashTable *PHT_obj = new ParallelHashTable(65536);
    ParallelHashTable *PHT_type = new ParallelHashTable(65536);

    Server* S1 = new Server(8001,NULL,PHT_obj, PHT_type);
    Server* S2 = new Server(8002,NULL,PHT_obj, PHT_type);


    S1->Start();
    S2->Start();


    
    S1->Wait();
    S2->Wait();


}
