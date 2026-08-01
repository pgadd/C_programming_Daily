//You have an RGB LED connected to Timer 3. Red is on Channel 1, Green is on Channel 2, and Blue is on Channel 3. In UI/UX, colors are often passed as 32-bit hex codes (e.g., 0xFF5733 where Red is FF, Green is 57, Blue is 33). You must extract these 8-bit values and scale them to a hardware timer's ARR resolution.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t volatile CR1;
    uint32_t volatile ARR;   // Period
    uint32_t volatile CCR1;  // Red Channel
    uint32_t volatile CCR2;  // Green Channel
    uint32_t volatile CCR3;  // Blue Channel
} TIM_RGB_TypeDef;

void set_rgb_color(TIM_RGB_TypeDef *tim, uint32_t hex_color);
void set_rgb_color(TIM_RGB_TypeDef *tim, uint32_t hex_color) {
    uint32_t green = ((hex_color >> 8) & 0xFF);
    uint32_t blue = (hex_color & 0xFF);
    uint32_t red = ((hex_color >> 16) & 0xFF);
    
    tim -> CCR3 = (blue * tim->ARR) / 255; //blue 
    tim -> CCR2 = (green * tim->ARR) / 255; //green
    tim -> CCR1 = (red * tim->ARR) / 255; //red

}

int main(void){
    TIM_RGB_TypeDef sim_tim = {0};

    sim_tim.ARR = 1000;

    set_rgb_color(&sim_tim, 0x804020);

    printf("Red: %d, Green: %d, Blue: %d", sim_tim.CCR1, sim_tim.CCR2, sim_tim.CCR3);
}