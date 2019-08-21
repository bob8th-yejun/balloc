#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// #define DEBUG

#ifdef DEBUG
extern void debug(const char *fmt, ...);
#endif
extern void *sbrk(intptr_t increment);

#pragma pack(push, 1)
typedef struct fastbin{
    uint64_t next;
}fastbin;

typedef struct binHeader{
    uint32_t size;
    uint64_t next;
}binHeader;

typedef struct sortbinHeader{
    uint32_t size;
    uint64_t next;
    uint64_t prev;
    uint64_t index;
}sortbinHeader;

typedef struct binFooter{
    uint32_t size;
}binFooter;
#pragma pack(pop)

#ifdef DEBUG
uint32_t max_size;
uint32_t fastbin_count = 0;
unsigned int sortbin_count, sortbin_indexsize;
#endif
fastbin *fastbin_head, *fastbin_tail;
binHeader *smallbin_head, *smallbin_tail;
binHeader *unsortbin_root, *unsortbin_tail;
sortbinHeader *sortbin_head, *sortbin_tail, *sortbin_index;

void indexSortbin() {
    ;
}
void mvindexSortbin() {
    ;
}
void unindexSortbin() {
    ;
}

void pushFastbin(fastbin *ptr) {
#ifdef DEBUG
    debug("[+] pushFastbin : ptr(%p)\n", ptr);
    debug("[+] pushFastbin : fastbin_count(%d)\n", ++fastbin_count);
#endif
    if (fastbin_head == NULL) {
        fastbin_head = fastbin_tail = ptr;
        ptr->next = (uint64_t)NULL;
#ifdef DEBUG
        debug("[=] pushFastbin : head(%p) & tail(%p) set\n\n", fastbin_head, fastbin_tail);
#endif
    }
    else {
        // debug("[T] next(%p)\n", ((fastbin*)fastbin_tail)->next);
        ((fastbin *)fastbin_tail)->next = ptr;
        // debug("[T] next(%p)\n", ((fastbin*)fastbin_tail)->next);
        // debug("[P] ptr(%p), ptr-next(%p)\n", ptr, &(ptr->next));
        // debug("[D] ptr(%lu), ptr-next(%lu)\n", *((uint64_t*)ptr), ptr->next);
        ptr->next = (uint64_t)NULL;
        // debug("[P] NULL -> ptr(%p), ptr-next(%p)\n", ptr, &(ptr->next));
        // debug("[D] NULL -> ptr(%lu), ptr-next(%lu)\n", *((uint64_t*)ptr), ptr->next);
        // debug("[T] next(%p)\n", ((fastbin*)fastbin_tail)->next);
        fastbin *oldtail = fastbin_tail;
        fastbin_tail = ptr;
        // debug("[T] next(%p)\n", ((fastbin*)oldtail)->next);
        // debug("[=] pushFastbin : oldtail(%p {next(%p)}) -> tail(%p)\n\n", oldtail, oldtail->next, fastbin_tail);
    }
}
void *popFastbin() {
#ifdef DEBUG
    debug("[+] popFastbin : fastbin_count(%d)\n", fastbin_count);
    fastbin *oldhead = fastbin_head;
#endif
    if (fastbin_head == NULL)
        return NULL;
    void *p = fastbin_head;
    // debug("[T] p = %p\n", p);
    // debug("[T] head = %p\n", &(fastbin_head->next));
    // debug("[T] next = %p\n", ((fastbin *)fastbin_head)->next);
    fastbin_head = ((fastbin *)fastbin_head)->next;
#ifdef DEBUG
    debug("[=] popFastbin : oldhead(%p {next(%p)}) -> tail(%p)\n\n", oldhead, oldhead->next, fastbin_head);
    fastbin_count--;
#endif
    return p;
}

void pushSmallbin(void *ptr, uint32_t size) {
    return;
}
void *popSmallbin(uint32_t size) {
    return NULL;
}

void pushUnsortbin(void *ptr, uint32_t size) {
    return;
}
void *popUnsortbin(uint32_t size) {
    return NULL;
}

void pushSortbin(void *ptr, uint32_t size) {
    return;
}
void *popSortbin(uint32_t size) {
    return NULL;
}

void pushNode(void *ptr, uint32_t size) {
    if (size == 8)
        pushFastbin(ptr);
    else if (size < 32)
        pushSmallbin(ptr, size);
    else
        //pushSortbin(ptr, size);
        pushUnsortbin(ptr, size);
}

void* popNode(uint32_t size) {
    void *p = NULL;
    if (size == 8) {
        p = popFastbin(size);
    }
    else if (size < 32) {
        p = popSmallbin(size);
    }
    else {
        // p = popSortbin(size);
        p = popUnsortbin(size);
    }
    return p;
}

void *myalloc(uint32_t size)
{
#ifdef DEBUG
    debug("[+] myalloc : size(%d)\n", size);
#endif
    if (size == 0)
        return NULL;
    size = ((size + 11) >> 3) << 3;

    void *p = popNode(size);
    if (p != NULL) {
#ifdef DEBUG       
        debug("%u size recycle (ptr = %p)\n", size, p);
#endif
    }
    else {
#ifdef DEBUG
        debug("%u size new alloc\n", size);
#endif
        p = sbrk(size);
    }
    (*(uint32_t *)p) = size;
#ifdef DEBUG
    max_size += size;
    debug("alloc : size(%u) ptr(%p)\n", size, p);
    debug("[=] alloc : size(%u), ptr(%p)\n\n", (*(uint32_t *)(p)) & 0xFFFFFFF8, (uint32_t *)(p));
    debug("[=] heap max: %u\n\n", max_size);
#endif
    return p + 4;
}

void myfree(void *ptr)
{
#ifdef DEBUG
    debug("[+] myfree(%p)\n", ptr);
#endif
    if (ptr == NULL)
        return;
    ptr -= 4;
    pushNode(ptr, (*(uint32_t *)ptr) & 0xFFFFFFF8);
#ifdef DEBUG
    debug("[=] free : size(%u), ptr(%p)\n\n", (*(uint32_t *)ptr) & 0xFFFFFFF8, (uint32_t *)(ptr));
#endif
}

void *myrealloc(void *ptr, uint32_t size)
{
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