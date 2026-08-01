//In advanced audio or motor control, we need to change the PWM duty cycle extremely fast (e.g., thousands of times per second). Instead of the CPU calculating it, we pre-calculate an array of Duty Cycles in RAM, and a "DMA" (Direct Memory Access) hardware block feeds them into the CCR register automatically. Let's simulate a DMA pointer engine.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint16_t *buffer_start; // Pointer to the start of the duty cycle array
    uint16_t *current_ptr;  // Pointer tracking the current duty cycle to load
    uint16_t buffer_length; // Number of elements in the array
} DMA_Sim_t;

typedef struct { 
    uint32_t volatile ARR; 
    uint32_t volatile CCR1; 
} TIM_TypeDef;

void process_dma_transfer(DMA_Sim_t *dma, TIM_TypeDef *tim);
void process_dma_transfer(DMA_Sim_t *dma, TIM_TypeDef *tim) {
    tim->CCR1 = (uint32_t)*(dma->current_ptr);

    dma->current_ptr++;

    uint16_t end_addy = *(dma->buffer_start) + dma->buffer_length;

    if (*(dma->current_ptr) == end_addy) {
        dma->current_ptr = dma->buffer_start;
    }

}


int main(void){
    uint16_t sine_wave[4] = {0, 500, 1000, 500};

    DMA_Sim_t my_dma = { .buffer_start = sine_wave, .current_ptr = sine_wave, .buffer_length = 4 };

    TIM_TypeDef sim_tim = { .ARR = 1000 };

    for(int x = 0; x < 10; x++){
        process_dma_transfer(&my_dma, &sim_tim);
        printf("CCR1: %d\n", sim_tim.CCR1);
    }

    return 0;
}