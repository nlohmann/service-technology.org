#ifndef TOOLS_H
#define TOOLS_H

#include "outcome.h"
#include "problem.h"

/*!
 - name: the tool's name
 - prepare: function for preparing a problem
 - interpret: function for interpreting the tool's result
 - program: array containing the system call
 */
class Tool {
public:

	virtual ~Tool();

    string name;
	virtual int prepare(Problem& p) = 0;
	virtual enum Outcome::State interpret(int code, string file) = 0;
    char** program;

	enum Outcome::State forknrun(char** args, string logfile);
	enum Outcome::State run(Problem& problem);
};


#endif
