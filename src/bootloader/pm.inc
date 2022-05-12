%include 'src/bootloader/gdt.inc'
[ bits 16]
switch_to_pm:
    cli ; We must switch of interrupts until we have
        ; set -up the protected mode interrupt vector
        ; otherwise interrupts will run riot.
    lgdt [gdt_descriptor] ; Load our global descriptor table , which defines
    ; the protected mode segments ( e.g. for code and data )
    mov eax , cr0 ; To make the switch to protected mode , we set
    or eax , 0x1 ; the first bit of CR0 , a control register
    mov cr0 , eax
    jmp CODE_SEG:init_pm ; Make a far jump ( i.e. to a new segment ) to our 32 - bit code
    
[bits 32]
init_pm:
    mov ax, DATA_SEG ; Now in PM , our old segments are meaningless ,
    mov ds, ax ; so we point our segment registers to the
    mov ss, ax ; data selector we defined in our GDT
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000 ; Update our stack position so it is right
    mov esp, ebp ; at the top of the free space.

    ;; Set up VBE mode info block in memory to be easier to work with
    ; mov esi, mode_info_block
    ; mov edi, 0x9000
    ; mov ecx, 64                 ; Mode info block is 256 bytes / 4 = # of dbl words
    ; rep movsd

    ; Set cursor values for printing
    xor eax, eax                ; Clear EAX for the instructions below
    mov al, [0x450]             ; Byte at address 0x450 = last BIOS column position
    mov [cur_col], eax          ; Copy to current column
    mov al, [0x451]             ; Byte at address 0x451 = last BIOS row position
    mov [cur_row], eax          ; Copy to current row

    mov ax, [0x44a]             ; Word at address 0x44a = # of columns (screen width)
    mov [screen_width], eax     ; Copy to screen width

    call BEGIN_PM ; Finally , call some well - known label