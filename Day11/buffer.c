//ADC readings are noisy. A single reading might spike falsely. We need a moving average filter. Division is extremely slow on Cortex-M0 (RP2040) because it lacks a hardware divider. We must average exactly $8$ samples so we can use bitwise right-shifts (>> 3) instead of the / operator.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint16_t *buffer_start; // Pointer to start of a statically allocated array of 8 uint16_t
    uint16_t *head;         // Tracks where to insert the next sample
    uint32_t running_sum;   // Maintains the sum of the 8 samples
    uint8_t count;          // How many samples we've collected (up to 8)
    void (*on_threshold_exceeded)(uint16_t avg_val); // Callback
} ADC_Filter_t;


void process_adc_sample(ADC_Filter_t *filter, uint16_t new_sample);
void process_adc_sample(ADC_Filter_t *filter, uint16_t new_sample){
    filter->running_sum = (uint32_t)(filter->running_sum - *filter->head);

    filter->running_sum += (uint32_t)new_sample;
    *(filter->head) = new_sample;

    filter->head++;
    if(filter->head == filter->buffer_start + 8){
        filter->head = filter->buffer_start;
    }

    if (filter->count < 8) {
        filter->count++;
    }


    if(filter -> count == 8){
        if((filter -> running_sum >> 3) > 3000) {
            filter -> on_threshold_exceeded(filter -> running_sum >> 3);
        }
    }

}

void callback(uint16_t avg_val);
void callback(uint16_t avg_val){
    printf("[ALERT] Threshold Exceeded! Avg: %d\n", avg_val);
}


int main(void){
    uint16_t raw_ram[8] = {0};
    ADC_Filter_t filter = {0};

    filter.buffer_start = raw_ram;
    filter.head = raw_ram;
    filter.on_threshold_exceeded = callback;

    uint16_t data[10] = {2000, 2100, 2050, 4000, 4050, 4095, 3900, 4000, 4000, 4050};
    uint16_t *pnt = data;

    for (int x = 0; x < 10; x++){
        process_adc_sample(&filter, *pnt);
        pnt++;
    }

    return 0;
}