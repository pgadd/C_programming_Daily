#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void initDisplay(uint8_t **frameBuffer);
void initDisplay(uint8_t **frameBuffer) {
    *frameBuffer = (uint8_t*)malloc(1024);

    if (*frameBuffer == NULL){
        printf("malloc Failed");
        return;
    }

    (*frameBuffer)[1] = 0xFF;
}

int main(void) {
    uint8_t *screen = NULL;
    initDisplay(&screen);
    printf("First value: %X", screen[1]);

    free(screen);
    return 0;
    
}