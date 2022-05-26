#include <kernelUtils.h>
#include <vars/addresses.h>
#include <memory/memory.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <graphics/vbe.h>
#include <libc/stdio.h>

#include <interrupts/idt.h>
#include <interrupts/exceptions.h>
#include <interrupts/pic.h>

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

    deinitialize_memory_region(0x1000, 0x28000);
    deinitialize_memory_region(MMAP_AREA, max_blocks / BLOCKS_PER_BYTE);
    puts("[ "); fgColor = 0xFF00FF00; puts("OK"); fgColor = 0xFFFFFFFF; puts(" ] Physical Memory Manager initialized\n\r");
    printPhysicalMemory();

    idt_init();
    idt_set_descriptor(0, (void*)div_by_0_handler, TRAP_GATE_FLAGS); // Divide by 0 handler
    idt_set_descriptor(1, (void*)reserved_int_1_handler, TRAP_GATE_FLAGS); // Int 1 handler (reserved)
    idt_set_descriptor(3, (void*)breakpoint_handler, TRAP_GATE_FLAGS); // Breakpoint handler
    idt_set_descriptor(4, (void*)overflow_handler, TRAP_GATE_FLAGS); // Overflow handler
    idt_set_descriptor(5, (void*)bound_range_handler, TRAP_GATE_FLAGS); // Bound range Exceeded handler
    idt_set_descriptor(6, (void*)invalid_opcode_handler, TRAP_GATE_FLAGS); // Invalid Opcode handler
    idt_set_descriptor(7, (void*)device_not_available_handler, TRAP_GATE_FLAGS); // Device not available handler
    idt_set_descriptor(8, (void*)double_fault_handler, TRAP_GATE_FLAGS); // Double Fault Handler (no return!)
    idt_set_descriptor(10, (void*)invalid_tss_handler, TRAP_GATE_FLAGS); // Invalid TSS handler
    idt_set_descriptor(11, (void*)segment_not_present_handler, TRAP_GATE_FLAGS); // Segment not present handler
    idt_set_descriptor(12, (void*)stack_segment_fault_handler, TRAP_GATE_FLAGS); // Stack segment fault handler
    idt_set_descriptor(13, (void*)general_protection_fault_handler, TRAP_GATE_FLAGS); // General protection fault handler
    idt_set_descriptor(14, (void*)page_fault_handler, TRAP_GATE_FLAGS); // General protection fault handler

    disablePIC();
    remapPIC();

    __asm__ __volatile__ ("sti;");

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

    puts("[ "); fgColor = 0xFFFF8800; puts("INFO"); fgColor = 0xFFFFFFFF; puts(" ] Jumping to kernel at 0xC0100000...\n\r");

    ((void (*)(void))0xC0100000)();
    
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
        printf(": base: 0x%x", SMAP_entry->base_address);
        printf(" length: 0x%x", SMAP_entry->length);
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
    printf("[ *** ] Total memory: 0x%x", (SMAP_entry->base_address + SMAP_entry->length - 1));
    printf("\n\r[ *** ] Total 4KB blocks: %x", max_blocks);
    printf("\n\r[ *** ] Used or reserved blocks: %x", used_blocks);
    printf("\n\r[ *** ] Free or available blocks: %x", max_blocks - used_blocks);
    puts("\n\r");
    fgColor = oldFG;
}