/*!
\file Sweep.h
\author Harro
\status new
\brief Global data for SweepLine method

All general data used in the SweepLine method can be found here. 
*/

#pragma once
#include <Core/Dimensions.h>
#include <Exploration/SimpleProperty.h>
#include <Net/NetState.h>
#include <Exploration/Firelist.h>
#include <SweepLine/SweepListStore.h>
#include <SweepLine/SweepRingStore.h>
#include <SweepLine/SweepEmptyStore.h>
#include <semaphore.h>

/*!
\brief Collection of information related SweepLine method

All general data used in the SweepLine method can be found here. 
*/
template <class T>
class Sweep
{
public:
	/// Constructor
	Sweep(SimpleProperty& property, NetState& ns, SweepEmptyStore& st, Firelist& firelist, index_t number_of_fronts);
	/// Destructor
	~Sweep();
	/// Run sweepline method
	bool run();
	/// Run sweepline method multithreaded
	bool runThreads();

private:
/// transfer struct for the start of a parallel search thread
struct tpSweepArguments {
	/// the initial net state (first thread only)
	NetState* ns;
	/// Semaphore in front of this thread
	sem_t* frontSemaphore;
	/// Semaphore behind this thread
	sem_t* backSemaphore;
	/// flag indicating whether this thread is the front thread
	bool* frontrunner;
	/// the number of the current thread
	int threadNumber;
	/// the total number of threads
	int number_of_threads;
	/// number of transient states in the local store
	int64_t* transient_count;
	/// number of persistent states added by the local store
	int64_t* persistent_count;
	/// number of calls to the local store (edges)
	int64_t* call_count;
	/// the SweepLine object with further parameters
	Sweep<T>* sweep;
};

	/// starting point for the sweepline method (single- or multithreaded)
	static void* threadSweep(void* container);
	/// initialise the sweepline front for a thread
	bool initFront(tpSweepArguments& args, SweepRingStore<T>& store, int32_t& back_progress, int32_t& sem_countdown);
	/// run the front until the end, return a state fulfilling the property or NULL if none
	NetState* runFront(SweepRingStore<T>& store, Firelist& fl, SimpleProperty& sp, tpSweepArguments& args, int32_t& bp, int32_t& semcnt);
	/// finalise the front
	void deinitFront(SweepRingStore<T>& store, tpSweepArguments& args);
	/// check all states with the current progress measure and calculate their successors
	bool checkStates(SweepRingStore<T>& store, Firelist& fl, SimpleProperty& sp, NetState& ns, tpSweepArguments& args);
	/// extend the lists of persistent states to before the minimal progress measure
	void extendLeft(int32_t& bp);
	/// update enabledness information for the current state if the marking has been replaced
	static void updateState(NetState& ns);
	/// forward the state count to the dummy store for later printing
	void forwardStateCount();

	/// the number of fronts running over the search space
	index_t nr_of_fronts;
	/// the property to check for
	SimpleProperty& prop;
	/// the initial state of the net
	NetState& start;
	/// the firelist for the property
	Firelist& fl;
	/// Dummy store for counting markings and calls
	SweepEmptyStore& count_store;
	/// the list of new persistent states (starting point, minimal progress measure)
	SweepListStore<T>* start_new_persistent;
	/// the list of old persistent states (starting point, minimal progress measure)
	SweepListStore<T>* start_old_persistent;
	/// counter for persistent states
	int64_t*** persistent_count;
	/// counter for transient states
	int64_t*** transient_count;
	/// counter for edges (calls)
	int64_t*** call_count;
	/// maximal number of transient states
	int64_t max_transient_count;
	/// the number of buckets the store for transient states must contain
	index_t store_size;
	/// the maximal progress measure of a single transition
	index_t front_offset;
	/// a flag telling all threads to terminate
	bool exit;
};

#include <SweepLine/Sweep.inc>
