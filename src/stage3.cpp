#include <kernelUtils.h>
#include <vars/addresses.h>
#include <memory/memory.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <graphics/vbe.h>
#include <libc/stdio.h>

#include <fs/fat32.h>

void printPhysicalMemory();

extern "C" __attribute__ ((section(".st3_entry"))) void st3_main() {
    uint32_t num_SMAP_entries; 
    uint32_t total_memory; 
    SMAP_entry_t *SMAP_entry;
    uint8_t ext[3];

    initKernel();
    puts("[ "); fgColor = 0xFF00FF00; puts("OK"); fgColor = 0xFFFFFFFF; puts(" ] VBE initialized\n\r");
    FAT_Init();
    puts("[ "); fgColor = 0xFF00FF00; puts("OK"); fgColor = 0xFFFFFFFF; puts(" ] FAT32 initialized\n\r");
    puts("[ "); fgColor = 0xFFFF8800; puts("INFO"); fgColor = 0xFFFFFFFF; puts(" ] Starting PMM initialization...\n\r");
    // Initializing physical memory manager
    num_SMAP_entries = *(uint32_t *)MMAP_ADDRESS;
    SMAP_entry       = (SMAP_entry_t *)(MMAP_ADDRESS + 4);
    SMAP_entry += num_SMAP_entries - 1;
    total_memory = SMAP_entry->base_address + SMAP_entry->length - 1;

    initialize_memory_manager(MMAP_AREA, total_memory);

    SMAP_entry = (SMAP_entry_t *)(MMAP_ADDRESS + 4);
    for (uint32_t i = 0; i < num_SMAP_entries; i++) {
        if (SMAP_entry->type == 1)
            initialize_memory_region(SMAP_entry->base_address, SMAP_entry->length);

        SMAP_entry++;
    }

    deinitialize_memory_region(0x1000, 0xF000);
    deinitialize_memory_region(MMAP_AREA, max_blocks / BLOCKS_PER_BYTE);
    puts("[ "); fgColor = 0xFF00FF00; puts("OK"); fgColor = 0xFFFFFFFF; puts(" ] Physical Memory Manager initialized\n\r");
    printPhysicalMemory();

    // Load kernel file here!!
    load_file((uint8_t*)"KERNEL  BIN", 11, KERNEL_ADDRESS, ext);
    //////////////////////////


    puts("[ "); fgColor = 0xFFFF8800; puts("INFO"); fgColor = 0xFFFFFFFF; puts(" ] Starting VMM initialization...\n\r");
    // Initializing virtual memory manager
    initialize_virtual_memory_manager();
    // Identity map VBE framebuffer
    const uint32_t fb_size_in_bytes = (vbe->gfx_mode->y_resolution * vbe->gfx_mode->linear_bytes_per_scanline);
    uint32_t fb_size_in_pages = fb_size_in_bytes / PAGE_SIZE;
    if (fb_size_in_pages % PAGE_SIZE > 0) fb_size_in_pages++;
    fb_size_in_pages *= 2;

    for (uint32_t i = 0, fb_start = vbe->gfx_mode->physical_base_pointer; i < fb_size_in_pages; i++, fb_start += PAGE_SIZE)
        map_page((void *)fb_start, (void *)fb_start);

    deinitialize_memory_region(vbe->gfx_mode->physical_base_pointer, fb_size_in_pages * BLOCK_SIZE);

    // Set physical memory manager variables for kernel to use
    *(uint32_t *)PHYS_MEM_MAX_BLOCKS  = max_blocks;
    *(uint32_t *)PHYS_MEM_USED_BLOCKS = used_blocks;

    // Remove lower half kernel mapping 
    for (uint32_t virt = KERNEL_ADDRESS; virt < 0x400000; virt += PAGE_SIZE)
        unmap_page((void *)virt);

    // Reload CR3 register to flush TLB to update unmapped pages
    __asm__ __volatile__ ("movl %CR3, %ECX; movl %ECX, %CR3");

    // Store current page directory for kernel to use
    *(uint32_t *)CURRENT_PAGE_DIR_ADDRESS = (uint32_t)current_page_directory;

    puts("[ "); fgColor = 0xFF00FF00; puts("OK"); fgColor = 0xFFFFFFFF; puts(" ] Vitual Memory Manager initialized\n\r");

    printf("Welcome from %s%d bootloader at add%c: 0x%x! Time: %.2f", "Stage", 3, 'r', 0x9000, 10.5867);

    ((void (*)(void))0xC0000000)();
    
    for (;;);
}

void printPhysicalMemory()  
{
    uint32_t oldFG = fgColor;
    fgColor = 0xFFAAAAAA;
    uint32_t num_entries = *(uint32_t *)MMAP_ADDRESS;
    SMAP_entry_t *SMAP_entry = (SMAP_entry_t *)(MMAP_ADDRESS + 4);
    for (uint32_t i = 0; i < num_entries; i++) {
        printf("[ *** ] Region: 0x%02x", i);
        printf(": base: 0x%llx", SMAP_entry->base_address);
        printf(" length: 0x%llx", SMAP_entry->length);
        printf(" type: 0x%x", SMAP_entry->type);
        switch(SMAP_entry->type) {
            case 1:
                puts(" (Available)");
                break;
            case 2: 
                puts(" (Reserved)");
                break;
            case 3: 
                puts(" (ACPI Reclaim)");
                break;
            case 4: 
                puts(" (ACPI NVS Memory)");
                break;
            default: 
                puts(" (Reserved)");
                break;
        }
        puts("\n\r");
        SMAP_entry++;
    }
    SMAP_entry--;
    printf("[ *** ] Total memory: 0x%llx", (SMAP_entry->base_address + SMAP_entry->length - 1));
    printf("\n\r[ *** ] Total 4KB blocks: %x", max_blocks);
    printf("\n\r[ *** ] Used or reserved blocks: %x", used_blocks);
    printf("\n\r[ *** ] Free or available blocks: %x", max_blocks - used_blocks);
    puts("\n\r");
    fgColor = oldFG;
}