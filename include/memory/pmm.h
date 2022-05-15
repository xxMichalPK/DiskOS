#pragma once
#include <stdint.h>

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BYTE 8

extern uint32_t *memory_map;
extern uint32_t max_blocks;
extern uint32_t used_blocks;

void set_block(const uint32_t bit);
void unset_block(const uint32_t bit);
int32_t find_first_free_blocks(const uint32_t num_blocks);
void initialize_memory_manager(const uint32_t start_address, const uint32_t size);
void initialize_memory_region(const uint32_t base_address, const uint32_t size);
void deinitialize_memory_region(const uint32_t base_address, const uint32_t size);
uint32_t *allocate_blocks(const uint32_t num_blocks);
void free_blocks(const uint32_t *address, const uint32_t num_blocks);