#pragma once
#include <kernel/types.h>

// Тип обработчика системного вызова
typedef int (*syscall_handler_t)(uint32_t, uint32_t, uint32_t);

// Регистрация системного вызова
void syscall_register(uint8_t num, syscall_handler_t handler);