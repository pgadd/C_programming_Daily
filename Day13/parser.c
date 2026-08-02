//Your MCU is receiving commands from a Bluetooth module. You must read bytes one by one in the UART Interrupt Service Routine (ISR) and store them in a static RAM buffer. When you receive a newline character ('\n'), you must trigger a callback to process the command.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    char *buffer_start;     // Start of static memory
    char *head;             // Current write position
    uint16_t buffer_length; // Max size of buffer
    void (*on_cmd_ready)(char *cmd_string);
} UART_RX_Context_t;

static UART_RX_Context_t rx_ctx;

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

void UART_RX_IRQHandler(UART_TypeDef *uart);
void UART_RX_IRQHandler(UART_TypeDef *uart) {
    if (uart -> SR & (1 << 5)){
        rx_ctx.head = uart -> DR;

        if (*rx_ctx.head == '\n'){
            uart -> DR = '\0';
            rx_ctx.on_cmd_ready(rx_ctx.buffer_start);

            rx_ctx.head = rx_ctx.buffer_start;
            return;
        }

        rx_ctx.head++;
    }

    uint16_t *end_addr = rx_ctx.buffer_length + rx_ctx.buffer_start;

    if (rx_ctx.head == end_addr) {

        rx_ctx.head = rx_ctx.buffer_start;
    }

}

int main(void){


    return 0;
}

