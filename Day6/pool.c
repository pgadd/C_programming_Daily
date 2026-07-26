#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define POOL_BLOCKS 4
#define BLOCK_BYTES 32

typedef struct {
    uint8_t payload[BLOCK_BYTES]; // 32 bytes of raw storage
    uint8_t is_allocated;         // 0 = Free, 1 = In Use
} MemoryBlock_t;

static MemoryBlock_t firmware_pool[POOL_BLOCKS];

void* pool_alloc(void);
void* pool_alloc(void) {
    for (int i = 0; i < 4; i++) {
        if (firmware_pool[i].is_allocated == 0) {
            firmware_pool[i].is_allocated = 1;

            return firmware_pool[i].payload;
        }
    }
    printf("-> [CRITICAL] Memory pool exhausted!\n");
    return NULL;

}

void pool_free(void *ptr);
void pool_free(void *ptr){
    if (ptr == NULL){
        return;
    }

    for (int i = 0; i < POOL_BLOCKS; i++){
        if (ptr == firmware_pool[i].payload){
            firmware_pool[i].is_allocated = 0;
            return;
        }
    }

}

int main(void) {
    uint8_t* buf1 = pool_alloc();
    uint8_t* buf2 = pool_alloc();
    uint8_t* buf3 = pool_alloc();
    uint8_t* buf4 = pool_alloc();

    uint8_t* buf5 = pool_alloc();

    buf2[0] = 'A';
    buf2[1] = 'R';
    buf2[2] = 'M';
    buf2[3] = '\0';
    printf("Buffer 2 owns RAM Address %p and contains string: '%s'\n", (void*)buf2, buf2);

    pool_free(buf2);

    uint8_t* buf6 = pool_alloc();
    printf("mem allocated: %p\n", (void*)buf6);

    return 0;

}