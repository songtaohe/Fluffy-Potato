#include "index.h"

struct Node* TestInsert(struct Node* root, double latmin,double lonmin, double latmax, double lonmax, int key);
double LeastEnlargement(struct Rect target, struct Rect cur);
void Dump(struct Node* cur);

int IndexRTree::Overlay(struct Rect r1, struct Rect r2)
{
    int ox = 1;
    int oy = 1;

    if(r1.latmin > r2.latmax) ox = 0;
    if(r2.latmin > r1.latmax) ox = 0;

    if(r1.lonmin > r2.lonmax) oy = 0;
    if(r2.lonmin > r1.lonmax) oy = 0;

    return ox*oy;
}



double IndexRTree::LeastEnlargement(struct Rect target, struct Rect cur)
{
    double f = 0;
    f += cur.latmin < target.latmin ? target.latmin - cur.latmin: 0;
    f += cur.lonmin < target.lonmin ? target.lonmin - cur.lonmin: 0;
    f += cur.latmax > target.latmax ? cur.latmax - target.latmax: 0;
    f += cur.lonmax > target.lonmax ? cur.lonmax - target.lonmax: 0;
    return f;
}


IndexRTree::Node* IndexRTree::ChooseLeaf(IndexRTree::Node* cur, struct Rect newRect)
{
    if(cur->IsLeaf == 1) return cur;
    double MinLeastEnlargement = 10000000000.0;
    int next = 0;
    for(int i = 0;i < cur->NumC; i++)
    {
        double curLE = LeastEnlargement(newRect, cur->C[i]->R);
        if(curLE < MinLeastEnlargement)
        {
            MinLeastEnlargement = curLE;
            next = i;
        }
    }

    return ChooseLeaf(cur->C[next],newRect);
}


void IndexRTree::UpdateRect(IndexRTree::Node* cur, struct Rect newRect)
{
    if(cur == NULL) return;

    if(cur->R.latmin > newRect.latmin) cur->R.latmin = newRect.latmin;
    if(cur->R.lonmin > newRect.lonmin) cur->R.lonmin = newRect.lonmin;

    if(cur->R.latmax < newRect.latmax) cur->R.latmax = newRect.latmax;
    if(cur->R.lonmax < newRect.lonmax) cur->R.lonmax = newRect.lonmax;

    UpdateRect(cur->F,cur->R);
}

void IndexRTree::LocalUpdateRect(IndexRTree::Node* cur)
{
    struct Rect nR;

    nR = cur->C[0]->R;

    for(int i = 1;i < cur->NumC;i++)
    {
        if(nR.latmin > cur->C[i]->R.latmin) nR.latmin = cur->C[i]->R.latmin;
        if(nR.lonmin > cur->C[i]->R.lonmin) nR.lonmin = cur->C[i]->R.lonmin;
        if(nR.latmax < cur->C[i]->R.latmax) nR.latmax = cur->C[i]->R.latmax;
        if(nR.lonmax < cur->C[i]->R.lonmax) nR.lonmax = cur->C[i]->R.lonmax;
    }

    cur->R = nR;

}


IndexRTree::Node* IndexRTree::Split(IndexRTree::Node* cur, IndexRTree::Node* newNode)
{
    //Re-designate Children
    // TODO Optimization
    for(int i = _M_/2; i< _M_; i++)
    {
        newNode->C[i-_M_/2+1] = cur->C[i];
        cur->C[i]->F = newNode;
    }

    newNode->NumC = _M_/2+1;
    cur->NumC = _M_/2;

    LocalUpdateRect(newNode);
    LocalUpdateRect(cur);


    if(cur->F == NULL)
    {   // A new root
        IndexRTree::Node* newRoot;
        newRoot = (IndexRTree::Node*)malloc(sizeof(IndexRTree::Node));
        cur->F = newRoot;
        newNode->F = newRoot;
        newRoot->NumC = 2;
        newRoot->C[0] = cur;
        newRoot->C[1] = newNode;
        newRoot->IsLeaf = 0;
        newRoot->F = NULL;

        LocalUpdateRect(newRoot);

        return newRoot;
    }

    if(cur->F->NumC < _M_)
    {
        cur->F->C[cur->F->NumC] = newNode;
        newNode->F = cur->F;
        cur->F->NumC ++;

        UpdateRect(cur->F, newNode->R);

        return NULL;
    }
    else
    {
        IndexRTree::Node* _new = (IndexRTree::Node*)malloc(sizeof(IndexRTree::Node));
        _new->R = newNode->R;
        _new->NumC = 1;
        _new->C[0] = newNode;
        _new->C[0]->F = _new;
        _new->IsLeaf = 0;

        return Split(cur->F, _new);
    }

}


