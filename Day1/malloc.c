#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct{
    int messageID;
    float timestamp;
}LogEntry;

void main() {
    int num;
    printf("How much to store?: ");
    scanf("%d", &num);

    LogEntry *arr = (LogEntry*)malloc(num * sizeof(LogEntry));

    if (arr == NULL) {
        printf("Failed");
    }

    for (int x = 0; x < num; x++) {
        arr[x].messageID = x;
        arr[x].timestamp = x * 1.5;

        printf("x: %d, messageID: %d, timestamp: %f \n", x, arr[x].messageID, arr[x].timestamp);

    }

    free(arr);
    
} 