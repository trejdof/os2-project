//
// Created by os on 7/27/23.
#include "../h/slab.h"
#include "../h/buddy.h"
#include "../h/list.h"
#include "../test/printing.hpp"

void copyString(char *destination, const char *source);
static kmem_cache_t* small_memory_buffers[13] = {0};
static kmem_cache_t* caches[23] = {0}; // need to keep track of all caches in one array, first 13 are for buffers

static int id_obj = 13;
static int id_buff = 0;

void kmem_init(void *space, int block_num){
    buddyInit(space, block_num);
}

size_t round_to_upper_4096(uint64 size){
    uint64 block_size = 4096;
    while(block_size < size){
        block_size*=2;
    }
    return block_size;
}
kmem_cache_t *kmem_cache_create(const char *name, size_t size, void (*ctor)(void *), void (*dtor)(void *)) {
    // allocating space for struct kmem_cache_t
    kmem_cache_t* cache = (kmem_cache_t *)buddy_alloc(sizeof(kmem_cache_t));

    if (cache == 0)
        return 0; // buddy couldnt allocate


    copyString(cache->name, name);
    cache->real_obj_size = size;
    cache->obj_size = (size > sizeof (list_t))? size : sizeof(list_t);

    //cache->maximum_objects = cache->slab_size / cache->obj_size; //asdasd
    cache->slab_size = 4096;
    cache->maximum_objects = cache->slab_size / cache->obj_size;
    while(cache->maximum_objects < 8) {

        cache->slab_size <<= 1;
        cache->maximum_objects = cache->slab_size / cache->obj_size;
    }

    cache->growth_indicator = -1;

    //set pointers to slabs to nullptr initially
    cache->empty = nullptr;
    cache->full = nullptr;
    cache->partial = nullptr;

    cache->ctor = ctor;
    cache->dtor = dtor;
    if(cache->ctor)
        caches[id_obj++] = cache;
    else
        caches[id_buff++] = cache;
    return cache;
}
struct slab* choose_slab(kmem_cache_t* cachep);

void *kmem_cache_alloc(kmem_cache_t *cachep) {

    struct slab* my_slab = choose_slab(cachep); // choosing one slab from empty/partial lists while keeping them organized

    //getting one object from free_list of slab we found
    // need to make sure free_list is regularly populated
    void* ret_obj = list_pop((struct list_t **) &my_slab->free_list);
    if(cachep->ctor != 0) cachep->ctor(ret_obj);
    return ret_obj;
}


struct slab* slabInit(kmem_cache_t* cache){
    if (cache == 0) return 0;

    //allocates space for slab struct
    struct slab* newslab = (slab*)buddy_alloc(sizeof(struct slab));
    if (newslab == 0) return 0;

    //allocates space for actual slab memory that should be given to rest of system
    // if memory couldnt be allocated makes sure it deallocates memory allocated for struct above
    struct list_t* memory = (struct list_t*)buddy_alloc(cache->slab_size);
    if (memory == 0){
        buddy_free(newslab);
        return 0;
    }

    //sets slab's memory and free list to memory given by buddy allocator
    newslab->memory = memory;
    newslab->free_list = memory;

    newslab->previous = nullptr;
    newslab->next = nullptr;
    newslab->obj_counter = 0;


    // iterates through slab's memory to link all spaces for objects
    struct list_t *previous = 0;
    struct list_t* memory_pointer = memory;
    uint8* startingAddress = (uint8*)memory;

    for (uint i = 1; i <= cache->maximum_objects; i++){
        memory_pointer->prev = previous;
        memory_pointer->next = (struct list_t*)(startingAddress + i * cache->obj_size);
        //uint64 test = (uint64)(startingAddress + i * cache->obj_size);
        //printInt((uint64)memory_pointer->next);
        //printString("\n");
        previous = memory_pointer;
        //if(cache->ctor != 0) cache->ctor(previous);
        memory_pointer = memory_pointer->next;
    }
    //previous->prev = previous;
    previous->next = 0;

    return newslab;
}

