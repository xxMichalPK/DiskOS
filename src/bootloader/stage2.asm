%define STAGE2_SIZE 6
%include 'src/bootloader/sizes.inc'
[BITS 16]
[org 0x8000]
STAGE2:
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    mov si, OK_MSG
    mov cx, [OK_MSG.len]
    call printc16

    mov si, BOOTEDMSG
    call print16
    
    jmp get_memory_map
    prepareBOOT:
    call enableA20

    mov si, KERNELLOADINGMSG
    call print16

    call load_font
    call load_kernel

    mov si, SETVBEMSG
    call print16

    jmp configVBE
    hlt
    jmp $

StartPM:
    call switch_to_pm
    jmp $

%include 'src/bootloader/lib/print16.inc'
%include 'src/bootloader/lib/print32.inc'
%include 'src/bootloader/disk.inc'
%include 'src/bootloader/mmap.inc'
%include 'src/bootloader/VESA.inc'
%include 'src/bootloader/input.inc'
%include 'src/bootloader/pm.inc'

[BITS 16]
enableA20:
    in al, 0x92
    or al, 2
    out 0x92, al

    mov si, OK_MSG
    mov cx, [OK_MSG.len]
    call printc16

    mov si, A20OK
    call print16

    ret

load_kernel:
    ; Getting first data sector
	mov ax, [EBPB_SECTORS_PER_FAT]
	mov bx, [BPB_TOTAL_FATS]
	mul bx
	xor bx, bx
	; Add reserved sectors
	add ax, [BPB_RESERVED_SECTORS]
	add ax, [BPB_HIDDEN_SECTORS]

	; Load this sector into the DAP, then read to disk buffer
	mov [dap_lba_lo], ax
	mov word [dap_sector_count], 1
	mov bx, BUFFER
	mov word [dap_offset], BUFFER
	call read_sectors_lba

	.search_dir:
		mov ax, ds			; Root dir is now in [buffer]
		mov es, ax			; Set DI to this info
		mov di, BUFFER

		mov cx, word [BPB_DIRECTORY_ENTRIES]	; Search all entries
		mov ax, 0								; Searching at offset 0
	
	.next_root_entry:
		xchg cx, dx					; We use CX in the inner loop...

		mov si, FILENAME			; Start searching for kernel filename
		mov cx, 11
		rep cmpsb
		je .found_file_to_load		; Pointer DI will be at offset 11

		add ax, 32					; Bump searched entries by 1 (32 bytes per entry)

		mov di, BUFFER				; Point to next entry
		add di, ax

		xchg dx, cx					; Get the original CX back
		loop .next_root_entry

        mov si, ERR_MSG
        mov cx, [ERR_MSG.len]
        call printc16
		mov si, NO_KERNEL			; If file is not found, bail out
		call print16
		jmp $
	
	.found_file_to_load:
		mov ax, word [es:di+0x0F]
		mov word [CLUSTER], ax

		mov ax, [BPB_SECTORS_PER_CLUSTER]
		mov bx, [CLUSTER - 2]
		mul bx
		mov dx, ax
		xor bx, bx
		; Getting first data sector
		mov ax, [EBPB_SECTORS_PER_FAT]
		mov bx, [BPB_TOTAL_FATS]
		mul bx
		add ax, [BPB_RESERVED_SECTORS]
		add ax, [BPB_HIDDEN_SECTORS]
		add ax, dx

		; Load this sector into the DAP, then read to disk buffer
		mov [dap_lba_lo], ax
		mov word [dap_sector_count], 1
		mov bx, STAGE3_OFFSET
		mov word [dap_offset], STAGE3_OFFSET
		call read_sectors_lba

		; Get to our Cluster Number
		mov ax, word [CLUSTER]
		call cluster_to_lba
		mov [dap_lba_lo], ax

        mov ax, STAGE3_SECTORS
        mov dl, [BOOT_DRIVE]
        mov word [dap_sector_count], ax
        call read_sectors_lba
        ret

load_font:
    ; Getting first data sector
	mov ax, [EBPB_SECTORS_PER_FAT]
	mov bx, [BPB_TOTAL_FATS]
	mul bx
	xor bx, bx
	; Add reserved sectors
	add ax, [BPB_RESERVED_SECTORS]
	add ax, [BPB_HIDDEN_SECTORS]

	; Load this sector into the DAP, then read to disk buffer
	mov [dap_lba_lo], ax
	mov word [dap_sector_count], 1
	mov bx, BUFFER
	mov word [dap_offset], BUFFER
	call read_sectors_lba

	.search_dir:
		mov ax, ds			; Root dir is now in [buffer]
		mov es, ax			; Set DI to this info
		mov di, BUFFER

		mov cx, word [BPB_DIRECTORY_ENTRIES]	; Search all entries
		mov ax, 0								; Searching at offset 0
	
	.next_root_entry:
		xchg cx, dx					; We use CX in the inner loop...

		mov si, FONT_NAME			; Start searching for kernel filename
		mov cx, 11
		rep cmpsb
		je .found_file_to_load		; Pointer DI will be at offset 11

		add ax, 32					; Bump searched entries by 1 (32 bytes per entry)

		mov di, BUFFER				; Point to next entry
		add di, ax

		xchg dx, cx					; Get the original CX back
		loop .next_root_entry

        mov si, ERR_MSG
        mov cx, [ERR_MSG.len]
        call printc16
		mov si, NO_FONT			; If file is not found, bail out
		call print16
		jmp $
	
	.found_file_to_load:
		mov ax, word [es:di+0x0F]
		mov word [CLUSTER], ax

		mov ax, [BPB_SECTORS_PER_CLUSTER]
		mov bx, [CLUSTER - 2]
		mul bx
		mov dx, ax
		xor bx, bx
		; Getting first data sector
		mov ax, [EBPB_SECTORS_PER_FAT]
		mov bx, [BPB_TOTAL_FATS]
		mul bx
		add ax, [BPB_RESERVED_SECTORS]
		add ax, [BPB_HIDDEN_SECTORS]
		add ax, dx

		; Load this sector into the DAP, then read to disk buffer
		mov [dap_lba_lo], ax
		mov word [dap_sector_count], 1
		mov bx, FONT_OFFSET
		mov word [dap_offset], FONT_OFFSET
		call read_sectors_lba

		; Get to our Cluster Number
		mov ax, word [CLUSTER]
		call cluster_to_lba
		mov [dap_lba_lo], ax

        mov ax, 0x04 ; Size of font in sectors
        mov dl, [BOOT_DRIVE]
        mov word [dap_sector_count], ax
        call read_sectors_lba
        ret

