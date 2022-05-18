#include <stdint.h>
#include <fs/fat32.h>
#include <graphics/vbe.h>

extern "C" __attribute__ ((section(".st3_entry"))) void stage3() {
    FAT_Init();
    VBE_Init();
    printFiles();
    VBE.PrintString("\n\n\rHello this is the 3'rd stage of DiskOS Bootloader!\n\r");
    for (;;);
}