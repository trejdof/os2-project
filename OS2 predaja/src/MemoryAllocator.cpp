#include "../h/MemoryAllocator.h"

MemoryAllocator::segDesc *MemoryAllocator::freeSegHead = nullptr;

void MemoryAllocator::init (uint64 heap_start) {

    MemoryAllocator::freeSegHead = (segDesc *) heap_start;
    freeSegHead->size = ((uint64) HEAP_END_ADDR - (uint64) heap_start) / MEM_BLOCK_SIZE;
    freeSegHead->next = nullptr;
}

void *MemoryAllocator::mem_alloc (size_t size) {

    if (size < 0) return nullptr;
    segDesc *curr = MemoryAllocator::freeSegHead, *prev = nullptr;

    //Jumps out when place is found or when theres no free space(curr is nullptr)
    for (; curr != nullptr; prev = curr, curr = curr->next){
        if (size <= curr->size) break;
    }

    if (curr == nullptr) {
        return nullptr;
    }

    if (size < curr->size) {
        //At least one block left unallocated
        size_t newSegDesc = (size_t) curr + size * MEM_BLOCK_SIZE;
        segDesc *tmp = (segDesc *) newSegDesc;
        tmp->size = curr->size - size;
        tmp->next = curr->next;
        tmp->prev = curr->prev;
        if (prev) prev->next = tmp; //
        else {
            MemoryAllocator::freeSegHead = tmp;
        }

    } else {
        //No blocks left to allocate
        if (prev) prev->next = curr->next;
        else MemoryAllocator::freeSegHead = curr->next;

    }
    curr->size = size;
    segDesc* test = curr + 1;
    return test; // return curr moved for sizeof(segHeader);
}

int MemoryAllocator::mem_free (void *p) {


    segDesc *tmp = (segDesc *) p - 1;

    // No free space, what we dealoc will be only free space on heap
    if (!freeSegHead) {
        tmp->next = nullptr;
        tmp->prev = nullptr;
        MemoryAllocator::freeSegHead = tmp;
        return 0;
    }

    segDesc *curr = freeSegHead;

    //find the place for segment in the list
    for (; curr < tmp && curr->next != nullptr; curr = curr->next);

    //segment is first in the list
    if (curr == freeSegHead) {
        curr->prev = tmp;
        tmp->prev = nullptr;
        tmp->next = curr;
        MemoryAllocator::freeSegHead = tmp;
        tryToMerge(tmp, tmp->next);
        return 0;
    } else {
        //segment is last in the list
        if (tmp > curr) {
            curr->next = tmp;
            tmp->prev = curr;
            tmp->next = nullptr;
            tryToMerge(tmp->prev, tmp);
            return 0;
        } else {
            //segment is somewhere in the middle
            tmp->next = curr;
            tmp->prev = curr->prev;
            curr->prev->next = tmp;
            curr->prev = tmp;
            tryToMerge(tmp, tmp->next);
            tryToMerge(tmp->prev, tmp);
            return 0;

        }
    }

}

void MemoryAllocator::tryToMerge (segDesc *main, segDesc *to_merge) {
    if (main + main->size == to_merge) {
        //Needs to be merged
        main->size += to_merge->size;
        main->next = to_merge->next;
        to_merge->next->prev = main;

    }

}






