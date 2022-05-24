#pragma once
#include <stdint.h>
#include <IO.h>

#define PIC_1_CMD 0x20
#define PIC_1_DATA 0x21

#define PIC_2_CMD 0xA0
#define PIC_2_DATA 0xA1

#define NEW_IRQ_0 0x20
#define NEW_IRQ_8 0x28

#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#define IRQ_0 0x20
#define IRQ_1 0x21
#define IRQ_2 0x22
#define IRQ_3 0x23
#define IRQ_4 0x24
#define IRQ_5 0x25
#define IRQ_6 0x26
#define IRQ_7 0x27
#define IRQ_8 0x28
#define IRQ_9 0x29
#define IRQ_10 0x30
#define IRQ_11 0x31
#define IRQ_12 0x32
#define IRQ_13 0x33
#define IRQ_14 0x34
#define IRQ_15 0x35

static void send_pic_eoi(uint8_t irq) {
    if (irq >= 8) outportb(PIC_2_CMD, PIC_EOI);
    outportb(PIC_1_CMD, PIC_EOI);
}

static void disablePIC(void) {
    outportb(PIC_2_DATA, 0xFF);
    outportb(PIC_1_DATA, 0xFF);
}

static void set_irq_mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) port = PIC_1_DATA;
    else {
        irq -= 8;
        port = PIC_2_DATA;
    }

    value = inportb(port) | (1 << irq);
    outportb(port, value);
}

static void clear_irq_mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) port = PIC_1_DATA;
    else {
        irq -=8;
        port = PIC_2_DATA;
    }

    value = inportb(port) & ~(1 << irq);
    outportb(port, value);
}

static void remapPIC(void) {
    uint8_t pic_1_mask, pic_2_mask;

    pic_1_mask = inportb(PIC_1_DATA);
    io_wait();
    pic_2_mask = inportb(PIC_2_DATA);
    io_wait();

    outportb(PIC_1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outportb(PIC_2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outportb(PIC_1_DATA, NEW_IRQ_0);
    io_wait();
    outportb(PIC_2_DATA, NEW_IRQ_8);
    io_wait();

    outportb(PIC_1_DATA, 0x4);
    io_wait();
    outportb(PIC_2_DATA, 0x2);
    io_wait();

    outportb(PIC_1_DATA, ICW4_8086);
    io_wait();
    outportb(PIC_2_DATA, ICW4_8086);
    io_wait();

    outportb(PIC_1_DATA, pic_1_mask);
    io_wait();
    outportb(PIC_2_DATA, pic_2_mask);
}
