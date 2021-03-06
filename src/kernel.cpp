#include <kernelUtils.h>
#include <vars/addresses.h>
#include <memory/memory.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <graphics/vbe.h>
#include <libc/stdio.h>

#include <fs/fat32.h>

void printPhysicalMemory();

extern "C" void kMain(void) {
    const char* copyMSG = "-- DiskOS v0.1 - Copyright Michal Pazurek 2022 --\n\r"
                          "--   Github:  https://github.com/xxMichalPk/   --\n\r\n\r";
    const char* prompt = "diOS:> ";

    uint32_t num_SMAP_entries; 
    uint32_t total_memory; 
    SMAP_entry_t *SMAP_entry;

    initKernel();
    puts("[ "); fgColor = 0xFF00FF00; puts("OK"); fgColor = 0xFFFFFFFF; puts(" ] VBE initialized\n\r");
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
    puts("[ "); fgColor = 0xFF00FF00; puts("OK"); fgColor = 0xFFFFFFFF; puts(" ] Vitual Memory Manager initialized\n\r");

    // Info for unhandled functions:
    /*puts("[ "); fgColor = 0xFFFF8800; puts("ERR INFO"); fgColor = 0xFFFFFFFF; puts(" ] Interrupts are not supported!\n\r");
    puts("[ "); fgColor = 0xFFFF8800; puts("ERR INFO"); fgColor = 0xFFFFFFFF; puts(" ] No Keyboard driver!\n\r");
    puts("[ "); fgColor = 0xFFFF8800; puts("ERR INFO"); fgColor = 0xFFFFFFFF; puts(" ] No Mouse driver!\n\r");
    puts("[ "); fgColor = 0xFFFF8800; puts("ERR INFO"); fgColor = 0xFFFFFFFF; puts(" ] No Disk driver!\n\r");*/

    FAT32_BPP* fat = (FAT32_BPP*)0x7c00;
    uint32_t oldFG = fgColor;
    fgColor = 0xFFAAAAAA;
    puts("-------- File Allocation Table (FAT32) Info --------\n\r");
    printf("\t\tStart address:       0x%x\n\r", fat->START);
    printf("\t\tOEM ID:              %s\n\r", fat->OEM_ID);
    printf("\t\tBytes Per Sector:    %d\n\r", fat->BPB_BYTES_PER_SECTOR);
    printf("\t\tSectors Per Cluster: %d\n\r", fat->BPB_SECTORS_PER_CLUSTER);
    printf("\t\tReserved Sectors:    %d\n\r", fat->BPB_RESERVED_SECTORS);
    printf("\t\tTotal FAT's:         %d\n\r", fat->BPB_TOTAL_FATS);
    printf("\t\tDirectories:         %d\n\r", fat->BPB_DIRECTORY_ENTRIES);
    printf("\t\tTotal Sectors:       %d\n\r", fat->BPB_TOTAL_SECTORS);
    printf("\t\tMedia Type:          0x%x\n\r", fat->BPB_MEDIA_TYPE);
    printf("\t\tSectors Per FAT:     %d\n\r", fat->BPB_SECTORS_PER_FAT);
    printf("\t\tSectros Per Track:   %d\n\r", fat->BPB_SECTORS_PER_TRACK);
    printf("\t\tTotal Heads:         %d\n\r", fat->BPB_TOTAL_HEADS);
    printf("\t\tHidden Sectors:      %d\n\r", fat->BPB_HIDDEN_SECTORS);
    printf("\t\tTotal Large Sectors: %d\n\r", fat->BPB_LARGE_TOTAL_SECTORS);
    puts("-------- Extended File Allocation Table Info -------\n\r");
    printf("\t\tSectors Per FAT:     %d\n\r", fat->EBPB_SECTORS_PER_FAT);
    printf("\t\tFlags:               %d\n\r", fat->EBPB_FLAGS);
    printf("\t\tFAT Version:         %d\n\r", fat->EBPB_FAT_VER);
    printf("\t\tRoot Directory Clust:%d\n\r", fat->EBPB_ROOT_DIR_CLUSTER);
    printf("\t\tFS Info (FAT32 only):0x%x\n\r", fat->EBPB_FSINFO_LBA);
    printf("\t\tBackup VBR:          0x%x\n\r", fat->EBPB_BACKUP_VBR_LBA);
    printf("\t\tReserved:            %s\n\r", fat->EBPB_RESERVED);
    printf("\t\tDisk number (hex):   0x%x\n\r", fat->EBPB_DISK_NUM);
    printf("\t\tNT Flags:            %d\n\r", fat->EBPB_NT_FLAGS);
    printf("\t\tSignature (hex):     0x%x\n\r", fat->EBPB_SIGNATURE);
    printf("\t\tVolume ID:           0x%x\n\r", fat->EBPB_VOLUME_ID);
    printf("\t\tVolume Label:        %s\n\r", fat->EBPB_VOLUME_LABEL);
    printf("\t\tFAT System ID:       %s\n\r", fat->EBPB_SYS_ID);

    fgColor = oldFG;
    puts("\n\r");
    fgColor = 0xFF22FFAA;
    puts(copyMSG);
    // fgColor = 0xFFFF2050;
    // //puts(prompt);
    // fgColor = 0xFFDDDDDD;
    // puts("\n\n\r");

    // puts("[ "); fgColor = 0xFFFF0000; puts("ERR"); fgColor = 0xFFFFFFFF; puts(" ] Going into an infinite loop...\n\r");
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