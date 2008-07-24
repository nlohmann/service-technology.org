#ifndef TINV_H
#define TINV_H

#include "net.H"

#if defined(PREDUCTION) || defined(CYCLE) || defined(STRUCT) || defined(SWEEP)
class summand
{
public:
	Node * var;
	long int value;
	summand * next;
	summand(Node *, long int);
};

class equation
{
public:
	summand * sum;
	void apply();
	void applyunit(); // applies equations where unit matrix is appended
	equation(Node *);
	equation * next;
};

class invariant
{
public:
	summand * support;
};

void tsolve();
void psolve();
void progress_measure();
	

#endif

extern int ProgressSpan;
extern int ZeroProgress;
extern int MonotoneProgress;
#endif
