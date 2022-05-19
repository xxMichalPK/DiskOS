#pragma once
#include <stdint.h>

// Singly linked list nodes for blocks of memory
typedef struct malloc_block {
    uint32_t size;  // Size of this memory block in bytes
    bool free;      // Is this block of memory free?
    struct malloc_block *next;  // Next block of memory
} malloc_block_t;

extern malloc_block_t *malloc_list_head;
extern uint32_t malloc_virt_address;
extern uint32_t malloc_phys_address;
extern uint32_t total_malloc_pages;

void malloc_init(const uint32_t bytes);
void malloc_split(malloc_block_t *node, const uint32_t size);
void *malloc_next_block(const uint32_t size);
void merge_free_blocks(void);
void malloc_free(void *ptr);