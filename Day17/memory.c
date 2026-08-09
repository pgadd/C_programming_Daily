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
    volatile uint32_t CR; // Control Reg: Bit 0 is 'START'
    volatile uint32_t SR; // Status Reg: Bit 1 is 'BUSY'
} CRYPTO_TypeDef;

#define UART1_BASE_PTR   ((UART_TypeDef *)0x40011000UL)
#define CRYPTO_BASE_PTR  ((CRYPTO_TypeDef *)0x50060000UL)

// RTOS Types & Simulated APIs
typedef uint32_t TickType_t;
typedef long BaseType_t;
typedef void* SemaphoreHandle_t;
typedef struct { uint32_t dummy; } StaticSemaphore_t;

#define pdFALSE (0)
#define pdTRUE  (1)
#define portMAX_DELAY 0xFFFFFFFFUL

// Simulated RTOS Semaphore APIs
SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t *pxSemaphoreBuffer);
SemaphoreHandle_t xSemaphoreCreateMutexStatic(StaticSemaphore_t *pxMutexBuffer);
SemaphoreHandle_t xSemaphoreCreateCountingStatic(uint32_t uxMaxCount, uint32_t uxInitialCount, StaticSemaphore_t *pxSemaphoreBuffer);
BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore);
BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken);
void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken);
void taskENTER_CRITICAL(void);
void taskEXIT_CRITICAL(void);

// ==========================================
// 2. STATIC ALLOCATION & SYSTEM STATE
// ==========================================

static StaticSemaphore_t uart_rx_sem_buffer;
static SemaphoreHandle_t uart_rx_sem;

static StaticSemaphore_t crypto_mutex_buffer;
static SemaphoreHandle_t crypto_mutex;

static StaticSemaphore_t dma_count_sem_buffer;
static SemaphoreHandle_t dma_count_sem;

// Shared Hardware Resource Pool
typedef struct {
    uint8_t in_use; // 0 = Free, 1 = In Use
    uint32_t dma_id;
} DMA_Channel_t;

static DMA_Channel_t dma_pool[3] = {
    {0, 100}, {0, 101}, {0, 102}
};

// ==========================================
// 3. YOUR IMPLEMENTATION AREA
// ==========================================

// Challenge 1: The ISR to Task Synchronization
// Goal: In the UART ISR, give the binary semaphore to unblock a task.
// Don't forget the context switch yield mechanism we learned on Day 16!
void UART1_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // 1. Give the uart_rx_sem from the ISR.
    xSemaphoreGiveFromISR(uart_rx_sem, &xHigherPriorityTaskWoken);
    
    // 2. Yield if a higher priority task was woken.
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Challenge 2: Mutex Protected Hardware
// Goal: A task wants to encrypt data using the single CRYPTO hardware block.
// 1. Take the crypto_mutex (Wait indefinitely).
// 2. Safely point to CRYPTO_BASE_PTR.
// 3. Atomically SET Bit 0 in CR to start encryption (using |= is safe here ONLY because we hold the Mutex!).
// 4. Give the crypto_mutex back.
void Secure_Crypto_Task(void *pvParameters) {

    volatile CRYPTO_TypeDef *ptr = CRYPTO_BASE_PTR;

    while(1) {
        // Implement logic here
        xSemaphoreTake(crypto_mutex, portMAX_DELAY);

        ptr->CR |= 1UL;

        xSemaphoreGive(crypto_mutex);

        vTaskDelay(10);
    }
}

// Challenge 3: Counting Semaphore & Critical Section Resource Allocator
// Goal: We have 3 DMA channels. A task needs to request one safely.
// 1. Take the dma_count_sem (Wait indefinitely).
// 2. Enter a Critical Section (to protect the `in_use` array check).
// 3. Iterate through `dma_pool` using STRICT POINTER ARITHMETIC (no []). Find the first channel where in_use == 0.
// 4. Set in_use = 1.
// 5. Assign the found channel pointer to the double pointer parameter `**assigned_dma`.
// 6. Exit the Critical Section.
void Allocate_DMA_Channel(DMA_Channel_t **assigned_dma) {
    // Implement logic here
    xSemaphoreTake(dma_count_sem, portMAX_DELAY);

    taskENTER_CRITICAL();

    DMA_Channel_t *ptr = dma_pool;

    for (int i = 0; i < 3; i++){
        if(ptr->in_use == 0) {
            ptr->in_use = 1;
            *assigned_dma = ptr; 

            break;
        }
        ptr++;
    }

    taskEXIT_CRITICAL();
}