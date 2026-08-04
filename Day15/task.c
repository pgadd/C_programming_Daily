#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    volatile uint32_t MODER;   // 0x00
    volatile uint32_t OTYPER;  // 0x04
    volatile uint32_t OSPEEDR; // 0x08
    volatile uint32_t PUPDR;   // 0x0C
    volatile uint32_t IDR;     // 0x10
    volatile uint32_t ODR;     // 0x14
    volatile uint32_t BSRR;    // 0x18 - Bit Set/Reset Register
} GPIO_TypeDef;

typedef enum {
    TASK_STATE_INIT = 0,
    TASK_STATE_RUN,
    TASK_STATE_ERROR
} TaskState_t;

typedef struct {
    GPIO_TypeDef *port;
    uint8_t pin_number;     // e.g., 5 for Pin 5
    TaskState_t *state_ptr; // Points to shared state
} HardwareConfig_t;

// Simulated FreeRTOS Delay
void vTaskDelay(uint32_t ticks);
void vTaskDelay(uint32_t ticks) {
    for (int x = 0; x < ticks; x ++){

    }
}

// YOUR TASK:
// Write the infinite loop for the task.
// 1. Cast pvParameters to a HardwareConfig_t pointer safely.
// 2. Dereference state_ptr to get the current state.
// 3. Switch on the state:
//    - If INIT: Do nothing.
//    - If RUN: Atomically SET the GPIO pin using BSRR (Bits 0-15 are SET).
//    - If ERROR: Atomically RESET the GPIO pin using BSRR (Bits 16-31 are RESET).
// 4. Delay for 100 ticks.
// CONSTRAINT: Protect against pointer dereferencing traps (check for NULL params).
void Hardware_Control_Task(void *pvParameters);
void Hardware_Control_Task(void *pvParameters) {
    while(1){

        HardwareConfig_t *ptr = pvParameters;
        if(pvParameters == NULL){
            return;
        }

        switch(*(ptr->state_ptr)) {
            case(TASK_STATE_INIT):
                break;
            case(TASK_STATE_RUN):
                ptr->port->BSRR = (1 << ptr->pin_number);
                break;
            case(TASK_STATE_ERROR):
                ptr->port->BSRR = (1 << (ptr->pin_number+16));
                break;
        }

        vTaskDelay(100);

    }
    
}

int main(void){
    HardwareConfig_t sample;
    Hardware_Control_Task(&sample);

    printf("current state: %d, BSRR: %X\n", sample.state_ptr, sample.port->BSRR);

}