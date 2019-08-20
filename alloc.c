#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern void debug(const char *fmt, ...);
extern void *sbrk(intptr_t increment);

typedef struct fastbin{
    void* next;
}fastBin;

typedef struct binHeader{
    uint32_t size;
    void *next;
}binHeader;

typedef struct sortbinHeader{
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

void sortSortbin() {
    ;
}
void unsortSortbin() {
    ;
}

void pushFastbin(void *ptr) {
    ;
}
void *popFastbin() {
    ;
}

void pushSmallbin(void *ptr, uint32_t size) {
    ;
}
void *popSmallbin(uint32_t size) {
    ;
}

void pushUnsortbin(void *ptr, uint32_t size) {
    ;
}
void *popUnsortbin(uint32_t size) {
    ;
}

void pushSortbin(void *ptr, uint32_t size) {
    ;
}
void *pushSortbin(void size) {
    ;
}

void pushNode(void *ptr, uint32_t size) {
    if (size == 8) { //fastbin
        ;
    }
    else if (size < 32) { //smallbin
        ;
    }
    else { //unsortbin or sortbin
        ;
    }
}

void popNode(uint32_t size) {
    if (size == 8) { //fastbin
        ;
    }
    else if (size < 32) { //smallbin
        ;
    }
    else { //unsortbin or sortbin
        ;
    }
}

void *myalloc(uint32_t size)
{
    size = ((size + 11) >> 3) << 3;
    void *p = popNode();
    if (p == NULL)
        p = sbrk(size);
    debug("alloc(%u): %p\n", (unsigned int)size, p);
    max_size += size;
    debug("max: %u\n", max_size);
    return p;
}

void *myrealloc(void *ptr, uint32_t size)
{
    size = ((size +7) >> 3) << 3;
    void *p = NULL;
    if (size != 0)
    {
        p = myalloc(size);
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
    pushNode(ptr, *(uint32_t*)(ptr - 4) & 0xFFFFFFF8);
}
