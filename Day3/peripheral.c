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
    *configPtr = (SPI_Config_t*)malloc(sizeof(SPI_Config_t));

    if(*configPtr == NULL) {
        printf("Malloc failed");
        return;
    }

    SPI_Config_t *cfg = *configPtr;
    cfg -> clock_speed_hz = 1000000;
    cfg -> mode =  0;
    cfg -> error_handler = on_error_callback;

}

int main(void) {
    SPI_Config_t *spi1 = NULL;
    init_SPI_Driver(&spi1);

    if (spi1 != NULL) {
        printf("SPI Driver Initialized! Clock: %u Hz, Mode: %d\n", 
               spi1->clock_speed_hz, spi1->mode);

        printf("Simulating communication fault...\n");
        spi1->error_handler();

        free(spi1);
        spi1 = NULL;
    }

    return 0;
}