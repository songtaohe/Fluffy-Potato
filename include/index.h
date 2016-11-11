#include "common.h"
#include "interface.h"

#ifndef IndexHeader
#define IndexHeader

struct IndexEntity
{
    union Shape shape;
    void* toHash; // to hash entity and data
    struct IndexEntity* next; // query list
};

class IndexBase;

struct Type
{
    unsigned char index_type;
    unsigned char shape_type;
    unsigned int flag;
    IndexBase * indexbase;
};

class IndexBase{
public:
    IndexBase(int type){this->type = type;};
    virtual struct IndexEntity** QueryRect(struct Rect range, int boundary) = 0;
    virtual int Insert(struct IndexEntity* data) = 0;  //Copy
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


#define _M_ 16
class IndexRTree : public IndexBase
{
private:
	/*struct Rect
	{
    	double MinX;
    	double MinY;
    	double MaxX;
    	double MaxY;
	};*/
	
	typedef struct Node
	{
    	struct Rect R;
    	struct Node* F;
    	struct Node* C[_M_];
    	int NumC;
    	int IsLeaf;
    	//void* key;
    	struct IndexEntity key;
	} Node;

	IndexRTree::Node * Insert(IndexRTree::Node* root,struct Rect newRect, struct IndexEntity newKey);
	void Dump(IndexRTree::Node* cur);
	int Overlay(struct Rect r1, struct Rect r2);
	double LeastEnlargement(struct Rect target, struct Rect cur);
	IndexRTree::Node* ChooseLeaf(IndexRTree::Node* cur, struct Rect newRect);
	void UpdateRect(IndexRTree::Node* cur, struct Rect newRect);
	void LocalUpdateRect(IndexRTree::Node* cur);
	IndexRTree::Node* Split(IndexRTree::Node* cur, IndexRTree::Node* newNode);
	int QueryRecursive(IndexRTree::Node* node, struct IndexEntity **Cur, struct Rect r);	

public:
	IndexRTree(int shape_type):IndexBase(INDEX_RTREE)
	{
		this->shape_type = shape_type;
		this->root = (IndexRTree::Node*)malloc(sizeof(IndexRTree::Node));
		this->root->R.latmin = -1000;
    	this->root->R.lonmin = -1000;
    	this->root->R.latmax = 1000;
    	this->root->R.lonmax = 1000;

    	this->root->F = NULL;
    	for(int i = 0;i < _M_; i++) this->root->C[i] = NULL;


    	this->root->NumC = 0;
    	//this->root->key = -1;
    	this->root->IsLeaf = 1;

		pthread_rwlock_init(&(this->rwlock), NULL);
	};

	IndexRTree::Node *root;

	struct IndexEntity** QueryRect(struct Rect range, int boundary);
    int Insert(struct IndexEntity * data);

	pthread_rwlock_t rwlock;
    int shape_type;
};



#endif
