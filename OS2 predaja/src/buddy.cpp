//
// Created by os on 7/2/23.
//

#include "../h/buddy.h"
#include "../test/printing.hpp"


// Constants
#define BLOCK_SIZE (4096)     // Minimum block size (must be power of 2)
#define BLOCKS_PER_BYTE 4      // Number of blocks represented by each byte
#define NSIZES_MAX 12


uint64* free_list[13];
uint64 buddy_heap_start;
uint64 buddy_heap_end;


void buddyInit(const void* space, int block_num) { // block_num = 4096
    buddy_heap_start = align_to_4096(space);
    buddy_heap_end = buddy_heap_start + 4096 * BLOCK_SIZE;

    //set all entries to 0 except last one
    for (int i = 0; i < NSIZES_MAX + 1; i++) free_list[i] = nullptr;
    free_list[NSIZES_MAX] = (uint64*)buddy_heap_start;

    bitmapInit();

}

void* buddy_alloc(uint32 size) {
    // calculate number of blocks that need to be allocated (power of 2)
    int blocks_to_allocate = size / BLOCK_SIZE;
    blocks_to_allocate += size % BLOCK_SIZE > 0 ? 1 : 0;

    int entry_in_list = find_power_of_two(blocks_to_allocate);

    // we have chunk in exact entry just return its address
    if (free_list[entry_in_list] != 0) {
        //prepare first chunk in free_list[entry] and relink
        void* allocated_chunk = free_list[entry_in_list];
        free_list[entry_in_list] = (uint64*) *free_list[entry_in_list];

        //mark as used in bitmap
        int bit_index = bitmap_get_bit_index(allocated_chunk, entry_in_list, NSIZES_MAX);
        bitmap_set_bit_value(bit_index, 1);


        return allocated_chunk;
    }

    // look for chunks of bigger sizes
    for (int i = entry_in_list + 1; i <= NSIZES_MAX; i++){
        uint8* allocated_chunk;
        if(free_list[i] != nullptr) {

            allocated_chunk = (uint8*)free_list[i];
            free_list[i] = (uint64*) *free_list[i];

            //mark as used in current entry
            int bit_index = bitmap_get_bit_index(allocated_chunk, i, NSIZES_MAX);
            bitmap_set_bit_value(bit_index, 1);

            //split chunk and mark as used
            for(int j = i - 1; j >= entry_in_list; j--){
                free_list[j] = (uint64*)(allocated_chunk + (uint64)(BLOCK_SIZE << j));
                *free_list[j] = 0;

                bit_index = bitmap_get_bit_index(allocated_chunk, j, NSIZES_MAX);
                bitmap_set_bit_value(bit_index, 1);
                //printString("Bit index: ");
                //printInt(bit_index);
                //printString("\n");
            }
        return allocated_chunk;
        }

    }
    return 0;
}

void buddy_free(void *pa){

    if(((uint64)pa % BLOCK_SIZE) != 0 || (char*)pa < (char*)buddy_heap_start || (uint64)pa >= (uint64) HEAP_END_ADDR)
        return;
    //find corresponding bit_value in bitmap
    int entry_num = 0;
    int bit_index = -1;
    int bit_value;

    for (int i = 0; i <= NSIZES_MAX; i++) {
        bit_index = bitmap_get_bit_index(pa, i, NSIZES_MAX);
        bit_value = check_bit_value(bit_index);
        entry_num = i;
        if (bit_value > 0) break;
    }

    //merge buddies and free them
    for(int i = entry_num; i <= NSIZES_MAX; i++){
        // find buddy index and its value
        if(bit_index == 0) {
            freelist_insert(NSIZES_MAX, pa);
            return;
        }
        bitmap_set_bit_value(bit_index,0);

        int buddy_index = find_buddy_index(bit_index);
        int buddy_value = check_bit_value(buddy_index);

        //buddy is not free, just insert freed chunk into freelist
        if (buddy_value == 1) {
            freelist_insert(i, pa);
            break;
        }
        else {
            // buddy is free should merge and remove from freelist
            void *buddy_address = get_address(i, pa, buddy_index - bit_index);
            remove_from_freelist(i, buddy_address);
            pa = buddy_address > pa ? pa : buddy_address; //
            bit_index = bitmap_get_bit_index(pa,i+1,NSIZES_MAX);
        }
    }


}

int find_power_of_two(unsigned block_num){
    int old = block_num;
    int cnt = -1;
    while (block_num > 0) {
        block_num  = block_num >> 1;
        cnt++;
    }

    if (old - (1 << cnt) > 0) cnt ++;

    return cnt;
}

uint64 align_to_4096(const void* space){
    uint64 number = (uint64)space;
    if (number % 4096 == 0) {
        return number;
    } else {
        return ((number / 4096) + 1) * 4096;
    }
}



void* get_address(int entry, void* address, int sign){
    return (uint64*)((long)address + (int)(BLOCK_SIZE << entry) * sign);
}
void freelist_insert(int entry, void* address){

    uint64* list_begin = free_list[entry];

    *(uint64*)address = (uint64)list_begin;
    list_begin = (uint64*)address;
    free_list[entry] = list_begin;
}


void remove_from_freelist(int entry, void* address) {
    uint64* cursor = free_list[entry];

    if (cursor == address) {
        free_list[entry] = (uint64*)*(uint64*)(address);
        return;
    }

    while (cursor != nullptr) {
        if ((uint64*)(*cursor) == (uint64*)address) {
            *cursor = *(uint64*)address;
            break;
        }
        cursor = (uint64*)(*cursor);
    }
}




