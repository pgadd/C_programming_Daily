#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ==========================================
// 1. HARDWARE & RTOS SCAFFOLDING 
// ==========================================

// ARM Cortex-M System Control Register (for Power Management)
typedef struct {
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;     // 0x10 - System Control Register
} SCB_TypeDef;
#define SCB_BASE_PTR ((SCB_TypeDef *)0xE000ED00UL)

// Simulated ADC Peripheral
typedef struct {
    volatile uint32_t SR;      // Status Register
    volatile uint32_t CR1;     // Control Register 1
    volatile uint32_t CR2;     // Control Register 2
    volatile uint32_t DR;      // Data Register
} ADC_TypeDef;
#define ADC1_BASE_PTR ((ADC_TypeDef *)0x40012000UL)

// RTOS Types
typedef uint32_t StackType_t;
typedef void* TaskHandle_t;
typedef long BaseType_t;
#define pdFALSE (0)
#define pdTRUE  (1)
#define portMAX_DELAY 0xFFFFFFFFUL

// Dummy System State
SCB_TypeDef dummy_scb = {0};
ADC_TypeDef dummy_adc = {0};
TaskHandle_t processing_task_handle = (TaskHandle_t)0xDEADBEEF; // Dummy handle
uint32_t fake_adc_buffer[5] = {100, 200, 300, 9999, 400};       // 9999 is our fault trigger

// Dummy RTOS APIs
void vTaskNotifyGiveFromISR(TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken) {
    printf("[RTOS] Notification given from ISR to Task %p\n", xTaskToNotify);
    *pxHigherPriorityTaskWoken = pdTRUE; // Simulating a higher priority task woke up
}

void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken) {
    if (xHigherPriorityTaskWoken) {
        printf("[RTOS] portYIELD_FROM_ISR triggered (Context Switch Pending)\n");
    }
}

uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit, uint32_t xTicksToWait) {
    printf("[RTOS] Task unblocked by Notification\n");
    return 1; // Simulated received notification
}

void vTaskDelete(TaskHandle_t xTaskToDelete) {
    if (xTaskToDelete == NULL) {
        printf("[RTOS] Task deleted ITSELF.\n");
    } else {
        printf("[RTOS] Task deleted.\n");
    }
    // In a real system, this doesn't return if deleting itself.
}

// ==========================================
// 2. YOUR IMPLEMENTATION AREA
// ==========================================

// Function 1: The Idle Hook (Power Management)
// Goal: Set the SLEEPDEEP bit (Bit 2) in the System Control Register (SCR),
// then execute the Wait For Interrupt instruction to pause the CPU clock.
// CONSTRAINT: Use volatile casting properly. Use bitwise `|=`. 
// Do NOT overwrite other bits in the SCR.
void vApplicationIdleHook(void) {
    // 1. Create a safe volatile pointer to the SCR register
    
    // 2. Set bit 2 (SLEEPDEEP) using 1UL
    
    // 3. Execute the WFI instruction (Simulate it via printf for this exercise)
    // printf("[HARDWARE] WFI Executed. CPU Sleeping...\n");
}


// Function 2: The ISR Handler
// Goal: Clear the ADC interrupt flag, then notify the processing task.
// The ADC Status Register (SR) bit 1 (EOC - End Of Conversion) is cleared by writing 0 to it.
// All other bits in SR must remain untouched. 
// After clearing the flag, use vTaskNotifyGiveFromISR.
void ADC1_Interrupt_Handler(TaskHandle_t target_task) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // 1. Safely point to the ADC1 SR register
    
    // 2. Clear bit 1 (EOC) using bitwise AND combined with a bitwise NOT (Clear-then-set methodology).
    
    // 3. Notify the target_task.
    
    // 4. Yield if necessary.
}


// Function 3: The Processing Task
// Goal: Wait for notification, then process a buffer of ADC values using strict pointer math.
// CONSTRAINT: Zero array brackets []. Do NOT use index variables like 'int i'. 
// Use pointer advancement (ptr++) exclusively.
void ADC_Processing_Task(void *pvParameters) {
    // The params is a double pointer to the start of the buffer array.
    uint32_t **buffer_pp = (uint32_t **)pvParameters;
    
    // Check for NULL parameters
    
    while (1) {
        // 1. Wait indefinitely for a notification (use ulTaskNotifyTake, clear count on exit)
        
        printf("[TASK] Waking up to process data...\n");

        // 2. Dereference the double pointer to get the current buffer start address
        uint32_t *data_ptr = *buffer_pp;
        
        // 3. Iterate exactly 5 times using pointer arithmetic. 
        // If any value is > 4095, print a fault message, and delete THIS task (using NULL).
        // If a task deletes itself, it must immediately enter an infinite loop (while(1);) 
        // because vTaskDelete(NULL) might not return immediately depending on the port.
        
        
        printf("[TASK] Buffer processed successfully.\n");
        // Loop repeats and goes back to sleep at ulTaskNotifyTake
    }
}

// ==========================================
// 3. MAIN TEST HARNESS
// ==========================================
int main(void) {
    printf("--- Booting Embedded System (Day 16) ---\n\n");

    // 1. Simulate the Idle Hook running when system starts
    dummy_scb.SCR = 0x00000000;
    vApplicationIdleHook();
    printf("SCR State (Expected 0x4): 0x%X\n\n", dummy_scb.SCR);

    // 2. Setup Task Parameters
    uint32_t *active_buffer = fake_adc_buffer;

    // 3. Simulate Task Execution Pipeline
    printf("--- Simulating ISR Firing ---\n");
    dummy_adc.SR = (1UL << 1); // Simulate EOC flag set by hardware
    
    ADC1_Interrupt_Handler(processing_task_handle);
    
    printf("ADC SR State (Expected 0x0): 0x%X\n\n", dummy_adc.SR);

    printf("--- Simulating RTOS Context Switch to Task ---\n");
    // Call the task manually for the test harness. 
    // We break the while(1) internally just to prevent a PC lockup.
    // In your code, you must include the while(1). I will evaluate it manually.
    ADC_Processing_Task(&active_buffer);

    return 0;
}