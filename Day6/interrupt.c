#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

volatile uint8_t adc_status_reg = 0x00;

void ADC_IRQHandler(void);
void ADC_IRQHandler(void) {
    adc_status_reg |= (1 << 2); //turns bit 2 into 1;
}

uint8_t wait_for_adc_conversion(void);
uint8_t wait_for_adc_conversion(void) {
    while(!(adc_status_reg & (1 << 2 ))) { //checks if bit 2 is 0
        int cycles = 0;
        while(cycles <= 5){
            if(cycles == 5){
                ADC_IRQHandler();
            }
            cycles++;
        }
    }
    adc_status_reg &= ~(1 << 2); //Turns bit 2 into 0;
    return 1;
}

int main(void){
    wait_for_adc_conversion();
    printf("Success, final val: %X", adc_status_reg);

    return 0;
}