#pragma once
#include <stdint.h>
#include <disk/ata.h>
#include <graphics/vbe.h>

typedef struct {
    // FAT12, FAT16
    uint8_t START[3];
    uint8_t OEM_ID[8];
    uint16_t BPB_BYTES_PER_SECTOR;
    uint8_t BPB_SECTORS_PER_CLUSTER;
    uint16_t BPB_RESERVED_SECTORS;
    uint8_t BPB_TOTAL_FATS;
    uint16_t BPB_DIRECTORY_ENTRIES;
    uint16_t BPB_TOTAL_SECTORS;
    uint8_t BPB_MEDIA_TYPE;
    uint16_t BPB_SECTORS_PER_FAT;
    uint16_t BPB_SECTORS_PER_TRACK;
    uint16_t BPB_TOTAL_HEADS;
    uint32_t BPB_HIDDEN_SECTORS;
    uint32_t BPB_LARGE_TOTAL_SECTORS;
    // Extended Bios Parameter Block(EBPB), used in FAT32
    uint32_t EBPB_SECTORS_PER_FAT;
    uint16_t EBPB_FLAGS;
    uint16_t EBPB_FAT_VER;
    uint32_t EBPB_ROOT_DIR_CLUSTER;
    uint16_t EBPB_FSINFO_LBA;
    uint16_t EBPB_BACKUP_VBR_LBA;
    uint8_t EBPB_RESERVED[12];
    uint8_t EBPB_DISK_NUM;
    uint8_t EBPB_NT_FLAGS;
    uint8_t EBPB_SIGNATURE;
    uint32_t EBPB_VOLUME_ID;
    uint8_t EBPB_VOLUME_LABEL[11];
    uint8_t EBPB_SYS_ID[8];
} __attribute__((packed)) FAT32_BPP_t;

FAT32_BPP_t* FAT32_BPB;
uint32_t FAT_SIZE;
uint32_t FIRST_DATA_SECTOR;
void FAT_Init() {
    FAT32_BPB = (FAT32_BPP_t*)0x7c00;
    FAT_SIZE = (FAT32_BPB->BPB_SECTORS_PER_FAT == 0)? FAT32_BPB->EBPB_SECTORS_PER_FAT : FAT32_BPB->BPB_SECTORS_PER_FAT;
    FIRST_DATA_SECTOR = FAT32_BPB->BPB_RESERVED_SECTORS + (FAT32_BPB->BPB_TOTAL_FATS * FAT_SIZE) + FAT32_BPB->BPB_DIRECTORY_ENTRIES + 1;
}

void printFiles() {
    uint32_t *buffer;
    VBE.PrintString("Files on disk:\n\r");
    rw_sectors(1, FIRST_DATA_SECTOR, (uint32_t)buffer, READ_WITH_RETRY);
    int p = 0;
    bool empty = false;

    while (!empty) {
        for (int l = 0; l < 11; l++) {
            if (*((char*)buffer+p) == '\0' && l == 0) { empty = true; break; }
            VBE.PutChar(*((char*)buffer+p));
            p++;
        }
        p += (32-11);
        VBE.PrintString("  |  ");
    }

    // uint32_t startClust = 0;
    // int strPTR = 0;
	// char *str = "TEST11  TXT";
	// bool first = true;
	// int iter = 0;

    // while (!empty) {
	// 	char ch = *((char*)buffer+p);
	// 	if (ch == str[strPTR] && iter != 11) {
	// 		if (first) {
	// 			first = false;
	// 			iter++;
	// 			strPTR++;
	// 		} else if (iter < 11) {
	// 			iter++;
	// 			strPTR++;
	// 		}
	// 	} else if (iter == 11) {
    //         startClust = *((char*)buffer+p+9);
    //         break;
	// 	} else {
	// 		strPTR = 0;
	// 		first = true;
	// 		iter = 0;
	// 	}
	// }
}