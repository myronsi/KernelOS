#pragma once
#include <kernel/types.h>

// Инициализация контроллера прерываний
void irq_init();

// Отправка сигнала End Of Interrupt (EOI) для указанного IRQ.
void irq_send_eoi(uint8_t irq);

// Регистрация обработчика для указанного IRQ.
// Обработчик определяется как функция без аргументов (при необходимости, можно изменить сигнатуру).
void register_irq_handler(uint8_t irq, void (*handler)(void));