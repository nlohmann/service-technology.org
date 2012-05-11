
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "outcome.h"


/*!
	Constructs an Outcome object for a tool and initializes it.

	\param toolname The tool for which the outcome is created.
*/
Outcome::Outcome(string toolname) : tool(toolname), state(Outcome::Undefined) {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

/*!
	Destructor.
*/
Outcome::~Outcome() {
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

/*!
 Writes verification state and the tool's name to the outcome variable.
 That is, a mutex is acquired, the information written and the mutex released.
 Additionally, it notifies waiting threads using the condition variable.

 \param verification state the tool yielded
 \param name of the tool that generated the verification state
 */
void Outcome::publish(enum State result, string toolname) {
    pthread_mutex_lock(&mutex);
    if (state == Outcome::Undefined) { // do not overwrite a previous result
		state = result;
	    tool = toolname;
	}
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

/*!
 \param  exit code
 \return Positive on 0, Negative on 1, otherwise Undefined

 \note only use this function with exit codes (e.g. by wait)
 */
enum Outcome::State Outcome::code2state(int code) {
    switch (WEXITSTATUS(code)) {
        case  0:
            return Outcome::Positive;
        case  1:
            return Outcome::Negative;
        default:
            return Outcome::Undefined;
    }
}

/*!
 \param  verification state (i.e., Positive or Negative or Undefined)
 \return 0 on Positive, 1 on Negative, 2 on Undefined
 */
int Outcome::state2code(enum Outcome::State state) {
    assert(state == Positive || state == Negative || state == Undefined);

    if (state == Positive) {
        return 0;
    } else if (state == Negative) {
        return 1;
    } else {
        return 2;
    }
}

