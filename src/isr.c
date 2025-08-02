#include "../include/isr.h"
#include "../include/idt.h"
#include "../include/screen.h"
#include "../include/util.h"
#include "../include/system.h"
#include "../include/kb.h"

string exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr0() { 
    print("ISR0: "); print(exception_messages[0]); print("\n");
    uint32 eip;
    asm volatile("mov 4(%%ebp), %0" : "=r"(eip));
    print("EIP: "); print(int_to_string(eip)); print("\n");
    uint32 cs;
    asm volatile("mov 8(%%ebp), %0" : "=r"(cs));
    print("CS: "); print(int_to_string(cs)); print("\n");
    uint32 esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    print("ESP: "); print(int_to_string(esp)); print("\n");
    asm("hlt"); 
}
void isr1() { 
    print("ISR1: "); print(exception_messages[1]); print("\n");
    uint32 eip;
    asm volatile("mov 4(%%ebp), %0" : "=r"(eip));
    print("EIP: "); print(int_to_string(eip)); print("\n");
    uint32 cs;
    asm volatile("mov 8(%%ebp), %0" : "=r"(cs));
    print("CS: "); print(int_to_string(cs)); print("\n");
    uint32 esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    print("ESP: "); print(int_to_string(esp)); print("\n");
    asm("hlt"); 
}
void isr2() { print("ISR2: "); print(exception_messages[2]); print("\n"); asm("hlt"); }
void isr3() { print("ISR3: "); print(exception_messages[3]); print("\n"); asm("hlt"); }
void isr4() { print("ISR4: "); print(exception_messages[4]); print("\n"); asm("hlt"); }
void isr5() { print("ISR5: "); print(exception_messages[5]); print("\n"); asm("hlt"); }
void isr6() { 
    print("ISR6: "); print(exception_messages[6]); print("\n"); 
    uint32 eip;
    asm volatile("mov 4(%%ebp), %0" : "=r"(eip));
    print("EIP: "); print(int_to_string(eip)); print("\n");
    asm("hlt"); 
}
void isr7() { print("ISR7: "); print(exception_messages[7]); print("\n"); asm("hlt"); }
void isr8() { 
    print("ISR8: "); print(exception_messages[8]); print("\n");
    uint32 error_code;
    asm volatile("pop %0" : "=r"(error_code));
    print("Error code: "); print(int_to_string(error_code)); print("\n");
    uint32 eip;
    asm volatile("mov 4(%%ebp), %0" : "=r"(eip));
    print("EIP: "); print(int_to_string(eip)); print("\n");
    uint32 cs;
    asm volatile("mov 8(%%ebp), %0" : "=r"(cs));
    print("CS: "); print(int_to_string(cs)); print("\n");
    uint32 esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    print("ESP: "); print(int_to_string(esp)); print("\n");
    asm("hlt");
}
void isr9() { print("ISR9: "); print(exception_messages[9]); print("\n"); asm("hlt"); }
void isr10() { print("ISR10: "); print(exception_messages[10]); print("\n"); asm("hlt"); }
void isr11() { print("ISR11: "); print(exception_messages[11]); print("\n"); asm("hlt"); }
void isr12() { print("ISR12: "); print(exception_messages[12]); print("\n"); asm("hlt"); }
void isr13() { 
    print("ISR13: "); print(exception_messages[13]); print("\n");
    uint32 error_code;
    asm volatile("pop %0" : "=r"(error_code));
    print("Error code: "); print(int_to_string(error_code)); print("\n");
    asm("hlt"); 
}
void isr14() { 
    print("ISR14: "); print(exception_messages[14]); print("\n");
    uint32 error_code;
    asm volatile("pop %0" : "=r"(error_code));
    print("Error code: "); print(int_to_string(error_code)); print("\n");
    asm("hlt"); 
}
void isr15() { print("ISR15: "); print(exception_messages[15]); print("\n"); asm("hlt"); }
void isr16() { print("ISR16: "); print(exception_messages[16]); print("\n"); asm("hlt"); }
void isr17() { print("ISR17: "); print(exception_messages[17]); print("\n"); asm("hlt"); }
void isr18() { print("ISR18: "); print(exception_messages[18]); print("\n"); asm("hlt"); }
void isr19() { print("ISR19: "); print(exception_messages[19]); print("\n"); asm("hlt"); }
void isr20() { print("ISR20: "); print(exception_messages[20]); print("\n"); asm("hlt"); }
void isr21() { print("ISR21: "); print(exception_messages[21]); print("\n"); asm("hlt"); }
void isr22() { print("ISR22: "); print(exception_messages[22]); print("\n"); asm("hlt"); }
void isr23() { print("ISR23: "); print(exception_messages[23]); print("\n"); asm("hlt"); }
void isr24() { print("ISR24: "); print(exception_messages[24]); print("\n"); asm("hlt"); }
void isr25() { print("ISR25: "); print(exception_messages[25]); print("\n"); asm("hlt"); }
void isr26() { print("ISR26: "); print(exception_messages[26]); print("\n"); asm("hlt"); }
void isr27() { print("ISR27: "); print(exception_messages[27]); print("\n"); asm("hlt"); }
void isr28() { print("ISR28: "); print(exception_messages[28]); print("\n"); asm("hlt"); }
void isr29() { print("ISR29: "); print(exception_messages[29]); print("\n"); asm("hlt"); }
void isr30() { print("ISR30: "); print(exception_messages[30]); print("\n"); asm("hlt"); }
void isr31() { print("ISR31: "); print(exception_messages[31]); print("\n"); asm("hlt"); }

