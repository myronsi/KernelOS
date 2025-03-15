#pragma once
#include <kernel/types.h>

// Структура регистров при прерывании
typedef struct {
    uint32_t ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} isr_registers_t;

// Тип обработчика прерываний
typedef void (*isr_handler_t)(isr_registers_t*);

// Регистрация обработчика
void register_interrupt_handler(uint8_t num, isr_handler_t handler);

// Инициализация обработчиков прерываний
void isr_init(void);