#include <common/stdio.h>
#include <common/string.h>
#include <kernel/types.h>
#include <common/stdarg.h>
#include <kernel/vga.h>  // Для vga_putchar

static void print_num(uint32_t num, int base, int *printed) {
    char buffer[32];
    int i = 0;
    if (num == 0) {
        vga_putchar('0');
        (*printed)++;
        return;
    }
    while (num && i < (int)(sizeof(buffer) - 1)) {
        int rem = num % base;
        buffer[i++] = (rem < 10) ? '0' + rem : 'a' + rem - 10;
        num /= base;
    }
    for (int j = i - 1; j >= 0; j--) {
        vga_putchar(buffer[j]);
        (*printed)++;
    }
}

int vprintf(const char *format, va_list args) {
    int printed = 0;
    for (int i = 0; format[i] != '\0'; i++) {
        char c = format[i];
        if (c == '%') {
            i++;
            if (format[i] == 'd') {
                int num = va_arg(args, int);
                print_num((uint32_t)num, 10, &printed);
            }
        } else {
            vga_putchar(c);
            printed++;
        }
    }
    return printed;
}

int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}