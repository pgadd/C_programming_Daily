//State Machine implementation example

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

const Transition_t fsm_table[] = {
    {STATE_IDLE, EVT_COIN_INSERT, STATE_BREWING, &on_start_brew},
    {STATE_BREWING, EVT_BREW_DONE, STATE_IDLE, &on_dispense_done},
    {STATE_BREWING, EVT_OUT_OF_WATER, STATE_FAULT, &on_water_fault},
    {STATE_FAULT, EVT_RESET, STATE_IDLE, &on_system_reset}
};

void send_event(MachineState_t *state_ptr, MachineEvent_t evt, const Transition_t *table, size_t table_size);
void send_event(MachineState_t *state_ptr, MachineEvent_t evt, const Transition_t *table, size_t table_size){
    int states = table_size;

    for (int x = 0; x < states; x++){
        if (table[x].current_state == *state_ptr && table[x].trigger_event == evt){
            table[x].action_cb();

            *state_ptr = table[x].next_state;
            return;
        } 
    }

     printf("-> [WARNING] Event ignored in current state.\n");


}

int main(void){
    MachineState_t myMachine = STATE_IDLE;

    send_event(&myMachine, EVT_COIN_INSERT, fsm_table, 4);
    printf("Current state: %d\n", myMachine);

    send_event(&myMachine, EVT_OUT_OF_WATER, fsm_table, 4);
    printf("Current state: %d, current val: %X\n", myMachine, HW_STATUS_REG);

    send_event(&myMachine, EVT_COIN_INSERT, fsm_table, 4);
    printf("Current state: %d\n", myMachine);

    send_event(&myMachine, EVT_RESET, fsm_table, 4);
    printf("Current state: %d, current val: %X\n", myMachine, HW_STATUS_REG);

    return 0;
}