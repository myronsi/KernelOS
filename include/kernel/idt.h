#ifndef IDT_H
#define IDT_H

#pragma once
#include <kernel/types.h>  // Assuming types like uint8_t, uint32_t are defined here

// Define the structure for an IDT entry
typedef struct {
    uint16_t base_low;  // Lower 16 bits of the handler address
    uint16_t sel;       // Segment selector
    uint8_t always0;    // Must be 0
    uint8_t flags;      // Gate type and attributes
    uint16_t base_high; // Upper 16 bits of the handler address
} __attribute__((packed)) idt_entry_t;

// Define the structure for the IDT pointer
typedef struct {
    uint16_t limit;     // Size of the IDT - 1
    uint32_t base;      // Base address of the IDT
} __attribute__((packed)) idt_ptr_t;

// Declare the global IDT array and pointer as extern
extern idt_entry_t idt[256];  // Array of 256 IDT entries
extern idt_ptr_t idtp;        // IDT pointer for lidt instruction

// Function declarations
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_load(void *idt_ptr);
void idt_init(void);

#endif