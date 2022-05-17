[BITS 16]
[org 0x7f00]
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

    ;call load_font
    ;call load_kernel

    mov si, SETVBEMSG
    call print16

    jmp configVBE
    hlt
    jmp $

StartPM:
    ;call switch_to_pm
    jmp $

%include 'src/bootloader/lib/print16.inc'
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

FONT_OFFSET equ 0x1000
KERNEL_OFFSET equ 0x9000
KERNELLOADINGMSG: db '[ *** ] Loading kernel and other components into memory...', 0x0A, 0x0D, 0
SETVBEMSG: db '[ *** ] Configuring VBE - press "c" for manual configuration...', 0x0A, 0x0D, 0
A20OK:     db 'Enabled A20 Line', 0x0A, 0x0D, 0
BOOTEDMSG:    db 'Loaded bootloader from FAT disk at address 0x7f00', 0x0A, 0x0D, 0
FAILUREMSG:   db 'An error ocured! HALTING SYSTEM...', 0x0A, 0x0D, 0
OK_MSG: db '[', 0x07, ' ', 0x00, 'O', 0x02, 'K', 0x02, ' ', 0x00, ']', 0x07, ' ', 0x00
    .len: dw $ - OK_MSG - 7
ERR_MSG: db '[', 0x07, ' ', 0x00, 'E', 0x04, 'R', 0x04, 'R', 0x04, ' ', 0x00, ']', 0x07, ' ', 0x00
    .len: dw $ - ERR_MSG - 8
[BITS 32]
BEGIN_PM:
    cli
    call KERNEL_OFFSET
    jmp $

times 5*512 - ($-$$) db 0