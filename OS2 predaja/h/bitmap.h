//#define UNUSED_BITS 1

// Created by os on 7/20/23.
//

#ifndef XV6_OS2_BITMAP_H
#define XV6_OS2_BITMAP_H

#include "../h/buddy.h"

#define BITMAP_SIZE ((4096 * 2) / 8)
#define BLOCK_SIZE (4096)

void bitmapInit();
int bitmap_get_bit_index(void* address, int power, int NSIZES_MAX);
int check_bit_value(int bit_index);
void bitmap_set_bit_value(int bit_index, int value);
int find_buddy_index(int index);
void printChars(int numRows);

#endif //XV6_OS2_BITMAP_H
