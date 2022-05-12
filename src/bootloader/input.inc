[BITS 16]
;; input_number: Subroutine to input a number
;; Outputs:
;;   BX = number
input_number:
    xor bx, bx
    mov cx, 10
    .next_digit:
        mov ah, 0
        int 0x16         ; BIOS get keystroke, AH = scancode, AL = ascii char
        mov ah, 0x0E
        int 0x10         ; BIOS teletype output

        cmp al, 0x08
        jne .check_enter
        mov ax, bx
        xor dx, dx
        div cx          ; DX:AX / CX, AX = quotient, DX = remainder
        mov bx, ax
        jmp .next_digit

        .check_enter:
            cmp al, 0x0D     ; Enter key
            je .done
            sub al, '0'     ; Convert ascii number to integer
            mov ah, 0       ; Isolate AL value
            imul bx, bx, 10 ; BX *= 10
            add bx, ax      ; BX += AL
    jmp .next_digit

    .done:
        ret