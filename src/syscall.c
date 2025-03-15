#include <kernel/types.h>
#include <kernel/syscall.h>

// Таблица системных вызовов
static syscall_handler_t syscall_table[256];

// Регистрация системного вызова
void syscall_register(uint8_t num, syscall_handler_t handler) {
    syscall_table[num] = handler;
}

// Обработчик прерывания системного вызова
void syscall_handler(isr_registers_t regs) {
    uint8_t num = regs.eax;
    if (syscall_table[num]) {
        regs.eax = syscall_table[num](regs.ebx, regs.ecx, regs.edx);
    } else {
        regs.eax = -1;  // Неподдерживаемый вызов
    }
}