int kmem_cache_shrink(kmem_cache_t *cachep){

    //sets indicator right
    if (cachep->growth_indicator != 1){
        cachep->growth_indicator = 0;
    }

    uint32 freed = 0;
    while(cachep->empty != 0){
        struct slab* current = (struct slab*) list_pop((struct list_t**)&cachep->empty);
        buddy_free(current->memory);
        buddy_free(current);
        freed++;
    }
    return (freed * cachep->slab_size);
}
void kmem_cache_free(kmem_cache_t *cachep, void *objp){

    // finding corresponding slab
    uint8* address = (uint8*)objp;
    struct slab* target_slab = 0;
    for(struct slab* curr = cachep->partial; curr; curr=curr->next){
        if (address >= (uint8*)curr->memory && address < (uint8*)curr->memory + cachep->slab_size){
            target_slab = curr;
        }
    }
    for(struct slab* curr = cachep->full; curr; curr=curr->next) {
        if (address >= (uint8 *) curr->memory && address < (uint8 *) curr->memory + cachep->slab_size) {
            target_slab = curr;
        }
    }

    if (target_slab == 0) return;


    // moving it between empty, partial, full lists
    if (target_slab->obj_counter == cachep->maximum_objects){
        list_remove((struct list_t**)&cachep->full, (struct list_t*)target_slab);
        list_push((struct list_t**)&cachep->partial, (struct list_t*)target_slab);
    }
    target_slab->obj_counter--;
    if (target_slab->obj_counter == 0){
        list_remove((struct list_t**)&cachep->partial, (struct list_t*)target_slab);
        list_push((struct list_t**)&cachep->empty, (struct list_t*)target_slab);
    }

    // put freed object in freelist
    list_push(&target_slab->free_list, (struct list_t*)objp);
    // call destructor if it exists on freed object
    if (cachep->dtor !=0) cachep->dtor(objp);
}
char* intToString(int number, char *str);
uint8 find_adequate_small_memory_buffer(uint64 actual_size){
    uint64 buffer_size = 32; // 2^5
    uint8 buffer_index = 0;
    while (buffer_size < actual_size){
        buffer_size = buffer_size << 1;
        buffer_index++;
    }
    return buffer_index;
}
void *kmalloc(size_t size){
    uint64 actual_size = size + 1; // 1 for header
    //find adequate small memory buffer

    uint8 entry = find_adequate_small_memory_buffer(actual_size);

    if(small_memory_buffers[entry] == 0){ // need to allocate new one
        char name[3];
        //creates cache because there wasnt one
        small_memory_buffers[entry] = kmem_cache_create(name, 1 << (entry + 5), 0, 0); // najmanji je je 2^5
        caches[entry] = small_memory_buffers[entry];
    }
    // allocates one object from small_memory_buffer with adequate size
    uint8* allocated = (uint8*)kmem_cache_alloc(small_memory_buffers[entry]);

    if (allocated == 0) return allocated;

    *allocated = entry; // set entry so we can keep track for later
    // returns one address after entry written
    return (void*)(allocated + 1);
}
void kfree(const void *objp){
    if (objp == 0) return;
    uint8* actual_location = (uint8*)objp - 1; // 1 for header
    uint8 entry = *actual_location;
    kmem_cache_free(small_memory_buffers[entry], actual_location);
}

void kmem_cache_destroy(kmem_cache_t *cachep){
    int i = 0;
    while (i < 23) {
        if (caches[i] == cachep){
            caches[i] = 0;
            break;
        }
        i++;
    }

    struct slab* slabs_to_destroy[3] = {cachep->full, cachep->partial, cachep->empty};

    for (int j = 0; j < 3; j++){
        struct slab* curr = slabs_to_destroy[j];
        struct slab* next = 0;
        while(curr){
            next = curr->next;
            buddy_free(curr->memory);
            buddy_free(curr);
            curr = next;
        }
    }
    buddy_free(cachep);
}

struct slab* choose_slab(kmem_cache_t* cachep){

    struct slab* my_slab = 0;
    struct slab* curr = nullptr;
    uint32 max_obj = 0;

