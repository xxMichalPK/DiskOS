#include <disk/ata.h>

//-----------------------------------------------
// Read/write sectors to/from disk using ATA PIO
// Inputs:
//   1: File size in sectors from file table entry
//   2: Starting sector from file table entry
//   3: Address to Read/Write data from/to
//   4: Command to perform (Reading or Writing)
//-----------------------------------------------
void rw_sectors(const uint16_t size_in_sectors, uint32_t starting_sector, const uint32_t address, const uint8_t command)
{
    uint8_t head = 0;
    uint16_t cylinder = 0;
    uint8_t test_byte = 0;

    // Convert given starting sector to correct CHS values
    // "Typical" disk limits are Cylinder: 0-1023, Head: 0-15, Sector: 1-63 (sector is 1-based!
    if (starting_sector > 63) {
        head = starting_sector / 63;
        starting_sector = starting_sector % 63;
        if (starting_sector == 0) {
            starting_sector = 63;
            head--;
        }
        if (head > 15) {
            cylinder = head / 16;
            head = head % 16;
        }
    }

    //if (cylinder >= 1024) {
    //    // TODO: Error for disk limit reached
    //}
    
    // TODO: Change to LBA later on? Either 28/48bit LBA
    // CHS -> LBA = ((cylinder * 16 + head) * 63) + sector - 1
    // LBA -> CHS =
    //   cylinder = LBA / (16 * 63);
    //   temp     = LBA % (16 * 63);
    //   head     = temp / 63;
    //   sector   = temp % 63 + 1;
    
    // Port 1F6h head/drive # bits: 7: always set(1), 6 = CHS(0) or LBA(1), 5 = always set(1), 4 = drive # (0 = primary, 1 = secondary),
    //   3-0: Head #
    __asm__ __volatile__ ("outb %%al, %%dx" : : "a"(0xA0 | head),           "d"(0x1F6) );  // Head/drive # port - send head/drive #
    __asm__ __volatile__ ("outb %%al, %%dx" : : "a"(size_in_sectors),       "d"(0x1F2) );  // Sector count port - # of sectors to read/write
    __asm__ __volatile__ ("outb %%al, %%dx" : : "a"(starting_sector),       "d"(0x1F3) );  // Sector number port - sector to start at (1-based!)
    __asm__ __volatile__ ("outb %%al, %%dx" : : "a"(cylinder & 0xFF),       "d"(0x1F4) );  // Cylinder low port - cylinder low #
    __asm__ __volatile__ ("outb %%al, %%dx" : : "a"((cylinder >> 8) & 0xFF),"d"(0x1F5) );  // Cylinder high port - cylinder high #
    __asm__ __volatile__ ("outb %%al, %%dx" : : "a"(command),               "d"(0x1F7) );  // Command port - send read/write command

    if (command == READ_WITH_RETRY) {
        for (int8_t i = size_in_sectors; i > 0; i--) {
            while (1) {
                // Poll status port after reading 1 sector
                __asm__ __volatile__ ("inb %%dx, %%al" : "=a"(test_byte) : "d"(0x1F7) );  // Read status register (port 1F7h)
                if (!(test_byte & (1 << 7)))    // Wait until BSY bit is clear
                    break;
            }

            // Read dbl words from DX port # into EDI, CX # of times
            // DI = address to read into, cx = # of double words to read for 1 sector,
            // DX = data port
            __asm__ __volatile__ ("rep insl" : : "D"(address), "c"(128), "d"(0x1F0) ); 

            // 400ns delay - Read alternate status register
            for (uint8_t i = 0; i < 4; i++)
                __asm__ __volatile__ ("inb %%dx, %%al" : : "d"(0x3F6), "a"(0) );   // Use any value for "a", it's just to keep the al
        }

    } else if (command == WRITE_WITH_RETRY) {
        // Keep trying until sector buffer is ready
        while (1) {
            __asm__ __volatile__ ("inb %%dx, %%al" : "=a"(test_byte) : "d"(0x1F7) );  // Read status register (port 1F7h)
            if (!(test_byte & (1 << 7)))    // Wait until BSY bit is clear
                break;
        }

        __asm__ __volatile__ ("1:\n"
                              "outsw\n"               // Write words from (DS:)SI into DX port
                              "jmp .+2\n"             // Small delay after each word written to port
                              "loop 1b"               // Write words until CX = 0
                              // SI = address to write sectors from, CX = # of words to write,
                              // DX = data port
                              : : "S"(address), "c"(256*size_in_sectors), "d"(0x1F0) );  

        // Send cache flush command after write command is finished
        __asm__ __volatile__ ("outb %%al, %%dx" : : "a"(0xE7), "d"(0x1F7) );  // Command port - cache flush

        // Wait until BSY bit is clear after cache flush
        while (1) {
            __asm__ __volatile__ ("inb %%dx, %%al" : "=a"(test_byte) : "d"(0x1F7) );
            if (!(test_byte & (1 << 7)))
                break;
        }
    }

    // TODO: Handle disk write error for file table here...
    //   Check error ata pio register here...
}