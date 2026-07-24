#include <stdio.h>
#include <stdint.h>

void main() {
    uint8_t payload[8] = {0xAA, 0x14, 0x22, 0x15, 0x24, 0x14, 0x21, 0xFF};

    uint8_t *point = &payload;
    point++;
    
    for (int x = 0; x < 3; x++) {
        uint16_t temp = *point;
        point++;

        temp = (temp << 8) | *point;
        point++;

        printf("Current value: %X \n", temp);

    }

}