STAGE3_OFFSET equ 0x9000
FONT_OFFSET equ 0x1000
FILENAME: db 'STAGE3  BIN'
FONT_NAME:   db 'TERMU16NBIN'
CLUSTER: dw 0

NO_KERNEL: db "Couldn't find STAGE3.BIN", 0
NO_FONT: db "Couldn't find TERMU16N.BIN", 0

KERNELLOADINGMSG: db '[ *** ] Loading kernel and other components into memory...', 0x0A, 0x0D, 0
SETVBEMSG: db '[ *** ] Configuring VBE - press "c" for manual configuration...', 0x0A, 0x0D, 0
A20OK:     db 'Enabled A20 Line', 0x0A, 0x0D, 0
BOOTEDMSG:    db 'Loaded bootloader from FAT disk at address 0x7f00', 0x0A, 0x0D, 0
FAILUREMSG:   db 'An error ocured! HALTING SYSTEM...', 0x0A, 0x0D, 0
OK_MSG: db '[', 0x07, ' ', 0x00, 'O', 0x02, 'K', 0x02, ' ', 0x00, ']', 0x07, ' ', 0x00
    .len: dw $ - OK_MSG - 7
ERR_MSG: db '[', 0x07, ' ', 0x00, 'E', 0x04, 'R', 0x04, 'R', 0x04, ' ', 0x00, ']', 0x07, ' ', 0x00
    .len: dw $ - ERR_MSG - 8

BPB_OEM_ID                  equ 0x7c00+0x03   ;Must be 8 bytes long
BPB_BYTES_PER_SECTOR        equ 0x7c00+0x0B   ;Bytes Per sector, Usually 512
BPB_SECTORS_PER_CLUSTER     equ 0x7c00+0x0D   ;Sectors Per Cluster
BPB_RESERVED_SECTORS        equ 0x7c00+0x0E   ;Number of Reserved Sectors, Including Boot Record, Logical start of FAT
BPB_TOTAL_FATS              equ 0x7c00+0x10   ;Number of FAT tables, almost always 2
BPB_DIRECTORY_ENTRIES       equ 0x7c00+0x11   ;Number of Directory Entries
BPB_TOTAL_SECTORS           equ 0x7c00+0x13   ;Total sectors in the logical volume, if 0 it means there is over 65535
BPB_MEDIA_TYPE              equ 0x7c00+0x15   ;Media Descriptor Type
BPB_SECTORS_PER_FAT         equ 0x7c00+0x16   ;Sectors per FAT,FAT12/FAT16 only
BPB_SECTORS_PER_TRACK       equ 0x7c00+0x18   ;Sectors per track
BPB_TOTAL_HEADS             equ 0x7c00+0x1A   ;Number of heads or sides on the storage media
BPB_HIDDEN_SECTORS          equ 0x7c00+0x1C   ;Number of hidden sectors, LBA of beginning of partition
BPB_LARGE_TOTAL_SECTORS     equ 0x7c00+0x20   ;Large amount of sectors on media, set if over 65535
;Extended Bios Parameter Block(EBPB), used in FAT32             
EBPB_SECTORS_PER_FAT        equ 0x7c00+0x24   ;Sectors per FAT
EBPB_FLAGS                  equ 0x7c00+0x28   ;Flags
EBPB_FAT_VER                equ 0x7c00+0x2A   ;FAT Version number
EBPB_ROOT_DIR_CLUSTER       equ 0x7c00+0x2C   ;The cluster number of the root directory, usually 2
EBPB_FSINFO_LBA             equ 0x7c00+0x30   ;The Logical LBA of the FSInfo structure
EBPB_BACKUP_VBR_LBA         equ 0x7c00+0x32   ;The Logical LBA of the backup boot sector
EBPB_RESERVED               equ 0x7c00+0x34   ;RESERVED, 12 Bytes
EBPB_DISK_NUM               equ 0x7c00+0x40   ;Drive number, value is arbitrary
EBPB_NT_FLAGS               equ 0x7c00+0x41   ;Flags in Windows NT, Reserved
EBPB_SIGNATURE              equ 0x7c00+0x42   ;Signature, must be 0x28 or 0x29
EBPB_VOLUME_ID              equ 0x7c00+0x43   ;VolumeID 'Serial' number, used for tracking volumes between computers
EBPB_VOLUME_LABEL           equ 0x7c00+0x47   ;Volume label string, 11 Bytes
EBPB_SYS_ID                 equ 0x7c00+0x52   ;System identifier string, 8 Bytes

BUFFER     equ 0x7c00+0x200
BOOT_DRIVE equ 0x7c00+0x40

[BITS 32]
BEGIN_PM:
    cli
    call STAGE3_OFFSET
    jmp $

times (STAGE2_SIZE*512) - ($-$$) db 0