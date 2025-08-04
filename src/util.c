#include "../include/util.h"
#include "../include/screen.h"

#define HEAP_SIZE 524288
static char heap[HEAP_SIZE];
static uint32 heap_pos = 0;

typedef struct {
    void* ptr;
    uint32 size;
} heap_block_t;

static heap_block_t blocks[1024];
static uint32 block_count = 0;

void* malloc(uint32 nbytes)
{
    if (nbytes == 0) nbytes = 1;
    if (heap_pos + nbytes > HEAP_SIZE) {
        return NULL;
    }
    void* ptr = &heap[heap_pos];
    heap_pos += nbytes;
    if (block_count < 1024) {
        blocks[block_count].ptr = ptr;
        blocks[block_count].size = nbytes;
        block_count++;
    } else {
    }
    return ptr;
}

void free(void* ptr)
{
    if (ptr == NULL) return;
    for (uint32 i = 0; i < block_count; i++) {
        if (blocks[i].ptr == ptr) {
            for (uint32 j = i; j < block_count - 1; j++) {
                blocks[j] = blocks[j + 1];
            }
            block_count--;
            return;
        }
    }
}

void* realloc(void* ptr, uint32 size)
{
    if (ptr == NULL) {
        return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return malloc(1); // Минимальный размер
    }
    uint32 old_size = 0;
    for (uint32 i = 0; i < block_count; i++) {
        if (blocks[i].ptr == ptr) {
            old_size = blocks[i].size;
            break;
        }
    }
    if (old_size == 0) {
        return NULL;
    }
    void* new_ptr = malloc(size);
    if (!new_ptr) {
        return NULL;
    }
    uint32 copy_size = old_size < size ? old_size : size;
    memory_copy((char*)ptr, (char*)new_ptr, copy_size);
    free(ptr);
    return new_ptr;
}

uint32 strlen(const char* str)
{
    uint32 len = 0;
    while (str[len]) len++;
    return len;
}

void __stack_chk_fail_local(void)
{
    while (1);
}

void memory_copy(char *source, char *dest, int nbytes)
{
    for (int i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8 *dest, uint8 val, uint32 len)
{
    uint8 *temp = (uint8 *)dest;
    for (; len != 0; len--) *temp++ = val;
}

char* int_to_ascii(int n, char str[])
{
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0) str[i++] = '-';
    str[i] = '\0';
    return str;
}

int str_to_int(string ch)
{
    int n = 0;
    int p = 1;
    int len = strlen(ch);
    for (int i = len - 1; i >= 0; i--)
    {
        n += ((int)(ch[i] - '0')) * p;
        p *= 10;
    }
    return n;
}

string int_to_string(int n)
{
    string ch = malloc(50);
    if (!ch) {
        return "";
    }
    int_to_ascii(n, ch);
    int len = strlen(ch);
    int i = 0;
    int j = len - 1;
    while (i < (len / 2 + len % 2))
    {
        char tmp = ch[i];
        ch[i] = ch[j];
        ch[j] = tmp;
        i++;
        j--;
    }
    return ch;
}

uint8 inb(uint16 port)
{
    uint8 result;
    __asm__ volatile("inb %1, %0" : "=a" (result) : "dN" (port));
    return result;
}

void outb(uint16 port, uint8 value)
{
    __asm__ volatile("outb %0, %1" : : "a" (value), "dN" (port));
}

uint16 inw(uint16 port)
{
    uint16 result;
    __asm__ volatile("inw %1, %0" : "=a" (result) : "dN" (port));
    return result;
}

void outw(uint16 port, uint16 value)
{
    __asm__ volatile("outw %0, %1" : : "a" (value), "dN" (port));
}

uint32 get_heap_pos(void)
{
    return heap_pos;
}