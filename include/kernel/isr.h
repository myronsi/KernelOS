#ifndef ISR_H
#define ISR_H

#include <kernel/types.h>

typedef struct {
    uint32_t ds;                  
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; 
    uint32_t int_no, err_code;     
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*isr_t)(registers_t*);

void register_interrupt_handler(uint8_t n, isr_t handler);

void isr_init();

#endif