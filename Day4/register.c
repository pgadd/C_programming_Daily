#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct {
    uint32_t CR1;
} UART_Hardware_t;


void set_uart_mode(UART_Hardware_t *uart, uint8_t mode_val);
void set_uart_mode(UART_Hardware_t *uart, uint8_t mode_val) {
    uart -> CR1 &= ~(0b111 << 3);
    uart -> CR1 |= (mode_val << 3); 
}

int main(void){
    UART_Hardware_t uart;
    uart.CR1 = 0xFFFFFFFF;

    printf("Value of CR1: %X\n", uart.CR1);

    set_uart_mode(&uart, 2);

    printf("Value of CR1: %X\n", uart.CR1);

    return 0;
}