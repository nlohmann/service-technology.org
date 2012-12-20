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
#include <pthread.h>
#include <SweepLine/Sweep.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <InputOutput/Reporter.h>

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
Sweep<T>::Sweep(SimpleProperty& property, NetState& ns, SweepEmptyStore& st, Firelist& firelist, index_t number_of_fronts) 
: nr_of_fronts(number_of_fronts), prop(property), start(ns), fl(firelist), count_store(st), max_transient_count(0), exit(false)
{
	// determine the necessary store sizes from min and max progress measure
	int32_t foff(0), boff(0);
	for(index_t i=0; i<Net::Card[TR]; ++i)
	{
		if (Transition::ProgressMeasure[i]>foff)
			foff = Transition::ProgressMeasure[i];
		if (Transition::ProgressMeasure[i]<boff)
			boff = Transition::ProgressMeasure[i];

	}
	front_offset = (index_t) foff;
	store_size = (index_t) (foff-boff+1);

	// obtain initial (empty) lists of stores for persistent states
	start_new_persistent = new SweepListStore<T>(store_size);
	start_old_persistent = new SweepListStore<T>(store_size);

	persistent_count = (int64_t***) calloc(SIZEOF_INT64_T,nr_of_fronts);
	transient_count = (int64_t***) calloc(SIZEOF_INT64_T,nr_of_fronts);
	call_count = (int64_t***) calloc(SIZEOF_INT64_T,nr_of_fronts);
}

/*!
 * \brief Destructor.
 */
