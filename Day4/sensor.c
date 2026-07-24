#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
    
    uint8_t *data = (uint8_t*)malloc(5);

    if (data == NULL){
        printf("Malloc error");
        return 0;
    }

    data[0] = 0x01;
    data[1] = 0x89;
    data[2] = 0x00;
    data[3] = 0x09;
    data[4] = 0x80;

    uint8_t *ptr = data;

    for (int i = 0; i < 5; i++){
        const char *online;
        const char *battery;
        const char *overheat;

        if (*ptr & (1 << 0)){
            online = "ONLINE";
        } else {
            online = "OFFLINE";
        }

        if (*ptr & (1 << 3)) {
            battery = "Battery LOW";
        } else {
            battery = "Battery OK";
        }

        if (*ptr & (1 << 7)) {
            overheat = "OVERHEAT FAULT";
        } else {
            overheat = "Temp NORMAL";
        }

        printf("Sensor %d, value [%X]:  %s | %s | %s \n", i, *ptr, online, battery, overheat);

        ptr++;

    }

    free (data);

    return 0;
}