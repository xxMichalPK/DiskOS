gdt_start:
    gdt_null: ; the mandatory null descriptor
        dd 0x0 ; ’dd ’ means define double word ( i.e. 4 bytes )
        dd 0x0
    gdt_code: ; the code segment descriptor
        dw 0xffff ; Limit ( bits 0 -15)
        dw 0x0 ; Base ( bits 0 -15)
        db 0x0 ; Base ( bits 16 -23)
        db 10011010b ; 1st flags , type flags
        db 11001111b ; 2nd flags , Limit ( bits 16 -19)
        db 0x0 ; Base ( bits 24 -31)
    gdt_data: ; the data segment descriptor
        dw 0xffff ; Limit ( bits 0 -15)
        dw 0x0 ; Base ( bits 0 -15)
        db 0x0 ; Base ( bits 16 -23)
        db 10010010b ; 1st flags , type flags
        db 11001111b ; 2nd flags , Limit ( bits 16 -19)
        db 0x0 ; Base ( bits 24 -31)
    gdt_end: ; The reason for putting a label at the end of the

    gdt_descriptor:
        dw gdt_end - gdt_start - 1  ; Size of our GDT , always less one
                                    ; of the true size
        dd gdt_start ; Start address of our GDT

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
