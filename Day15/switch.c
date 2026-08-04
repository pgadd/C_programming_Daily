//In advanced driver development, you sometimes need to manually yield the processor from an ISR by forcing a context switch. You do this by setting the PENDSVSET bit in the Cortex-M Interrupt Control and State Register (ICSR).

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// SCB Register Overlay
typedef struct {
    volatile uint32_t CPUID;   // 0x00
    volatile uint32_t ICSR;    // 0x04 - Interrupt Control and State Register
    volatile uint32_t VTOR;    // 0x08
    volatile uint32_t AIRCR;   // 0x0C
} SCB_TypeDef;

#define SCB_BASE_PTR ((SCB_TypeDef *)0xE000ED00UL)
#define ICSR_PENDSVSET_POS (28U)


void Trigger_PendSV_Context_Switch(void);
void Trigger_PendSV_Context_Switch(void) {
    uint32_t *ptr = (uint32_t*)SCB_BASE_PTR;
    ptr++;
    *ptr = (1UL << 28);
}

int main(void){
    SCB_TypeDef sample;
    Trigger_PendSV_Context_Switch();
    printf("%d", sample.ICSR);

    return 0;
}