/**
  *  Worg
  */
#include <stdio.h>
#include <stdlib.h>

#define MIN_PID 300
#define MAX_PID 5000

char* bitmap;

int allocate_map(void) {
    bitmap = malloc(MAX_PID - MIN_PID + 1);
    if (bitmap == NULL) {
        return -1;
    }

    for (int pid = MIN_PID; pid <= MAX_PID; pid++) {
        bitmap[pid - MIN_PID] = 0;
    }
    return 1;
}

int allocate_pid(void) {
    for (int pid = MIN_PID; pid <= MAX_PID; pid++) {
        if (!bitmap[pid - MIN_PID]) {  //  found a free one
            bitmap[pid - MIN_PID] = 1;
            return pid;
        }
    }

    return -1;
}

void release_pid(int pid) {
    bitmap[pid - MIN_PID] = 0;
}

int main(int argc, char **argv) {
    return 0;
}
