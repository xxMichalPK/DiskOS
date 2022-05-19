#include <fs/fat32.h>
#include <libc/string.h>
#include <libc/stdio.h>

FAT32_BPP_t* FAT32_BPB = 0;
uint32_t FAT_SIZE;
uint32_t FIRST_DATA_SECTOR = 0;
void FAT_Init() {
    FAT32_BPB = (FAT32_BPP_t*)0x7c00;
    FAT_SIZE = (FAT32_BPB->BPB_SECTORS_PER_FAT == 0)? FAT32_BPB->EBPB_SECTORS_PER_FAT : FAT32_BPB->BPB_SECTORS_PER_FAT;
    FIRST_DATA_SECTOR = FAT32_BPB->BPB_RESERVED_SECTORS + (FAT32_BPB->BPB_TOTAL_FATS * FAT_SIZE) + FAT32_BPB->BPB_DIRECTORY_ENTRIES + 1;
}

void printFiles() {
    uint32_t *buffer;
    vbe->PrintString("Files on disk:\n\r");
    rw_sectors(1, FIRST_DATA_SECTOR, (uint32_t)buffer, READ_WITH_RETRY);
    int p = 0;
    bool empty = false;

    while (!empty) {
        for (int l = 0; l < 11; l++) {
            if (*((char*)buffer+p) == '\0' && l == 0) { empty = true; break; }
            vbe->PutChar(*((char*)buffer+p));
            p++;
        }
        p += (32-11);
        vbe->PrintString("  |  ");
    }
}

//-------------------------------------
// Check for filename in filetable
// Input 1: File name to check
//       2: Length of file name
// Output 1: Error Code 0-success, 1-Fail
//       (2): File pointer will point to start of file table entry if found
//-------------------------------------
uint8_t *check_filename(uint8_t *filename, const uint16_t filename_length)
{
    uint16_t i; 
    uint8_t *ptr;
    rw_sectors(1, FIRST_DATA_SECTOR, (uint32_t)ptr, READ_WITH_RETRY);

    bool stop = false;
    while (!stop) {
        for (i = 0; i < filename_length;) {
            if (filename[i] == ptr[i]) i++;
            if (i == 10 || (i == filename_length && ptr[i] == ' ')) { // Found file
                stop = true;
                break;
            }
            if (filename[i] != ptr[i]) break;
        }

        if (!stop)
            ptr += 32;     // Go to next file table entry
    }

    return (uint8_t*)ptr;
}

static uint16_t readi16(uint8_t *buff, size_t offset)
{
    uint8_t *ubuff = buff + offset;
    return ubuff[1] << 8 | ubuff[0];
}
static uint32_t readi32(uint8_t *buff, size_t offset) {
    uint8_t *ubuff = buff + offset;
    return
        ((ubuff[3] << 24) & 0xFF000000) |
        ((ubuff[2] << 16) & 0x00FF0000) |
        ((ubuff[1] << 8) & 0x0000FF00) |
        (ubuff[0] & 0x000000FF);
}

// load_file: Read a file from filetable and its sectors into a memory location
//
// input 1: File name (address)
//       2: File name length   
//       3: Memory offset to load data to
//       4: File extension
// output 1: Return code
uint16_t load_file(uint8_t *filename, uint16_t filename_length, uint32_t address, uint8_t *file_ext)
{
    uint16_t return_code = 0;
    uint8_t *file_ptr;

    // Check for filename in file table, if successful file_ptr points to file table entry
    file_ptr = check_filename(filename, filename_length);  

    if (*file_ptr == 0)  // File not found
        return 0;

	// Get file type into variable to pass back
    for (uint8_t i = 0; i < 3; i++)
        file_ext[i] = file_ptr[8+i];
    
    // Get the cluster in which the file starts
    uint16_t cluster = readi16(file_ptr, 26);

    // Calculate the first sector of the file
    uint32_t sector = ((cluster - 2) * FAT32_BPB->BPB_SECTORS_PER_CLUSTER) + FIRST_DATA_SECTOR;

    // Calculate num sectors to load
    uint32_t size = readi32(file_ptr, 28);
    if (size % 512 != 0) size = size / 512 + 1;
    else size = size / 512;

    return_code = 1;    // Init return code to 'success'

    // Read sectors using ATA PIO
    rw_sectors(size, sector, address, READ_WITH_RETRY);

    return return_code;	
}