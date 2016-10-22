#include "hashTable.h"
#include <string.h>

ParallelHashTable::ParallelHashTable(int size)
{
    this->table = (struct HashTableEntry**)malloc(sizeof(struct HashTableEntry*) * size);
    this->rwlock = (pthread_rwlock_t*)malloc(sizeof(pthread_rwlock_t) * size);
    this->size = size;
    for(int i = 0; i< size; i++)
    {
        this->table[i] = NULL;
        pthread_rwlock_init(&(this->rwlock[i]), NULL);

    } 
}


uint64_t ParallelHashTable::FNV_Hash_64(char *name, int len)
{
    uint64_t hash = (uint64_t)14695981039346656037;
    for(int i = 0; i< len; i++)
    {
        hash = hash * 1099511628211;
        hash = hash xor ((uint8_t)name[i]);
    }
    return hash;
}

struct HashTableEntry* ParallelHashTable::Query(char* name)
{
    uint64_t key = this->FNV_Hash_64(name,strlen(name));
    uint32_t index = key % (this->size);
    struct HashTableEntry * ret = NULL;

    pthread_rwlock_rdlock(&(this->rwlock[index]));
    {
        struct HashTableEntry ** cur = &(this->table[index]);
        while(*cur!=NULL)
        {
            if((*cur)->Key == key)
            {
                int cmp = strcmp((*cur)->name, name);
                if(cmp == 0)
                {
                    ret = (*cur);
                    break;
                }
            }
            
            cur = &((*cur) -> next);
        }        
    }
    pthread_rwlock_unlock(&(this->rwlock[index]));
    
    return ret;
}

void* ParallelHashTable::Insert(char* name, void* buf, int bufsize)
{
    uint64_t key = this->FNV_Hash_64(name,strlen(name));
    uint32_t index = key % (this->size);

    struct HashTableEntry ** cur = &(this->table[index]);
    pthread_rwlock_wrlock(&(this->rwlock[index]));
    {    
        while(*cur!= NULL)
        {
            cur = &((*cur)->next);
        }

        *cur = (struct HashTableEntry*) malloc(sizeof(struct HashTableEntry));
        (*cur) -> next = NULL;
        (*cur) -> name = (char*)malloc(sizeof(char) * strlen(name)+1);
        memcpy((*cur) -> name, name, strlen(name));
        (*cur) -> name[strlen(name)] = 0;

        if(buf!=NULL)
        {
            (*cur) -> data = (void*)malloc(bufsize);  
            memcpy((*cur) -> data, buf, bufsize);
        }


        (*cur) -> type = HashTableLocal; // Local
        (*cur) -> Key = key; // TBD
        (*cur) -> index = NULL;

    }
    pthread_rwlock_unlock(&(this->rwlock[index]));

    return (void*)(*cur); // This will be used as key of 
}

void ParallelHashTable::Delete(char* name)
{
    //TODO
    return;
}

