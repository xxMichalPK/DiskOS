#include <kernelUtils.h>
#include <vars/addresses.h>
#include <memory/memory.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <graphics/vbe.h>
#include <libc/stdio.h>

#include <fs/fat32.h>

extern "C" __attribute__ ((section(".krn_entry"))) void kMain(void) {
    const char* copyMSG = "-- DiskOS v0.1 - Copyright Michal Pazurek 2022 --\n\r"
                          "--   Github:  https://github.com/xxMichalPk/   --\n\r\n\r";
    const char* prompt = "diOS:> ";

    //initKernel();
    vbe->gfx_mode = (vbe_mode_info_t*)0x5000;
    vbe->sysFont = (ASM_FONT*)0x1000;

    //vbe->PlotPixel(0,0,0xFFFFFFFF);
    vbe->TriangleDraw(Point {0, 0}, Point { 140, 0 }, Point {70, 140}, 0xFFFF2299);
    // Info for unhandled functions:
    /*puts("[ "); fgColor = 0xFFFF8800; puts("ERR INFO"); fgColor = 0xFFFFFFFF; puts(" ] Interrupts are not supported!\n\r");
    puts("[ "); fgColor = 0xFFFF8800; puts("ERR INFO"); fgColor = 0xFFFFFFFF; puts(" ] No Keyboard driver!\n\r");
    puts("[ "); fgColor = 0xFFFF8800; puts("ERR INFO"); fgColor = 0xFFFFFFFF; puts(" ] No Mouse driver!\n\r");
    puts("[ "); fgColor = 0xFFFF8800; puts("ERR INFO"); fgColor = 0xFFFFFFFF; puts(" ] No Disk driver!\n\r");*/

    /*FAT32_BPP_t* fat = (FAT32_BPP_t*)0x7c00;
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
    puts(copyMSG);*/
    // fgColor = 0xFFFF2050;
    // //puts(prompt);
    // fgColor = 0xFFDDDDDD;
    // puts("\n\n\r");

    // puts("[ "); fgColor = 0xFFFF0000; puts("ERR"); fgColor = 0xFFFFFFFF; puts(" ] Going into an infinite loop...\n\r");
    for (;;);
}