//Parallel Hash Table
#include <malloc.h>
#include <pthread.h>
#include "common.h"

#ifndef HashTableHeader
#define HashTableHeader

#define HashTableLocal      1

struct HashTableEntry
{
    int type;// Local? Remote?
    uint64_t Key;
    char* name;
    void* data;
    int route; // You may need to get this data from another machine.
    struct HashTableEntry* next;
    void* index;
    //There should be something more
    // E.g. Time of Life?
    // E.g. Read Only?

};


class ParallelHashTable
{
public:
    ParallelHashTable(int size);

    uint64_t FNV_Hash_64(char* name, int len);
    struct HashTableEntry* Query(char* name);
    void* Insert(char* name, void * buf, int bufsize);
    void Delete(char* name);
    //uint64_t GenerateKey(uint16_t MachineID, uint8_t LocalID);


    int size;
    struct HashTableEntry** table;
    pthread_rwlock_t* rwlock;

};

#endif
