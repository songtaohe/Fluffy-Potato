#include "common.h"
#include "interface.h"
#include "cluster.h"

class Client
{
public:
    Client(Cluster * cluster);

//*********  Core API functions **********
    //Create a new type. 
    // index_type could be 'INDEX_LIST' or 'INDEX_RTREE' 
    int CreateType(char* name, int shape_type, int index_type, int flag);

    //Store an object with full name. t_name@sub_name
    int StoreObject(char* t_name, char* sub_name, union Shape shape, void* buf, int length);    

    //Store an object to the server. The 'subname' will be automatically generated.
    int StoreObjectNameless(char* t_name, void* buf, int length);
 
    //Query the name list of objects within a certain range
    //The result only contain sub_name
    int QueryObjectRange(char* t_name, struct Rect range, char** result); 
    
    //Load object by full name
    int LoadObject(char* t_name, char* sub_name, void** result, int* length);









    char* sendbuf;
    char* recvbuf;
    Cluster * cluster;
    Node* thisNode;

};
