
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "utils.h"

char* append(char* s1, char* s2) {
    char* s = malloc(sizeof(char) * 256);
    strcpy(s, s1);
    strcat(s, s2);
    return s;
}

void killProcess(void* arg) {
    pid_t* pid = (pid_t*)arg;
    debug_print("%s: killing %d\n", __func__, *pid);
    kill(*pid, SIGKILL);
}

void killThread(void* arg) {
    pthread_t* tid = (pthread_t*)arg;
    debug_print("%s: canceling %lu\n", __func__, (unsigned long)*tid);
    pthread_cancel(*tid);
}

void killThreads(void* arg) {
    pthread_t* tid;

    for (tid = (pthread_t*)arg; *tid != 0; tid++) {
        debug_print("%s: canceling %lu\n", __func__, (unsigned long)*tid);
        pthread_cancel(*tid);
    }
}
