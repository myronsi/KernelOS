#include <kernel/keyboard.h>
#include <kernel/irq.h>
#include <common/string.h>
#include <common/io.h>  // Для inb

static char scancode_table[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

void keyboard_handler() {
    unsigned char scancode = inb(0x60);  // Читаем сканкод с порта 0x60
    if (scancode < sizeof(scancode_table)) {
        char c = scancode_table[scancode];
        if (c != 0) {
            vga_putchar(c);  // Выводим символ на экран
        }
    }
    irq_send_eoi(1);  // Отправляем сигнал завершения для IRQ1
}

void keyboard_init() {
    register_irq_handler(1, keyboard_handler);  // Регистрируем обработчик для IRQ1
}