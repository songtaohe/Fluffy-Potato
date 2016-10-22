#ifndef PSDDPF_HEADER
#define PSDDPF_HEADER

#define CMD_Create_Type            1
#define CMD_Store_Obj              2
#define CMD_Load_Obj               3
#define CMD_Query_Obj              4

struct Header
{
    unsigned char cmd;
    unsigned char cmd_header; // Use the pointer of this field.
};

struct Rect
{
    double latmin;
    double latmax;
    double lonmin;
    double lonmax;
};

struct Point
{
    double lat;
    double lon;
};

struct Gird
{
    int latGird;
    int lonGird;
    //TBD
};


struct CreateTypeHeader
{
    unsigned char index_type; // The way to index it. E.g., no index, qtree, rtree ...
    int typeNameLen;
    unsigned char typeName;
};

struct StoreObjPointHeader
{
    unsigned int typeNameLen;
    unsigned int subNameLen;
    unsigned int objSize;
    unsigned int segmentSize; // Should be equal to the objSize if there is only on segment
    unsigned int segmentId; // start from 1.
    unsigned int segmentTotal; // Should be one if the data is smaller than one packet's capacity. 
    struct Point point;
    unsigned char data;
};

struct LoadObjHeader
{
    unsigned int typeNameLen;
    unsigned int subNameLen;
    unsigned char data;
};

struct QueryObjHeader
{
    unsigned int typeNameLen;
    struct Rect queryRect;
    unsigned char data;
};


#endif
