[BITS 16]
configVBE:
    mov al, 0
    mov ah, 0x86
    mov dx, 10
    mov cx, dx
    int 0x15

    mov ah, 0x01
    int 0x16
    cmp al, 'c' ; Check if 'c' pressed
    jne .bootDefaults   ; NO? Boot with defaults

    jmp .confirm

    .bootDefaults:
        mov word [width], 1024
        mov word [height], 768
        mov word [bpp], 32
        jmp set_up_vbe

    .confirm:
        mov si, OK_MSG
        mov cx, [OK_MSG.len]
        call printc16
        mov si, press_ent_str
        call print16
        .getENTER:
            mov ah, 0x00
            int 0x16
            cmp al, 0x0D            ; Check if enter pressed
            jne .getENTER    ; If no - repeat
        jmp input_gfx_values

;; User input x/y resolution and bpp values
input_gfx_values:
    xor ax, ax
    xor ecx, ecx
    cmp word [width], 0  ;; Already has values set, skip
    jne set_up_vbe

    mov ah,0x02
    mov bh, 0x00
    push ax
	push bx
	push cx
	call get_cur
	pop cx
	pop bx
	pop ax
    add dh, 1
    mov dl, 0x01
    int 0x10

    mov si, choose_gfx_string
    call print16
    
    .setX:
        mov si, width_string
        call print16

        call input_number
        mov [width], bx
        cmp word [width], 0
        je .setDefaultX

    .setY:
        mov si, height_string
        call print16

        call input_number
        mov [height], bx
        cmp word [height], 0
        je .setDefaultY

    .setBPP:
        mov si, bpp_string
        call print16

        call input_number
        mov [bpp], bl
        cmp word [bpp], 0
        je .setDefaultBPP

    jmp set_up_vbe

    .setDefaultX:
        mov word [width], 1024
        jmp .setY
    
    .setDefaultY:
        mov word [height], 768
        jmp .setBPP
    
    .setDefaultBPP:
        mov word [bpp], 32
        jmp set_up_vbe

;; Set up vbe info structure
set_up_vbe:
    sti
    xor ax, ax
    mov es, ax  ; Reset ES to 0

    mov ax, 4F00h
    mov di, vbe_info_block
    int 0x10

    cmp ax, 4Fh
    jne error

    mov ax, word [vbe_info_block.video_mode_pointer]
    mov [offset], ax
    mov ax, word [vbe_info_block.video_mode_pointer+2]
    mov [t_segment], ax
        
    mov fs, ax
    mov si, [offset]

;; Get next VBE video mode
.find_mode:
    mov dx, [fs:si]
    inc si
    inc si
    mov [offset], si
    mov [mode], dx

    cmp dx, word 0FFFFh	        ; at end of video mode list?
    je end_of_modes

    mov ax, 4F01h		        ; get vbe mode info
    mov cx, [mode]
    mov di, mode_info_block		; Mode info block mem address
    int 0x10

    cmp ax, 4Fh
    jne error

    ;; Compare values with desired values
    mov ax, [width]
    cmp ax, [mode_info_block.x_resolution]
    jne .next_mode

    mov ax, [height]					
    cmp ax, [mode_info_block.y_resolution]
    jne .next_mode

    mov al, [bpp]
    cmp al, [mode_info_block.bits_per_pixel]
    jne .next_mode

    mov ax, 4F02h	; Set VBE mode
    mov bx, [mode]
    or bx, 4000h	; Enable linear frame buffer, bit 14
    xor di, di
    int 0x10

    cmp ax, 4Fh
    jne error

    jmp StartPM    ; Move on to set up GDT & 32bit protected mode

.next_mode:
    mov ax, [t_segment]
    mov fs, ax
    mov si, [offset]
    jmp .find_mode

error:
    mov ax, 0E46h	; Print 'F'
    int 0x10
    cli
    hlt

