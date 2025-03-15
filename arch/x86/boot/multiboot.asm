; Multiboot-заголовок для совместимости с GRUB
section .multiboot
align 4

multiboot_header:
    dd 0x1BADB002              ; Магическое число
    dd 0x00000003              ; Флаги: выравнивание + запросить информацию памяти
    dd -(0x1BADB002 + 0x03)    ; Контрольная сумма

; Точка входа (GRUB передаёт управление сюда)
global _start
extern kernel_entry
_start:
    jmp kernel_entry           ; Переход к основному коду загрузки