template <class T>
Sweep<T>::~Sweep()
{
	free(call_count);
	free(transient_count);
	free(persistent_count);
	delete start_new_persistent;
	delete start_old_persistent;
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
	if (nr_of_fronts>1) return runThreads();

	// gather the arguments for the run
	bool frontrunner(true);
	tpSweepArguments args;
	args.frontrunner = &frontrunner;
	args.threadNumber = 0;
	args.number_of_threads = 1;
	args.sweep = this;
	args.ns = &start;
	args.call_count = NULL;
	persistent_count[0] = &args.persistent_count;
	transient_count[0] = &args.transient_count;
	call_count[0] = &args.call_count;

	// run the sweepline method and get the result
	void* return_value(threadSweep((void*) &args));
        if (return_value) start = *(NetState*) return_value;

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
    // flag for the most advanced front
    bool* frontrunner = (bool*) calloc(nr_of_fronts, sizeof(bool));
    // semaphore values controlled by the following mutex
    index_t* semval = (index_t*) calloc(nr_of_fronts, sizeof(index_t));
    // mutex controlling false->true change for frontrunner flag and semaphore values
    pthread_mutex_t* mutex = (pthread_mutex_t*) calloc(nr_of_fronts, sizeof(pthread_mutex_t));
    pthread_cond_t* cond = (pthread_cond_t*) calloc(nr_of_fronts, sizeof(pthread_cond_t));
    // a thread for each front
    pthread_t* runner_thread = (pthread_t*) calloc(nr_of_fronts, sizeof(pthread_t));
    // arguments for each front/thread
    tpSweepArguments* args = (tpSweepArguments*) calloc(nr_of_fronts,
                           sizeof(tpSweepArguments));
    // the first front gets the start state and is also the most advanced front
    args[0].ns = &start;
    frontrunner[0] = true;
    // set the arguments for all fronts and obtain the semaphores
    for (index_t i = 0; i < nr_of_fronts; i++) {
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

        // initialise data structure for threads
        args[i].frontMutex = (i>0 ? &mutex[i-1] : &mutex[nr_of_fronts-1]);
        args[i].backMutex = &mutex[i];
        args[i].frontCond = (i>0 ? &cond[i-1] : &cond[nr_of_fronts-1]);
        args[i].backCond = &cond[i];
        args[i].frontSemaphore = (i>0 ? &semval[i-1] : &semval[nr_of_fronts-1]);
        args[i].backSemaphore = &semval[i];
        args[i].frontrunner = frontrunner;
        args[i].threadNumber = i;
        args[i].number_of_threads = nr_of_fronts;
        args[i].call_count = NULL;
        args[i].sweep = this;

        persistent_count[i] = &args[i].persistent_count;
        transient_count[i] = &args[i].transient_count;
        call_count[i] = &args[i].call_count;
    }

    // create the threads
    for (index_t i = 0; i < nr_of_fronts; i++)
        if (UNLIKELY(pthread_create(runner_thread + i, NULL, threadSweep, args + i))) {
            // LCOV_EXCL_START
            rep->status("threads could not be created");
            rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }


    //// THREADS ARE RUNNING AND SEARCHING

    // wait for all threads to finish
    prop.value = false;
    for (index_t i = 0; i < nr_of_fronts; i++) {
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
    // LCOV_EXCL_START
    if (UNLIKELY(mutex_destruction_status)) {
        rep->status("mutexes could not be destroyed");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

    // free the allocated memory
    free(runner_thread);
    free(frontrunner);
    for(index_t i=0; i<nr_of_fronts; ++i) 
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
void* Sweep<T>::threadSweep(void* container) {
    tpSweepArguments* arguments = (tpSweepArguments*) container;
    Sweep<T>* sweep(arguments->sweep);

    // allocate memory for state and edge counters
    arguments->persistent_count = (int64_t*) calloc(SIZEOF_INT64_T,1);
    arguments->transient_count = (int64_t*) calloc(SIZEOF_INT64_T,1);
    arguments->call_count = (int64_t*) calloc(SIZEOF_INT64_T,1);

    // first thread polls here until all threads have initialised
    if (!arguments->threadNumber)
        for(index_t i = 0; i < arguments->sweep->nr_of_fronts; ++i)
            while (!*(arguments->sweep->call_count[i])) ;

    // Make copies of SimpleProperty and FireList
    SimpleProperty* sp = sweep->prop.copy();
    Firelist* myFirelist = sweep->fl.createNewFireList(sp);

    // Create the Store for this thread
    SweepRingStore<T> store(sweep->store_size,sweep->front_offset);
    if (arguments->ns) {
        store.init(sweep->start_old_persistent,sweep->start_new_persistent);
        store.searchAndInsert(*arguments->ns,NULL,0);
        ++(*arguments->transient_count);
    }

    // state variable for the current state, deleted on thread termination
    arguments->ns = NetState::createNetStateFromInitial();

    // how many progress values must be added below the lowest value so far
    // when a new persistent marking with the lowest possible progress value is created
    int32_t back_progress(sweep->store_size-sweep->front_offset-1);

    // how often the front must advance before the next thread can start
    int32_t sem_countdown(sweep->store_size-1);

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

    // signal the next thread to terminate
    if (arguments->number_of_threads>1) {
        pthread_mutex_lock(arguments->backMutex);
        *arguments->backSemaphore = 1;
        pthread_cond_signal(arguments->backCond);
        pthread_mutex_unlock(arguments->backMutex);
    }

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
    if (args.number_of_threads>1) {
        pthread_mutex_lock(args.frontMutex);
        while (!args.frontrunner[args.threadNumber] && !*args.frontSemaphore) 
            pthread_cond_wait(args.frontCond,args.frontMutex);
        if (!args.frontrunner[args.threadNumber]) --*args.frontSemaphore;
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
    // run until termination notice is given
    while (!exit) {
        // check states at current progress measure and compute successors
        if (checkStates(store,fl,sp,*(args.ns),args)) return args.ns;

        // check if we need to precede the persistent store lists
        // with new, empty stores
        if (bp && store.checkNewPersistent()) extendLeft(bp);

		// if we're not the foremost front, check if we may advance
		if (args.number_of_threads>1) {
                    pthread_mutex_lock(args.frontMutex);
                    while (!args.frontrunner[args.threadNumber] && !*args.frontSemaphore)
		        pthread_cond_wait(args.frontCond,args.frontMutex);
		    if (!args.frontrunner[args.threadNumber]) --*args.frontSemaphore;
                    pthread_mutex_unlock(args.frontMutex);
		}

        // compute and forward the state count
        if (args.frontrunner[args.threadNumber]) forwardStateCount();

        // advance the front to the next progress measure
        if (!store.advanceProgress()) break;

        // correct the number of transient states due to progress advance
        *args.transient_count -= store.getNumberOfDeletedStates();

        // check if the next front may advance also
        if (semcnt) --semcnt; 
        else if (args.number_of_threads>1) 
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
	// clear the transient states and check if there were new persistent states at all
	if (store.clear()) exit=true;

	// make the following front the foremost one
	if (args.number_of_threads>1) {
        // we're not the foremost front anymore
	    args.frontrunner[args.threadNumber] = false;
        pthread_mutex_lock(args.backMutex);
        // activate the following front
        args.frontrunner[(args.threadNumber+1)%nr_of_fronts] = true;
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
bool Sweep<T>::checkStates(SweepRingStore<T>& store, Firelist& fl, SimpleProperty& sp, NetState& ns, tpSweepArguments& args)
{
	// get a state from the store (transient or new persistent)
    // note: the state will not be deleted from the store but transferred to the seen / old persistent list
    while (store.popState(ns))
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
            // fire one transition, calculate the successor state
            Transition::fire(ns, currentFirelist[currentEntry]);
            // add the state to the store
            store.setOffset(Transition::ProgressMeasure[currentFirelist[currentEntry]]);
            ++(*args.call_count);
            if (!store.searchAndInsert(ns,NULL,0)) {
                if (store.insertedIsNewPersistent()) ++(*args.persistent_count);
                else ++(*args.transient_count);
            }
		    // return to the original state
            Transition::backfire(ns, currentFirelist[currentEntry]);
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
	while (bp-->0)
	{
		sls = new SweepListStore<T>();
		sls->setNext(start_new_persistent);
		start_new_persistent = sls;

		sls = new SweepListStore<T>();
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
void Sweep<T>::forwardStateCount()
{
	int64_t trcnt(0),pscnt(0),ccnt(0);

    // add the counters of all threads representing a front
    // each thread counts the states it has added to the global storage
	for(index_t i=0; i<nr_of_fronts; ++i)
	{
		trcnt += **(transient_count[i]);
		pscnt += **(persistent_count[i]);
		ccnt += **(call_count[i]);
	}

    // adapt the maximum for transient states if necessary
	if (trcnt<max_transient_count) trcnt=max_transient_count;
	else max_transient_count=trcnt;

    // save the counter values, no distinguishing transient and persistent states possible
	count_store.setMarkings(trcnt+pscnt);
	count_store.setCalls(ccnt);
}

