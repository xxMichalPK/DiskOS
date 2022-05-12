[BITS 16]
get_cur:
	mov ah, 0x03
	int 0x10
	ret

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

printc16:
	mov bp, si
	mov bh, 0
	mov ah, 0x13
	mov al, 0x03
	push ax
	push bx
	push cx
	call get_cur
	pop cx
	pop bx
	pop ax
	int 0x10
	ret

;; printh16: Suboutine to print a hex string
printh16:
	mov cx, 4	; offset in string, counter (4 hex characters)
	.hex_loop:
		mov ax, dx	              ; Hex word passed in DX
		and al, 0Fh               ; Use nibble in AL
		mov bx, hex_to_ascii
		xlatb                     ; AL = [DS:BX + AL]

		mov bx, cx                ; Need bx to index data
		mov [hexString+bx+1], al  ; Store hex char in string
		ror dx, 4                 ; Get next nibble
	loop .hex_loop 

	mov si, hexString             ; Print out hex string
	mov cx, 6                     ; Length of string

hexString: db '0x0000'
hex_to_ascii: db '0123456789ABCDEF'