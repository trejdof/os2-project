//
// Created by os on 7/27/23.
//

#ifndef OS1_PROJECT_MAIN_SLAB_H
#define OS1_PROJECT_MAIN_SLAB_H

#include "../h/buddy.h"
#include "../h/list.h"

struct slab {
    struct slab* next;
    struct slab* previous;
    struct list_t* free_list; // list of free memory chunks
    struct list_t* memory; //actual phyiscal memory given to this slab
    uint32 obj_counter;
};


typedef struct kmem_cache_s {
 char name[15];
 struct slab* empty;
 struct slab* partial;
 struct slab* full;

 void(*ctor)(void *);
 void(*dtor)(void *);

 int growth_indicator; // -1 = never shrinked, 0 = shrinked, 1 = first shrinked then expanded

 size_t slab_size; // in blocks
 size_t real_obj_size;
 size_t obj_size;
 size_t maximum_objects;

}kmem_cache_t;

#define BLOCK_SIZE (4096)
void kmem_init(void *space, int block_num);  // DONE

kmem_cache_t *kmem_cache_create(const char *name, size_t size,
                                void (*ctor)(void *),
                                void (*dtor)(void *)); // Allocate cache    DONE

int kmem_cache_shrink(kmem_cache_t *cachep); // Shrink cache DONE

void *kmem_cache_alloc(kmem_cache_t *cachep); // Allocate one object from cache  DONE

void kmem_cache_free(kmem_cache_t *cachep, void *objp); // Deallocate one object from cache  DONE

void *kmalloc(size_t size); // Alloacate one small memory buffer   DONE

void kfree(const void *objp); // Deallocate one small memory buffer DONE

void kmem_cache_destroy(kmem_cache_t *cachep); // Deallocate cache

void kmem_cache_info(kmem_cache_t *cachep); // Print cache info

int kmem_cache_error(kmem_cache_t *cachep); // Print error message


#endif //OS1_PROJECT_MAIN_SLAB_H
