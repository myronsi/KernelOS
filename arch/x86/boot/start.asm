section .text
bits 32

extern kernel_main

global kernel_entry   ; <- Метка экспортирована!

kernel_entry:
    ; Настройка сегментов
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Настройка стека
    mov esp, stack_top

    ; Вызов kernel_main
    call kernel_main

    ; Бесконечный цикл
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384   ; 16 KiB стек
stack_top: