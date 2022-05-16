%define BOOT_SIZE 5
%define FONT_SIZE 4
%include 'src/bootloader/sizes.inc'

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

mov [BOOT_DRIVE], dl    ; Save the boot drive number
mov [EBPB_DISK_NUM], dl

; Start of setting the stack
xor ax,ax
cli
mov ds,ax
mov ss,ax
mov esp, 0x9000         ; Setting stack! Do not forget!!!!!!!!!!
sti
; End of setting the stack

mov ah, 0x00
mov al, 0x03
int 0x10

mov si, OK_MSG
mov cx, [OK_MSG.len]
call printc16

mov si, BOOTEDMSG
call print16

nop
call load_stage2
nop

jmp $

%include 'src/bootloader/lib/print16.inc'
%include 'src/bootloader/disk.inc'

[BITS 16]
load_stage2:
    ;; set up ES:BX memory address/segment:offset to load sector(s) into
    mov bx, STAGE2              ; load sector to memory address of STAGE2 

    ;; Set up disk read
    mov dl, [BOOT_DRIVE]        ; drive saved in BOOT_DRIVE
    mov ch, 0x00                ; cylinder 0
    mov dh, 0x00                ; head 0
    mov cl, 0x02                ; starting sector to read from disk

    mov al, BOOT_SIZE               ; # of sectors to read
    call read_disk
    jmp STAGE2

BOOT_DRIVE: dd 0
cylinder: dw 0
head: db 0
start_sector: db 0

DISK_ERR_MSG: db 'DISK READ ERROR!', 0x0A, 0x0D, 0
BOOTEDMSG:    db 'Started at address 0x7c00', 0x0A, 0x0D, 0
FAILUREMSG:   db 'An unexpected error ocured! HALTING SYSTEM...', 0x0A, 0x0D, 0
OK_MSG: db '[', 0x07, ' ', 0x00, 'O', 0x02, 'K', 0x02, ' ', 0x00, ']', 0x07, ' ', 0x00
    .len: dw $ - OK_MSG - 7
ERR_MSG: db '[', 0x07, ' ', 0x00, 'E', 0x04, 'R', 0x04, 'R', 0x04, ' ', 0x00, ']', 0x07, ' ', 0x00
    .len: dw $ - ERR_MSG - 8

times 510 - ($-$$) db 0
dw 0xAA55

[BITS 16]
STAGE2:
    jmp get_memory_map
    prepareBOOT:
    mov si, OK_MSG
    mov cx, [OK_MSG.len]
    call printc16
    mov si, STAGE2MSG
    call print16

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

%include 'src/bootloader/mmap.inc'
%include 'src/bootloader/VESA.inc'
%include 'src/bootloader/input.inc'
%include 'src/bootloader/lib/print32.inc'
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

load_font:
    mov bx, FONT_OFFSET              ; load sector to memory address of STAGE2

    ;; Set up disk read
    mov dl, [BOOT_DRIVE]        ; drive saved in BOOT_DRIVE
    mov ch, 0x00                ; cylinder 0
    mov dh, 0x00                ; head 0
    mov cl, BOOT_SIZE + 2         ; starting sector to read from disk

    mov al, FONT_SIZE           ; # of sectors to read
    call read_disk
    ret

load_kernel:
    ;; set up ES:BX memory address/segment:offset to load sector(s) into
    mov bx, KERNEL_OFFSET              ; load sector to memory address of STAGE2

    ;; Set up disk read
    mov dl, [BOOT_DRIVE]            ; drive saved in BOOT_DRIVE
    mov ch, 0x00                    ; cylinder 0
    mov dh, 0x00                    ; head 0
    mov cl, BOOT_SIZE + FONT_SIZE + 2 ; starting sector to read from disk

    mov al, KERNEL_SECTORS          ; # of sectors to read
    call read_disk
    ret

FONT_OFFSET equ 0x1000
KERNEL_OFFSET equ 0x9000
KERNELLOADINGMSG: db '[ *** ] Loading kernel and other components into memory...', 0x0A, 0x0D, 0
STAGE2MSG: db 'Loaded 2nd stage bootloader', 0x0A, 0x0D, 0
SETVBEMSG: db '[ *** ] Configuring VBE - press "c" for manual configuration...', 0x0A, 0x0D, 0
A20OK:     db 'Enabled A20 Line', 0x0A, 0x0D, 0

[BITS 32]
BEGIN_PM:
    cli
    call KERNEL_OFFSET
    jmp $

times (BOOT_SIZE + 1)*512 - ($-$$) db 0