#pragma once
#include <stdint.h>

enum {
    READ_WITH_RETRY = 0x20,
    WRITE_WITH_RETRY = 0x30
} ata_pio_commands;

void rw_sectors(const uint16_t size_in_sectors, uint32_t starting_sector, const uint32_t address, const uint8_t command);