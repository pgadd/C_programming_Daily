#include <stdio.h>
#include <stdlib.h>


void on_error_callback(void);
void on_error_callback(void){
    printf("CRITICAL: Peripheral Communication Failure!\n");
}


typedef struct { 
    uint32_t clock_speed_hz;
    uint8_t mode;
    void (*error_handler)(void);
} SPI_Config_t;

void init_SPI_Driver(SPI_Config_t **configPtr);
void init_SPI_Driver(SPI_Config_t **configPtr){
    
}

int main(void) {


    return 0;
}