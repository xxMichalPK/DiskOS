#include <kernelUtils.h>
#include <libc/stdio.h>
#include <interrupts/exceptions.h>

// Divide by 0 handler
__attribute__ ((interrupt)) void div_by_0_handler(int_frame_t* int_frame) {
    fgColor = 0xFFFF0000;
    puts("\n\rDIVIDE BY 0 ERROR\n\r");
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void reserved_int_1_handler(int_frame_t* int_frame) {
    fgColor = 0xFFFF0000;
    puts("\n\rRESERVED ERROR\n\r");
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void breakpoint_handler(int_frame_t* int_frame) {
    fgColor = 0xFFFF0000;
    puts("\n\rBREAKPOINT ERROR\n\r");
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void overflow_handler(int_frame_t* int_frame) {
    fgColor = 0xFFFF0000;
    puts("\n\rOVERFLOW ERROR\n\r");
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void bound_range_handler(int_frame_t* int_frame) {
    fgColor = 0xFFFF0000;
    puts("\n\rBOUND RANGE EXCEEDED ERROR\n\r");
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void invalid_opcode_handler(int_frame_t* int_frame) {
    Cursor = { 0, 0 };
    fgColor = 0xFFFF0000;
    puts("INVALID OPCODE\n\r");
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void device_not_available_handler(int_frame_t* int_frame) {
    fgColor = 0xFFFF0000;
    puts("\n\rDEVICE NOT AVAILABLE\n\r");
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void double_fault_handler(int_frame_t* int_frame, uint32_t errorCode) {
    vbe->ClearScreen(0xFFFF1111);
    Cursor = {0,0};
    fgColor = 0xFF000000;
    puts("DOUBLE FAULT DETECTED!\n\r");
    printf("ERROR CODE: 0x%x", errorCode);
    while (1) { __asm__ volatile ("cli; hlt"); }
}

__attribute__ ((interrupt)) void invalid_tss_handler(int_frame_t* int_frame, uint32_t errorCode) {
    fgColor = 0xFFFF0000;
    printf("\n\rINVALID TSS - CODE: 0x%x", errorCode);
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void segment_not_present_handler(int_frame_t* int_frame, uint32_t errorCode) {
    fgColor = 0xFFFF0000;
    printf("\n\rSEGMENT NOT PRESENT - CODE: 0x%x", errorCode);
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void stack_segment_fault_handler(int_frame_t* int_frame, uint32_t errorCode) {
    fgColor = 0xFFFF0000;
    printf("\n\rSTACK SEGMENT FAULT - CODE: 0x%x", errorCode);
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void general_protection_fault_handler(int_frame_t* int_frame, uint32_t errorCode) {
    Cursor = { 0, 0 };
    fgColor = 0xFFFF0000;
    printf("GENERAL PROTECTION FAULT - CODE: 0x%x", errorCode);
    fgColor = 0xFFFFFFFF;
    int_frame->eip += 4;
}

__attribute__ ((interrupt)) void page_fault_handler(int_frame_t* int_frame, uint32_t errorCode) {
    Cursor = { 0, 0 };
    fgColor = 0xFFFF0000;
    uint32_t faultingAddr = 0;
    __asm__ __volatile__ ("movl %%cr2, %%eax; movl %%eax, %0;" : "=r"(faultingAddr));
    printf("PAGE FAULT AT 0x%x - CODE: 0x%x", faultingAddr, errorCode);
    puts("\n\r( ");
    if ((1 & (errorCode >> 0))) puts("Page present - "); else puts("Page not present - ");
    if ((1 & (errorCode >> 1))) puts("Write error - "); else puts("Read error - ");
    if ((1 & (errorCode >> 2))) puts("Caused by user - "); else puts("Caused by supervisor process - ");
    
    if ((1 & (errorCode >> 3))) puts("Reserved write - ");
    if ((1 & (errorCode >> 4))) puts("Instruction Fetch - ");
    if ((1 & (errorCode >> 5))) puts("Protection key - ");
    if ((1 & (errorCode >> 6))) puts("Shadow stack - ");
    if ((1 & (errorCode >> 15))) puts("Software Guard Extensions - ");
    puts(" )");
    while (1) { __asm__ volatile ("cli; hlt"); }
}