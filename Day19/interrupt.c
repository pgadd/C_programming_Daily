#include <stdint.h>
#include <stdio.h>

// ==========================================
// 1. HARDWARE & RTOS SCAFFOLDING 
// ==========================================

// Simulated NVIC (Nested Vectored Interrupt Controller)
typedef struct {
    uint32_t volatile ISER[8]; // Interrupt Set Enable Register
    uint32_t volatile ICER[8]; // Interrupt Clear Enable Register
    uint32_t volatile ISPR[8]; // Interrupt Set Pending Register
    uint32_t volatile ICPR[8]; // Interrupt Clear Pending Register
    uint32_t volatile IABR[8]; // Interrupt Active bit Register
    uint32_t reserved[56];
    uint32_t volatile IPR[60]; // Interrupt Priority Registers (4 priority bytes per 32-bit register)
} NVIC_TypeDef;

#define NVIC_BASE_PTR ((NVIC_TypeDef *)0xE000E100UL)

// Simulated ADC & GPIO Registers
typedef struct {
    uint32_t volatile SR; // Bit 1: EOC (End of Conversion)
    uint32_t volatile DR; // 16-bit Data
} ADC_TypeDef;

typedef struct {
    uint32_t volatile BSRR;
} GPIO_TypeDef;

#define ADC1_BASE_PTR  ((ADC_TypeDef *)0x40012000UL)
#define GPIOB_BASE_PTR ((GPIO_TypeDef *)0x40020400UL)

// RTOS Types & Simulated APIs
typedef long BaseType_t;
typedef void* QueueHandle_t;
typedef struct { uint32_t dummy; } StaticQueue_t;

#define pdFALSE 0
#define pdTRUE  1
#define pdPASS  1
#define portMAX_DELAY 0xFFFFFFFFUL
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 5 // Priority 5 or numerically higher (e.g., 6, 7) is RTOS safe

QueueHandle_t xQueueCreateStatic(uint32_t uxQueueLength, uint32_t uxItemSize, uint8_t *pucQueueStorageBuffer, StaticQueue_t *pxQueueBuffer);
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken);
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, uint32_t xTicksToWait);
void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken);

// Global System State
typedef enum {
    SENSOR_STATE_IDLE,
    SENSOR_STATE_NORMAL,
    SENSOR_STATE_ALARM
} SensorState_t;

static SensorState_t current_sensor_state = SENSOR_STATE_IDLE;

// ==========================================
// 2. STATIC MEMORY POOLS
// ==========================================
#define QUEUE_LEN 5
#define ITEM_SIZE sizeof(uint16_t)

static StaticQueue_t adc_queue_tcb;
static uint8_t adc_queue_buffer[QUEUE_LEN * ITEM_SIZE];
static QueueHandle_t adc_queue = NULL;


// ==========================================
// 3. YOUR IMPLEMENTATION AREA
// ==========================================

// Challenge 1: The NVIC Priority Configurator
// Goal: Set the priority of the ADC Interrupt (IRQ 18).
// 1. Point to the NVIC_BASE_PTR.
// 2. The IPR array holds 4 priority bytes per 32-bit register. 
//    To find the target register, divide IRQ by 4. To find the byte offset, modulo by 4.
// 3. Calculate the bit shift (byte offset * 8).
// 4. Use STRICT POINTER ARITHMETIC (no []) to access the correct IPR register, and set the 
//    priority to a value that is strictly RTOS safe (e.g., Priority 6).
// 5. Clear the existing priority bits for this IRQ first, then set the new priority.
void Configure_ADC_Interrupt_Priority(uint8_t irq_number, uint8_t target_priority) {
    // Implement logic here
    NVIC_TypeDef *nvic = NVIC_BASE_PTR;

    uint8_t target_register = irq_number/4;
    uint8_t byte_offset = irq_number % 4;

    uint32_t bit_shift = byte_offset * 8;

    uint32_t *ptr = nvic -> IPR;

    ptr += target_register;

    *ptr |= ((uint32_t)target_priority << byte_offset);

}

// Challenge 2: The ISR (Deferred Trigger)
// Goal: Handle the hardware interrupt and safely pass data to the RTOS.
// 1. Safely cast to ADC1_BASE_PTR.
// 2. Read the ADC Data Register (DR) into a local uint16_t variable.
// 3. Clear the EOC hardware flag (Bit 1) in the SR using Clear-then-Set mask.
// 4. Send the local variable to the `adc_queue` using the FromISR API.
// 5. Yield if a higher priority task was woken.
void ADC1_IRQHandler(void) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    // Implement logic here
    ADC_TypeDef *ptr = ADC1_BASE_PTR;

    uint16_t var = ptr->DR;

    ptr->SR &= ~(1 << 1);

    xQueueSendFromISR(adc_queue, &var, &higher_priority_task_woken);

    portYIELD_FROM_ISR(higher_priority_task_woken);
}

// Challenge 3: The Deferred Processing Task
// Goal: Wait for data, run a state machine, and drive hardware atomically.
// 1. Block indefinitely on xQueueReceive using `adc_queue`.
// 2. Check the received value:
//    - If value < 2000: Set state to SENSOR_STATE_NORMAL.
//    - If value >= 2000: Set state to SENSOR_STATE_ALARM.
// 3. Switch on the state:
//    - NORMAL: Atomically RESET GPIOB Pin 3 (using upper 16 bits of BSRR).
//    - ALARM: Atomically SET GPIOB Pin 3 (using lower 16 bits of BSRR).
// CONSTRAINT: Trap NULL queue handle parameter.
void ADC_Processing_Task(void *pvParameters) {
    // Implement logic here
    if(pvParameters == NULL){
        while(1){};
    }

    xQueueReceive(adc_queue, pvParameters, portMAX_DELAY);

    if (pvParameters < 2000) {
        current_sensor_state = SENSOR_STATE_NORMAL;
    } else if (pvParameters >= 2000) {
        current_sensor_state = SENSOR_STATE_ALARM;
    }

    switch (current_sensor_state){
        case(SENSOR_STATE_NORMAL):
            GPIOB_BASE_PTR -> BSRR = (1UL << (3 + 16));
            break;
        case(SENSOR_STATE_ALARM):
            GPIOB_BASE_PTR -> BSRR = (1UL << 3);
            break;
    }

}