#pragma once
#include <kernel/types.h>
#include "stddef.h"

// Работа с памятью и строками
void *memset(void *dest, int c, size_t count);
void* memcpy(void* dest, const void* src, size_t size);
size_t strlen(const char* str);