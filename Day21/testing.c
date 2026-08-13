#include <stdint.h>
#include <stdio.h>

// ==========================================
// 1. HARDWARE & RTOS SCAFFOLDING 
// ==========================================

// Simulated GPIO Register for Fault LED (Pin 13)
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
} GPIO_TypeDef;

#define GPIOC_BASE_PTR ((GPIO_TypeDef *)0x40020800UL)

// RTOS Types & APIs
typedef void* TaskHandle_t;
typedef void* QueueHandle_t;
typedef long BaseType_t;
typedef uint32_t TickType_t;

#define pdFALSE 0
#define pdTRUE  1
#define pdPASS  1
#define portMAX_DELAY 0xFFFFFFFFUL

BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);
BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify);
void vTaskDelay(TickType_t xTicksToWait);

// System State
typedef struct {
    uint64_t idle_ticks;
} SystemPerf_t;

typedef struct {
    uint8_t payload[4]; // 4 bytes of raw data
    uint8_t checksum;   // Expected to be the sum of the 4 payload bytes
} SensorPacket_t;

SystemPerf_t global_perf = {0};
QueueHandle_t integration_queue = (QueueHandle_t)0x1234; // Dummy handle
TaskHandle_t actuator_task = (TaskHandle_t)0x5678;       // Dummy handle

// ==========================================
// 2. YOUR IMPLEMENTATION AREA
// ==========================================

// Challenge 1: The Catastrophic Hardware Lock-Down
// Goal: This is called by the RTOS if a task overflows its static stack.
// 1. Iterate through the `pcTaskName` char pointer using strict pointer math (no []).
// 2. Print each character until you hit the null terminator '\0'.
// 3. Atomically SET Pin 13 on GPIOC (Turn on the Red Fault LED) using 1UL shifting.
// 4. Trap the CPU indefinitely.
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // Implement logic here
    char *ptr = pcTaskName;

    for (;;) {
        if(*ptr != '\0') {
            printf("%c\n", *ptr);
        }
        if(*ptr == '\0') {
            GPIOC_BASE_PTR->BSRR = (1UL << 13);
            while(1) {}
        }

        ptr++;
    }
}


// Challenge 2: The Performance Monitor
// Goal: Increment the 64-bit idle counter safely.
// 1. You are given a double pointer to the SystemPerf_t struct.
// 2. Protect against pointer dereferencing traps (NULL checks).
// 3. Protect against integer overflow of the `idle_ticks` variable (though it's 64-bit, 
//    show me you remember how to check before adding).
// 4. Increment the counter.
void vApplicationIdleHook_Custom(SystemPerf_t **perf_state_ptr) {
    // Implement logic here
    if (perf_state_ptr == NULL) {
        printf("NULL pointer");
        while(1) {};
    }
    if (*perf_state_ptr == NULL){
        printf("NULL pointer too");
        while(1) {};
    }

    if((*perf_state_ptr)->idle_ticks == UINT64_MAX){
        printf("Overflow reached");
        return;
    }

    (*perf_state_ptr)->idle_ticks++;

}


// Challenge 3: The Integration Task
// Goal: Pull a pointer from a queue, validate memory, and notify another task.
// 1. The task parameter `pvParameters` is a pointer to the `QueueHandle_t`. Safely cast it. Trap NULLs.
// 2. Block on xQueueReceive to receive a pointer to a `SensorPacket_t` (so your local buffer needs to be a pointer).
// 3. If received, use strict pointer arithmetic to iterate exactly 4 times over the `payload` array inside the struct.
// 4. Calculate the sum of the 4 bytes.
// 5. If the calculated sum matches `packet->checksum`, notify the `actuator_task` using xTaskNotifyGive.
void Data_Integration_Task(void *pvParameters) {
    // Implement logic here
    if(pvParameters == NULL) {
        printf("NULL pointer");
        while(1) {};
    }

    QueueHandle_t active_queue = (QueueHandle_t)pvParameters;
    
    while(1) {
        SensorPacket_t *local;

        BaseType_t value = xQueueReceive(active_queue, &local, portMAX_DELAY);

        uint32_t sum = 0;
        if(value == pdTRUE && local != NULL) {
            uint8_t *ptr = local->payload;
            for (int x = 0; x < 4; x++){
                sum += *ptr;
                ptr++;
            }

            if(sum == local->checksum){
                xTaskNotifyGive(actuator_task);
             }
        }

    }

}