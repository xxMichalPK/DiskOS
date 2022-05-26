#pragma once
#include <libc/string.h>        //strcpy, strcat
#include <memory/memory.h>      //memcpy, memset
#include <libc/ctype.h>         //isdigit
#include <stddef.h>
#include <stdbool.h> 
#include <stdint.h>
#include <stdarg.h>
#include <graphics/vbe.h>

static inline void putc(char ch);
void puts(const char* str);
int is_format_letter(char c);
void vsprintf(char * str, void (*putchar)(char), const char * format, va_list arg);
void vsprintf_helper(char * str, void (*putchar)(char), const char * format, uint32_t * pos, va_list arg);
void printf(const char * s, ...);