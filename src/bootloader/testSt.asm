[bits 16]
[org 0x7F00]
mov ah, 0x00
mov al, 0x03
int 0x10

mov ah, 0x0E
mov al, 'T'
int 0x10
mov al, 'E'
int 0x10
mov al, 'S'
int 0x10
mov al, 'T'
int 0x10
mov al, ' '
int 0x10
mov al, 'S'
int 0x10
mov al, 'T'
int 0x10
mov al, 'A'
int 0x10
mov al, 'G'
int 0x10
mov al, 'E'
int 0x10

times 512-($-$$) db 0