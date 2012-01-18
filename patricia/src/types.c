
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "types.h"

/*!
 \param  exit code
 \return Positive on 0, Negative on 1, otherwise Undefined
 
 \note only use this function with exit codes (e.g. by wait)
 */
enum VerificationState code2state(int code) {
    switch (WEXITSTATUS(code)) {
        case  0:
            return Positive;
        case  1:
            return Negative;
        default:
            return Undefined;
    }
}

/*!
 \param  verification state (i.e., Positive or Negative or Undefined)
 \return 0 on Positive, 1 on Negative, 2 on Undefined
 */
int state2code(enum VerificationState state) {
    assert(state == Positive || state == Negative || state == Undefined);

    if (state == Positive) {
        return 0;
    } else if (state == Negative) {
        return 1;
    } else {
        return 2;
    }
}

/*!
 This function extracts the problem information from the arguments passed to the program.
 
 \param number of arguments
 \param vector of arguments
 \return problem struct containing the extracted information
 */
struct problem* mkProblem(int argc, char* argv[]) {
    struct problem* result;

    if (argc < 3) {
        return NULL;
    }

    result = malloc(sizeof(struct problem));
    result->name = argv[1];
    result->net = argv[2];
    result->formula = argv[3];
    return result;
}

/*!
 Writes verification state and the tool's name to the outcome variable.
 That is, a mutex is acquired, the information written and the mutex released.
 Additionally, it notifies waiting threads using the condition variable.
 
 \param mutex protected variable the information is written to
 \param verification state the tool yielded
 \param name of the tool that generated the verification state
 */
void publish(struct outcome* outcome, enum VerificationState result, char* tool) {
    pthread_mutex_lock(&outcome->mutex);
    outcome->state = result;
    outcome->tool = tool;
    pthread_cond_signal(&outcome->cond);
    pthread_mutex_unlock(&outcome->mutex);
}
