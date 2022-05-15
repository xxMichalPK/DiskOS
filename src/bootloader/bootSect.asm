%define BOOT_SIZE 5
%define FONT_SIZE 4
%include 'src/bootloader/sizes.inc'

[bits 16]
[org 0x7c00]

xor ax, ax             ; Ensure data & extra segments are 0 to start, can help
mov es, ax             ; with booting on hardware
mov ds, ax

mov [BOOT_DRIVE], dl    ; Save the boot drive number

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