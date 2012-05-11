#ifndef ACTION_H
#define ACTION_H

#include <vector>
#include "action.h"
#include "tools.h"
#include "problem.h"
#include "outcome.h"

using std::vector;

/*!
 Class that contains the necessary information for the different kinds of actions.
 Depending on the type an object contains the following information:
 - Parallel: a number of actions
 - Try: two (sequential) actions
 - Run: a tool and a timeout
 */
class Action {
public:

	enum Type { Parallel, Sequence, Run };

	/// Constructor for parallel and sequential actions 
	Action(bool par, vector<Action*>& actions);

	/// Constructor for parallel and sequential actions 
	Action(bool par, Action* act1, Action* act2);

	/// Constructor for a single action
	Action(Tool* tool, int timeout);

	/// Destructor
	~Action();

	/// Main call to perform a (complex) action
	Outcome perform(Problem& problem);

	/// To perform parallel actions
	Outcome performParallel(Problem& problem);

	/// To perform a sequence of actions
	Outcome performSequence(Problem& problem);

	/// To perform a single action
	Outcome performRunWithTimeout(Problem& problem);

	/// To perform a single action
	Outcome performRunWithoutTimeout(Problem& problem);

	void show(int indent);

    enum Type type;
	vector<Action*> actions;
	Tool* tool;
	unsigned int timeout;
};


#endif
