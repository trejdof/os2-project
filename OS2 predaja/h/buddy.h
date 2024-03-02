//
// Created by os on 7/2/23.
//

#ifndef XV6_OS2_BUDDY_H
#define XV6_OS2_BUDDY_H
#include "../lib/hw.h"
#include "../h/bitmap.h"


void* buddy_alloc(uint32 size);
void buddy_free(void *pa);


void buddyInit(const void* space, int block_num);
int find_power_of_two(unsigned block_num);
uint64 align_to_4096(const void* space);
void freelist_insert(int entry, void* address);
void* get_address(int entry, void* address, int sign);
void remove_from_freelist(int entry, void* address);

#endif //XV6_OS2_BUDDY_H
