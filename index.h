#include "common.h"
#include "interface.h"

#ifndef IndexHeader
#define IndexHeader

struct IndexEntity
{
    union{
        struct Rect rect;
        struct Point point;
    };
    void* toHash; // to hash entity and data
    struct IndexEntity* next; // query list
};


class IndexBase{
public:
    IndexBase(int type){this->type = type;};
    virtual struct IndexEntity** QueryRect(struct Rect range, int boundary) = 0;
    virtual int Insert(struct IndexEntity* data) = 0;
    int type;
};


class IndexList : public IndexBase
{
public:
    IndexList(int shape_type): IndexBase(INDEX_LIST)
    {
        this->shape_type = shape_type;
        pthread_rwlock_init(&(this->rwlock), NULL);
    };

    struct IndexEntity** QueryRect(struct Rect range, int boundary);
    int Insert(struct IndexEntity * data);

    struct IndexEntity* LinkHead = NULL;
    struct IndexEntity* LinkTail = NULL;
    
    pthread_rwlock_t rwlock;  
    int shape_type;  
};



#endif
