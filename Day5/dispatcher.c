#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef void (*TimerCallback_t)(void);

typedef struct {
    uint32_t interval_ms;
    uint32_t elapsed_ms;
    TimerCallback_t callback;
    uint8_t is_active;
} TimerTask_t;

int register_task(TimerTask_t *pool, uint32_t pool_size, uint32_t interval, TimerCallback_t cb);
int register_task(TimerTask_t *pool, uint32_t pool_size, uint32_t interval, TimerCallback_t cb) {
    for (int x = 0; x < pool_size; x++) {
        if (pool[x].is_active == 0) {
            pool[x].interval_ms = interval; 
            pool[x].callback = cb;          
            pool[x].elapsed_ms = 0;        
            pool[x].is_active = 1;
            return x;
        }
    }
    return -1;
}

void dispatch_tasks(TimerTask_t *pool, uint32_t size);
void dispatch_tasks(TimerTask_t *pool, uint32_t size) {
    for (int x = 0; x < size; x++){
        if(pool[x].is_active == 1){
            pool[x].elapsed_ms += 1;
            if (pool[x].elapsed_ms >= pool[x].interval_ms) {
                pool[x].callback();
                pool[x].elapsed_ms = 0;
            }
        }
    }

}

void read_sensor_cb(void);
void read_sensor_cb(void) {
    printf("-> [TASK] Reading temperature sensor...\n");
}

void heartbeat_led_cb(void);
void heartbeat_led_cb(void) {
    printf("-> [TASK] Heartbeat LED Blink!\n");
}

int main(void){
    TimerTask_t *task = (TimerTask_t*)calloc(3, sizeof(TimerTask_t));
    if (task == NULL) {
        printf("Malloc failed");
        return 0;
    }


    printf("Registering tasks into Heap Pool...\n");
    register_task(task, 3, 3, read_sensor_cb);   // Slot 0: runs every 3 ms
    register_task(task, 3, 5, heartbeat_led_cb); // Slot 1: runs every 5 ms
    printf("Tasks registered! Starting hardware clock simulation...\n\n");

    // --- PHASE 2: THE HARDWARE CLUCK SIMULATION (t = 1 to 15 ms) ---
    for (uint32_t t = 1; t <= 15; t++) {
        printf("[Time: %2u ms]\n", t);
        
        // On every millisecond tick, we simply tell the dispatcher to update the pool.
        // The dispatcher handles all the math and callback firing!
        dispatch_tasks(task, 3);
    }
    
    free(task);

    return 0;
}