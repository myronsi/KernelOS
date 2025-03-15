#include <kernel/types.h>
#include <kernel/vga.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t *vga_buffer = (uint16_t*)0xB8000;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint8_t color = 0x0F;  // Белый на чёрном

// Инициализация
void vga_init() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = (color << 8) | ' ';
        }
    }
}

// Вывод символа
void vga_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (color << 8) | c;
        cursor_x++;
    }

    // Прокрутка экрана
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= VGA_HEIGHT) {
        for (int y = 0; y < VGA_HEIGHT - 1; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
            }
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}