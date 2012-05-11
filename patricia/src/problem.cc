#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "verbose.h"
#include "cmdline.h"
#include "problem.h"
#include "action.h"
#include "tools/sara.h"
#include "tools/lola.h"

using std::cerr;
using std::endl;

extern gengetopt_args_info args_info;

/*!
 This constructor extracts the problem information from the arguments passed to the program.

 \param number of arguments
 \param vector of arguments
 */
Problem::Problem(int argc, char* argv[]) {
    if (argc < 2) abort(4,"Too few arguments: patricia <name> <net> [formula]");

    name = argv[0];
    net = argv[1];

    if (argc == 3) {
		formula = argv[2];
		status("task files will be prepared");
	} else {
		formula = NULL;
		status("task files are expected to exist");
	}
}

/*!
 This function returns an action that solves the given problem.

 \return action solving the problem
 */
Action* Problem::plan() {
	// note: the Tool/Action objects created below by new are deleted by the destructor Action::~Action()
	Action *findpath, *sara, *statepredicate, *par, *result;

	/* Existing plans so far:
		- SL:  Call Sara and Lola-Statepredicate concurrently.
		- FSL: Call Lola-Findpath first. If no solution is found, continue as in SL.
		If new plans are added, their name must also be added to the enum list found
		under the option -p (--plan) in cmdline.ggo. */
	switch (args_info.plan_arg) {
		case plan_arg_SL:
		    sara = new Action(new Sara(*this), 0);
		    statepredicate = new Action(new Statepredicate(*this), 0);
			result = new Action(true, sara, statepredicate);
			break;

		case plan_arg_FSL:
		default:
		    findpath = new Action(new Findpath(*this), 200);
		    sara = new Action(new Sara(*this), 0);
		    statepredicate = new Action(new Statepredicate(*this), 0);
			par = new Action(true, sara, statepredicate);
			result = new Action(false, findpath, par);
	}
	return result;
}

