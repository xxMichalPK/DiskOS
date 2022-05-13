#pragma once
#include <stdint.h>
#define UNUSED(x) (void)(x)

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

/*static inline void *memcpy(void *to, void* from, uint32_t n) {
    __asm__ __volatile__(
    "movl (%1), %0 \n\t"
    "movl %0, (%2) \n\t"
    : "=r" (n) 
    : "r"(from),"r"(to)
    : "memory"
    );
    return to;
}*/

static inline void *memcpy(void *dst, void const *src, uint64_t len) {
    unsigned char *pcDst = (unsigned char *)dst;
    unsigned char const *pcSrc = (unsigned char const *)src;

    if (len >= sizeof(long) * 2
    &&  ((uintptr_t)src & (sizeof(long) - 1)) == ((uintptr_t)dst & (sizeof(long) - 1))) {
        while (((uintptr_t)pcSrc & (sizeof(long) - 1)) != 0) {
            *pcDst++ = *pcSrc++;
            len--;
        }
        long *plDst = (long *)pcDst;
        long const *plSrc = (long const *)pcSrc;
        /* manually unroll the loop */
        while (len >= sizeof(long) * 4) {
            plDst[0] = plSrc[0];
            plDst[1] = plSrc[1];
            plDst[2] = plSrc[2];
            plDst[3] = plSrc[3];
            plSrc += 4;
            plDst += 4;
            len -= sizeof(long) * 4;
        }
        while (len >= sizeof(long)) {
            *plDst++ = *plSrc++;
            len -= sizeof(long);
        }
        pcDst = (unsigned char *)plDst;
        pcSrc = (unsigned char const *)plSrc;
    }
    while (len--) {
         *pcDst++ = *pcSrc++;
    }
    return dst;
}