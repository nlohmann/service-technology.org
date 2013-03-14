/*!
\file SweepRingStore.h
\author Harro
\status new
\brief Store for transient markings

Each front in the SweepLine method needs one array of stores for transient markings. 
Since markings need to be retrieved from this array, the only possible encoding
for this array is the FullCopyEncoder. Multithreading is disregarded as at most
one thread can access one store at any time.
*/

#pragma once
#include <Core/Dimensions.h>
#include <SweepLine/SweepStore.h>
#include <SweepLine/SweepListStore.h>
#include <Stores/NetStateEncoder/FullCopyEncoder.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <Net/NetState.h>

/*!
\brief Store for transient markings of the SweepLine method

Each front in the SweepLine method needs one array of stores for transient markings. 
Since markings need to be retrieved from this array, the only possible encoding
for this array is the FullCopyEncoder. Multithreading is disregarded as at most
one thread can access one store at any time.
*/
template <class T>
class SweepRingStore
{
public:
	/// constructor with size of the store and maximal positive progress
	SweepRingStore(index_t _size, index_t _front_offset, index_t _transient_offset, index_t _nr_of_threads = 1);
	/// destructor
	~SweepRingStore();

	/// check if a state is in the store at progress offset, insert it if not
	bool searchAndInsert(NetState& ns, int32_t offset, T** payload, index_t thread);
	/// get a state at active progress and relocate it to swap or permanent storage
	bool popState(NetState& ns, index_t thread);
	/// advance the active progress by one
	bool advanceProgress();
	/// initialise the store with the persistent states
	void init(SweepListStore<T>* oldpstates, SweepListStore<T>* newpstates);
	/// delete all transient states and check if there aren't new persistent ones at all
	bool clear();
	/// check for a new persistent state in the bucket with the lowest progress value
	bool checkNewPersistent();
	/// check if the last inserted state was made persistent
	bool insertedIsNewPersistent(index_t thread);
	/// get the number of deleted transient states during the last progress advance
	int64_t getNumberOfDeletedStates();

	void printState(NetState& ns);
private:
	/// size of the store ring for transient states
	index_t size;
	/// maximal progress of a single transition
	index_t front_offset;
	/// store element for the current progress value
	index_t active;
	/// progress offset at which transient state are forgotten
	index_t transient_offset;
	/// maximal number of threads to access this store simultaneously
	index_t nr_of_threads;
	/// size of the last deleted store bucket
	int64_t deleted_store_size;
	/// encoder for all transient and new persistent states
	FullCopyEncoder* fullencoder;
	/// encoder for old persistent states
	CopyEncoder* sigpencoder;
	/// state counter for transient store
	int64_t* count;
	/// ring of stores for transient states
	SweepStore<T>** store;
	/// swap space for states with computed successors
	SweepStore<T>* samevalue;
	/// connectors from the store ring to the stores of old persistent states with the same progress values
	SweepListStore<T>** oldpersistent;
	/// connectors from the store ring to the stores of new persistent states with the same progress values
	SweepListStore<T>** newpersistent;
	/// flag indicating if the list of new persistent states is empty
	bool new_persistent_empty;
	/// flag indicating whether the last inserted state is persistent or transient
	bool* inserted_persistent;
};

#include <SweepLine/SweepRingStore.inc>
