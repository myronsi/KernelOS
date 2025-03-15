#pragma once
#include <kernel/types.h>

// Флаги страниц
#define VMM_PRESENT   0x01
#define VMM_WRITABLE  0x02
#define VMM_USER      0x04

void vmm_init();
void vmm_map_page(void* virt, void* phys, uint32_t flags);