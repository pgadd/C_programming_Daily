#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct { uint32_t volatile BSRR; } GPIO_TypeDef;

// Global Function Pointer for the Timer Callback
static void (*timer_callback)(void) = NULL;

void register_timer_isr(void (*callback_func)(void)) {
    timer_callback = callback_func;
}

typedef struct {
    uint32_t volatile CR1;   // Control Register 1 (Bit 0 = CEN: Counter Enable)
    uint32_t volatile SR;    // Status Register (Bit 0 = UIF: Update Interrupt Flag)
    uint32_t volatile PSC;   // Prescaler
    uint32_t volatile ARR;   // Auto-Reload Register
    uint32_t volatile CNT;   // Counter
} TIM_TypeDef;

void my_led_toggle_callback(void);
void my_led_toggle_callback(void){
    GPIO_TypeDef portA;

    portA.BSRR = 5;
    printf("output: %X", portA.BSRR);
    portA.BSRR = (5 << 16);
    printf("output: %X", portA.BSRR);
    
}

void TIM2_IRQHandler(TIM_TypeDef *tim);
void TIM2_IRQHandler(TIM_TypeDef *tim){
    if (tim->SR & 1) {
        tim->SR &= ~1;
    }
    if (timer_callback != NULL){
        timer_callback();
    }
}

int main(void) {
    TIM_TypeDef sim_tim2;
    sim_tim2.SR |= 1;

    TIM2_IRQHandler(&sim_tim2);

    return 0;
}