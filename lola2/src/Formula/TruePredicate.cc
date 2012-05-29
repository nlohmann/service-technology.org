/*!
\file TruePredicate.cc
\author Karsten
\status new

\brief derives constant predicate TRUE
*/

#pragma once

#include <Core/Dimensions.h>
#include <Formula/TruePredicate.h>

TruePredicate::TruePredicate()
{
	value = true;
}

index_t TruePredicate::getUpSet(index_t* stack, bool* onstack)
{
	return 0;
}

index_t TruePredicate::countAtomic()
{
	return 0;
}

index_t TruePredicate::collectAtomic(AtomicStatePredicate**)
{
	return 0;
}
