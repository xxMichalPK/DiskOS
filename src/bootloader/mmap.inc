memmap_entries equ 0x3000       ; Store number of memory map entries here
get_memory_map:
    pusha
    xor di, di
    mov es, di
    mov di, 0x3004              ; Memory map entries start here
    xor ebx, ebx                ; EBX = 0 to start, will contain continuation values
    xor bp, bp                  ; BP will store the number of entries
    mov edx, 'PAMS'             ; EDX = 'SMAP' but little endian
    mov eax, 0E820h             ; bios interrupt function number
    mov [ES:DI+20], dword 1     ; Force a valid ACPI 3.x entry
    mov ecx, 24                 ; Each entry can be up to 24 bytes
    int 15h                     ; Call the interrupt
    jc .error                   ; If carry is set, function not supported or errored

    cmp eax, 'PAMS'             ; EAX should equal 'SMAP' on successful call
    jne .error
    test ebx, ebx               ; Does EBX = 0? if so only 1 entry or no entries :(
    jz .error
    jmp .start                  ; EBX != 0, have a valid entry

.next_entry:
    mov edx, 'PAMS'             ; Some BIOS may clobber edx, reset it here
    mov ecx, 24                 ; Reset ECX
    mov eax, 0E820h             ; Reset EAX
    int 15h

.start:
    jcxz .skip_entry            ; Memory map entry is 0 bytes in length, skip
    mov ecx, [ES:DI+8]          ; Low 32 bits of length
    or ecx, [ES:DI+12]          ; Or with high 32 bits of length, or will also set the ZF
    jz .skip_entry              ; Length of memory region returned = 0, skip
    
.good_entry:
    inc bp                      ; Increment number of entries
    add di, 24

.skip_entry:
    test ebx, ebx               ; If EBX != 0, still have entries to read 
    jz .done
    jmp .next_entry

.error:
    stc
    popa
    jmp prepareBOOT
.done:
    mov [memmap_entries], bp
    clc
    popa
    jmp prepareBOOT