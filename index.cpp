#include "index.h"

int IndexList::Insert(struct IndexEntity * data)
{
    printf("Inside Insert \n");
    pthread_rwlock_wrlock(&(this->rwlock));
    {
        if(this->LinkHead == NULL)
        {
            this->LinkHead = (struct IndexEntity*)malloc(sizeof(struct IndexEntity));
            this->LinkTail = this->LinkHead;
            memcpy(this->LinkHead, data, sizeof(struct IndexEntity));
            this->LinkHead->next = NULL;
        }
        else
        {
            this->LinkTail->next = (struct IndexEntity*)malloc(sizeof(struct IndexEntity));
            memcpy(this->LinkTail->next, data, sizeof(struct IndexEntity)); 
            this->LinkTail = this->LinkTail->next;
            this->LinkTail->next = NULL;
        }
    }
    pthread_rwlock_unlock(&(this->rwlock));
    return 0;
}


struct IndexEntity** IndexList::QueryRect(struct Rect range, int boundary)
{
    struct IndexEntity ** ret_head = NULL;
    struct IndexEntity * ret_tail = NULL;

    pthread_rwlock_rdlock(&(this->rwlock));
    {
        struct IndexEntity *head = this->LinkHead;
        while(head!=NULL)
        {
            int flag = 0;
            switch(this->shape_type)
            {
                case SHAPE_POINT:
					printf("Index List %.2f %.2f %.2f %.2f %.2f %.2f \n",range.latmin,range.lonmin,range.latmax,range.lonmax,head->shape.point.lat, head->shape.point.lon);
                    if(head->shape.point.lat >= range.latmin && head->shape.point.lat <= range.latmax)
                        if(head->shape.point.lon >= range.lonmin && head->shape.point.lon <= range.lonmax)
                            flag = 1;
                    break;
                
                case SHAPE_RECT:
                
                    break;

                case SHAPE_GIRD:
    
                    break;

                default:
                    break;
            }

            if(flag == 1)
            {
                if(ret_head == NULL)
                {
                    ret_head = (struct IndexEntity**)malloc(sizeof(struct IndexEntity*));
                    *ret_head = (struct IndexEntity*)malloc(sizeof(struct IndexEntity));
                    ret_tail = *ret_head;

                    memcpy(ret_tail, head, sizeof(struct IndexEntity));

                    ret_tail->next = NULL;
                }
                else
                {
                    ret_tail->next = (struct IndexEntity*)malloc(sizeof(struct IndexEntity));
                    memcpy(ret_tail->next, head, sizeof(struct IndexEntity));
                    ret_tail = ret_tail->next;
                    ret_tail->next = NULL;
                }
            }
            head = head->next;
        }

    }
    pthread_rwlock_unlock(&(this->rwlock));
    
    return ret_head;
}
