#ifndef TOOLS_LOLA2_H
#define TOOLS_LOLA2_H

#include "problem.h"
#include "tools.h"

class Lola2 : public Tool {
public:

	int prepare(Problem& problem);
	enum Outcome::State interpret(int code, string logfile);
};

class Findpath2 : public Lola2 {
public:

	Findpath2(Problem& p);
	~Findpath2();
};

class Statepredicate2 : public Lola2 {
public:

	Statepredicate2(Problem& p);
	~Statepredicate2();
};

#endif
