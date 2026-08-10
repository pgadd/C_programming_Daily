#include <stdint.h>
#include <stdio.h>

// ==========================================
// 1. HARDWARE & RTOS SCAFFOLDING 
// ==========================================

// Simulated Hardware Registers
typedef struct {
    volatile uint32_t SR; 
    volatile uint32_t DR;
} UART_TypeDef;

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
} GPIO_TypeDef;

#define UART1_BASE_PTR ((UART_TypeDef *)0x40011000UL)
#define GPIOA_BASE_PTR ((GPIO_TypeDef *)0x40020000UL)

// RTOS Types & Dummy APIs
typedef void* QueueHandle_t;
typedef struct { uint32_t dummy; } StaticQueue_t;
typedef long BaseType_t;
typedef uint32_t TickType_t;

#define pdFALSE 0
#define pdTRUE  1
#define pdPASS  1
#define portMAX_DELAY 0xFFFFFFFFUL

QueueHandle_t xQueueCreateStatic(uint32_t uxQueueLength, uint32_t uxItemSize, uint8_t *pucQueueStorageBuffer, StaticQueue_t *pxQueueBuffer);
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken);
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);
BaseType_t xQueuePeek(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);
void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken);

// ==========================================
// 2. STATIC MEMORY POOLS
// ==========================================

#define CMD_QUEUE_LEN 5
#define CMD_ITEM_SIZE sizeof(uint32_t)

// YOUR JOB: Define the static arrays required for the queue here.
// You need a StaticQueue_t variable and a uint8_t array sized correctly for 5 uint32_t items.

StaticQueue_t variable;
uint8_t buffer_portable[5 * sizeof(uint32_t)];

// ==========================================
// 3. YOUR IMPLEMENTATION AREA
// ==========================================

// Challenge 1: Static Queue Initialization
// Goal: Allocate the queue using your static memory blocks and return the handle via the double pointer.
void Init_Command_Queue(QueueHandle_t **queue_handle_ptr) {
    // Implement logic here. Assign the result to **queue_handle_ptr.
    **queue_handle_ptr = xQueueCreateStatic(5, 32, buffer_portable, &variable);

}

// Challenge 2: The ISR Sender
// Goal: A hardware interrupt fired. Read the UART Data Register into a local uint32_t variable. 
// Clear Bit 5 (RXNE) in the Status Register using clear-then-set.
// Send the data to the queue from the ISR. Trigger a yield if required.
void UART1_IRQHandler(QueueHandle_t target_queue) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    volatile UART_TypeDef *uart = UART1_BASE_PTR;

    // Implement logic here.
    uint32_t uart_data = uart->DR;

    uart->DR &= ~(1UL << 5);

    xQueueSendFromISR(target_queue, &uart_data, &higher_priority_task_woken);
}

// Challenge 3: The Processing Task (Receive & Peek)
// Goal: Wait up to 100 ticks for a command.
// 1. PEEK the queue first. If the command equals 0xFFFFFFFF (a simulated faulty packet), 
//    extract it using xQueueReceive into a dummy variable to clear it out, and do nothing else.
// 2. If it is a valid command, extract it using xQueueReceive.
//    - If command == 1: Atomically SET GPIOA Pin 5 via BSRR.
//    - If command == 2: Atomically RESET GPIOA Pin 5 via BSRR (using the upper 16 bits).
// CONSTRAINT: Safely cast pvParameters to a QueueHandle_t pointer and handle NULL traps.
void Command_Processor_Task(void *pvParameters) {
    while(1) {
        // Implement logic here.
        QueueHandle_t queue;
        uint32_t dummy;

        xQueuePeek(&queue, pvParameters, 100);

        if(pvParameters == 0xFFFFFFFF) {
            dummy = xQueueReceive(queue, pvParameters, 100);
        }

        if(pvParameters == 1) {
            GPIOA_BASE_PTR->BSRR = (1UL << 5);
        }

        if(pvParameters == 2) {
            GPIOA_BASE_PTR->BSRR = (1UL << (5 + 16));
        }

    }
}