#include <kernel/isr.h>
#include <kernel/irq.h>
#include <common/io.h>
#include <common/stdio.h>
#include <kernel/idt.h>

#define IDT_ENTRIES 256

extern void idt_load(void *idt_ptr);

extern void *isr_stub_table[];  // Объявляем таблицу обработчиков, которая есть в ASM

extern idt_ptr_t idtp;  // idtp должен быть объявлен в IDT

isr_t interrupt_handlers[IDT_ENTRIES];

void isr_handler(isr_registers_t *regs) {
    if (interrupt_handlers[regs->int_no]) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    } else {
        printf("Unhandled interrupt: %d\n", regs->int_no);
    }
}

void irq_handler(isr_registers_t *regs) {
    if (regs->int_no >= 32 && regs->int_no < 48) {
        irq_send_eoi(regs->int_no - 32);
    }
    if (interrupt_handlers[regs->int_no]) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void isr_init() {
    // Устанавливаем обработчики для исключений процессора (0-31)
    for (int i = 0; i < 32; i++) {
        idt_set_gate(i, (uint32_t)isr_stub_table[i], 0x08, 0x8E);
    }

    // Загружаем новую таблицу IDT
    idt_load(&idtp);
}