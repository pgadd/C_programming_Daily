
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

void CUSTOM_HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);

void CUSTOM_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
