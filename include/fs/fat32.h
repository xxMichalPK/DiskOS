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

typedef struct DirectoryEntry_t {
	char name[11];
	uint8_t attrib;
	uint8_t userattrib;

	char undelete;
	uint16_t createtime;
	uint16_t createdate;
	uint16_t accessdate;
	uint16_t clusterhigh;
    uint16_t modifiedtime;
	uint16_t modifieddate;
	uint16_t clusterlow;
	uint32_t filesize;

} __attribute__ ((packed)) DirectoryEntry_t;

typedef union FAT32_attribute_t {
  struct {
    uint8_t readOnly : 1;
    uint8_t hidden   : 1;
    uint8_t system   : 1;
    uint8_t volume_id: 1;
    uint8_t directory: 1;
    uint8_t archive  : 1;
    uint8_t unused   : 2;
  } __attribute__ ((packed));
  uint8_t attribute_byte;
} FAT32_attribute_t;

enum ENTRY_TYPES {
  NO_MORE_ENTRIES_IN_DIRECTORY  = 0,
  ENTRY_UNUSED                  = 1,
  LONG_FILE_NAME                = 2,
  REGULAR_FILE                  = 4,
  DIRECTORY                     = 8,
  FILE_ERROR                    = 0xFF
};

extern FAT32_BPP_t* FAT32_BPB;
extern uint32_t FIRST_DATA_SECTOR;

void FAT_Init();
void printFiles();
uint8_t *check_filename(uint8_t *filename, const uint16_t filename_length);