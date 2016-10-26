#include "cluster.h"
#include "common.h"
#include "interface.h"


int Node::NodeInit(char* name, char* ip, char* baseport)
{
    this->name = (char*)malloc(sizeof(char)*(strlen(name)+1));
    memcpy(this->name, name, strlen(name)+1);

    this->ip = (char*)malloc(sizeof(char)*(strlen(ip)+1));
    memcpy(this->ip, ip, strlen(ip)+1);

    this->baseport = (char*)malloc(sizeof(char)*(strlen(baseport)+1));
    memcpy(this->baseport, baseport, strlen(baseport)+1);

    memset((char *) &(this->node_addr), 0, sizeof(this->node_addr));
    this->node_addr.sin_family = AF_INET;
    this->node_addr.sin_port = htons(atoi(baseport));
    inet_aton(ip, &(this->node_addr.sin_addr));

    this->node_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct timeval tv;
    tv.tv_sec = 2; // 2 second timeout
    tv.tv_usec = 0;
    setsockopt(this->node_socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));

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

        this->nodelist[i].NodeInit(name,ip,baseport);

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
