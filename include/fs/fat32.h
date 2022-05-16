#pragma once
#include <stdint.h>

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
} __attribute__((packed)) FAT32_BPP;