void irq0() {
    outportb(0x20, 0x20);
}

void irq1() {
    uint8 scancode = inportb(0x60);
    if (scancode < 0x80) {
        kb_handler(scancode);
    }
    outportb(0x20, 0x20);
}

void isr_install() {
    print("Starting ISR installation\n");
    print("Enabling PS/2 port\n");
    outportb(0x64, 0xAE);
    for (volatile int i = 0; i < 10000; i++);
    print("Clearing keyboard buffer\n");
    for (int i = 0; i < 5; i++) {
        while (inportb(0x64) & 0x01) {
            inportb(0x60);
        }
        for (volatile int j = 0; j < 10000; j++);
    }
    print("Setting IDT gates\n");
    set_idt_gate(0, (uint32)isr0);
    set_idt_gate(1, (uint32)isr1);
    set_idt_gate(2, (uint32)isr2);
    set_idt_gate(3, (uint32)isr3);
    set_idt_gate(4, (uint32)isr4);
    set_idt_gate(5, (uint32)isr5);
    set_idt_gate(6, (uint32)isr6);
    set_idt_gate(7, (uint32)isr7);
    set_idt_gate(8, (uint32)isr8);
    set_idt_gate(9, (uint32)isr9);
    set_idt_gate(10, (uint32)isr10);
    set_idt_gate(11, (uint32)isr11);
    set_idt_gate(12, (uint32)isr12);
    set_idt_gate(13, (uint32)isr13);
    set_idt_gate(14, (uint32)isr14);
    set_idt_gate(15, (uint32)isr15);
    set_idt_gate(16, (uint32)isr16);
    set_idt_gate(17, (uint32)isr17);
    set_idt_gate(18, (uint32)isr18);
    set_idt_gate(19, (uint32)isr19);
    set_idt_gate(20, (uint32)isr20);
    set_idt_gate(21, (uint32)isr21);
    set_idt_gate(22, (uint32)isr22);
    set_idt_gate(23, (uint32)isr23);
    set_idt_gate(24, (uint32)isr24);
    set_idt_gate(25, (uint32)isr25);
    set_idt_gate(26, (uint32)isr26);
    set_idt_gate(27, (uint32)isr27);
    set_idt_gate(28, (uint32)isr28);
    set_idt_gate(29, (uint32)isr29);
    set_idt_gate(30, (uint32)isr30);
    set_idt_gate(31, (uint32)isr31);
    set_idt_gate(32, (uint32)irq0);
    set_idt_gate(33, (uint32)irq1);
    print("Remapping PIC\n");
    outportb(0x20, 0x11);
    for (volatile int i = 0; i < 10000; i++);
    outportb(0xA0, 0x11);
    for (volatile int i = 0; i < 10000; i++);
    outportb(0x21, 0x20);
    for (volatile int i = 0; i < 10000; i++);
    outportb(0xA1, 0x28);
    for (volatile int i = 0; i < 10000; i++);
    outportb(0x21, 0x04);
    for (volatile int i = 0; i < 10000; i++);
    outportb(0xA1, 0x02);
    for (volatile int i = 0; i < 10000; i++);
    outportb(0x21, 0x01);
    for (volatile int i = 0; i < 10000; i++);
    outportb(0xA1, 0x01);
    for (volatile int i = 0; i < 10000; i++);
    outportb(0x21, 0xFE);
    outportb(0xA1, 0xFF);
    print("Calling set_idt\n");
    set_idt();
    print("Before enabling interrupts\n");
    asm volatile("sti");
    print("ISR installation complete\n");
}