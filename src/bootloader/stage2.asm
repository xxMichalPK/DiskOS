[bits 16]
[org 0x7c00]
KERNEL_OFFSET equ 0x2000

mov dl, [0x1900]        ; Get boot drive from saved address
mov [BOOT_DRIVE], dl    ; Save the boot drive number

mov bp, 0x9000 ; Set -up the stack.
mov sp, bp

mov ah, 0x00
mov al, 0x03
int 0x10

mov si, OK_MSG
mov cx, [OK_MSG.len]
call print_color_string

mov si, BOOTEDMSG
call print_string

call load_stage2
call switch_to_pm

jmp $

%include 'src/bootloader/lib/print_string.asm'
%include 'src/bootloader/pm.asm'

[BITS 16]
load_stage2:
    ;; set up ES:BX memory address/segment:offset to load sector(s) into
    mov bx, STAGE2              ; load sector to memory address 0x1000 
    mov es, bx                  
    mov bx, 0x0                 ; ES:BX = 0x1000:0x0

    ;; Set up disk read
    mov dh, 0x00                ; head 0
    mov dl, [BOOT_DRIVE]        ; drive saved in BOOT_DRIVE
    mov ch, 0x00                ; cylinder 0
    mov cl, 0x02                ; starting sector to read from disk

    read_disk:
        mov ah, 0x02                ; BIOS int 13h/ah=2 read disk sectors
        mov al, 0x04                ; # of sectors to read
        int 0x13                    ; BIOS interrupts for disk functions

        jc .disk_read_err           ; retry if disk read error (carry flag set/ = 1)

        ret
        ;; reset segment registers for RAM
        ;mov ax, STAGE2
        ;mov ds, ax                  ; data segment
        ;mov es, ax                  ; extra segment
        ;mov fs, ax                  ; ""
        ;mov gs, ax                  ; ""
        ;mov ss, ax                  ; stack segment

        .disk_read_err:
            cmp di, 5
            je .stop
            
            inc di
            mov si, DISK_ERR_MSG
            call print_string
            jmp read_disk

            .stop:
                hlt
                jmp $

[BITS 32]
BEGIN_PM:
    mov ebx,0xb8000    ; The video address
    mov al,'!'         ; The character to be print
    mov ah,0x0F        ; The color: white(F) on black(0)
    mov [ebx],ax        ; Put the character into the video memory
    jmp $

BOOT_DRIVE: dd 0
DISK_ERR_MSG: db 'DISK READ ERROR!', 0x0A, 0x0D, 0
BOOTEDMSG:    db 'Started at address 0x7c00', 0x0A, 0x0D, 0
OK_MSG: db '[', 0x07, ' ', 0x00, 'O', 0x02, 'K', 0x02, ' ', 0x00, ']', 0x07, ' ', 0x00
    .len: dw $ - OK_MSG - 7
ERR_MSG: db '[', 0x07, ' ', 0x00, 'E', 0x04, 'R', 0x04, 'R', 0x04, ' ', 0x00, ']', 0x07, ' ', 0x00
    .len: dw $ - ERR_MSG - 8

times 4*512 - ($-$$) db 0