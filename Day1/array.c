#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


void analyzeBuffer(uint8_t *buffer);

void analyzeBuffer(uint8_t *buffer) {
    int x = (int)sizeof(buffer);
    printf("Sizeof Buffer: %d", x);
}

void main(){
    uint8_t tx_buffer[256];

    int x = (int)sizeof(tx_buffer);
    printf("main Size of buffer: %d\n", x);

    //When an array is passed through another function, it just shows up as pointer, so the sizeof operator would return the size of the pointer instead of the size of the array.
    analyzeBuffer(tx_buffer);

}