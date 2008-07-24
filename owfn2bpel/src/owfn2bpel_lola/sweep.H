#ifndef SWEEP_H
#define SWEEP_H

#ifdef SWEEP
class pendinglist
{
public:
	binDecision * content;
	long int value; // progress_value;
	pendinglist * next;  
	int size;
};

int sweep();
#endif
#endif
