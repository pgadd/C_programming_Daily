#include <stdint.h>
#include <stdio.h>

// ==========================================
// 1. HARDWARE & RTOS SCAFFOLDING 
// ==========================================

// Simulated GPIO & ADC Registers
typedef struct {
    uint32_t volatile MODER;
    uint32_t volatile ODR;
    uint32_t volatile BSRR;
} GPIO_TypeDef;

typedef struct {
    uint32_t volatile SR; // Bit 1 = Data Ready
    uint32_t volatile DR; 
} ADC_TypeDef;

#define GPIOA_BASE_PTR ((GPIO_TypeDef *)0x40020000UL)
#define ADC1_BASE_PTR  ((ADC_TypeDef *)0x40012000UL)

// Dummy Hardware State
GPIO_TypeDef dummy_gpioA = {0};
ADC_TypeDef  dummy_adc1  = {0};

// RTOS Types & Simulated APIs
typedef void* TimerHandle_t;
typedef struct { uint32_t dummy; } StaticTimer_t;
typedef void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);
typedef uint32_t TickType_t;
typedef long BaseType_t;

#define pdFALSE 0
#define pdTRUE  1
#define pdPASS  1

TimerHandle_t xTimerCreateStatic(const char * const pcTimerName, 
                                 const TickType_t xTimerPeriodInTicks,
                                 const uint32_t uxAutoReload,
                                 void * const pvTimerID,
                                 TimerCallbackFunction_t pxCallbackFunction,
                                 StaticTimer_t *pxTimerBuffer);

BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait);
BaseType_t xTimerReset(TimerHandle_t xTimer, TickType_t xTicksToWait);
void *pvTimerGetTimerID(TimerHandle_t xTimer);

// ==========================================
// 2. STATIC MEMORY POOLS
// ==========================================
#define MAX_TIMERS 2
static StaticTimer_t timer_pool[MAX_TIMERS];
static uint32_t allocated_timers = 0;

// Global Handle for Watchdog
TimerHandle_t watchdog_timer = NULL;

// ==========================================
// 3. YOUR IMPLEMENTATION AREA
// ==========================================

// Challenge 1: The Static Timer Allocator
// Goal: Allocate a timer from the `timer_pool` without using [] brackets.
// 1. Guard against overflow (allocated_timers >= MAX_TIMERS).
// 2. Use strict pointer arithmetic (+ offset) to get the next StaticTimer_t block.
// 3. Call xTimerCreateStatic. 
// 4. Assign the returned handle to the double pointer `ppxTimerHandle`.
// 5. Increment allocated_timers.
void Allocate_Software_Timer(TickType_t period, uint32_t is_periodic, void *timer_id, 
                             TimerCallbackFunction_t callback, TimerHandle_t **ppxTimerHandle) {
    // Implement logic here
    if (allocated_timers >= MAX_TIMERS){
        return;
    }

    StaticTimer_t *ptr = timer_pool + allocated_timers;

    **ppxTimerHandle = xTimerCreateStatic("timer", period, pdFALSE, timer_id, callback, ptr);

    allocated_timers++;
}


// Challenge 2: The Watchdog Bite Callback
// Goal: This callback fires ONLY if the system task hangs and fails to reset the timer.
// 1. The `pvTimerID` passed to this timer during creation was a pointer to GPIOA_BASE_PTR.
// 2. Extract the ID using `pvTimerGetTimerID(xTimer)`.
// 3. Safely cast the void pointer to a `volatile GPIO_TypeDef *`.
// 4. Atomically RESET Pin 8 (the motor enable pin) using the upper 16 bits of BSRR. 
void Watchdog_Callback(TimerHandle_t xTimer) {
    // Implement logic here
    volatile GPIO_TypeDef *ptr = pvTimerGetTimerID(xTimer);

    ptr->BSRR = (1 << (8 + 16U));
    
}


// Challenge 3: The Periodic Hardware Poller
// Goal: Execute every 10ms to check if ADC data is ready, avoiding CPU spin-loops.
// 1. Extract the Timer ID, cast it to a `volatile ADC_TypeDef *`.
// 2. Check if Bit 1 (Data Ready) in the SR is 1.
// 3. If it is 1, read the DR register into a local variable (simulate reading clears it).
// 4. Print the local variable: "[POLLER] ADC Read: %d\n"
void ADC_Poller_Callback(TimerHandle_t xTimer) {
    // Implement logic here
    volatile ADC_TypeDef *ptr = (volatile ADC_TypeDef *)pvTimerGetTimerID(xTimer);
    uint32_t local = 0;

    if (ptr->SR & (1UL << 1)){
        local = ptr->DR;
    }

    printf("[POLLER] ADC read: %d\n", local);
}

int main(void){
    volatile TimerHandle_t **timer = &watchdog_timer;
    volatile StaticTimer_t **pool = &timer_pool;

    Allocate_Software_Timer(500, pdFALSE, &dummy_gpioA, Watchdog_Callback, timer);

    Allocate_Software_Timer(10, pdTRUE, &dummy_adc1, ADC_Poller_Callback, timer);

    xTimerStart( timer, 0);

    if (dummy_adc1.SR == 2){
        ADC_Poller_Callback(timer_pool);
    }

    Watchdog_Callback(watchdog_timer);

    if(dummy_gpioA.BSRR == (1 << (8 + 16))){
        printf("Successfuly shut of pin 8");
    }

}