#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#ifdef DEBUG
extern void debug(const char *fmt, ...);
#endif
extern void *sbrk(intptr_t increment);

typedef struct fastbin{
    void* next;
}fastbin;

typedef struct binHeader{
    uint32_t size;
    void *next;
}binHeader;

typedef struct sortbinHeader{
    uint32_t size;
    void *next;
    void *prev;
    void *index;
}sortbinHeader;

typedef struct binFooter{
    uint32_t size;
}binFooter;

#ifdef DEBUG
unsigned int max_size;
#endif
void *fastbin_head, *fastbin_tail;
void *smallbin_head, *smallbin_tail;
void *unsortbin_root, *unsortbin_tail;
void *sortbin_head, *sortbin_tail, *sortbin_index;
unsigned int sortbin_count, sortbin_indexsize;

void indexSortbin() {
    ;
}
void mvindexSortbin() {
    ;
}
void unindexSortbin() {
    ;
}

void pushFastbin(void *ptr) {
#ifdef DEBUG
    debug("[+] pushFastbin : ptr(%p)\n", ptr);
    void *oldtail = fastbin_tail;
#endif
    if (fastbin_head == NULL) {
        fastbin_head = fastbin_tail = ptr;
#ifdef DEBUG
        debug("[=] pushFastbin : head(%p) & tail(%p) set\n\n", fastbin_head, fastbin_tail);
#endif
    }
    else {
        ((fastbin *)fastbin_tail)->next = ptr;
        ((fastbin *)ptr)->next = NULL;
        fastbin_tail = ptr;
#ifdef DEBUG
        debug("[=] pushFastbin : oldtail(%p {next(%p)}) -> tail(%p)\n\n", oldtail, ((fastbin*)oldtail)->next, fastbin_tail);
#endif
    }
}
void *popFastbin() {
    // void *p = fastbin_head;
    // fastbin_head = ((fastbin *)fastbin_head)->next;
    // return p;
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
void *popSortbin(uint32_t size) {
    ;
}

void pushNode(void *ptr, uint32_t size) {
    // if (size == 8)
    //     pushFastbin(ptr);
    // else if (size < 32)
    //     pushSmallbin(ptr, size);
    // else
    //     pushSortbin(ptr, size);
    pushFastbin(ptr);
}

void* popNode(uint32_t size) {
    void *p = NULL;
    if (size == 8) {
        popFastbin(size);
    }
    else if (size < 32) {
        popSmallbin(size);
    }
    else {
        popSortbin(size);
    }
    return p;
}

void *myalloc(uint32_t size)
{
   // printf("[+] ");
#ifdef DEBUG
    debug("[+] myalloc : size(%d)\n", size);
#endif
    if (size == 0)
        return NULL;
    size = ((size + 11) >> 3) << 3;

    void *p = popNode(size);
    if (p != NULL) {
        ;
    }
    else {
        p = sbrk(size);
        (*(uint32_t *)p) = size; 
    }
#ifdef DEBUG
    max_size += size;
    debug("alloc : size(%u) ptr(%p)\n", size, p);
    debug("[=] heap max: %u\n\n", max_size);
#endif
    return p + 4;
}

void myfree(void *ptr)
{
    // printf("[myfree] ");
#ifdef DEBUG
    debug("[+] myfree(%p)\n", ptr);
#endif
    if (ptr == NULL)
        return;
    pushNode(ptr, (*(uint32_t *)(ptr - 4)) & 0xFFFFFFF8);
#ifdef DEBUG
    debug("[=] free : size(%u), ptr(%p)\n\n", (*(uint32_t *)(ptr - 4)) & 0xFFFFFFF8, (uint32_t *)(ptr - 4));
#endif
}

void *myrealloc(void *ptr, uint32_t size)
{
    // printf("[myrealloc] ");
#ifdef DEBUG
    debug("[+] myrealloc : %d\n", size);
#endif
    void *p = myalloc(size);
    if (ptr != NULL) {
        if (p != NULL)
            memcpy(p, ptr, size);
        myfree(ptr);
    }
#ifdef DEBUG
    debug("[=] realloc : oldptr(%p), size(%u), newptr(%p)\n\n", ptr, size, p);
#endif
    return p;
}