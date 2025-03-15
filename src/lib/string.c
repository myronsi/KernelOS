#include <kernel/types.h>

void *memset(void *ptr, int value, size_t size) {
    uint8_t *p = (uint8_t*)ptr;
    while (size--) *p++ = value;
    return ptr;
}

void *memcpy(void *dest, const void *src, size_t size) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    while (size--) *d++ = *s++;
    return dest;
}

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}