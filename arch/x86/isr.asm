; Обработчики прерываний и исключений
section .text
bits 32

; Макрос для обработчиков без кода ошибки
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0                ; Фиктивный код ошибки
    push byte %1               ; Номер прерывания
    jmp isr_common
%endmacro

; Макрос для обработчиков с кодом ошибки
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1               ; Номер прерывания
    jmp isr_common
%endmacro

; Создание обработчиков для 0-31 исключений CPU
; Обработчики без кода ошибки
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8  ; Исключение с кодом ошибки
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE 30
ISR_NOERRCODE 31

global isr_stub_table
isr_stub_table:
    dd isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
    dd isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
    dd isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
    dd isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31

; Общая часть для всех обработчиков
extern isr_handler
isr_common:
    pusha                      ; Сохраняем регистры
    mov ax, ds
    push eax

    mov ax, 0x10               ; Загрузка сегмента данных ядра
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler           ; Вызов C-обработчика

    pop eax                    ; Восстанавливаем сегменты
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8                 ; Очистка кода ошибки и номера
    iret