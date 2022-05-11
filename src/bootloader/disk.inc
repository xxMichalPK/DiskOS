[BITS 16]
read_disk:
    mov ah, 0x02                ; BIOS int 13h/ah=2 read disk sectors
    int 0x13                    ; BIOS interrupts for disk functions

    jc .disk_read_err           ; retry if disk read error (carry flag set/ = 1)
    ret

    .disk_read_err:
        cmp di, 5
        je .stop
        
        inc di
        mov si, ERR_MSG
        mov cx, [ERR_MSG.len]
        call printc16

        mov si, DISK_ERR_MSG
        call print16
        jmp read_disk

        .stop:
            mov si, ERR_MSG
            mov cx, [ERR_MSG.len]
            call printc16

            mov si, FAILUREMSG
            call print16
            hlt
            jmp $