end_of_modes:
    mov si, mode_not_found_string
    call print16
    .try_again:
        xor ax, ax
        int 0x16
        cmp al, 'n'
        jne .check_no
        mov word [width], 0
        mov word [height], 0
        mov word [bpp], 0

        mov ax, 0x0E0A       ; Print newline
        int 0x10
        mov al, 0Dh
        int 0x10
        jmp input_gfx_values

    .check_no:
        cmp al, 'y'
        jne .try_again
        mov word [width], 1024      ; Take default values
        mov word [height], 768
        mov byte [bpp], 32 
        jmp set_up_vbe

;; DATA AREA FOR VESA Support
;; String constants and length values
press_ent_str: db 'Press [ENTER] to start the configuration!', 0x0A, 0x0D, 0
choose_gfx_string: db 218,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196, 'Input graphics mode values', 196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,191, 0
width_string: db 0x0A, 0x0D, '   Width [1024]: ', 0
height_string: db 0x0A, 0x0D, '   Height [768]: ', 0
bpp_string: db 0x0A, 0x0D, '   BPP [32]: ', 0
mode_not_found_string: db 0x0A, 0x0D, '   Video mode not found!', 0x0A, 0x0D, '   Use defaults - 1024x768 32BPP? [y/n]: ', 0

;; VBE Variables
width: dw 0
height: dw 0
bpp: db 0
offset: dw 0
t_segment: dw 0	; "segment" is keyword in fasm
mode: dw 0

vbe_info_block:
	.vbe_signature: db 'VBE2'
	.vbe_version: dw 0          ; Should be 0300h? BCD value
	.oem_string_pointer: dd 0 
	.capabilities: dd 0
	.video_mode_pointer: dd 0
	.total_memory: dw 0
	.oem_software_rev: dw 0
	.oem_vendor_name_pointer: dd 0
	.oem_product_name_pointer: dd 0
	.oem_product_revision_pointer: dd 0
	.reserved: times 222 db 0
	.oem_data: times 256 db 0

mode_info_block:
    ;; Mandatory info for all VBE revisions
	.mode_attributes: dw 0
	.window_a_attributes: db 0
	.window_b_attributes: db 0
	.window_granularity: dw 0
	.window_size: dw 0
	.window_a_segment: dw 0
	.window_b_segment: dw 0
	.window_function_pointer: dd 0
	.bytes_per_scanline: dw 0

    ;; Mandatory info for VBE 1.2 and above
	.x_resolution: dw 0
	.y_resolution: dw 0
	.x_charsize: db 0
	.y_charsize: db 0
	.number_of_planes: db 0
	.bits_per_pixel: db 0
	.number_of_banks: db 0
	.memory_model: db 0
	.bank_size: db 0
	.number_of_image_pages: db 0
	.reserved1: db 1

    ;; Direct color fields (required for direct/6 and YUV/7 memory models)
	.red_mask_size: db 0
	.red_field_position: db 0
	.green_mask_size: db 0
	.green_field_position: db 0
	.blue_mask_size: db 0
	.blue_field_position: db 0
	.reserved_mask_size: db 0
	.reserved_field_position: db 0
	.direct_color_mode_info: db 0

    ;; Mandatory info for VBE 2.0 and above
	.physical_base_pointer: dd 0     ; Physical address for flat memory frame buffer
	.reserved2: dd 0
	.reserved3: dw 0

    ;; Mandatory info for VBE 3.0 and above
	.linear_bytes_per_scan_line: dw 0
    .bank_number_of_image_pages: db 0
    .linear_number_of_image_pages: db 0
    .linear_red_mask_size: db 0
    .linear_red_field_position: db 0
    .linear_green_mask_size: db 0
    .linear_green_field_position: db 0
    .linear_blue_mask_size: db 0
    .linear_blue_field_position: db 0
    .linear_reserved_mask_size: db 0
    .linear_reserved_field_position: db 0
    .max_pixel_clock: dd 0

    .reserved4: times 190 db 0      ; Remainder of mode info block