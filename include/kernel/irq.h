#pragma once
#include <kernel/types.h>

// Инициализация контроллера прерываний
void irq_init();

// Отправка EOI (End Of Interrupt)
void irq_send_eoi(uint8_t irq);