#pragma once

#include <kernel/types.h>  // Для uint8_t, uint16_t и т.д.

// Чтение байта из порта
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// Запись байта в порт
static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Чтение слова (16 бит) из порта
static inline uint16_t inw(uint16_t port) {
    uint16_t data;
    asm volatile("inw %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// Запись слова (16 бит) в порт
static inline void outw(uint16_t port, uint16_t data) {
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}

// Управление прерываниями
static inline void cli() {
    asm volatile("cli");
}

static inline void sti() {
    asm volatile("sti");
}