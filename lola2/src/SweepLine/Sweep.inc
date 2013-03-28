/*
\file Sweep.cc
\author Harro
\status new

\brief basic routines for SweepLine method
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <map>
#include <cmdline.h>
#include <pthread.h>
#include <SweepLine/Sweep.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <InputOutput/Reporter.h>
#include <iostream>
#include <Stores/NetStateEncoder/BitEncoder.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <Stores/NetStateEncoder/FullCopyEncoder.h>
#include <Stores/NetStateEncoder/SimpleCompressedEncoder.h>
#include <Net/Marking.h>

using std::cout;
using std::endl;

extern gengetopt_args_info args_info;
extern Reporter* rep;

/*!
 * \brief Constructor for the sweepline method
 * \param property The property to check for
 * \param ns The initial state of the net
 * \param firelist A firelist applicable to the property
 * \param number_of_fronts The number of fronts running over the state space simultaneously.
 *        The number of threads created may be any multiple of this.
 *        (At this time the number is the same.)
 */
template <class T>
Sweep<T>::Sweep(SimpleProperty& property, NetState& ns, SweepEmptyStore& st, Firelist& firelist, index_t number_of_fronts, index_t number_of_threads)
: nr_of_fronts(number_of_fronts), prop(property), start(ns), fl(firelist), count_store(st), max_transient_count(0), exit(false)
{
	// determine the necessary store sizes from min and max progress measure
	int32_t foff(0), boff(0), negcount(0);
	std::map<int32_t,int32_t> negmap;
	for(index_t i=0; i<Net::Card[TR]; ++i)
	{
//cout << "progress(" << Net::Name[TR][i] << ")= " << Transition::ProgressMeasure[i] << endl;
		if (Transition::ProgressMeasure[i]>foff)
			foff = Transition::ProgressMeasure[i];
		if (Transition::ProgressMeasure[i]<boff)
			boff = Transition::ProgressMeasure[i];
		// count the transitions with negative progress and bucket-sort the progress values
		if (Transition::ProgressMeasure[i]<0) {
			++negcount;
			++negmap[Transition::ProgressMeasure[i]];
		}
	}
	front_offset = (index_t) foff;
	store_size = (index_t) (foff-boff+1);

	// find the negative progress offset at which transient states are forgotten
	int32_t negtcount(args_info.sweeplinedelay_arg*negcount/100);
	transient_offset = 0;
	for(std::map<int32_t,int32_t>::reverse_iterator it=negmap.rbegin(); it!=negmap.rend() && negtcount>0; ++it)
	{
		negtcount -= it->second;
		transient_offset = (index_t)(-it->first);
	}

	rep->status("transition progress range [%d,%d], transients in [%d,%d]",boff,foff,-(int32_t)(transient_offset),foff);

	// compute the true number of threads (as given unless lower than number_of_fronts)
	if ((threads_per_front = number_of_threads / nr_of_fronts)) {
		nr_of_threads = number_of_threads;
	} else {
		nr_of_threads = nr_of_fronts;
		rep->status("total number of threads changed to %d", nr_of_threads);
	}
	if (nr_of_threads % nr_of_fronts > 0) ++threads_per_front;

	// obtain initial (empty) lists of stores for persistent states
	start_new_persistent = new SweepListStore<T>(nr_of_threads, store_size);
	start_old_persistent = new SweepListStore<T>(nr_of_threads, store_size);

	// reserve memory for counting states and edges for all threads/fronts
	persistent_count = (int64_t***) calloc(SIZEOF_INT64_T,nr_of_threads);
	transient_count = (int64_t***) calloc(SIZEOF_INT64_T,nr_of_threads);
	call_count = (int64_t***) calloc(SIZEOF_INT64_T,nr_of_threads);

	// reserve memory for thread-local mutexes for the idle state / front change
    idleMutex = (pthread_mutex_t**) malloc(nr_of_threads * SIZEOF_VOIDP);
    idleCond = (pthread_cond_t**) malloc(nr_of_threads * SIZEOF_VOIDP);
	idle = (bool**) calloc(SIZEOF_VOIDP, nr_of_threads);
	frontptr = (index_t**) malloc(nr_of_threads * SIZEOF_VOIDP);
	call = (bool**) malloc(nr_of_threads * SIZEOF_VOIDP);
	helpers = (index_t**) malloc(nr_of_fronts * SIZEOF_VOIDP);

	// reserve memory for pointers to the stores
	store = (SweepRingStore<T>**) malloc(nr_of_fronts * SIZEOF_VOIDP);

	// get encoders
	fullencoder = new FullCopyEncoder(nr_of_threads);
    switch (args_info.encoder_arg) {
    case encoder_arg_bit:
        sigpencoder = new BitEncoder(nr_of_threads);
        break;
    case encoder_arg_copy:
        sigpencoder = new CopyEncoder(nr_of_threads);
        break;
    case encoder_arg_simplecompressed:
        sigpencoder = new SimpleCompressedEncoder(nr_of_threads);
        break;
    case encoder_arg_fullcopy:
        sigpencoder = new FullCopyEncoder(nr_of_threads);
        break;
    }
//	sigpencoder = new CopyEncoder(nr_of_threads);
}

