#include <interrupts/idt.h>
#include <libc/stdio.h>

// Defualt exception handler (no error code)
__attribute__ ((interrupt)) void default_excp_handler(int_frame_t* int_frame) {
    puts("DEFAULT EXCEPTION HANDLER - NO ERROR CODE");
    int_frame->eip += 4; // Continue next code
}

// Defualt exception handler (includes error code)
__attribute__ ((interrupt)) void default_excp_handler_err_code(int_frame_t* int_frame, uint32_t errorCode) {
    puts("DEFAULT EXCEPTION HANDLER - ERROR CODE: ");
    printf("%x", errorCode);
    __asm__ volatile("cli; hlt");
}

// Defualt interrupt handler
__attribute__ ((interrupt)) void default_int_handler(int_frame_t* int_frame) {
    puts("DEFAULT INTERRUPT HANDLER");
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_desc_t* descriptor = &idt[vector];
 
    descriptor->base_low       = (uint32_t)isr & 0xFFFF;
    descriptor->cs             = 0x08;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->base_high      = ((uint32_t)isr >> 16) & 0xFFFF;
}
 
void idt_init(void);
void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * 256 - 1;
 
    // Set up the default interrupts (0-32)
    for (uint8_t vector = 0; vector < 32; vector++) {
        if (vector == 8 || vector == 10 || vector == 11 || vector == 12 || vector == 13 || vector == 14 || vector == 17 || vector == 21) {
            // Error code present
            idt_set_descriptor(vector, (void*)default_excp_handler_err_code, TRAP_GATE_FLAGS);
        } else {
            // No error code
            idt_set_descriptor(vector, (void*)default_excp_handler, TRAP_GATE_FLAGS);
        }
    }

    // Set up the rest of interrupts (32-255)
    for (uint16_t vector = 32; vector < 256; vector++) {
        idt_set_descriptor(vector, (void*)default_int_handler, INT_GATE_FLAGS);
    }
 
    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
}