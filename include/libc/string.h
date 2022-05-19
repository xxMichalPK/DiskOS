#pragma once
#include <stdint.h>
#include <stddef.h>

static unsigned int strlen(const char *s)
{
    unsigned int count = 0;
    while(*s!='\0')
    {
        count++;
        s++;
    }
    return count;
}

static char* strcpy(char* destination, const char* source)
{
    // return if no memory is allocated to the destination
    if (destination == NULL) {
        return NULL;
    }
    // take a pointer pointing to the beginning of the destination string
    char *ptr = destination;
 
    // copy the C-string pointed by source into the array
    // pointed by destination
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
 
    // include the terminating null character
    *destination = '\0';
 
    // the destination is returned by standard `strcpy()`
    return ptr;
}

static char* strcat(char* destination, const char* source)
{
    // make `ptr` point to the end of the destination string
    char* ptr = destination + strlen(destination);
 
    // appends characters of the source to the destination string
    while (*source != '\0') {
        *ptr++ = *source++;
    }
 
    // null terminate destination string
    *ptr = '\0';
 
    // the destination is returned by standard `strcat()`
    return destination;
}

static uint8_t strcmp(char *src1, char *src2, uint32_t bytes) {
  // Iterate through each byte
  for (uint32_t i = 0; i < bytes; i++) {
    // Compare the byte, if not equal, return
    if (src1[i] != src2[i]) {
      return 0;
    }
  }
  // If we exited, that means the strings are equal, bail with True
  return 1;
}