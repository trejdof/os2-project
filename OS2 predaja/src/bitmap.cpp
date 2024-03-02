//
// Created by os on 7/20/23.


#include "../h/bitmap.h"
#include "../test/printing.hpp"



unsigned char bitmap[BITMAP_SIZE];

extern uint64 buddy_heap_start;
void bitmapInit(){
    for (int i = 1; i < BITMAP_SIZE; i++)bitmap[i] = 0;
    bitmap[0] = 0b10000000;
}
int bitmap_get_bit_index(void* address, int entry, int NSIZES_MAX){
    int blocks_in_entry = 1 << entry;
    int row_start_bit_index = 0;

    //NSIZES = 4 ENTRY = 1
    // 0 1 2
    // 1 + 2 + 4
    for(int i = 0; i < NSIZES_MAX - entry; i++){
        row_start_bit_index += 1 << i;
    }
    int index_in_row = (uint64)((uint64)address - buddy_heap_start) / BLOCK_SIZE / blocks_in_entry;
    int bit_index = row_start_bit_index + index_in_row;

    return bit_index;
}

int check_bit_value(int bit_index){

    int byte_index = bit_index / 8;
    int bit_offset = bit_index % 8;

    return (bitmap[byte_index] & (1 << (7 - bit_offset))) != 0;


}

void bitmap_set_bit_value(int bit_index, int value) {
    int byte_index = bit_index / 8;
    int bit_offset = bit_index % 8;
    //printString("Byte index: "); printInt(byte_index);printString("\n");
    //printString("Bit offset: "); printInt(bit_offset);printString("\n");

    if (value == 1) {
        bitmap[byte_index] |= (1 << (7 - bit_offset));   // set
    } else {
        bitmap[byte_index] &= ~(1 << (7 - bit_offset));  // clear
    }
}


int find_buddy_index(int bit_index){
    int buddy_index = -1;
    if ((bit_index & 1) > 0)buddy_index = ++bit_index;
    else buddy_index = --bit_index;

    return buddy_index;
}
void printChars(int numRows) {
    for (int i = 0; i < BITMAP_SIZE; i++){
        printCharBinary(bitmap[i]);
        printString("\n");
    }
}

