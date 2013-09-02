/*!
\author Sascha Lamp
\file Invariants.h
\status new
*/

#ifndef INVARIANTS_H
#define INVARIANTS_H

#include <Core/Dimensions.h>
#include <cstdlib>

class TransitionInv {
public:
	TransitionInv() : u(NULL), using_k(false) {}
	void solve();
	
	bool *u;
	int k;
	bool using_k;
};

#endif /* INVARIANTS_H */
