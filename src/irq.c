#include <kernel/irq.h>
#include <kernel/types.h>
#include <common/io.h>

#define IRQ_COUNT 16

// Массив указателей на обработчики IRQ (первоначально все нулевые)
void (*irq_handlers[IRQ_COUNT])(void) = {0};

void register_irq_handler(uint8_t irq, void (*handler)(void)) {
    if (irq < IRQ_COUNT) {
        irq_handlers[irq] = handler;
    }
}

void irq_init() {
    // Инициализация контроллеров PIC
    // Отправляем команды и данные для перенастройки PIC
    outb(0x20, 0x11);   // Начало инициализации для master PIC
    outb(0xA0, 0x11);   // Начало инициализации для slave PIC

    outb(0x21, 0x20);   // Смещение для master PIC: IRQ 0-7 -> 0x20-0x27
    outb(0xA1, 0x28);   // Смещение для slave PIC: IRQ 8-15 -> 0x28-0x2F

    outb(0x21, 0x04);   // Сообщаем master PIC, что есть slave на IRQ2
    outb(0xA1, 0x02);   // Сообщаем slave PIC, что его кассирует master на IRQ2

    outb(0x21, 0x01);   // Режим 8086 для master PIC
    outb(0xA1, 0x01);   // Режим 8086 для slave PIC

    // Размаскировать все прерывания
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void irq_send_eoi(uint8_t irq) {
    // Если IRQ сработал на slave PIC, сначала отправляем EOI на slave
    if (irq >= 8)
        outb(0xA0, 0x20);
    // Затем отправляем EOI на master PIC
    outb(0x20, 0x20);
}
