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
	TransitionInv() : u(NULL) {}
	void solve();
	
	bool *u;
};

#endif /* INVARIANTS_H */
