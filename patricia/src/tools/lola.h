#ifndef TOOLS_LOLA_H
#define TOOLS_LOLA_H

#include "problem.h"
#include "tools.h"

class Lola : public Tool {
public:

	int prepare(Problem& problem);
	enum Outcome::State interpret(int code, string logfile);
};

class Findpath : public Lola {
public:

	Findpath(Problem& p);
	~Findpath();
};

class Statepredicate : public Lola {
public:

	Statepredicate(Problem& p);
	~Statepredicate();
};

#endif
