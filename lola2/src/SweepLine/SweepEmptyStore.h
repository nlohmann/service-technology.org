/*!
\file SweepEmptyStore.h
\author Harro
\status new
\brief Replacement for EmptyStore to count markings and edges (only)

This class is an extension of the class EmptyStore. It never contains
any markings. Its only purpose is to count the markings that may be
distributed over many SweepRingStores.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/Store.h>
#include <Stores/EmptyStore.h>

/*!
\brief Replacement for EmptyStore to count markings and edges

This class is an extension of the class EmptyStore. It never contains
any markings. Its only purpose is to count the markings that may be
distributed over many SweepRingStores.
*/
class SweepEmptyStore : public EmptyStore<void>
{
public:
	SweepEmptyStore();

	void incrementMarkings();
	void incrementCalls();
	void setMarkings(int64_t count);
	void setCalls(int64_t count);
};

