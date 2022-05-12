#pragma once
#include <stdint.h>

static inline void *memset(void *s, char c, unsigned int count)
{
   int d0, d1;
   __asm__ __volatile__(
   "rep\n\t"
   "stosb"
   : "=&c" (d0), "=&D" (d1)
   :"a" (c),"1" (s),"0" (count)
   :"memory");
   return s;
}

static inline void *memcpy(void *to, void* from, uint32_t n) {
    __asm__ __volatile__(
    "movl (%1), %0 \n\t"
    "movl %0, (%2) \n\t"
    : "=r" (n) 
    : "r"(from),"r"(to)
    : "memory"
    );
    return to;
}