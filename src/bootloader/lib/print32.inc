[BITS 32]
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f
CR equ 0x0d    ; Carriage return
LF equ 0x0a    ; Line feed


cur_row:      dd 0x00
cur_col:      dd 0x00
screen_width: dd 0x00

set_cursor:
    mov ecx, [cur_row]          ; EAX = cur_row
    imul ecx, [screen_width]    ; ECX = cur_row * screen_width
    add ecx, [cur_col]          ; ECX = cur_row * screen_width + cur_col

    ; Send low byte of cursor position to video card
    mov edx, 0x3d4
    mov al, 0x0f
    out dx, al                  ; Output 0x0f to 0x3d4
    inc edx
    mov al, cl
    out dx, al                  ; Output lower byte of cursor pos to 0x3d5

    ; Send high byte of cursor position to video card
    dec edx
    mov al, 0x0e
    out dx, al                  ; Output 0x0e to 0x3d4
    inc edx
    mov al, ch
    out dx, al                  ; Output higher byte of cursor pos to 0x3d5

    ret

print32:
    push edi
    push esi
    push ebx
    mov esi, eax                ; Set ESI to beginning of string

    ; Assume base of text video memory is ALWAYS 0xb8000
    mov ebx, VIDEO_MEMORY    ; EBX = beginning of video memory

    mov eax, [cur_row]          ; EAX = cur_row
    mul dword [screen_width]    ; EAX = cur_row * screen_width
    mov edx, eax                ; EDX = copy of offset to beginning of line
    add eax, [cur_col]          ; EAX = cur_row * screen_width + cur_col
    lea edi, [ebx + eax * 2]    ; EDI = memory location of current screen cell

    mov ah, WHITE_ON_BLACK 		; Set attribute
    jmp .getch
.repeat:
    cmp al, CR                  ; Is the character a carriage return?
    jne .chk_lf                 ;     If not skip and check for line feed
    lea edi, [ebx + edx * 2]    ; Set current video memory pointer to beginning of line
    mov dword [cur_col], 0      ; Set current column to 0
    jmp .getch                  ; Process next character
.chk_lf:
    cmp al, LF                  ; Is the character a line feed?
    jne .write_chr              ;     If not then write character
    mov eax, [screen_width]
    lea edi, [edi + eax * 2]    ; Set current video memory ptr to same pos on next line
    inc dword [cur_row]         ; Set current row to next line
    mov ah, WHITE_ON_BLACK ; Reset attribute
    jmp .getch                  ; Process next character

.write_chr:
    inc dword [cur_col]         ; Update current column
    stosw

.getch:
    lodsb                       ; Get character from string
    test al, al                 ; Have we reached end of string?
    jnz .repeat                 ;     if not process next character

.end:
    call set_cursor             ; Update hardware cursor position

    pop ebx
    pop esi
    pop edi
    ret

COLOR: db 0x0F

setColor:
    mov al, ah
    mov [COLOR], al
    ret

printc32:
    push edi
    push esi
    push ebx
    mov esi, eax                ; Set ESI to beginning of string

    ; Assume base of text video memory is ALWAYS 0xb8000
    mov ebx, VIDEO_MEMORY    ; EBX = beginning of video memory

    mov eax, [cur_row]          ; EAX = cur_row
    mul dword [screen_width]    ; EAX = cur_row * screen_width
    mov edx, eax                ; EDX = copy of offset to beginning of line
    add eax, [cur_col]          ; EAX = cur_row * screen_width + cur_col
    lea edi, [ebx + eax * 2]    ; EDI = memory location of current screen cell

    mov ah, [COLOR]
    jmp .getch
.repeat:
    cmp al, CR                  ; Is the character a carriage return?
    jne .chk_lf                 ;     If not skip and check for line feed
    lea edi, [ebx + edx * 2]    ; Set current video memory pointer to beginning of line
    mov dword [cur_col], 0      ; Set current column to 0
    jmp .getch                  ; Process next character
.chk_lf:
    cmp al, LF                  ; Is the character a line feed?
    jne .write_chr              ;     If not then write character
    mov eax, [screen_width]
    lea edi, [edi + eax * 2]    ; Set current video memory ptr to same pos on next line
    inc dword [cur_row]         ; Set current row to next line
    mov ah, WHITE_ON_BLACK ; Reset attribute
    jmp .getch                  ; Process next character

.write_chr:
    inc dword [cur_col]         ; Update current column
    stosw

.getch:
    lodsb                       ; Get character from string
    test al, al                 ; Have we reached end of string?
    jnz .repeat                 ;     if not process next character

.end:
    call set_cursor             ; Update hardware cursor position

    pop ebx
    pop esi
    pop edi
    ret


printOK32:
    mov eax, OK1
    call print32

    mov ah, 0x02
    call setColor
    mov eax, OK2
    call printc32

    mov eax, OK3
    call print32
    ret

OK1: db '[ ', 0
OK2: db 'OK', 0
OK3: db ' ] ', 0