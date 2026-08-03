//In professional firmware, we don't hardcode 0x40020000 for GPIOA and 0x40020400 for GPIOB. We use the Memory Map Offsets. You will write a pointer-math function that dynamically calculates a peripheral's memory address at runtime, casts it to a hardware struct, and modifies it.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// ARM Cortex-M Memory Map Defines
#define PERIPH_BASE       0x40000000UL
#define AHB2PERIPH_OFFSET 0x08000000UL
#define GPIOA_OFFSET      0x00000000UL
#define GPIOB_OFFSET      0x00000400UL

typedef struct {
    uint32_t volatile MODER;
    uint32_t volatile ODR;
    uint32_t volatile BSRR;
} GPIO_TypeDef;

GPIO_TypeDef* resolve_gpio_port(uint32_t base, uint32_t bus_offset, uint32_t port_offset);
GPIO_TypeDef* resolve_gpio_port(uint32_t base, uint32_t bus_offset, uint32_t port_offset) {
    uint32_t mem = base + bus_offset + port_offset;
    return (GPIO_TypeDef*)mem;
}

int main(void){
    uint8_t fake_ram_block[2048] = {0};

    // FIX: Properly declare the variable holding the address of your fake RAM
    uint32_t simulated_base = (uint32_t)(uintptr_t)fake_ram_block;

    // FIX: Pass the simulated base address!
    GPIO_TypeDef *mem = resolve_gpio_port(simulated_base, 0, 1024);

    mem->MODER = 0xFF;
    mem->BSRR |= (1 << 5);

    // Verify the math: the pointer difference should be exactly 1024 bytes
    printf("Base Address: %p\n", (void*)fake_ram_block);
    printf("Resolved Address: %p\n", (void*)mem);
    
    return 0;
}