
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "utils.h"

/*!
 \param first string
 \param second string
 \return newly created string consisting of the first and second argument

 \note the combined length of the strings passed must not exceed 255.
 \note the created string is not longer than 255 chars.
 */
char* append(char* s1, char* s2) {
    assert(strlen(s1) + strlen(s2) < 256);
    char* s = malloc(sizeof(char) * 256);
    strcpy(s, s1);
    strcat(s, s2);
    return s;
}

/*!
 Kills the process with the given pid.

 \param argument castable to (pid_t*)
 */
void killProcess(void* arg) {
    int status;
    pid_t* pid = (pid_t*)arg;
    debug_print("%s: killing %d\n", __func__, *pid);
    kill(*pid, SIGKILL);
    waitpid(*pid, &status, WNOHANG); // avoid zombies by requesting the exit status
}

/*!
 Kills a thread with the given thread id.

 \param argument castable to (pthread_t*).
 */
void killThread(void* arg) {
    pthread_t* tid = (pthread_t*)arg;
    debug_print("%s: canceling %lu\n", __func__, (unsigned long)*tid);
    pthread_cancel(*tid);
}

/*!
 Kills threads with the given thread ids.
 Pointer arithmetic is used to obtain the thread ids.

 \param argument castable to (pthread_t*).

 \note the behaviour is unspecified when the array of thread ids is not zero-terminated.
 */
void killThreads(void* arg) {
    pthread_t* tid;

    for (tid = (pthread_t*)arg; *tid != 0; tid++) {
        debug_print("%s: canceling %lu\n", __func__, (unsigned long)*tid);
        pthread_cancel(*tid);
    }
}
