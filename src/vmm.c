#include <kernel/types.h>
#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <common/string.h>

// Таблица страниц (предполагаем 32-битную систему)
uint32_t *page_directory = 0;

// Инициализация виртуальной памяти
void vmm_init() {
    page_directory = (uint32_t*)pmm_alloc();  // Выделяем физическую память под PD
    memset(page_directory, 0, 4096);

    // Настраиваем первую запись PD (тождественное отображение 0-4 MiB)
    page_directory[0] = (uint32_t)pmm_alloc() | 0x03;  // Present + R/W

    // Включаем страничную адресацию
    asm volatile(
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0" 
        :: "r"(page_directory)
    );
}

// Отображение виртуальной страницы на физическую
void vmm_map_page(void *virt, void *phys, uint32_t flags) {
    uint32_t pd_index = (uint32_t)virt >> 22;
    uint32_t pt_index = (uint32_t)virt >> 12 & 0x03FF;

    // Создаём таблицу страниц, если её нет
    if (!(page_directory[pd_index] & 0x1)) {
        uint32_t *pt = (uint32_t*)pmm_alloc();
        page_directory[pd_index] = (uint32_t)pt | flags;
        memset(pt, 0, 4096);
    }

    // Заполняем запись в таблице страниц
    uint32_t *pt = (uint32_t*)(page_directory[pd_index] & 0xFFFFF000);
    pt[pt_index] = (uint32_t)phys | flags;
}