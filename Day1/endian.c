#include <stdio.h>
#include <stdint.h>

uint32_t regValue = 0xDEADBEEF;
uint8_t *point = (uint8_t*)&regValue;

int i = 0;

void main() {
    for(i; i < 4; i++) {
        printf("Current address: %p, Current address: %X \n", (void*)point, *point);
        point++;
    }
}