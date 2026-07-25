#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void on_pin_toggle_callback(void);
void on_pin_toggle_callback(void) {
    printf("-> HARDWARE EVENT: Pin state toggled!  ");
}

typedef struct {
    uint32_t MODER;
    uint32_t ODR;
} GPIO_Regs_t;

typedef struct {
    GPIO_Regs_t *hardware;
    void (*on_toggle)(void);
} GPIO_Driver_t;

void init_gpio_driver(GPIO_Driver_t **driverPtr, GPIO_Regs_t *raw_ram);
void init_gpio_driver(GPIO_Driver_t **driverPtr, GPIO_Regs_t *raw_ram) {
    *driverPtr = (GPIO_Driver_t*)malloc(sizeof(GPIO_Driver_t));

    if (*driverPtr == NULL) {
        printf("Malloc failed");
        return;
    }

    (*driverPtr) -> hardware = raw_ram;
    raw_ram -> MODER = 0;
    raw_ram -> ODR = 0;
    (*driverPtr) -> on_toggle = on_pin_toggle_callback;
}

void pin_high(GPIO_Driver_t *driver, uint8_t pin_num);
void pin_low(GPIO_Driver_t *driver, uint8_t pin_num);
void pin_toggle(GPIO_Driver_t *driver, uint8_t pin_num);


void pin_high(GPIO_Driver_t *driver, uint8_t pin_num) {
    driver -> hardware -> ODR |= (1 << pin_num);
}

void pin_low(GPIO_Driver_t *driver, uint8_t pin_num) {
    driver -> hardware -> ODR &= ~(1 << pin_num);
}

void pin_toggle(GPIO_Driver_t *driver, uint8_t pin_num) {
    driver -> hardware -> ODR ^= (1 << pin_num);
    driver -> on_toggle();
}


int main(void) {
    GPIO_Regs_t *heap = (GPIO_Regs_t*)malloc(sizeof(GPIO_Regs_t));
    GPIO_Driver_t *myPinDriver = NULL;

    init_gpio_driver(&myPinDriver, heap);

    pin_high(myPinDriver, 12);
    printf("ODR value: %X\n", myPinDriver->hardware->ODR);

    pin_high(myPinDriver, 5);
    printf("ODR value: %X\n", myPinDriver->hardware->ODR);

    pin_toggle(myPinDriver, 12);
    printf("ODR value: %X\n", myPinDriver->hardware->ODR);

    pin_low(myPinDriver, 5);
    printf("ODR value: %X\n", myPinDriver->hardware->ODR);

    free(heap);
    free(myPinDriver);

    return 0;
}