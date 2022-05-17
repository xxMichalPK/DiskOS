[bits 16]
[org 0x7c00]
jmp short main
nop

BPB_OEM_ID                  db "MSDOS5.0"   ;Must be 8 bytes long
BPB_BYTES_PER_SECTOR        dw 0x0200       ;Bytes Per sector, Usually 512
BPB_SECTORS_PER_CLUSTER     db 0x00         ;Sectors Per Cluster
BPB_RESERVED_SECTORS        dw 0x0000       ;Number of Reserved Sectors, Including Boot Record, Logical start of FAT
BPB_TOTAL_FATS              db 0x02         ;Number of FAT tables, almost always 2
BPB_DIRECTORY_ENTRIES       dw 0x0000       ;Number of Directory Entries
BPB_TOTAL_SECTORS           dw 0x0000       ;Total sectors in the logical volume, if 0 it means there is over 65535
BPB_MEDIA_TYPE              db 0x00         ;Media Descriptor Type
BPB_SECTORS_PER_FAT         dw 0x0000       ;Sectors per FAT,FAT12/FAT16 only
BPB_SECTORS_PER_TRACK       dw 0x0000       ;Sectors per track
BPB_TOTAL_HEADS             dw 0x0000       ;Number of heads or sides on the storage media
BPB_HIDDEN_SECTORS          dd 0x00000000   ;Number of hidden sectors, LBA of beginning of partition
BPB_LARGE_TOTAL_SECTORS     dd 0x00000000   ;Large amount of sectors on media, set if over 65535
;Extended Bios Parameter Block(EBPB), used in FAT32             
EBPB_SECTORS_PER_FAT        dd 0x00000000   ;Sectors per FAT
EBPB_FLAGS                  dw 0x0000       ;Flags
EBPB_FAT_VER                dw 0x0000       ;FAT Version number
EBPB_ROOT_DIR_CLUSTER       dd 0x00000002   ;The cluster number of the root directory, usually 2
EBPB_FSINFO_LBA             dw 0x0000       ;The Logical LBA of the FSInfo structure
EBPB_BACKUP_VBR_LBA         dw 0x0000       ;The Logical LBA of the backup boot sector
EBPB_RESERVED times 12      db 0x00         ;RESERVED, 12 Bytes
EBPB_DISK_NUM               db 0x00         ;Drive number, value is arbitrary
EBPB_NT_FLAGS               db 0x00         ;Flags in Windows NT, Reserved
EBPB_SIGNATURE              db 0x29         ;Signature, must be 0x28 or 0x29
EBPB_VOLUME_ID              dd 0x00000000   ;VolumeID 'Serial' number, used for tracking volumes between computers
EBPB_VOLUME_LABEL           db "DISK OS    ";Volume label string, 11 Bytes
EBPB_SYS_ID                 db "FAT32   "   ;System identifier string, 8 Bytes

main:
xor ax, ax             ; Ensure data & extra segments are 0 to start, can help
mov es, ax             ; with booting on hardware
mov ds, ax

mov [BOOT_DRIVE], dl
mov [EBPB_DISK_NUM], dl ; Save the boot drive number

; Start of setting the stack
xor ax,ax
cli
mov ds,ax
mov ss,ax
mov esp, 0x9000         ; Setting stack! Do not forget!!!!!!!!!!
sti
; End of setting the stack

call prepareFAT
loadSTAGE2:
call LOAD_OFFSET

jmp $

print16:
	mov ah,0x0E
	.next:
		mov al,[si]
		or al,al
		jz .done
		int 0x10
		inc si
		jmp .next
	.done:
		ret

%include 'src/bootloader/disk.inc'

prepareFAT:
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

	search_dir:
		mov ax, ds			; Root dir is now in [buffer]
		mov es, ax			; Set DI to this info
		mov di, BUFFER

		mov cx, word [BPB_DIRECTORY_ENTRIES]	; Search all entries
		mov ax, 0								; Searching at offset 0
	
	next_root_entry:
		xchg cx, dx					; We use CX in the inner loop...

		mov si, FILENAME			; Start searching for kernel filename
		mov cx, 11
		rep cmpsb
		je found_file_to_load		; Pointer DI will be at offset 11

		add ax, 32					; Bump searched entries by 1 (32 bytes per entry)

		mov di, BUFFER				; Point to next entry
		add di, ax

		xchg dx, cx					; Get the original CX back
		loop next_root_entry

		mov si, NOT_FOUND			; If file is not found, bail out
		call print16
		jmp $
	
	found_file_to_load:
		mov ax, word [es:di+0x0F]
		mov word [CLUSTER], ax
		;mov ax, word [es:di+0Fh]	; Offset 11 + 15 = 26, contains 1st cluster
		;mov word [CLUSTER], ax

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
		mov bx, LOAD_OFFSET
		mov word [dap_offset], LOAD_OFFSET
		call read_sectors_lba

		; Get to our Cluster Number
		mov ax, word [CLUSTER]
		call cluster_to_lba
		mov [dap_lba_lo], ax

		; Get file size in Bytes
		mov bx, LOAD_OFFSET
		add bx, 28
		mov eax, dword [bx]

		; Divide by 512 to get file size in sectors, aka clusters to load
		mov bx, 512
		div bx

		; See if there's an uneven number of bytes. If so, load an extra cluster
		cmp dx, 0
		je .part2
		add ax, 1
		.part2:
			mov dl, [BOOT_DRIVE]
			mov word [dap_sector_count], ax
			call read_sectors_lba
			jmp loadSTAGE2

; Bootloader Variables
NOT_FOUND: db "Couldn't find bootable file", 0
FILENAME: db "STAGE2  BIN"		   	; File Name of Stage2
BOOT_DRIVE: db 0
CLUSTER: dw 0
LOAD_OFFSET equ 0x7F00	            ; Where Stage2 is loaded to
ENTRY_OFFSET equ 0x0803

times 510 - ($-$$) db 0
dw 0xAA55

BUFFER: