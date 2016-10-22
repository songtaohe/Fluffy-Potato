#include "common.h"
#include "interface.h"

#ifndef ClusterHeader
#define ClusterHeader

class Node
{
public: 
    Node(char* name, char* ip, char* baseport);

char* name;
char* ip;
char* baseport;



};

class Cluster
{
public:
    Cluster(char* thisNode, char* configFile);
    int Query(struct Point location); //Partition....   

    int num;
    Node *nodelist;
    int thisNode;  
};

#endif
