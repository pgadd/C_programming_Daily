#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void stateIdle();
void stateIdle() {
    printf("System is Idle\n");
}

void stateRun();
void stateRun() {
    printf("System is running\n");
}

void stateError();
void stateError() {
    printf("System Error\n");
}

int main(void) {
    void (*arr[3])(void) = {&stateIdle, &stateRun, &stateError};

    for (int x = 0; x < 3; x++) {
        arr[x]();
    }
    
    return 0;
}