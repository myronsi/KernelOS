#include "../include/util.h"

static char heap[16384];
static uint32 heap_pos = 0;

void* malloc(uint32 nbytes)
{
    if (heap_pos + nbytes > sizeof(heap)) {
        return 0;
    }
    void* ptr = &heap[heap_pos];
    heap_pos += nbytes;
    return ptr;
}

void free(void* ptr)
{

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

void* realloc(void* ptr, uint32 new_size)
{
    if (!ptr) {
        return malloc(new_size);
    }
    void* new_ptr = malloc(new_size);
    if (!new_ptr) {
        return 0;
    }
    memory_copy(ptr, new_ptr, new_size);
    return new_ptr;
}
