#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct { 
    uint32_t TX_COMP:1; 
    uint32_t RX_RDY:1; 
    uint32_t OVR_ERR:1; 
    uint32_t RSV:4;
    uint32_t IRQ_EN:1; 
    uint32_t RSV2:24; 
} Unsafe_IFR_t;

//essentially this function performs Read-Modify-Write, but if OVR_ERR (Bit 2) happened to be set to 1 by hardware at that exact moment, the compiler's Read-Modify-Write sequence reads Bit 2 as a 1, and then writes that 1 back to Bit 2, accidentally clearing the error flag before your error-handling firmware ever saw it.
void unsafe_enable_irq(Unsafe_IFR_t volatile *reg);
void unsafe_enable_irq(Unsafe_IFR_t volatile *reg) {
    reg->IRQ_EN = 1;
}

typedef struct { 
    uint32_t volatile IFR; 
} Safe_Regs_t;

void safe_enable_irq(Safe_Regs_t *reg);
void safe_enable_irq(Safe_Regs_t *reg){
    reg->IFR = 0x00000010;
}

int main(void) {
    uint32_t var = 0x00000004;
    unsafe_enable_irq((Unsafe_IFR_t*)var);

    var = 0x00000004;
    safe_enable_irq((Safe_Regs_t*)var);

    return 0;
}

