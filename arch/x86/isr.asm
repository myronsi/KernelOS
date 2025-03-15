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
ISR_NOERRCODE 0
ISR_NOERRCODE 1
; ... (аналогично для других исключений)
ISR_NOERRCODE 31

; Общая часть для всех обработчиков
extern isr_handler             ; Функция на C
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