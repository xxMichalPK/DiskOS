#pragma once
#include <stdint.h>

#define TRAP_GATE_FLAGS     0x8F // P = 1, DPL = 00, S = 0, Type = 1111 (32bit trap gate)
#define INT_GATE_FLAGS      0x8E // P = 1, DPL = 00, S = 0, Type = 1110 (32bit interrupt gate)
#define INT_GATE_USER_FLAGS 0xEE // P = 1, DPL = 11, S = 0, Type = 1110 (32bit interrupt gate, called from PL 3)

typedef struct {
	uint16_t    base_low;      // The lower 16 bits of the ISR's address
	uint16_t    cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t	    ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    base_high;     // The higher 32 bits of the ISR's address
} __attribute__((packed)) idt_desc_t;

typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;

__attribute__((aligned(0x10))) 
static idt_desc_t idt[256]; // Create an array of IDT entries; aligned for performance
static idtr_t idtr;

typedef struct {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t sp;
    uint32_t ss;
} __attribute__ ((packed)) int_frame_t;

extern void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
extern void idt_init();