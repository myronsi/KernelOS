#pragma once
#include <kernel/types.h>

// Инициализация физического менеджера памяти
void pmm_init();

// Выделение/освобождение страницы
void* pmm_alloc();
void pmm_free(void* addr);

// Получение информации о памяти
uint32_t pmm_get_free_memory();