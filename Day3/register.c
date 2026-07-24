#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint32_t MODER;
    uint32_t ODR;
    uint32_t IDR;
} GPIO_TypeDef_t;

int main(void) {
    uint8_t *hardware_ram = (uint8_t*)malloc(12);

    if (hardware_ram == NULL) {
        printf("ERROR: Malloc failed!\n");
        return 1;
    }
    
    for (int i = 0; i < 12; i++){
        hardware_ram[i] = 0;
    }

    GPIO_TypeDef_t *portA = (GPIO_TypeDef_t *)hardware_ram;

    portA -> MODER = 0x00000001;
    portA -> ODR = 0x00008000;

    printf("ODR via raw RAM: 0x%X\n", *((uint32_t *)(hardware_ram + 4)));

    free(hardware_ram);

    return 0;
}