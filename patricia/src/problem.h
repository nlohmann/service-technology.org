#ifndef PROBLEM_H
#define PROBLEM_H

#include <string>
#include <pthread.h>
//#include "action.h"

using std::string;

class Action;

/*!
 A problem consists of a formula and a net on which the formula is verified.
 Additionally, a problem has a name, which is used for
 */
class Problem {
public:

	/// Constructor.
	Problem(int argc, char* argv[]);

	/// Transforms a problem to an action.
	Action* plan();

    char* name;
    char* net;
    char* formula;
};

#endif
