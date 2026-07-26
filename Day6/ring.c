#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BUFFER_SIZE 8

typedef struct {
    uint8_t buffer[BUFFER_SIZE]; 
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} RingBuffer_t;

void rb_init(RingBuffer_t *rb);
void rb_init(RingBuffer_t *rb){
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

int rb_push(RingBuffer_t *rb, uint8_t data);
int rb_push(RingBuffer_t *rb, uint8_t data) {
    if (rb->count == BUFFER_SIZE) {
        printf("-> [OVERFLOW] Buffer full! Dropped byte: 0x%X\n", data);
        return -1;
    } 
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % BUFFER_SIZE;
    rb->count++;

    return 0;
}

int rb_pop(RingBuffer_t *rb, uint8_t *data_out);
int rb_pop(RingBuffer_t *rb, uint8_t *data_out){
    if (rb->count == 0) {
        return -1;
    }

    *data_out = rb->buffer[rb->tail];

    rb->tail = (rb->tail + 1) % BUFFER_SIZE;

    rb->count--;

    return 0;
}

int main(void) {
    RingBuffer_t buffer;
    rb_init(&buffer);

    rb_push(&buffer, 0xA1);
    rb_push(&buffer, 0xA2);
    rb_push(&buffer, 0xA3);
    rb_push(&buffer, 0xA4);
    rb_push(&buffer, 0xA5);
    rb_push(&buffer, 0xA6);
    rb_push(&buffer, 0xA7);
    rb_push(&buffer, 0xA8);
    printf("Count: %d\n", buffer.count);

    rb_push(&buffer, 0xA9);

    const uint8_t *out = 0;
    rb_pop(&buffer, &out);
    printf("data dropped: %X\n", out);
    rb_pop(&buffer, &out);
    printf("data dropped: %X\n", out);
    rb_pop(&buffer, &out);
    printf("data dropped: %X\n", out);

    rb_push(&buffer, 0xB1);
    rb_push(&buffer, 0xB2);

    printf("Count: %d\n", buffer.count);

    return 0;
}