#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t volatile CR1;   // Control Register 1 (Bit 0 = CEN: Counter Enable)
    uint32_t volatile SR;    // Status Register (Bit 0 = UIF: Update Interrupt Flag)
    uint32_t volatile PSC;   // Prescaler
    uint32_t volatile ARR;   // Auto-Reload Register
    uint32_t volatile CNT;   // Counter
} TIM_TypeDef;

void delay_ms(TIM_TypeDef *tim, uint32_t ms);
void delay_ms(TIM_TypeDef *tim, uint32_t ms){
    tim -> PSC = 15999;
    tim -> ARR = (ms - 1);
    tim -> SR &= ~(1);
    tim -> CR1 |= 1;

    while (!(tim -> SR & 1)) {

    }

    tim -> CR1 &= ~1;
    tim -> SR &= ~(1);

}
