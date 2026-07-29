#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET   = 1U
} GPIO_PinState;

// 2. Hardware Register Overlay (must use volatile!)
typedef struct {
    uint32_t volatile MODER;    // Mode register (2 bits per pin)
    uint32_t volatile OTYPER;   // Output type register
    uint32_t volatile OSPEEDR;  // Output speed register
    uint32_t volatile PUPDR;    // Pull-up/pull-down register (2 bits per pin)
    uint32_t volatile IDR;      // Input data register
    uint32_t volatile ODR;      // Output data register
    uint32_t volatile BSRR;     // Bit Set/Reset register
    uint32_t volatile LCKR;     // Configuration lock register
    uint32_t volatile AFR[2];   // Alternate function registers
} GPIO_TypeDef;

// 3. Standard HAL Initialization Structure
typedef struct {
    uint32_t Pin;  // Bitmask of pins to configure (e.g., (1UL << 5) | (1UL << 12))
    uint32_t Mode; // 0x00 = Input, 0x01 = Output, 0x02 = Alt Func, 0x03 = Analog
    uint32_t Pull; // 0x00 = No Pull, 0x01 = Pull-Up, 0x02 = Pull-Down
} GPIO_InitTypeDef;

typedef struct {
    GPIO_TypeDef *port;     // Pointer to the GPIO port controlling all SPI pins
    uint16_t pin_sck;       // Bitmask for Serial Clock pin (e.g., 1UL << 3)
    uint16_t pin_mosi;      // Bitmask for Master Out Slave In pin (e.g., 1UL << 4)
    uint16_t pin_miso;      // Bitmask for Master In Slave Out pin (e.g., 1UL << 5)
    uint16_t pin_cs;        // Bitmask for Chip Select pin (e.g., 1UL << 2)
    void (*on_transfer_complete)(uint8_t tx_val, uint8_t rx_val);
} SoftSPI_Handle_t;

void CUSTOM_HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void CUSTOM_HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init){
    for (uint8_t pin = 0; pin < 15; pin ++){
        if (GPIO_Init->Pin & (1UL << pin)){
            uint32_t shift = (pin * 2U);

            GPIOx -> MODER &= ~(0b11 << shift);
            GPIOx -> MODER |= (GPIO_Init->Mode << shift);

            GPIOx->PUPDR &= ~(0b11 << shift);
            GPIOx->PUPDR |= (GPIO_Init->Pull << shift);
        }
    }
}

void CUSTOM_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void CUSTOM_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    if (PinState == GPIO_PIN_SET)   {
        GPIOx->BSRR = GPIO_Pin;
    } else if (PinState == GPIO_PIN_RESET){
        GPIOx->BSRR = ((uint32_t)GPIO_Pin << 16);
    }

}

uint8_t soft_spi_transfer_byte(SoftSPI_Handle_t *spi, uint8_t tx_byte);
uint8_t soft_spi_transfer_byte(SoftSPI_Handle_t *spi, uint8_t tx_byte) {
    CUSTOM_HAL_GPIO_WritePin(spi->port, spi->pin_cs, GPIO_PIN_RESET);
    uint8_t tx_byte_original = tx_byte;

    for (uint8_t rx_byte = 0; rx_byte < 8; rx_byte++){
        if (tx_byte & 0x80) {
            CUSTOM_HAL_GPIO_WritePin(spi->port, spi->pin_mosi, GPIO_PIN_SET);
        } else {
            CUSTOM_HAL_GPIO_WritePin(spi->port, spi->pin_mosi, GPIO_PIN_RESET);
        }

        CUSTOM_HAL_GPIO_WritePin(spi->port, spi->pin_sck, GPIO_PIN_SET);

        if (spi->port->IDR & spi->pin_miso) {
            rx_byte |= 0x01;
        }

        CUSTOM_HAL_GPIO_WritePin(spi->port, spi->pin_sck, GPIO_PIN_RESET);

        if (rx_byte == 7) {
            tx_byte <<= 1;
            rx_byte <<= 1;
        }

        CUSTOM_HAL_GPIO_WritePin(spi->port, spi->pin_cs, GPIO_PIN_SET);

        spi->on_transfer_complete(tx_byte_original, rx_byte);

        return rx_byte;
    }

}


void spi_cb(uint8_t tx, uint8_t rx);
void spi_cb(uint8_t tx, uint8_t rx) {
    printf("-> SPI Complete | TX: 0x%02X | RX: 0x%02X", tx, rx);
}

int main(void){
    GPIO_TypeDef sim_spi_port = {0};
    SoftSPI_Handle_t spi;
    spi.pin_cs = (1UL << 2);
    spi.pin_sck = (1UL << 3);
    spi.pin_mosi = (1UL << 4);
    spi.pin_miso = (1UL << 5);

    spi.on_transfer_complete = spi_cb;

    sim_spi_port.IDR = (1UL << 5);

    uint8_t result = soft_spi_transfer_byte(&spi, 0xA5);

    printf("return value: %d", result);

    return 0;
}