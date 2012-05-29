/*!
\file FalsePredicate.cc
\author Karsten
\status new

\brief derives constant predicate TRUE
*/

#pragma once

#include <Core/Dimensions.h>
#include <Formula/FalsePredicate.h>

FalsePredicate::FalsePredicate()
{
	value = false;
}

index_t FalsePredicate::getUpSet(index_t* stack, bool* onstack)
{
	return 0;
}

index_t FalsePredicate::countAtomic()
{
	return 0;
}

index_t FalsePredicate::collectAtomic(AtomicStatePredicate**)
{
	return 0;
}
