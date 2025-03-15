#include <kernel/types.h>
#include <kernel/pmm.h>
#include <common/string.h>
#include "common/stddef.h"

#define PMM_BLOCK_SIZE 4096     // Размер блока памяти (4 KiB)
#define PMM_BLOCKS_PER_BYTE 8   // Блоков на байт в bitmap

static uint32_t *pmm_bitmap;
static uint32_t pmm_max_blocks;
static uint32_t pmm_used_blocks;

// Инициализация аллокатора
void pmm_init() {
    // Размер доступной памяти (предположим, что GRUB передал его)
    uint32_t mem_size = 1024 * 1024 * 128;  // 128 MiB для примера
    pmm_max_blocks = mem_size / PMM_BLOCK_SIZE;
    pmm_bitmap = (uint32_t*)0x100000;        // Bitmap начинается по адресу 1 MiB

    // Вся память занята по умолчанию
    memset(pmm_bitmap, 0xFF, pmm_max_blocks / PMM_BLOCKS_PER_BYTE);
    pmm_used_blocks = pmm_max_blocks;
}

// Выделение блока памяти
void* pmm_alloc() {
    for (uint32_t i = 0; i < pmm_max_blocks; i++) {
        if (!(pmm_bitmap[i / 32] & (1 << (i % 32)))) {
            pmm_bitmap[i / 32] |= (1 << (i % 32));
            pmm_used_blocks++;
            return (void*)(i * PMM_BLOCK_SIZE);
        }
    }
    return NULL;  // Нет свободных блоков
}

// Освобождение блока
void pmm_free(void *addr) {
    uint32_t block = (uint32_t)addr / PMM_BLOCK_SIZE;
    pmm_bitmap[block / 32] &= ~(1 << (block % 32));
    pmm_used_blocks--;
}

// Получение свободной памяти
uint32_t pmm_get_free_memory() {
    return (pmm_max_blocks - pmm_used_blocks) * PMM_BLOCK_SIZE;
}