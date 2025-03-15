#include <kernel/types.h>
#include <kernel/irq.h>
#include <common/io.h>

// Порты контроллеров PIC
#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

// Команды инициализации
#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define PIC_EOI   0x20  // End Of Interrupt

// Реинициализация PIC с перенастройкой IRQ
void irq_init() {
    // Начало инициализации
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

    // Смещение для IRQ0-IRQ7 (0x20 для первого PIC)
    outb(PIC1_DATA, 0x20);
    // Смещение для IRQ8-IRQ15 (0x28 для второго PIC)
    outb(PIC2_DATA, 0x28);

    // Соединение PIC (ведущий-ведомый)
    outb(PIC1_DATA, 0x04);  // Ведущий PIC имеет ведомый на линии IRQ2
    outb(PIC2_DATA, 0x02);

    // Режим 8086
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Размаскировать все прерывания (0x00 - разрешить)
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

// Отправка EOI для конкретного IRQ
void irq_send_eoi(uint8_t irq) {
    if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}