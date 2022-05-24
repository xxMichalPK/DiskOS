#pragma once
#include <interrupts/idt.h>

// Divide by 0 handler
extern void div_by_0_handler(int_frame_t* int_frame);
extern void reserved_int_1_handler(int_frame_t* int_frame);
extern void breakpoint_handler(int_frame_t* int_frame);
extern void overflow_handler(int_frame_t* int_frame);
extern void bound_range_handler(int_frame_t* int_frame);
extern void invalid_opcode_handler(int_frame_t* int_frame);
extern void device_not_available_handler(int_frame_t* int_frame);
extern void double_fault_handler(int_frame_t* int_frame, uint32_t errorCode);
extern void invalid_tss_handler(int_frame_t* int_frame, uint32_t errorCode);
extern void segment_not_present_handler(int_frame_t* int_frame, uint32_t errorCode);
extern void stack_segment_fault_handler(int_frame_t* int_frame, uint32_t errorCode);
extern void general_protection_fault_handler(int_frame_t* int_frame, uint32_t errorCode);
extern void page_fault_handler(int_frame_t* int_frame, uint32_t errorCode);