//You have a battery voltage sensor on Channel 1, a temperature sensor on Channel 5, and a light sensor on Channel 8. You must configure the ADC's Sequence Registers (SQR) to scan them in order, polling the EOC (End of Conversion) flag to safely extract the data before it gets overwritten.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t volatile SR;    // Status Reg (Bit 1 = EOC: End of conversion)
    uint32_t volatile CR2;   // Control Reg 2 (Bit 0 = ADON: ADC ON, Bit 30 = SWSTART: Start)
    uint32_t volatile SQR1;  // Sequence Reg 1 (Bits 23:20 = L (Length of sequence minus 1))
    uint32_t volatile SQR3;  // Sequence Reg 3 (Bits 4:0 = 1st conversion, Bits 9:5 = 2nd, Bits 14:10 = 3rd)
    uint32_t volatile DR;    // Data Reg (Lower 16 bits contain the ADC result)
} ADC_TypeDef;

typedef struct {
    uint16_t v_batt;
    uint16_t temp;
    uint16_t light;
} SensorData_t;

void read_sensor_sequence(ADC_TypeDef *adc, SensorData_t *output_struct);
void read_sensor_sequence(ADC_TypeDef *adc, SensorData_t *output_struct){
    adc->SQR1 |= (2 << 20);

    adc->SQR3 |= (1UL) | (5UL << 5) | (8UL << 10);

    adc -> CR2 |= (1);
    adc -> CR2 |= (1 << 30);

    uint16_t *dest_ptr = (uint16_t*)output_struct;
    for (int x = 0; x < 3; x++) {
        while (!(adc->SR & (1 << 1))) {
            // Blocking wait for EOC flag
        }

        *dest_ptr = (uint16_t)adc->DR;
        dest_ptr++;
    }

}


int main(void){
    ADC_TypeDef sim_adc = {0};
    SensorData_t my_sensors = {0};

    read_sensor_sequence(&sim_adc, &my_sensors);
    return 0;
}