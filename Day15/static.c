#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Simulated FreeRTOS types
typedef uint32_t StackType_t;
typedef struct { uint32_t dummy; } StaticTask_t;
typedef void (*TaskFunction_t)(void *);
typedef void* TaskHandle_t;

#define MAX_TASKS 3
#define STACK_DEPTH 128

// Static Memory Pools
static StaticTask_t task_tcb_pool[MAX_TASKS];
static StackType_t task_stack_pool[MAX_TASKS * STACK_DEPTH]; 
static uint32_t allocated_tasks = 0;

// FreeRTOS API you will call (Simulation signature):
// TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode, const char * const pcName, 
//                                const uint32_t ulStackDepth, void * const pvParameters, 
//                                uint32_t uxPriority, StackType_t * const puxStackBuffer, 
//                                StaticTask_t * const pxTaskBuffer);

// YOUR TASK:
// Implement this function. 
// 1. Guard against integer underflow/overflow (check if allocated_tasks >= MAX_TASKS).
// 2. Calculate the pointer to the next available TCB in task_tcb_pool. 
// 3. Calculate the pointer to the next available block of 128 StackType_t elements in task_stack_pool.
//    CONSTRAINT: ZERO array brackets [] allowed in your calculations. Use strict pointer arithmetic.
// 4. Call xTaskCreateStatic with priority 1.
// 5. Store the returned TaskHandle_t into the memory address provided by `ppxTaskHandle`.
void Allocate_System_Task(TaskFunction_t task_func, void *params, TaskHandle_t **ppxTaskHandle);
void Allocate_System_Task(TaskFunction_t task_func, void *params, TaskHandle_t **ppxTaskHandle){
    if (allocated_tasks >= MAX_TASKS) {
        return;
    }
    StaticTask_t *tcb_ptr = task_tcb_pool + allocated_tasks;
    StackType_t *stack_ptr = task_stack_pool + (allocated_tasks * STACK_DEPTH);

    xTaskCreateStatic(task_func, "sopme name", STACK_DEPTH, NULL, 1, *ppxTaskHandle);

}

int main(void){


    return 0;
}