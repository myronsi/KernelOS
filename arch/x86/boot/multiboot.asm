; Multiboot-заголовок для совместимости с GRUB
section .multiboot
align 4

multiboot_header:
    dd 0x1BADB002              ; Магическое число
    dd 0x00010003              ; Флаги: требование памяти и видеорежима
    dd -(0x1BADB002 + 0x00010003)  ; Контрольная сумма

; Точка входа (GRUB передаёт управление сюда)
global _start
extern kernel_entry

section .text
align 4
_start:
    cli                         ; Отключаем прерывания на время настройки
    mov esp, stack_top          ; Устанавливаем стек
    call kernel_entry           ; Переход к основному коду загрузки

    ; Если kernel_entry вернёт управление, останавливаем процессор
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384   ; 16 KiB стек
stack_top:
