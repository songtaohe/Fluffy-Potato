//Parallel Hash Table
#include <malloc.h>
#include <pthread.h>

struct HashTableEntry
{
    int type;// Local? Remote?
    uint64_t Key;
    char* name;
    void* data;
    int route; // You may need to get this data from another machine.
    struct HashTableEntry* next;

    //There should be something more
    // E.g. Time of Life?
    // E.g. Read Only?

};


class ParallerHashTable
{
Public:
    ParallerHashTable(int size);

    uint64_t FNV_Hash_64(char* name, int len);
    struct HashTableEntry Query(char* name);
    int Insert(char* name, void * buf, int bufsize);
    int Delete(char* name);

    int size;
    struct HashTableEntry** table;
    pthread_rwlock_t* rwlock;

}
