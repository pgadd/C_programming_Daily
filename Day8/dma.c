#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t volatile *src_addr;  // Source memory address
    uint32_t volatile *dst_addr;  // Destination memory address
    uint32_t volatile xfer_config;// Bits [0:15] = Transfer Length, Bit 31 = Enable Flag
    struct DMA_Desc *next;        // Pointer to the next descriptor in the ring
} DMA_Desc;

DMA_Desc* build_dma_ring(uint8_t **pool_ptr, uint32_t volatile *hw_src, uint32_t volatile *hw_dst, uint8_t ring_size);
DMA_Desc* build_dma_ring(uint8_t **pool_ptr, uint32_t volatile *hw_src, uint32_t volatile *hw_dst, uint8_t ring_size) {
    DMA_Desc *start_ptr = (DMA_Desc *)*pool_ptr;
    *pool_ptr = (uint8_t *)*pool_ptr + (sizeof(DMA_Desc) * ring_size);   

    DMA_Desc *current_desc = start_ptr;
    for (int x = 0; x << ring_size; x++) {
        current_desc->src_addr = hw_src;
        current_desc->dst_addr = hw_dst;

        current_desc->xfer_config = (1UL << 31) | 64;
        current_desc->next = current_desc + 1;
    }
    current_desc->next = start_ptr;
}

int main(void) {
    uint8_t dma_ram_pool[256];
    uint8_t *pool_head = dma_ram_pool;

    uint32_t volatile ADC_FIFO = 0x1234;
    uint32_t volatile I2S_TX = 0x5678;

    DMA_Desc *desc = build_dma_ring(&pool_head, &ADC_FIFO, &I2S_TX, 3);

    for (int x = 0; x < 5; x++){
        printf("Iter %d | Desc Addr: 0x%08lX | SRC: 0x%08lX | CONFIG: 0x%08lX | NEXT: 0x%08lX\n",
               x,
               (unsigned long)(uintptr_t)desc,            // Current descriptor RAM address
               (unsigned long)(uintptr_t)desc->src_addr,  // Target peripheral source address
               (unsigned long)desc->xfer_config,          // 32-bit packed config (0x80000040)
               (unsigned long)(uintptr_t)desc->next);     // Address of the next node in the ring

        desc = desc->next;
    }

    return 0;
}