/*!
 * \brief Destructor.
 */
template <class T>
Sweep<T>::~Sweep()
{
	free(store);
	free(idleMutex);
	free(idleCond);
	free(idle);
	free(frontptr);
	free(call);
	free(helpers);
	free(call_count);
	free(transient_count);
	free(persistent_count);
	delete start_new_persistent;
	delete start_old_persistent;
    delete fullencoder;
    delete sigpencoder;
}

/*!
 * \brief Run the sweepline method. If the number of fronts given is one, no multithreading will be done
 * \return If a state fulfilling the property could be found. In the positive case,
 *         the netstate given to the constructor will contain such a state.
 */
template <class T>
bool Sweep<T>::run()
{
	// check if we need a multithreaded run
	if (threads_per_front>1 || nr_of_fronts>1) return runThreads();

	// get a mutex and condition for thread synchronisation on termination
	terminating = running = nr_of_threads;
    // LCOV_EXCL_START
    if (UNLIKELY(pthread_mutex_init(&runMutex, NULL))) {
        rep->status("mutexes could not be created");
        rep->abort(ERROR_THREADING);
    }
    if (UNLIKELY(pthread_cond_init(&runCond, NULL))) {
        rep->status("mutex conditions could not be created");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

	// gather the arguments for the run using the main thread only
	bool frontrunner(true);
	tpSweepArguments args;
	args.frontrunner = &frontrunner;
	args.frontID = 0;
	args.globalThreadID = 0;
	args.sweep = this;
	args.ns = &start;
	args.call_count = NULL;
	persistent_count[0] = &args.persistent_count;
	transient_count[0] = &args.transient_count;
	call_count[0] = &args.call_count;

	// run the sweepline method and get the result
	void* return_value(frontSweep((void*) &args));
	prop.value = false;
	if (return_value) {
		prop.value = true;
		start = *(NetState*) return_value;
	}

    // destroy mutexes
    int mutex_destruction_status = 0;
    mutex_destruction_status |= pthread_cond_destroy(&runCond);
    mutex_destruction_status |= pthread_mutex_destroy(&runMutex);
    // LCOV_EXCL_START
    if (UNLIKELY(mutex_destruction_status)) {
        rep->status("mutexes could not be destroyed");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

	// make a final calculation of states and edges
    forwardStateCount();

    free(args.persistent_count);
    free(args.transient_count);
    free(args.call_count);
	return prop.value;
}

/*!
 * \brief Run the sweepline method in multithreaded mode
 * \return If a state fulfilling the property could be found. In the positive case,
 *         the netstate given to the constructor will contain such a state.
 */
template <class T>
bool Sweep<T>::runThreads()
{
	// get a mutex and condition for thread synchronisation on termination
	terminating = running = nr_of_threads;
    // LCOV_EXCL_START
    if (UNLIKELY(pthread_mutex_init(&runMutex, NULL))) {
        rep->status("mutexes could not be created");
        rep->abort(ERROR_THREADING);
    }
    if (UNLIKELY(pthread_cond_init(&runCond, NULL))) {
        rep->status("mutex conditions could not be created");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

    // flag for the most advanced front
    bool* frontrunner = (bool*) calloc(sizeof(bool), nr_of_fronts);
    // semaphore values controlled by the following mutex
    index_t* semval = (index_t*) calloc(sizeof(index_t), nr_of_fronts);
    // mutex controlling false->true change for frontrunner flag and semaphore values
    pthread_mutex_t* mutex = (pthread_mutex_t*) calloc(sizeof(pthread_mutex_t), nr_of_fronts);
    pthread_cond_t* cond = (pthread_cond_t*) calloc(sizeof(pthread_cond_t), nr_of_fronts);
    // a thread for each front
    pthread_t* runner_thread = (pthread_t*) calloc(sizeof(pthread_t), nr_of_threads);
    // arguments for each front/thread
    tpSweepArguments* args = (tpSweepArguments*) calloc(sizeof(tpSweepArguments), nr_of_threads);
    // the first front gets the start state and is also the most advanced front
    args[0].ns = &start;
    frontrunner[0] = true;

    // set the arguments for all fronts/threads and obtain the semaphores
    for (index_t i = 0; i < nr_of_threads; i++) {

		if (i < nr_of_fronts) {
	        // initialise mutexes and conditions
	        // LCOV_EXCL_START
	        if (UNLIKELY(pthread_mutex_init(&mutex[i], NULL))) {
	            rep->status("mutexes could not be created");
	            rep->abort(ERROR_THREADING);
	        }
	        if (UNLIKELY(pthread_cond_init(&cond[i], NULL))) {
	            rep->status("mutex conditions could not be created");
	            rep->abort(ERROR_THREADING);
	        }
	        // LCOV_EXCL_STOP

	        // initialise data structure for fronts
	        args[i].frontMutex = (i>0 ? &mutex[i-1] : &mutex[nr_of_fronts-1]);
	        args[i].backMutex = &mutex[i];
	        args[i].frontCond = (i>0 ? &cond[i-1] : &cond[nr_of_fronts-1]);
	        args[i].backCond = &cond[i];
	        args[i].frontSemaphore = (i>0 ? &semval[i-1] : &semval[nr_of_fronts-1]);
	        args[i].backSemaphore = &semval[i];
	        args[i].frontrunner = frontrunner;
	        args[i].frontID = i;
	        args[i].globalThreadID = i;
	        args[i].call_count = NULL;
	        args[i].sweep = this;

	        persistent_count[i] = &args[i].persistent_count;
	        transient_count[i] = &args[i].transient_count;
	        call_count[i] = &args[i].call_count;
		} else {
			// initialise data structure for helper threads
			args[i].frontID = i % nr_of_fronts;
			args[i].globalThreadID = i;
			args[i].call_count = NULL;
			args[i].sweep = this;

	        persistent_count[i] = &args[i].persistent_count;
	        transient_count[i] = &args[i].transient_count;
	        call_count[i] = &args[i].call_count;
		}
    }

    // create the threads
    for (index_t i = 0; i < nr_of_threads; i++)
        if (UNLIKELY(pthread_create(runner_thread + i, NULL, (i<nr_of_fronts ? frontSweep : threadSweep), args + i))) {
            // LCOV_EXCL_START
            rep->status("threads could not be created");
            rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }


    //// THREADS ARE RUNNING AND SEARCHING


    // wait for all threads to finish
    prop.value = false;
    for (index_t i = 0; i < nr_of_threads; i++) {
        void* return_value;
        if (UNLIKELY(pthread_join(runner_thread[i], &return_value))) {
            // LCOV_EXCL_START
            rep->status("threads could not be joined");
            rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }
        if (return_value) {
            prop.value = true;
            start = *(NetState*) return_value;
        }
    }

    // destroy mutexes
    int mutex_destruction_status = 0;
    for(index_t i=0; i<nr_of_fronts; ++i)
    {
        mutex_destruction_status |= pthread_cond_destroy(&cond[i]);
        mutex_destruction_status |= pthread_mutex_destroy(&mutex[i]);
    }
    mutex_destruction_status |= pthread_cond_destroy(&runCond);
    mutex_destruction_status |= pthread_mutex_destroy(&runMutex);

    // LCOV_EXCL_START
    if (UNLIKELY(mutex_destruction_status)) {
        rep->status("mutexes could not be destroyed");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

	// make a final calculation of states and edges
    forwardStateCount();

    // free the allocated memory
    free(runner_thread);
    free(frontrunner);
    for(index_t i=0; i<nr_of_threads; ++i) 
    {
        free(args[i].persistent_count);
        free(args[i].transient_count);
        free(args[i].call_count);
        delete args[i].ns;
    }
    free(args);
    free(mutex);
    free(cond);
    free(semval);

    return prop.value;
}

/*!
 * \brief main function for a thread (also used for single-threaded runs)
 * \param container Arguments needed by the thread
 * \return Pointer to a state fulfilling the property or NULL if none exists
 */
template <class T>
void* Sweep<T>::frontSweep(void* container) {
    tpSweepArguments* arguments = (tpSweepArguments*) container;
    Sweep<T>* sweep(arguments->sweep);
	index_t threadID(arguments->globalThreadID);

    // allocate memory for state and edge counters and such
    arguments->persistent_count = (int64_t*) calloc(SIZEOF_INT64_T,1);
    arguments->transient_count = (int64_t*) calloc(SIZEOF_INT64_T,1);
    arguments->call_count = (int64_t*) calloc(SIZEOF_INT64_T,1);

    // for idle checks by helpers and the front itself
	index_t helpers(0);
	sweep->helpers[threadID] = &helpers;
	bool idle(true);
	sweep->idle[threadID] = &idle;
	bool call(false);
	sweep->call[threadID] = &call;

    // Make copies of SimpleProperty and FireList
    SimpleProperty* sp = sweep->prop.copy();
    Firelist* myFirelist = sweep->fl.createNewFireList(sp);

    // Create the Store for this thread
	SweepRingStore<T> store(sweep->store_size,sweep->front_offset,sweep->transient_offset,
        sweep->fullencoder,sweep->sigpencoder,sweep->nr_of_threads);
	sweep->store[arguments->frontID] = &store;

	if (arguments->ns) {
		store.init(sweep->start_old_persistent,sweep->start_new_persistent);
		store.searchAndInsert(*arguments->ns,0,NULL,0);
		++(*arguments->transient_count);
	}

    // state variable for the current state, deleted on thread termination
    arguments->ns = NetState::createNetStateFromInitial();

    // how many progress values must be added below the lowest value so far
    // when a new persistent marking with the lowest possible progress value is created
    int32_t back_progress(sweep->store_size-sweep->front_offset-1);

    // how often the front must advance before the next thread can start
    int32_t sem_countdown(sweep->store_size-1);

	// allocate mutex and wait for init of all threads
    pthread_mutex_t mutex;
	pthread_cond_t cond;
	sweep->initThreadMutex(&mutex,&cond,threadID);
	idle = true;

    NetState* solution(NULL);
    // run until termination notice is given
    while (!sweep->exit)
    {
        // initialise the front
        if (!sweep->initFront(*arguments,store,back_progress,sem_countdown)) break;

        // run the front until a state fulfilling the property is found
        // or until no new persistent states have been generated when the front hits the final border
        if ((solution = sweep->runFront(store,*myFirelist,*sp,*arguments,back_progress,sem_countdown))) sweep->exit=true; 
            // no lock for exit, can only be set to true once and for all

        // finalise the front
        sweep->deinitFront(store,*arguments);
    }

    // signal the next front to terminate
    if (sweep->nr_of_fronts>1) {
        pthread_mutex_lock(arguments->backMutex);
        *arguments->backSemaphore = 1;
        pthread_cond_signal(arguments->backCond);
        pthread_mutex_unlock(arguments->backMutex);
    }

	// signal all helper threads of this front to terminate
	if (sweep->threads_per_front>1)
		for(index_t i = arguments->frontID + sweep->nr_of_fronts; i < sweep->nr_of_threads; i+=sweep->nr_of_fronts) {
			pthread_mutex_lock(sweep->idleMutex[i]);
			pthread_cond_signal(sweep->idleCond[i]);
			pthread_mutex_unlock(sweep->idleMutex[i]);		
		}

	sweep->terminateThreadMutex(&mutex, &cond);

    delete myFirelist;
    delete sp;
    return solution;
}

/*!
 * \brief main function for a thread (also used for single-threaded runs)
 * \param container Arguments needed by the thread
 * \return Pointer to a state fulfilling the property or NULL if none exists
 */
template <class T>
void* Sweep<T>::threadSweep(void* container) {
    tpSweepArguments* arguments = (tpSweepArguments*) container;
    Sweep<T>* sweep(arguments->sweep);

    // allocate memory for state and edge counters
    arguments->persistent_count = (int64_t*) calloc(SIZEOF_INT64_T,1);
    arguments->transient_count = (int64_t*) calloc(SIZEOF_INT64_T,1);
    arguments->call_count = (int64_t*) calloc(SIZEOF_INT64_T,1);

	index_t threadID(arguments->globalThreadID);
	index_t frontptr(arguments->frontID);
	sweep->frontptr[threadID] = &frontptr;
	bool call(false);
	sweep->call[threadID] = &call;
	bool idle(true);
	sweep->idle[threadID] = &idle;

    // Make copies of SimpleProperty and FireList
    SimpleProperty* sp = sweep->prop.copy();
    Firelist* myFirelist = sweep->fl.createNewFireList(sp);

    // state variable for the current state, deleted on thread termination
    arguments->ns = NetState::createNetStateFromInitial();

	// allocate local mutex
    pthread_mutex_t mutex;
	pthread_cond_t cond;
	sweep->initThreadMutex(&mutex,&cond,threadID);

    NetState* solution(NULL);
    // run until termination notice is given
    while (!sweep->exit)
    {
		bool idleresult(true);
		while (!sweep->exit) {
			bool* idletest(sweep->idle[frontptr]);
			if (!(idleresult = *idletest)) {
				pthread_mutex_lock(sweep->idleMutex[frontptr]);
				if (!(idleresult = *idletest))
					++*sweep->helpers[frontptr];
				pthread_mutex_unlock(sweep->idleMutex[frontptr]);
			}	
			if (idleresult) {
				if (++frontptr == sweep->nr_of_fronts) frontptr=0;
				if (frontptr != arguments->frontID) continue;
				pthread_mutex_lock(&mutex);
				idle = *sweep->idle[frontptr];
				while (idle && !sweep->exit)
					pthread_cond_wait(&cond, &mutex);
				call = false;
				pthread_mutex_unlock(&mutex);
				continue;
			} else break;
		}

		if (!sweep->exit) {
	        // check states at current progress measure and compute successors
	        if (sweep->checkStates(*sweep->store[frontptr],*myFirelist,*sp,*arguments)) {
				solution = arguments->ns;
				sweep->exit = true;
			}
		}

		if (!idleresult) {
			pthread_mutex_lock(sweep->idleMutex[frontptr]);
			--*sweep->helpers[frontptr];
			pthread_cond_signal(sweep->idleCond[frontptr]);
			pthread_mutex_unlock(sweep->idleMutex[frontptr]);
		}
		if (call) {
			call = false;
			frontptr = arguments->frontID;
		}
    }

	// wait until request for termination is accepted
	sweep->terminateThreadMutex(&mutex, &cond);

    delete myFirelist;
    delete sp;
    return solution;
}

/*!
 * \brief Initialise the front
 * \param args The arguments for this thread
 * \param store The store obtained by this thread
 * \param back_progress Return parameter. How many stores to add in front of the persistent store lists if necessary
 * \param sem_countdown Return parameter. How far the front must have progressed before the next front is unleashed
 * \return If the initialisation is complete (otherwise the thread should terminate)
 */
template <class T>
bool Sweep<T>::initFront(tpSweepArguments& args, SweepRingStore<T>& store, int32_t& back_progress, int32_t& sem_countdown)
{
	// if we are not the most advanced front, wait until our predecessor is far enough away
	if (nr_of_fronts>1) {
		pthread_mutex_lock(args.frontMutex);
		while (!args.frontrunner[args.frontID] && !*args.frontSemaphore) {
			pthread_cond_wait(args.frontCond,args.frontMutex);
//			checkCall(args);
		}
		if (!args.frontrunner[args.frontID]) --*args.frontSemaphore;
		pthread_mutex_unlock(args.frontMutex);
	}

	if (exit) return false;

	// set the return parameters
	back_progress = store_size-front_offset-1;
	sem_countdown = store_size-1;

	// link the persistent states into our store
	store.init(start_old_persistent,start_new_persistent);
	return true;
}

/*!
 * \brief Run the front until no more progress can be made or a witness state is found
 * \param store The store for this thread
 * \param fl A firelist for the property to check
 * \param sp The property to check
 * \param args Arguments given to the thread
 * \param bp By how many new stores the persistent store list must be preceded 
          in case a new persistent state with the lowest possible progress measure
          for this front shows up 
 * \param semcnt How far to advance before unleashing the following front
 * \return A witness state or NULL if none was found
 */
template <class T>
NetState* Sweep<T>::runFront(SweepRingStore<T>& store, Firelist& fl, SimpleProperty& sp, tpSweepArguments& args, int32_t& bp, int32_t& semcnt)
{
	index_t pval(0);

	index_t frontID(args.frontID);
	bool& frontIdle(*idle[frontID]);
	pthread_mutex_t* mutex(idleMutex[frontID]);
	pthread_cond_t* cond(idleCond[frontID]);
	index_t& frontHelpers(*helpers[frontID]);
    // run until termination notice is given
    while (!exit) {
		// tell dependent threads that we have work to do
		if (threads_per_front>1) {
			frontIdle = false;
			checkCall(args,true);
			for(index_t i=nr_of_fronts; i<nr_of_threads; ++i)
				if (*idle[i]) {
					pthread_mutex_lock(idleMutex[i]);
					*idle[i] = false;
					pthread_cond_signal(idleCond[i]);
					pthread_mutex_unlock(idleMutex[i]);
				}
/*
			index_t tocall((frontID+1) % nr_of_fronts);
			*call[tocall] = true;
			pthread_cond_signal(idleCond[tocall]);
			pthread_cond_signal(args.backCond);
*/
		}

        // check states at current progress measure and compute successors
		bool solved;
        if ((solved = checkStates(store,fl,sp,args))) exit = true;

		// tell dependent threads to look for work elsewhere
		// and wait for all threads still working here to clear the area before we advance
		if (threads_per_front>1) {
			pthread_mutex_lock(mutex);
			frontIdle = true;
			while (frontHelpers>0) {
				pthread_cond_wait(cond,mutex);
//				checkCall(args);
			}
			pthread_mutex_unlock(mutex);
		}

		// if a solution was found (by any thread), quit
		if (solved) return args.ns;
		if (exit) return NULL;

        // check if we need to precede the persistent store lists
        // with new, empty stores
        if (bp && store.checkNewPersistent()) extendLeft(bp);
        else if (bp) --bp;

		// if we're not the foremost front, check if we may advance
		if (nr_of_fronts>1) {
			pthread_mutex_lock(args.frontMutex);
			while (!args.frontrunner[frontID] && !*args.frontSemaphore) {
				pthread_cond_wait(args.frontCond,args.frontMutex);
//				checkCall(args);
			}
		    if (!args.frontrunner[frontID]) --*args.frontSemaphore;
			pthread_mutex_unlock(args.frontMutex);
		}

        // compute and forward the state count
        if (args.frontrunner[frontID] && !exit)
			forwardStateCount();

        // advance the front to the next progress measure
        if (!store.advanceProgress()) {
            bp = store.checkNewPersistent(bp);
            extendLeft(bp);
            break;
        }

        // correct the number of transient states due to progress advance
        *args.transient_count -= store.getNumberOfDeletedStates();

        // check if the next front may advance also
        if (semcnt) --semcnt; 
        else if (nr_of_fronts>1) 
        { 
            pthread_mutex_lock(args.backMutex);
            ++*args.backSemaphore;
            pthread_cond_signal(args.backCond);
            pthread_mutex_unlock(args.backMutex);
        }
    }
    return NULL;
}

/*!
 * \brief Finalise this front
 * \param store The store for this thread
 * \param args The arguments given to this thread
 */
template <class T>
void Sweep<T>::deinitFront(SweepRingStore<T>& store, tpSweepArguments& args)
{
//	forwardStateCount(true);

	// clear the transient states and check if there were new persistent states at all
	if (store.clear()) exit=true;

	// make the following front the foremost one
	if (nr_of_fronts>1) {
        // we're not the foremost front anymore
	    args.frontrunner[args.frontID] = false;
        pthread_mutex_lock(args.backMutex);
        // activate the following front
        args.frontrunner[(args.frontID+1)%nr_of_fronts] = true;
        pthread_cond_signal(args.backCond);
        // reset the semaphore, the following front will not use it anymore
        *args.backSemaphore = 0;
        pthread_mutex_unlock(args.backMutex);
    }
}


/*!
 * \brief Check all states with the current progress measure and compute their successors
 * \param store The store for this thread/front
 * \param fl The firelist for the property to check
 * \param sp The property to check
 * \param ns Space to store a netstate to, the witness on positive returns
 * \return If a witness state fulfilling the property has been found
 */
template <class T>
bool Sweep<T>::checkStates(SweepRingStore<T>& store, Firelist& fl, SimpleProperty& sp, tpSweepArguments& args)
{
	NetState& ns(*args.ns);
	index_t threadID(args.globalThreadID);
	int64_t* persistents(args.persistent_count);
	int64_t* transients(args.transient_count);
	int64_t* calls(args.call_count);

	// get a state from the store (transient or new persistent)
    // note: the state will not be deleted from the store but transferred to the seen / old persistent list
    while (!exit && store.popState(ns,threadID))
    {
        // adapt the enabledness information
        Sweep::updateState(ns);

        // check the property
        if (sp.initProperty(ns)) return true;

        // get the firelist, i.e. all transition that need to fire
        index_t* currentFirelist;
        index_t currentEntry = fl.getFirelist(ns, &currentFirelist);
        while (currentEntry-- > 0)
        {
			index_t t(currentFirelist[currentEntry]);
            // fire one transition, calculate the successor state
            Transition::fire(ns, t);

            // add the state to the store
            ++*calls;
            if (!store.searchAndInsert(ns,Transition::ProgressMeasure[t],NULL,threadID)) 
			{
                if (store.insertedIsNewPersistent(threadID)) ++*persistents;
                else ++*transients;
            }

		    // return to the original state
            Transition::backfire(ns, t);
        }
        delete[] currentFirelist;
    }

    // no more states in the store, witness not found
    return false;
}


/*!
 * \brief Preceed the persistent store lists with new, empty stores
 * \param bp The number of stores to insert
 */
template <class T>
void Sweep<T>::extendLeft(int32_t& bp)
{
	SweepListStore<T>* sls;
	while (bp>0)
	{
        --bp;

		sls = new SweepListStore<T>(nr_of_threads);
		sls->setNext(start_new_persistent);
		start_new_persistent = sls;

		sls = new SweepListStore<T>(nr_of_threads);
		sls->setNext(start_old_persistent);
		start_old_persistent = sls;
	}	
}

/*!
 * \brief Update the enabledness information after the marking in the state has been changed arbitrarily
 * \param The netstate with the new marking
 */
template <class T>
void Sweep<T>::updateState(NetState& ns)
{
    // we have no good value for the hash function
    // since only delta-t values can be computed
//  ns.HashCurrent = Marking::HashInitial;

    // check for currently enabled transitions
    for (index_t i = 0; i < Net::Card[TR]; i++)
    {
        Transition::checkEnabled(ns,i);
    }
}

/*!
 * \brief Update the counter for markings and calls in the dummy store
 */
template <class T>
void Sweep<T>::forwardStateCount(bool show)
{
	int64_t trcnt(0),pscnt(0),ccnt(0);

    // add the counters of all threads representing a front
    // each thread counts the states it has added to the global storage
	for(index_t i=0; i<nr_of_threads; ++i)
	{
		trcnt += **(transient_count[i]);
		pscnt += **(persistent_count[i]);
		ccnt += **(call_count[i]);
	}

    // adapt the maximum for transient states if necessary
	if (trcnt<max_transient_count) trcnt=max_transient_count;
	else max_transient_count=trcnt;

    // save the counter values, no distinguishing transient and persistent states possible
	if (exit || show) rep->status("%lld persistent markings, %lld transient markings (max)",pscnt,trcnt);
	count_store.setMarkings(trcnt+pscnt);
	count_store.setCalls(ccnt);
}

template <class T>
void Sweep<T>::initThreadMutex(pthread_mutex_t* mutex, pthread_cond_t* cond, index_t threadID)
{
	idleMutex[threadID] = mutex;
    idleCond[threadID] = cond;
	// LCOV_EXCL_START
	if (UNLIKELY(pthread_mutex_init(mutex, NULL))) {
		exit = true;
		rep->status("mutexes could not be created");
		rep->abort(ERROR_THREADING);
	}
    if (UNLIKELY(pthread_cond_init(cond, NULL))) {
		exit = true;
        rep->status("mutex conditions could not be created");
        rep->abort(ERROR_THREADING);
    }
	// LCOV_EXCL_STOP

	// wait until all threads have initialised
	pthread_mutex_lock(&runMutex);
	if (!--running) {
		pthread_mutex_unlock(&runMutex);
		pthread_mutex_lock(idleMutex[0]);
		*idle[0] = false;
		pthread_cond_signal(idleCond[0]);
		pthread_mutex_unlock(idleMutex[0]);
	} else pthread_mutex_unlock(&runMutex);
	pthread_mutex_lock(mutex);
	while (*idle[threadID] && !exit)
		pthread_cond_wait(cond, mutex);
	pthread_mutex_unlock(mutex);
	if (!threadID)
		for(index_t i=1; i<nr_of_fronts; ++i) {
			pthread_mutex_lock(idleMutex[i]);
			*idle[i] = false;
			pthread_cond_signal(idleCond[i]);
			pthread_mutex_unlock(idleMutex[i]);
		}
}

template <class T>
void Sweep<T>::terminateThreadMutex(pthread_mutex_t* mutex, pthread_cond_t* cond)
{
	// wait until request for termination is accepted
	pthread_mutex_lock(&runMutex);
	if (!--terminating)
		pthread_cond_broadcast(&runCond);
	else while (terminating)
		pthread_cond_wait(&runCond, &runMutex);
	pthread_mutex_unlock(&runMutex);

    int mutex_destruction_status = pthread_cond_destroy(cond);
    mutex_destruction_status |= pthread_mutex_destroy(mutex);
    // LCOV_EXCL_START
    if (UNLIKELY(mutex_destruction_status)) {
        rep->status("mutexes could not be destroyed");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP
}

template <class T>
void Sweep<T>::checkCall(tpSweepArguments& args, bool draw)
{
	if (draw || *call[args.frontID]) {
        for(index_t i = args.frontID + nr_of_fronts; i < nr_of_threads; i += nr_of_fronts)
		{
			*call[i] = draw;
			if (*idle[i]) {
				pthread_mutex_lock(idleMutex[i]);
				*idle[i] = false;
				pthread_cond_signal(idleCond[i]);
				pthread_mutex_unlock(idleMutex[i]);
			}
		}
		if (!draw) *call[(args.frontID+1) % nr_of_fronts] = true;
		*call[args.frontID] = false;
	}
}

