/*!
\file Deadlock.cc
\author Karsten
\status new

\brief Evaluates deadlocks 
*/

#include <cstring>
#include <cstdio>
#include "Net/Transition.h"
#include "Exploration/Deadlock.h"


void Deadlock::initProperty()
{
	value = false;
}

void Deadlock::checkProperty(index_t t)
{
	value = !Transition::CardEnabled;
}
