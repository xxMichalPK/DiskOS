%define SECTORS 2

[bits 16]
[org 0x7c00]

xor ax, ax             ; Ensure data & extra segments are 0 to start, can help
mov es, ax             ; with booting on hardware
mov ds, ax

mov [BOOT_DRIVE], dl    ; Save the boot drive number

cli
mov bp, 0x9000 ; Set -up the stack.
mov sp, bp
sti

mov ah, 0x00
mov al, 0x03
int 0x10

mov si, OK_MSG
mov cx, [OK_MSG.len]
call printc16

mov si, BOOTEDMSG
call print16

call load_stage2

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

    mov al, SECTORS                ; # of sectors to read
    call read_disk
    jmp STAGE2

BOOT_DRIVE: dd 0
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
    mov si, OK_MSG
    mov cx, [OK_MSG.len]
    call printc16

    mov si, STAGE2MSG
    call print16

    call enableA20

    call switch_to_pm
    jmp $

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

STAGE2MSG: db 'Loaded 2nd stage bootloader', 0x0A, 0x0D, 0
A20OK:     db 'Enabled A20 Line', 0x0A, 0x0D, 0

[BITS 32]
BEGIN_PM:
    call printOK32

    mov eax, PMMSG
    call print32

    jmp $

PMMSG:  db 'Landed in 32-bit protected mode', 0x0A, 0x0D, 0

times (SECTORS + 1)*512 - ($-$$) db 0