#ifndef PSDDPF_HEADER
#define PSDDPF_HEADER

#define RET_ERROR                  -1
#define RET_SUCCESS                0


#define CMD_CREATE_TYPE            1
#define CMD_Store_Obj              2
#define CMD_Load_Obj               3
#define CMD_Query_Obj              4

#define SHAPE_POINT                1
#define SHAPE_RECT                 2
#define SHAPE_GIRD                 3

#define INDEX_LIST                 1
#define INDEX_RTREE                2

#define OBJ_READONLY               1
#define OBJ_TEMPERORY              2
#define OBJ_PRESISTENT             4
#define OBJ_ATOM_FETCH             8


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
    unsigned int flag;
    int typeNameLength;
    char typeName;
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


struct ObjectAttribute
{
    int flag;
    int time_to_live;
    //TODO
};



#endif
