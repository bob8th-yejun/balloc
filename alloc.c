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

typedef struct sortedbinHeader{
    uint32_t size;
    uint64_t next;
    uint64_t prev;
    uint64_t index;
}sortedbinHeader;

typedef struct binFooter{
    uint32_t size;
}binFooter;
#pragma pack(pop)

#ifdef DEBUG
uint32_t max_size;
uint32_t fastbin_count = 0, smallbin_count;
unsigned int sortedbin_count, sortedbin_indexsize;
#endif
fastbin *fastbin_head, *fastbin_tail;
binHeader *smallbin_head, *smallbin_tail;
binHeader *unsortedbin_root, *unsortedbin_tail;
sortedbinHeader *sortedbin_head, *sortedbin_tail, *sortedbin_index;

// binHeader *popBinNextNode(binHeader* now) {
// #ifdef DEBUG
//     debug("[+] popBinNextNode : node(%p), next node(%p), next->next(%p)\n", now, now->next, ((binHeader *)(now->next))->next);
// #endif
//     binHeader *next = now->next;
//     now->next = next->next;
//     return next;
// #ifdef DEBUG
// #endif
// }

sortedbinHeader *popSortedbinNode(sortedbinHeader* now) {
    return NULL;
}

void indexSortedbin() {
    return;
}
void mvindexSortedbin() {
    return;
}
void unindexSortedbin() {
    return;
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
        ((fastbin *)fastbin_tail)->next = ptr;
        ptr->next = (uint64_t)NULL;
        fastbin *oldtail = fastbin_tail;
        fastbin_tail = ptr;
#ifdef DEBUG
        debug("[=] pushFastbin : oldtail(%p {next(%p)}) -> tail(%p)\n\n", oldtail, oldtail->next, fastbin_tail);
#endif
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
    fastbin_head = ((fastbin *)fastbin_head)->next;
#ifdef DEBUG
    debug("[=] popFastbin : oldhead(%p {next(%p)}) -> tail(%p)\n\n", oldhead, oldhead->next, fastbin_head);
    fastbin_count--;
#endif
    return p;
}

void pushSmallbin(binHeader *ptr, uint32_t size) {
#ifdef DEBUG
    debug("[+] pushSmallbin : ptr(%p) size(%u)\n", ptr, size);
    debug("[+] pushSmallbin : smallbin_count(%d)\n", ++smallbin_count);
    binHeader *oldtail = smallbin_tail;
#endif
    if (smallbin_head == NULL) {
        smallbin_head = smallbin_tail = ptr;
        ptr->size = (uint32_t)size;
        ptr->next = (uint64_t)NULL;
        ((binFooter *)(((void*)ptr) + size - 4))->size = (uint32_t)size;
#ifdef DEBUG
        debug("node : size(%d), next(%p)\n", *((uint32_t *)ptr), (ptr + 4));
        debug("[=] pushSmallbin : head(%p) & tail(%p) set\n\n", smallbin_head, smallbin_tail);
#endif
    }
    else {
        smallbin_tail->next = ptr;
        ptr->size = (uint32_t)size;
        ptr->next = (uint64_t)NULL;
        ((binFooter *)(((void*)ptr) + size - 4))->size = (uint32_t)size;
        smallbin_tail = ptr;
#ifdef DEBUG
        debug("node : size(%d), next(%p)\n", *((uint32_t *)ptr), (ptr + 4));
        debug("[=] pushSmallbin : oldtail(%p {next(%p)}) -> tail(%p)\n\n", oldtail, oldtail->next, fastbin_tail);
#endif
    }
}
void *popSmallbin(uint32_t size) {
#ifdef DEBUG
    debug("[+] popSmallbin : size(%u), smallbin_count(%d)\n", size, smallbin_count);
    binHeader *oldhead = smallbin_head;
#endif
    if (smallbin_head == NULL)
        return NULL;
    binHeader *p = smallbin_head, *prev;
    while(p != NULL) {
        // debug("[TW] pop-while : p(%p), p->size(%u), p->next(%p)\n", p, p->size, p->next);
        if(p->size == size) {
            break;
        }
        prev = p;
        p = (binHeader *)(p->next);
    }
    // debug("[TW] while-end : p(%p) head(%p)\n", p, smallbin_head);
    if (smallbin_head == p) {
        smallbin_head = (binHeader *)(smallbin_head->next);
    }
    else if(p != NULL) {
        // debug("[TW] call PopBinNextNode : p(%p)\n", p);
        if (p->next != NULL)
            prev->next = p->next;
        else {
            smallbin_tail = prev;
            prev->next = NULL;
        }
    }
    // debug("[TW] function-end : p(%p)\n", p);
#ifdef DEBUG
    if (p != NULL)
        smallbin_count--;
    debug("[=] popSmallbin\n\n");
#endif
    return p;
}

void pushUnsortedbin(void *ptr, uint32_t size) {
    return;
}
void *popUnsortedbin(uint32_t size) {
    return NULL;
}

void pushSortedbin(void *ptr, uint32_t size) {
    return;
}
void *popSortedbin(uint32_t size) {
    return NULL;
}

void pushNode(void *ptr, uint32_t size) {
    if (size == 8)
        pushFastbin(ptr);
    else if (size < 32)
        pushSmallbin(ptr, size);
    else
        //pushSortedbin(ptr, size);
        pushUnsortedbin(ptr, size);
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
        // p = popSortedbin(size);
        p = popUnsortedbin(size);
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
        debug("%u size new sbrk\n", size);
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