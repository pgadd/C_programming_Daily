//Blocking the CPU with while(!(SR & EOC)) is unacceptable in production RTOS environments. We need a state machine driven purely by the ADC's Interrupt Service Routine (ISR).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    ADC_STATE_IDLE,
    ADC_STATE_READING_CH1,
    ADC_STATE_READING_CH5,
    ADC_STATE_COMPLETE
} ADC_State_t;

typedef struct {
    ADC_State_t state;
    uint16_t *storage_ptr; // Double-pointer style tracking
} ADC_Context_t;

typedef struct {
    uint32_t volatile SR;    // Status Reg (Bit 1 = EOC: End of conversion)
    uint32_t volatile CR1;
    uint32_t volatile CR2;   // Control Reg 2 (Bit 0 = ADON: ADC ON, Bit 30 = SWSTART: Start)
    uint32_t volatile SQR1;  // Sequence Reg 1 (Bits 23:20 = L (Length of sequence minus 1))
    uint32_t volatile SQR3;  // Sequence Reg 3 (Bits 4:0 = 1st conversion, Bits 9:5 = 2nd, Bits 14:10 = 3rd)
    uint32_t volatile DR;    // Data Reg (Lower 16 bits contain the ADC result)
} ADC_TypeDef;

// Global context for the ISR
static ADC_Context_t adc_ctx;

void start_async_adc(ADC_TypeDef *adc, uint16_t **data_pool_head);
void start_async_adc(ADC_TypeDef *adc, uint16_t **data_pool_head){
    *adc_ctx.storage_ptr = **data_pool_head;

    adc_ctx.state = ADC_STATE_READING_CH1;

    adc -> SQR3 |= (0x15);

    adc->CR1 |= (1 << 5);

    adc -> CR2 |= 1;
    adc -> CR2 |= (1 << 30);
}

void ADC_IRQHandler(ADC_TypeDef *adc);
void ADC_IRQHandler(ADC_TypeDef *adc) {
    if(adc->SR & (1 << 1)){
        printf("EOC is set in SR");

        switch(adc_ctx.state){
            case (ADC_STATE_READING_CH1):
                *(adc_ctx.storage_ptr) = adc->DR;
                adc_ctx.storage_ptr++;
                adc->SQR3 = (0x15 << 4);
                adc->CR2 |= (1 << 30);
                adc_ctx.state = ADC_STATE_READING_CH5;
            case(ADC_STATE_READING_CH5):
                *(adc_ctx.storage_ptr) = adc->DR;
                adc_ctx.storage_ptr++;
                adc_ctx.state = ADC_STATE_COMPLETE;
            default:
                return;

        }
    }
}

int main(void){
    uint16_t adc_ram_pool[10];
    uint16_t *pool_ptr = adc_ram_pool;

    ADC_TypeDef sim_adc;
    start_async_adc(&sim_adc, &pool_ptr);

    sim_adc.SR |= (1 << 1); 
    sim_adc.DR = 1045;
    ADC_IRQHandler(&sim_adc);

    sim_adc.SR |= (1 << 1); 
    sim_adc.DR = 3099;
    ADC_IRQHandler(&sim_adc);

    printf("index 0: %d, index 1: %d", adc_ram_pool[0], adc_ram_pool[1]);

    return 0;
}