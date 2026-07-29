#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET   = 1U
} GPIO_PinState;

typedef struct {
    uint8_t raw_state;     // 2-bit state: (PinA_Bit << 1) | PinB_Bit
    int8_t  direction;     // +1 for Clockwise (CW), -1 for Counter-Clockwise (CCW)
    uint32_t step_count;   // Cumulative step number
} EncoderEvent_t;

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

static const int8_t quad_table[16] = {
    0,  // 00 -> 00 (No Change)
    1,  // 00 -> 01 (CW)
   -1,  // 00 -> 10 (CCW)
    0,  // 00 -> 11 (Bounce)
   -1,  // 01 -> 00 (CCW)
    0,  // 01 -> 01 (No Change)
    0,  // 01 -> 10 (Bounce)
    1,  // 01 -> 11 (CW)
    1,  // 10 -> 00 (CW)
    0,  // 10 -> 01 (Bounce)
    0,  // 10 -> 10 (No Change)
   -1,  // 10 -> 11 (CCW)
    0,  // 11 -> 00 (Bounce)
   -1,  // 11 -> 01 (CCW)
    1,  // 11 -> 10 (CW)
    0   // 11 -> 11 (No Change)
};

void decode_encoder_step(GPIO_TypeDef *port, uint16_t pin_a_mask, uint16_t pin_b_mask, uint8_t *last_state, EncoderEvent_t **pool_head, uint32_t *global_step_counter);
void decode_encoder_step(GPIO_TypeDef *port, uint16_t pin_a_mask, uint16_t pin_b_mask, uint8_t *last_state, EncoderEvent_t **pool_head, uint32_t *global_step_counter){
    uint32_t idr_val = port->IDR;

    uint8_t bit_a = (idr_val & pin_a_mask) ? 1 : 0;
    uint8_t bit_b = (idr_val & pin_b_mask) ? 1 : 0;

    uint8_t current_state = (bit_a << 1) | bit_b;

    if (current_state == *last_state) {
        return;
    }

    // Step D: Quadrature Direction Decoding
    // Create index: [Last State (high 2 bits)] [Current State (low 2 bits)]
    uint8_t table_index = (*last_state << 2) | current_state;
    int8_t direction = quad_table[table_index];

    // Handle Invalid/Bounce (0)
    // If direction is 0, it's either a bounce or an invalid double-bit transition
    if (direction == 0) {
        *last_state = current_state; // Update state
        return; // Do not allocate event
    }

    // Step E: Static Double-Pointer Allocation
    // Direction is either +1 or -1 here

    // 1. Increment Global Counter
    (*global_step_counter) += direction;

    // 2. Store data at current pool_head location
    // Double-dereference: *pool_head gives the pointer to the current slot
    (*pool_head)->raw_state = current_state;
    (*pool_head)->direction = direction;
    (*pool_head)->step_count = *global_step_counter;

    // 3. Advance the memory pool pointer using pointer arithmetic
    // Moves the pointer forward by sizeof(EncoderEvent_t) bytes
    (*pool_head)++;

    // 4. Update last_state
    *last_state = current_state;
}


int main(void){
    // 1. Declare Static RAM Pool and Tracking Pointer
    static EncoderEvent_t event_pool[10];
    EncoderEvent_t *pool_ptr = event_pool; // Points to the next free slot

    // 2. Declare State Variables
    uint8_t last_state = 0;
    uint32_t step_cnt = 0;

    // 3. Declare Simulated GPIO Port
    GPIO_TypeDef sim_enc_port = {0};

    // Pin Masks
    const uint16_t PIN_A = (1UL << 0);
    const uint16_t PIN_B = (1UL << 1);

    printf("Starting Clockwise Simulation...\n\n");

    // --- Simulate Clockwise User Turn (4 Successive Transitions) ---

    // Transition 1: 0b00 -> 0b01 (Pin B HIGH)
    // Expected: Dir +1, State 01
    sim_enc_port.IDR = 0x02; 
    decode_encoder_step(&sim_enc_port, PIN_A, PIN_B, &last_state, &pool_ptr, &step_cnt);

    // Transition 2: 0b01 -> 0b11 (Pin A and B HIGH)
    // Expected: Dir +1, State 11
    sim_enc_port.IDR = 0x03; 
    decode_encoder_step(&sim_enc_port, PIN_A, PIN_B, &last_state, &pool_ptr, &step_cnt);

    // Transition 3: 0b11 -> 0b10 (Pin A HIGH)
    // Expected: Dir +1, State 10
    sim_enc_port.IDR = 0x01; 
    decode_encoder_step(&sim_enc_port, PIN_A, PIN_B, &last_state, &pool_ptr, &step_cnt);

    // Transition 4: 0b10 -> 0b00 (Both LOW)
    // Expected: Dir +1, State 00
    sim_enc_port.IDR = 0x00; 
    decode_encoder_step(&sim_enc_port, PIN_A, PIN_B, &last_state, &pool_ptr, &step_cnt);

    // --- Verification Loop ---
    
    printf("Verification Results (Expected 4 steps, Dir: 1):\n");
    printf("------------------------------------------------------------\n");
    
    // Pointer-based loop iterating 4 times
    EncoderEvent_t *reader = event_pool;
    EncoderEvent_t *end_ptr = event_pool + 4; // Calculate end address

    while (reader < end_ptr) {
        printf("Step %lu | Dir: %d | Raw State: 0x%X | RAM Addr: %p\n", 
               reader->step_count, 
               reader->direction, 
               reader->raw_state, 
               (void*)reader);
        
        reader++; // Pointer arithmetic to next struct
    }

    printf("------------------------------------------------------------\n");
    printf("Total Steps Recorded: %lu\n", step_cnt);
    printf("Pool Pointer Advanced By: %lu bytes\n", (unsigned long)((uint8_t*)pool_ptr - (uint8_t*)event_pool));

    return 0;
}