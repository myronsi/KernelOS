#ifndef UTIL_H
#define UTIL_H

#include "types.h"

void memory_copy(char *source, char *dest, int nbytes);
void memory_set(uint8 *dest, uint8 val, uint32 len);
char* int_to_ascii(int n, char str[]);  
int str_to_int(string ch);
string int_to_string(int n);
void* malloc(uint32 nbytes);
void free(void* ptr);
uint32 strlen(const char* str);
void __stack_chk_fail_local(void);

#endif