int IndexRTree::QueryRecursive(IndexRTree::Node* node, struct IndexEntity **Cur, struct Rect r)
{   
    int ret = 0;
    
    if(node == NULL) return 0;
    printf("Node %p\n",node);
    if(node->IsLeaf)
    {   
        //printf("-->%d\n",node->NumC);
        for(int i = 0;i<node->NumC;i++)
        {   
            //if(node->C[i] == NULL) printf("Error\n");
            if(LeastEnlargement(r,node->C[i]->R) == 0)
            {   
                ret ++;
                //printf("Link %p\n",PythonLinkCur);
                
                **Cur = node->C[i]->key;
				(*Cur)->next = NULL;
                //printf("Node %p, %d, %d\n",node,i,node->C[i]->key);   
    			//FIXME This is not good, the last item will have a non-zero next entry.            
                if((*Cur)->next == NULL)
                {   
                    (*Cur)->next = (struct IndexEntity*)malloc(sizeof(struct IndexEntity));
                    //if(PythonLinkCur->next == NULL) printf("Error!\n");
                    (*Cur) = (*Cur)->next;
                    (*Cur)->next = NULL;
					(*Cur)->toHash = NULL; // FIXME This is not good, the last item will have a non-zero next entry.   
                }
                else
                {   
                    (*Cur) = (*Cur)->next;
                }
            }
        }
    }
    else
    {   
        for(int i = 0; i< node->NumC; i++)
        {   
            if(node->C[i] == NULL) printf("Error\n");
            if(Overlay(node->C[i]->R,r))
            {   
                ret += QueryRecursive(node->C[i], Cur, r);
            }
        
        }
    }
    
    return ret;
}



IndexRTree::Node * IndexRTree::Insert(IndexRTree::Node* root,struct Rect newRect, struct IndexEntity
 newKey)
{
    IndexRTree::Node* L = ChooseLeaf(root, newRect);

    //printf("Mark1\n");
    if(L->NumC < _M_)
    {
        L->C[L->NumC] = (IndexRTree::Node*)malloc(sizeof(IndexRTree::Node));
        L->C[L->NumC]->R = newRect;
        L->C[L->NumC]->key = newKey;
        //printf("newkey%d %p %d\n",newKey,L,L->NumC);
        L->NumC ++;

    //printf("Mark2\n");
        UpdateRect(L, newRect);
    //printf("Mark3\n");
        return root;
    }
    else
    {
        IndexRTree::Node* newRoot = NULL;
        IndexRTree::Node* newNode = (IndexRTree::Node*)malloc(sizeof(IndexRTree::Node));
        newNode->R = newRect;
        newNode->NumC = 1;
        newNode->C[0] = (IndexRTree::Node*)malloc(sizeof(IndexRTree::Node));
        newNode->C[0]->R = newRect;
        newNode->C[0]->key = newKey;
        newNode->C[0]->F = newNode;
        newNode->IsLeaf = 1;

        newRoot = Split(L, newNode);

        if(newRoot != NULL) return newRoot;
        else return root;
    }

}

void IndexRTree::Dump(IndexRTree::Node* cur)
{
    if(cur==NULL) return;

    printf("\nNode %p   [%.2lf, %.2lf, %.2lf, %.2lf]\n", cur,
                        cur->R.latmin, cur->R.lonmin, cur->R.latmax, cur->R.lonmax);

    //printf("%d\n",cur->NumC);
    for(int i = 0;i < cur->NumC; i++)
    {
        struct Rect R;
        if(cur->C[i] == NULL)
        {
            printf("\nStructure Incomplete\n");
            continue;
        }

        R  = cur->C[i]->R;
        printf("    -  %p [%.2lf, %.2lf, %.2lf, %.2lf]\n", cur->C[i],
                        R.latmin, R.lonmin, R.latmax, R.lonmax);
    }

    if(cur->IsLeaf == 0)
    for(int i = 0;i<cur->NumC;i++)
    {
        Dump(cur->C[i]);
    }
}



int IndexRTree::Insert(struct IndexEntity * data)
{
    printf("Inside Insert %.2f %.2f %.2f %.2f \n",data->shape.rect.latmin,data->shape.rect.lonmin,data->shape.rect.latmax,data->shape.rect.lonmax);
    pthread_rwlock_wrlock(&(this->rwlock));
	
	struct Rect* R = (struct Rect*)(&(data->shape));

	this->root = this->Insert(this->root,*R,*data); 	    


    pthread_rwlock_unlock(&(this->rwlock));
    return 0;
}


//Return a pointer of a pointer is a little wired...
struct IndexEntity** IndexRTree::QueryRect(struct Rect range, int boundary)
{
    struct IndexEntity ** ret_head = (struct IndexEntity**)malloc(sizeof(struct IndexEntity*));
	struct IndexEntity * ret_cur = NULL;
    //struct IndexEntity * ret_tail = NULL;
	int ret = 0;
    pthread_rwlock_rdlock(&(this->rwlock));
    
	*ret_head = (struct IndexEntity*)malloc(sizeof(struct IndexEntity)); 
	(*ret_head)->next = NULL;

	ret_cur = *ret_head; 

	printf("start query %.2f %.2f %.2f %.2f\n", range.latmin, range.lonmin, range.latmax, range.lonmax);

	ret = QueryRecursive(this->root, &ret_cur, range);


	

    pthread_rwlock_unlock(&(this->rwlock));
    
	printf("query done %d\n",ret);

	if(ret>0)
    	return ret_head;
	else
		return NULL;
}



