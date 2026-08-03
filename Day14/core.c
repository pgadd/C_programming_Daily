//When you want to enable an interrupt (like UART RX), you don't just configure the UART. You must tell the ARM Core's NVIC (Nested Vectored Interrupt Controller) to allow that specific interrupt line to wake the CPU. The ISER (Interrupt Set-Enable Register) is an array of 32-bit registers.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    // Each bit in this array controls one of 256 possible interrupts.
    // ISER[0] controls Interrupts 0-31. ISER[1] controls 32-63, etc.
    uint32_t volatile ISER[8]; 
} NVIC_TypeDef;

void enable_nvic_irq(NVIC_TypeDef *nvic, uint8_t irq_number);
void enable_nvic_irq(NVIC_TypeDef *nvic, uint8_t irq_number) {
    uint32_t index = irq_number / 32;
    uint32_t position = irq_number % 32;

    // Perfect pointer arithmetic!
    uint32_t *target_register = nvic->ISER + index;

    // FIX: Shift the 1 by the calculated position
    *target_register |= (1UL << position);
}

int main(void){
    NVIC_TypeDef sim_nvic = {0};

    enable_nvic_irq(&sim_nvic, 37);

    uint32_t *ptr = sim_nvic.ISER;

    for (int x = 0; x < 8; x++){
        printf("ISER: %X\n", *ptr);
        ptr++;
    }

    return 0;
}