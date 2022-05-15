[BITS 16]
read_disk:
    mov word [cylinder], 0  ; Reset CHS values
    mov byte [head], 0
    mov byte [start_sector], 0

    cmp al, 63          ; Sector limit (1-63)
    jle .canRead          ; Sector is <= 63, good to go
    xor ah, ah
    mov bl, 63
    div bl              ; starting_sector / 63, AH = remainder (sector % 63), AL = quotient (sector / 63)
    cmp ah, 0           ; sector % 63 = 0?
    jne .cylinder
    mov ah, 63          ; Yes, reset sector to 63
    dec al              ; decrement head # (sector / 63)
    .cylinder:
    mov [start_sector], ah  ; AH = sector #, AL = head #
    xor ah, ah
    mov bl, 16
    div bl                  ; Head / 16, AH = remainder (head % 16), AL = quotient (head / 16)
    mov [cylinder], al      ; Cylinder = head / 16
    mov [head], ah
    

    .canRead:
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