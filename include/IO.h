#pragma once
#include <stdint.h>

static inline uint8_t inportb(uint16_t port) {
    uint8_t r;
    asm volatile("inb %1, %0" : "=a" (r) : "Nd" (port));
    return r;
}

static inline void outportb(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

static inline uint16_t inports(uint16_t port) {
    uint16_t r;
    asm("inw %1, %0" : "=a" (r) : "dN" (port));
    return r;
}

static inline void outports(uint16_t port, uint16_t data) {
    asm("outw %1, %0" : : "dN" (port), "a" (data));
}

static inline uint32_t inportl(uint32_t port) {
    uint32_t r;
    asm("inl %1, %0" : "=a" (r) : "dN" (port));
    return r;
}

static inline void outportl(uint32_t port, uint32_t data) {
    asm("outl %1, %0" : : "dN" (port), "a" (data));
}

inline void io_wait() {
    asm volatile("outb %%al, $0x80" : : "a"(0));
}