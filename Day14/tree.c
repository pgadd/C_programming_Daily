//Your bootloader just handed control to main(). The system is running at a sluggish 16 MHz. You must write a driver to configure the Reset and Clock Control (RCC) to use the PLL, multiply the clock to 64 MHz, and set the AHB prescaler.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t volatile CR;       // Control Reg. Bit 24: PLLON. Bit 25: PLLRDY.
    uint32_t volatile CFGR;     // Config Reg. Bits [7:4]: HPRE (AHB Prescaler). 
                                // Bits [1:0]: SW (System Clock Switch).
    uint32_t volatile PLLCFGR;  // PLL Config Reg. 
                                // Bit 1: PLLSRC (0 = HSI, 1 = HSE)
                                // Bits [14:8]: PLLN (Multiplier)
} RCC_TypeDef;


void system_clock_config(RCC_TypeDef *rcc);
void system_clock_config(RCC_TypeDef *rcc){
    rcc->PLLCFGR &= ~(1 << 1);
    rcc -> PLLCFGR &= ~(0x127 << 8);

    rcc -> PLLCFGR |= (4 << 14);

    rcc->CR |= (1 << 24);

    while (!(rcc->CR & (1 << 25))){

    }

    rcc -> CFGR &= ~(0x15 << 4);

    rcc -> CFGR |= (0b11);
}

int main(void) {
    RCC_TypeDef sim_rcc = {0};
    sim_rcc.CR |= (1 << 25);

    system_clock_config(&sim_rcc);

    printf("PLLCFGR: %X, CFGR: %X", sim_rcc.PLLCFGR, sim_rcc.CFGR);

    return 0;
}