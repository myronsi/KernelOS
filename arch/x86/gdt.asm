; Глобальная таблица дескрипторов (GDT)
section .data
align 8

global gdt_start
gdt_start:
    dq 0x0                     ; Нулевой дескриптор

; Дескриптор кода: базовый адрес 0, лимит 4GB
gdt_code:
    dw 0xFFFF                  ; Лимит (биты 0-15)
    dw 0x0000                  ; База (биты 0-15)
    db 0x00                    ; База (биты 16-23)
    db 0x9A                    ; Флаги: присутствует, ring0, код/исполняемый
    db 0xCF                    ; Флаги + лимит (биты 16-19)
    db 0x00                    ; База (биты 24-31)

; Дескриптор данных
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92                    ; Флаги: данные, запись
    db 0xCF
    db 0x00

gdt_end:

; Описатель GDT для LGDT
global gdt_ptr
gdt_ptr:
    dw gdt_end - gdt_start - 1  ; Размер GDT
    dd gdt_start                ; Адрес GDT

; Загрузка GDT и обновление сегментов
global load_gdt
load_gdt:
    lgdt [gdt_ptr]
    jmp 0x08:.reload_segments   ; Перезагрузка CS
.reload_segments:
    mov ax, 0x10                ; Селектор данных
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret