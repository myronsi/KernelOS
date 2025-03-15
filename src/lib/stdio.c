#include <kernel/types.h>
#include <kernel/vga.h>
#include <common/stdio.h>
#include <common/stdarg.h>

// Базовая реализация printf
void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

// Форматированный вывод с поддержкой %s, %d, %x
void vprintf(const char *format, va_list args) {
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 's': {
                    char *s = va_arg(args, char*);
                    while (*s) vga_putchar(*s++);
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    if (num < 0) {
                        vga_putchar('-');
                        num = -num;
                    }
                    print_num(num, 10);
                    break;
                }
                case 'x': {
                    uint32_t num = va_arg(args, uint32_t);
                    print_num(num, 16);
                    break;
                }
                case 'c': {
                    char c = va_arg(args, int);
                    vga_putchar(c);
                    break;
                }
            }
        } else {
            vga_putchar(*format);
        }
        format++;
    }
}

// Вспомогательная функция для вывода чисел
static void print_num(uint32_t num, int base) {
    char buffer[32];
    char *ptr = buffer;

    if (num == 0) {
        vga_putchar('0');
        return;
    }

    while (num > 0) {
        int rem = num % base;
        *ptr++ = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
        num /= base;
    }

    while (ptr > buffer) {
        vga_putchar(*--ptr);
    }
}