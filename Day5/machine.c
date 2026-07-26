#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    STATE_IDLE,
    STATE_BREWING,
    STATE_FAULT,
} MachineState_t;

typedef enum {
    EVT_COIN_INSERT, 
    EVT_BREW_DONE, 
    EVT_OUT_OF_WATER, 
    EVT_RESET
} MachineEvent_t;

uint32_t HW_STATUS_REG = 0x00000000;

void on_start_brew(void);
void on_start_brew(void){
    printf("Grinding beans and boiling water...\n");
}

void on_dispense_done(void);
void on_dispense_done(void){
    printf("Coffee ready! Returning to idle.\n");
}

void on_water_fault(void);
void on_water_fault(void){
    printf("CRITICAL: Out of water!\n");
    HW_STATUS_REG |= (1 << 7);
}

typedef struct {
    MachineState_t current_state;
    MachineEvent_t trigger_event;
    MachineState_t next_state;
    void (*action_cb)(void);
} Transition_t;

void on_system_reset(void);
void on_system_reset(void){
    printf("System reset. Clearing alarms.\n");
    HW_STATUS_REG &= ~(1 << 7);
}

int main(void){


    return 0;
}