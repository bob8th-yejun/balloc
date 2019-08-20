#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern void debug(const char *fmt, ...);
extern void *sbrk(intptr_t increment);

typedef struct fastBin{
    void* next;
}fastBin;

typedef struct binHeader{
    uint32_t size;
    void *next;
}binHeader;

typedef struct sortBinHeader{
    uint32_t size;
    void *next;
    void *prev;
    void *index;
}sortBinHeader;

typedef struct binFooter{
    uint32_t size;
}binFooter;

unsigned int max_size;
void *fastbin_head, *fastbin_tail;
void *smallbin_head, *smallbin_tail;
void *unsortbin_root, *unsortbin_tail;
void *sortbin_head, *sortbin_tail, *sortbin_index;
unsigned int sortbin_count, sortbin_indexsize;

void *myalloc(size_t size)
{
    void *p = sbrk(size);
    debug("alloc(%u): %p\n", (unsigned int)size, p);
    max_size += size;
    debug("max: %u\n", max_size);
    return p;
}

void *myrealloc(void *ptr, size_t size)
{
    void *p = NULL;
    if (size != 0)
    {
        p = sbrk(size);
        if (ptr)
            memcpy(p, ptr, size);
        max_size += size;
        debug("max: %u\n", max_size);
    }
    debug("realloc(%p, %u): %p\n", ptr, (unsigned int)size, p);
    return p;
}

void myfree(void *ptr)
{
    debug("free(%p)\n", ptr);
}
