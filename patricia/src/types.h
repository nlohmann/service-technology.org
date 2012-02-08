#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>

/**********************
 * VERIFICATION STATE *
 *********************/

/*!
 There are three verification states:
 - Positive: A positive state indicates that the formula is fulfilled.
 - Negative: A negative state indicates that the formula is infeasible.
 - Undefined: This state signifies that the problem could not be solved.
 */
enum VerificationState { Positive, Negative, Undefined };

enum VerificationState code2state(int code);
int state2code(enum VerificationState state);


/***********
 * PROBLEM *
 **********/

/*!
 A problem consists of a formula and a net on which the formula is verified.
 Additionally, a problem has a name, which is used for
 */
struct problem {
    char* name;
    char* net;
    char* formula;
};

struct problem* mkProblem(int argc, char* argv[]);

/***********
 * OUTCOME *
 **********/

/*!
 The outcome variable contains the tool's name and the verification state.
 Since this variable is used in different threads, it is protected with a mutex.
 Furthermore, a condition variable is used for signaling a waiting thread that a new state has arrived.
 */
struct outcome {
    enum VerificationState state;
    char* tool;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void publish(struct outcome* outcome, enum VerificationState result, char* tool);



#endif
