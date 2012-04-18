/*!
\file Deadlock.cc
\author Karsten
\status approved 18.04.2012

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
