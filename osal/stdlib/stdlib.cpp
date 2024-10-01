#include <AbstractionLayer/IMemory.h>
#include <cstdlib>

void *MEM_Calloc(size_t nitems, size_t size)
{
    return calloc(nitems, size);
}

void MEM_Free(void *ptr)
{
    free(ptr);
}