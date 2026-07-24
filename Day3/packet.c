#include <stdio.h>
#include <stdlib.h>

struct GPS_Packet {
    uint16_t header;
    float latitude;
    uint8_t satellites_locked;
};

int main(void){

    uint8_t rx_buffer[7] = {0xAA, 0xAA, 0x00, 0x00, 0x16, 0x42, 0x08};

    struct GPS_Packet myPacket;

    uint8_t *pnt = &rx_buffer;

    myPacket.header = *pnt;

    pnt++;

    myPacket.header = (myPacket.header<<8) | *pnt;

    pnt++; 

    uint8_t *lat_ptr = (uint8_t*)&myPacket.latitude;
    for (int i = 0; i < 4; i++) {
        *lat_ptr = *pnt;
        lat_ptr++;
        pnt++;
    }

    myPacket.satellites_locked = *pnt;


    printf("Memory address: %p, header value: %d \n",  &myPacket.header, myPacket.header);

    printf("Memory address: %p, latitude value: %f \n",  &myPacket.latitude, myPacket.latitude);

    printf("Memory address: %p, satellite value: %d \n",  &myPacket.satellites_locked, myPacket.satellites_locked);

    return 0;
}