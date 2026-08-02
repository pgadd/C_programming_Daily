//Even in RTOS systems, we often need a blocking print function for fatal error handlers (HardFaults) where interrupts are disabled and the system is crashing. You must iterate through a null-terminated C string using a pointer and block the CPU until every byte is sent.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

void uart_print_blocking(UART_TypeDef *uart, const char *str);
void uart_print_blocking(UART_TypeDef *uart, const char *str) {
    while (*str != '\0'){
        while (!(uart -> SR & (1 << 7))) {

        }

        uart -> DR = (uint32_t)*str;

        str++;

    }

}

int main(void){
    UART_TypeDef sim_uart = { .SR = (1 << 7) };

    uart_print_blocking(&sim_uart, "FATAL ERROR\n");

    printf("str: %X", sim_uart.DR);

    return 0;
}