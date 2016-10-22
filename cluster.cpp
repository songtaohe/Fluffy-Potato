#include "cluster.h"


Node::Node(char* name, char* ip, char* baseport)
{
    this->name = (char*)malloc(sizeof(char)*(strlen(name)+1));
    memcpy(this->name, name, strlen(name)+1);

    this->ip = (char*)malloc(sizeof(char)*(strlen(ip)+1));
    memcpy(this->ip, ip, strlen(ip)+1);

    this->baseport = (char*)malloc(sizeof(char)*(strlen(baseport)+1));
    memcpy(this->baseport, baseport, strlen(baseport)+1);

}


Cluster::Cluster(char* thisNode, char* configFile)
{
    FILE *fp = fopen(configFile, "rt");
    char name[256];
    char ip[256];
    char baseport[256];

    fscanf(fp,"%d\n",&(this->num));
    this->nodelist = (Node*)malloc(sizeof(Node)*this->num);
    for(int i = 0; i< this->num; i++)
    {
        fscanf(fp,"%s",name);
        fscanf(fp,"%s",ip);
        fscanf(fp,"%s",baseport);

        this->nodelist[i].Node(name,ip,baseport);

        if(strcmp(name, thisNode) == 0)
        {
            this->thisNode = i;
        }        
    }


    fclose(fp);
}


int Cluster::Query(struct Point location)
{
    //TODO temperory four partition
    // 42.360935, -71.117900
    if(location.lat > 42.360935 && location.lon > -71.117900) return 1;
    if(location.lat > 42.360935 && location.lon <= -71.117900) return 2;
    if(location.lat <= 42.360935 && location.lon > -71.117900) return 3;
    if(location.lat <= 42.360935 && location.lon <= -71.117900) return 4;

    return this->thisNode;
}
