//Sample STM32 HAL GPIO library functions implementation

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef enum {
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET   = 1U
} GPIO_PinState;

// 2. Hardware Register Overlay (must use volatile!)
typedef struct {
    uint32_t volatile MODER;    // Mode register (2 bits per pin)
    uint32_t volatile OTYPER;   // Output type register
    uint32_t volatile OSPEEDR;  // Output speed register
    uint32_t volatile PUPDR;    // Pull-up/pull-down register (2 bits per pin)
    uint32_t volatile IDR;      // Input data register
    uint32_t volatile ODR;      // Output data register
    uint32_t volatile BSRR;     // Bit Set/Reset register
    uint32_t volatile LCKR;     // Configuration lock register
    uint32_t volatile AFR[2];   // Alternate function registers
} GPIO_TypeDef;

// 3. Standard HAL Initialization Structure
typedef struct {
    uint32_t Pin;  // Bitmask of pins to configure (e.g., (1UL << 5) | (1UL << 12))
    uint32_t Mode; // 0x00 = Input, 0x01 = Output, 0x02 = Alt Func, 0x03 = Analog
    uint32_t Pull; // 0x00 = No Pull, 0x01 = Pull-Up, 0x02 = Pull-Down
} GPIO_InitTypeDef;

void CUSTOM_HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);

void CUSTOM_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

void CUSTOM_HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init){
    for (uint8_t pin = 0; pin < 15; pin ++){
        if (GPIO_Init->Pin & (1UL << pin)){
            uint32_t shift = (pin * 2U);

            GPIOx -> MODER &= ~(3UL << shift);
            GPIOx -> MODER |= (GPIO_Init->Mode << shift);

            GPIOx->PUPDR &= ~(3UL << shift);
            GPIOx->PUPDR |= (GPIO_Init->Pull << shift);
        }
    }
}

void CUSTOM_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    if (PinState == GPIO_PIN_SET)   {
        GPIOx->BSRR = GPIO_Pin;
    } else if (PinState == GPIO_PIN_RESET){
        GPIOx->BSRR = ((uint32_t)GPIO_Pin << 16);
    }

}

int main(void) {
    GPIO_TypeDef sim_gpio = {0};
    GPIO_InitTypeDef init_cfg;
    init_cfg.Pin = (1UL << 5) | (1UL << 12);

    init_cfg.Mode = 0x01;
    init_cfg.Pull = 0x01;

    CUSTOM_HAL_GPIO_Init(&sim_gpio, &init_cfg);
    printf("Output: %X\n", sim_gpio.MODER);

    CUSTOM_HAL_GPIO_WritePin(&sim_gpio, (1UL << 5), GPIO_PIN_SET);
    printf("Output: %X\n", sim_gpio.BSRR);

    CUSTOM_HAL_GPIO_WritePin(&sim_gpio, (1UL << 12), GPIO_PIN_RESET);
    printf("Output: %X\n", sim_gpio.BSRR);

    return 0;
}