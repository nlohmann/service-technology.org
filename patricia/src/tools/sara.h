#ifndef TOOLS_SARA_H
#define TOOLS_SARA_H

#include "problem.h"
#include "tools.h"

class Sara : public Tool {
public:

	Sara(Problem&);
	~Sara();
	int prepare(Problem&);
	enum Outcome::State interpret(int code, string logfile);
};

#endif
