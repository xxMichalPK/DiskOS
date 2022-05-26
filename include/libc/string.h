#pragma once
#include <stdint.h>
#include <stddef.h>

int strlen(const char *s);
char* strcpy(char* destination, const char* source);
char* strcat(char* destination, const char* source);
uint8_t strcmp(char *src1, char *src2, uint32_t bytes);
int isspace(char c);
void itoa(char *buf, unsigned long int n, int base);
int atoi(char * string);
void sprintf(char * buf, const char * fmt, ...);