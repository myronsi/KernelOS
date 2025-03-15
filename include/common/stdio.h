#pragma once
#include "stdarg.h"   // Подключаем наш минимальный stdarg.h

// Прототип функции, которая использует va_list
void vprintf(const char* format, va_list args);

// Прототип функции printf, если он нужен
int printf(const char* format, ...);
