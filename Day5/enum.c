#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    CMD_GET_TEMP = 0x01,
    CMD_SET_LED  = 0x02,
    CMD_SYS_RESET = 0x03,
    CMD_UNKNOWN  = 0xFF
} CommandType_t;

typedef struct {
    CommandType_t cmd;
    uint16_t payload;
    uint8_t crc;
} ProtocolPacket_t;

CommandType_t parse_packet(uint8_t *raw_stream, ProtocolPacket_t *out_pkt);
CommandType_t parse_packet(uint8_t *raw_stream, ProtocolPacket_t *out_pkt) {
    uint8_t *ptr = raw_stream;

    if (*ptr != 0xAA) {
        return CMD_UNKNOWN;
    }

    ptr++;
    out_pkt -> cmd = *ptr;

    ptr++;
    uint16_t pay = *ptr;
    ptr++;
    pay = (pay << 8) | (*ptr);
    out_pkt->payload = pay;

    ptr++;
    out_pkt->crc = *ptr;

    return out_pkt->cmd;

}


int main(void) {
    uint8_t rx_buffer[5] = {0xAA, 0x02, 0x04, 0xD2, 0x88};
    ProtocolPacket_t *out_pkt;

    parse_packet(&rx_buffer, out_pkt);

    printf("Command ID: %d, payload: %d, CRC: %X", out_pkt->cmd, out_pkt->payload, out_pkt->crc);

    return 0;
}