#include <kernel/idt.h>

// Define the global IDT array and pointer
idt_entry_t idt[256];
idt_ptr_t idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;        // Lower 16 bits
    idt[num].base_high = (base >> 16) & 0xFFFF; // Upper 16 bits
    idt[num].sel = sel;                        // Segment selector
    idt[num].always0 = 0;                      // Always 0
    idt[num].flags = flags;                    // Flags
}

void idt_init(void) {
    // Set up idtp with the size and base of idt
    idtp.limit = sizeof(idt_entry_t) * 256 - 1;
    idtp.base = (uint32_t)&idt;

    // Initialize IDT entries here if needed
    // Load IDT with lidt (you'll need assembly for this, e.g., via an external function)
}