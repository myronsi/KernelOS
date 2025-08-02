bits    32
section .bss
    align 4
    stack_bottom:
        resb 32768
    stack_top:

section .text
    align 4
    dd 0x1BADB002
    dd 0x00
    dd -(0x1BADB002+0x00)

global start
extern kmain
start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, stack_top
    cli
    call kmain
    hlt