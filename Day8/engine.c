#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t volatile SR;
    uint32_t volatile DR;
} UART_TypeDef;

void on_rx_byte_callback(uint8_t *mem_ptr, uint8_t byte_val);
void on_rx_byte_callback(uint8_t *mem_ptr, uint8_t byte_val){
    printf("-> [RX EVENT] Stored byte 0x%X at SRAM address %p", byte_val, (void*)mem_ptr);
}

void drain_uart_rx(UART_TypeDef *uart, uint8_t **pool_head, void (*on_byte_processed)(uint8_t*, uint8_t));
void drain_uart_rx(UART_TypeDef *uart, uint8_t **pool_head, void (*on_byte_processed)(uint8_t*, uint8_t)){
    uint8_t temp;
    while (uart->SR & (1<<5)){
        temp = (uint8_t)uart->DR;
        **pool_head = temp;

        on_rx_byte_callback(*pool_head, temp);

        (*pool_head)++;
        uart->SR = 0;
    }
}

int main(void) {
    uint8_t sram_pool[64];
    uint8_t *pool_ptr = sram_pool;

    UART_TypeDef simulated_uart; 
    simulated_uart.SR = (1 << 5); 
    simulated_uart.DR = 0x41;

    drain_uart_rx(&simulated_uart, &pool_ptr, on_rx_byte_callback);
    printf("Verification: sram_pool[0] = 0x%02X | Next free pool address: %p\n", 
           *sram_pool, (void*)pool_ptr);

    return 0;
}