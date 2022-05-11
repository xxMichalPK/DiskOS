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