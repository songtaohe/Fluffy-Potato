#include "hashTable.h"
#include <string.h>

ParallerHashTable::ParallerHashTable(int size)
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


uint64_t ParallerHashTable::FNV_Hash_64(char *name, int len)
{
    uint64_t hash = 14695981039346656037;
    for(int i = 0; i< len; i++)
    {
        hash = hash * 1099511628211;
        hash = hash xor ((uint8_t)name[i]);
    }
    return hash;
}

int Insert(char* name, void* buf, int bufsize)
{
    uint32_t index = this->FNV_Hash_64(name,strlen(name));
    
    pthread_rwlock_wrlock(&(this->rwlock[index]));
    {    
        struct HashTableEntry ** cur = &(this->table[index]);
        while(*cur!= NULL)
        {
            cur = &((*cur)->next);
        }

        *cur = (struct HashTableEntry*) malloc(sizeof(struct HashTableEntry));
        (*cur) -> next = NULL;
        //(*cur) -> name = mall   TODO TODO TODO       


    }
    pthread_rwlock_unlick(&(this->rwlock[index]));

}
