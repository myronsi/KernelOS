#pragma once
#include "stdarg.h"   // Используем наш локальный stdarg.h

// Функция vprintf теперь возвращает int
int vprintf(const char* format, va_list args);
int printf(const char* format, ...);
