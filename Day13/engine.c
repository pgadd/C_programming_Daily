// You have 100 bytes of telemetry data to send. Instead of blocking, you will configure the UART to fire an interrupt every time it is ready for a new byte, feeding the hardware from RAM automatically.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    char *current_char;
    char *end_addr;
} UART_TX_Context_t;

static UART_TX_Context_t tx_ctx;

typedef struct {
    uint32_t volatile SR;   // Status Register
                            // Bit 7: TXE (Transmit Data Register Empty)
                            // Bit 5: RXNE (Read Data Register Not Empty)
                            // Bit 3: ORE (Overrun Error)
    uint32_t volatile DR;   // Data Register (Read to get RX, Write to send TX)
    uint32_t volatile BRR;  // Baud Rate Register
    uint32_t volatile CR1;  // Control Register 1
                            // Bit 13: UE (USART Enable)
                            // Bit 7: TXEIE (TXE Interrupt Enable)
                            // Bit 5: RXNEIE (RXNE Interrupt Enable)
                            // Bit 3: TE (Transmitter Enable)
                            // Bit 2: RE (Receiver Enable)
} UART_TypeDef;

void start_async_tx(UART_TypeDef *uart, char *string_start, uint16_t length);
void start_async_tx(UART_TypeDef *uart, char *string_start, uint16_t length) {

    tx_ctx.current_char = string_start;

    tx_ctx.end_addr = (string_start + length);

    uart->CR1 |= (1 << 7);

}

void UART_TX_IRQHandler(UART_TypeDef *uart);
void UART_TX_IRQHandler(UART_TypeDef *uart) {
    if ((uart->SR & (1 << 7)) && (uart->CR1 & (1 << 7))) {
        
        if (tx_ctx.current_char < tx_ctx.end_addr) {
            // FIX: Dereference to fetch the actual ASCII char from RAM
            uart->DR = *tx_ctx.current_char; 
            tx_ctx.current_char++;
        } else {
            // Stop condition: disable the interrupt so it stops firing!
            uart->CR1 &= ~(1 << 7);
        }
    } 
}

int main(void) {


    return 0;
}