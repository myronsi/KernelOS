#include <kernel/types.h>
#include <kernel/keyboard.h>
#include <common/io.h>

#define KEYBOARD_PORT 0x60

static char key_buffer[256];
static uint8_t buffer_pos = 0;

// Таблица скан-кодов (ASCII для клавиш без Shift)
static const char scancode_table[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

// Обработчик прерывания клавиатуры
void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_PORT);
    
    // Обработка только нажатий (старший бит = 0)
    if (!(scancode & 0x80)) {
        char c = scancode_table[scancode];
        if (buffer_pos < sizeof(key_buffer) - 1) {
            key_buffer[buffer_pos++] = c;
        }
    }

    // Отправка EOI
    irq_send_eoi(1);
}

// Инициализация клавиатуры
void keyboard_init() {
    // Регистрация обработчика для IRQ1
    register_irq_handler(1, keyboard_handler);
}