    // take from partial list the most populated one if there is partial list
    if (cachep-> partial != nullptr){
        curr = cachep->partial;
        max_obj = cachep->partial->obj_counter;
        while (curr != 0) {
            if (curr->obj_counter >= max_obj) {
                max_obj = curr->obj_counter;
                my_slab = curr;
            }
            curr = curr->next;
        }
    }
    else{ // take from list of empty slabs because there is nothing in partial list
        my_slab = cachep->empty;
    }

    if (my_slab == nullptr) { // there is no empty slab, need to allocate new slab
        cachep->growth_indicator = 1; //??????????????????????????
        my_slab = slabInit(cachep); // allocates slab and puts it in empty list
        if (my_slab == 0) return 0; // couldnt allocate new slab from buddy, program should end
        list_push((struct list_t**)&(cachep->empty), (struct list_t*)my_slab);
    }
    //
    if(my_slab->obj_counter == 0) { //put in partial list
        list_remove((struct list_t**)&(cachep->empty), (struct list_t*)my_slab);
        list_push((struct list_t**)&(cachep->partial), (struct list_t*)my_slab);
    }
    if(my_slab->obj_counter == cachep->maximum_objects - 1) { // put in full list
        list_remove((struct list_t**)&(cachep->partial), (struct list_t*)my_slab);
        list_push((struct list_t**)&(cachep->full), (struct list_t*)my_slab);
    }
    my_slab->obj_counter++;
    return my_slab;
}

void copyString(char *destination, const char *source) {
    while (*source) {
        *destination = *source;
        source++;
        destination++;
    }
    *destination = '\0'; // Add null terminator to mark the end of the copied string
}
void reverseString(char *str, size_t length) {
    size_t start = 0;
    size_t end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
char* intToString(int number, char *str) {
    if (number == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    int num = number;
    size_t length = 0;

    while (num > 0) {
        str[length++] = (char)('0' + (num % 10));
        num /= 10;
    }

    str[length] = '\0';

    reverseString(str, length);

    return str;
}
int getSlabObjNum(kmem_cache_t *cachep){
    int slab_obj_num = cachep->slab_size / cachep->real_obj_size;
    return slab_obj_num;
}

//int getCacheSize(kmem_cache_t *cachep){
//    int total_obj_num = 0;
//    struct slab* slabs[2] = {
//        cachep->partial,
//        cachep->full
//    };
//    for (int i = 0; i < 2; i++) {
//        for (struct slab *curr = slabs[i]; curr; curr = curr->next) {
//            total_obj_num += curr->obj_counter;
//        }
//    }
//    int total_obj_size = (total_obj_num * cachep->real_obj_size);
//    int block_num = total_obj_size / BLOCK_SIZE;
//    block_num += (total_obj_num * cachep->real_obj_size) % BLOCK_SIZE > 0 ? 1 : 0;
//    return block_num;
//}

void kmem_cache_info(kmem_cache_t *cachep){
    if (cachep == nullptr) return;
    int slab_num = 0;
    int size_in_blocks = 0;
    int obj_num = 0;
    struct slab* slabs[3] = {
            cachep->empty,
            cachep->partial,
            cachep->full
    };
    for (int i = 0; i < 3; i++) {
        for (struct slab *curr = slabs[i]; curr; curr = curr->next) {
            size_in_blocks += cachep->slab_size / BLOCK_SIZE;
            slab_num++;
            obj_num+= curr->obj_counter;

        }
    }
    printString("======================================");
    printString(cachep->name);
    printString("=======================================\n");
    printString("Velicina objekta izrazena u bajtovima: ");
    printInt(cachep->real_obj_size);
    printString("\nVelicina celog kesa(blokovi): ");
    printInt(size_in_blocks);
    printString("\nBroj ploca: ");
    printInt(slab_num);
    printString("\nBroj objekata u jednoj ploci: ");
    printInt(cachep->slab_size / cachep->obj_size);
    printString("\nProcentualna popunjenost kesa: ");
    printInt(100 * obj_num / (slab_num * cachep->slab_size / cachep->obj_size));
    printString("%\n");

}
//
