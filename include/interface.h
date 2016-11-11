
#ifndef PSDDPF_HEADER
#define PSDDPF_HEADER

#define DEBUG                      0


#define RET_ERROR                  -1
#define RET_FAILED                 -2
#define RET_SUCCESS                0


#define CMD_CREATE_TYPE            1
#define CMD_STORE_OBJ              2
#define CMD_LOAD_OBJ               3
#define CMD_QUERY_OBJECT_RANGE     4

#define SHAPE_POINT                1
#define SHAPE_RECT                 2
#define SHAPE_GIRD                 3
#define SHAPE_NULL                 4

#define INDEX_NULL                 0
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
    double lonmin;
    double latmax;
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

union Shape
{
    struct Point point;
    struct Rect rect;
    struct Gird gird;
};



struct Type;


struct CreateTypeHeader
{
    unsigned char index_type; // The way to index it. E.g., no index, qtree, rtree ...
    unsigned int flag;
    unsigned char shape_type;
    int typeNameLength;
    char typeName;
};

struct StoreObjectHeader
{
    unsigned int typeNameLength;
    unsigned int subNameLength;
    unsigned int objSize; 
    unsigned int segmentSize; // Should be equal to the objSize if there is only on segment
    unsigned int segmentId; // start from 1.
    unsigned int segmentTotal; // Should be one if the data is smaller than one packet's capacity. 
    union Shape shape;
    int dataOffset;
    unsigned char data;
};

struct LoadObjectHeader
{
    unsigned int typeNameLength;
    unsigned int subNameLength;
    unsigned char data;
};

struct QueryObjectRangeHeader
{
    unsigned int typeNameLength;
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