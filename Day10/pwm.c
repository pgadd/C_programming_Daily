#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t volatile CR1;
    uint32_t volatile CCMR1; // Capture/Compare Mode Register 1
    uint32_t volatile CCER;  // Capture/Compare Enable Register
    uint32_t volatile CNT;
    uint32_t volatile PSC;
    uint32_t volatile ARR;
    uint32_t volatile CCR1;  // Channel 1 Duty Cycle Register
} TIM_PWM_TypeDef;

void init_pwm_channel1(TIM_PWM_TypeDef *tim, uint32_t pwm_freq_hz, uint8_t duty_percent);
void init_pwm_channel1(TIM_PWM_TypeDef *tim, uint32_t pwm_freq_hz, uint8_t duty_percent) {
    tim -> PSC = 0;
    tim -> ARR = ((uint32_t)(16000000/pwm_freq_hz) - 1);
    tim->CCR1 = (tim->ARR * duty_percent) / 100;

    tim -> CCMR1 |= (0b110 << 26);

    tim -> CCER |= 1;

    tim-> CR1 |= 1;

}


int main(void){
    TIM_PWM_TypeDef sample;

    init_pwm_channel1(&sample, 10000, 25);

    printf("ARR: %d, CCR1: %d", sample.ARR, sample.CCR1);

}