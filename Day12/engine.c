//A "breathing" LED smoothly fades from 0% to 100% and back down to 0%. Using delay_ms() to do this freezes the CPU. We need a state-machine function that gets called repeatedly by a main loop, updating the brightness by a step_size without ever blocking.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    FADE_UP,
    FADE_DOWN
} FadeState_t;

typedef struct {
    uint16_t current_pwm; // Current CCR value
    FadeState_t state;    // Direction of the fade
    uint16_t step_size;   // How much to increment/decrement per call
} BreathingCtx_t;

typedef struct { uint32_t volatile ARR; uint32_t volatile CCR1; } TIM_TypeDef;

void update_breathing_led(TIM_TypeDef *tim, BreathingCtx_t *ctx);
void update_breathing_led(TIM_TypeDef *tim, BreathingCtx_t *ctx) {
    switch (ctx->state){
        case (FADE_UP):
            ctx->current_pwm += ctx->step_size;
            if (ctx->current_pwm > tim->ARR) {
                ctx->current_pwm = tim->ARR;
                ctx->state = FADE_DOWN;
            }
            break;
        case (FADE_DOWN):
            if (ctx->current_pwm < ctx->step_size){
                ctx->current_pwm = 0;
                ctx->state = FADE_UP;
            } else{
                ctx->current_pwm -= ctx->step_size;
            }
            break;

    }

    tim->CCR1 = ctx->current_pwm;
}

int main(void) {
    TIM_TypeDef sim_tim = {0}; 
    sim_tim.ARR = 100;

    BreathingCtx_t led_ctx = { .current_pwm = 0, .state = FADE_UP, .step_size = 30 };

    for (int x = 0; x < 8; x++){
        update_breathing_led(&sim_tim, &led_ctx);\
        printf("CCR1: %d, state: %d\n", sim_tim.CCR1, led_ctx.state);
    }

    return 0;
}