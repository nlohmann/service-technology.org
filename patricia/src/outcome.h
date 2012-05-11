#ifndef OUTCOME_H
#define OUTCOME_H

#include <string>
#include <pthread.h>
#include "outcome.h"

using std::string;

/*!
 The outcome variable contains the tool's name and the verification state.
 Since this variable is used in different threads, it is protected with a mutex.
 Furthermore, a condition variable is used for signaling a waiting thread that a new state has arrived.
 */
class Outcome {
public:
	enum State { Positive, Negative, Undefined };

	/// Constructor.
	Outcome(string toolname);

	/// Destructor.
	~Outcome();

	/// Publish a result in this Outcome.
	void publish(enum State result, string tool);

	/// Convert exit code to Outcome result.
	static enum State code2state(int);

	/// Convert Outcome result to exit code.
	static int state2code(enum State result);

    enum State state;
    string tool;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};




#endif
