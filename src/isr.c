#include <kernel/types.h>
#include <kernel/isr.h>
#include <common/stdio.h>

// Таблица обработчиков прерываний
static isr_handler_t isr_handlers[256];

// Регистрация обработчика
void register_interrupt_handler(uint8_t num, isr_handler_t handler) {
    isr_handlers[num] = handler;
}

// Общий обработчик (вызывается из ассемблерного кода)
void isr_handler(isr_registers_t regs) {
    if (isr_handlers[regs.int_no]) {
        isr_handlers[regs.int_no](regs);
    } else {
        printf("Unhandled interrupt: %d\n", regs.int_no);
    }
}