#include <kernel/idt.h>

// Определения для записи IDT
idt_entry_t idt[256];
idt_ptr_t idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_load(void *idt_ptr) {
    // Загрузка IDT через инструкцию lidt.
    __asm__ __volatile__("lidt (%0)" : : "r"(idt_ptr));
}

void idt_init(void) {
    // Set up idtp with the size and base of idt
    idtp.limit = sizeof(idt_entry_t) * 256 - 1;
    idtp.base = (uint32_t)&idt;

    // Initialize IDT entries here if needed
    // Load IDT with lidt (you'll need assembly for this, e.g., via an external function)
}