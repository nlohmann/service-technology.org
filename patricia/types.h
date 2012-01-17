#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>

enum VerificationState { Positive, Negative, Undefined };

struct problem {
    char* name;
    char* net;
    char* formula;
};

enum VerificationState code2state(int code);
int state2code(enum VerificationState state);
struct problem* mkProblem(int argc, char* argv[]);

struct outcome {
    enum VerificationState state;
    char* tool;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};
void publish(struct outcome* outcome, enum VerificationState result, char* tool);

#endif
