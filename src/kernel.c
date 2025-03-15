#include <kernel/types.h>
#include <kernel/vga.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/keyboard.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>
#include <common/stdio.h>   // Подключаем ваш заголовок с объявлением printf

// Главная функция ядра
void kernel_main() {
    vga_init();         // Инициализация видеовывода
    pmm_init();         // Инициализация физического аллокатора
    vmm_init();         // Настройка виртуальной памяти
    isr_init();         // Установка обработчиков прерываний
    irq_init();         // Настройка контроллера прерываний
    keyboard_init();    // Включение клавиатуры

    printf("Kernel Booted!\n");
    printf("Free Memory: %d KB\n", pmm_get_free_memory() / 1024);

    // Вечный цикл ядра
    while (1) {
        asm("hlt");
    }
}
