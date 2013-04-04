/*!
\file ParallelExploration.cc
\author Gregor
\status new
\brief Evaluates simple property (only SET of states needs to be computed) in parallel.
*/

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <semaphore.h>
#include <fcntl.h>
#include <Net/Marking.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <Net/Net.h>
#include <Exploration/ParallelExploration.h>
#include <Exploration/Firelist.h>
#include <Stores/Store.h>
#include <Stores/EmptyStore.h>
#include <SweepLine/SweepEmptyStore.h>
#include <SweepLine/Sweep.h>
#include <InputOutput/Reporter.h>
#include <cmdline.h>

extern gengetopt_args_info args_info;
extern Reporter* rep;


/// transfer struct for the start of a parallel search thread
struct tpDFSArguments {
	/// the initial net state
    NetState* ns;
    /// the store to use (this will be the same of all threads)
    Store<void>* myStore;
    /// initial firelist, all used firelists will be created from this one.
    Firelist* baseFireList;
    /// the property to be checked, each thread will create its own copy
    SimpleProperty* resultProperty;
    /// the number of the current thread
    int threadNumber;
    /// the total number of threads
    int number_of_threads;
    /// the exploration object
    ParallelExploration* pexploration;
};

void* ParallelExploration::threadPrivateDFS(void* container) {
    tpDFSArguments* arguments = (tpDFSArguments*) container;
    NetState &ns = *arguments->ns;
    Store<void> &myStore = *arguments->myStore;
    Firelist &baseFireList = *arguments->baseFireList;
    int threadNumber = arguments->threadNumber;
    int number_of_threads = arguments->number_of_threads;
    SimpleProperty* resultProperty = arguments->resultProperty;

    return arguments->pexploration->threadedExploration(ns,
            myStore, baseFireList, resultProperty, threadNumber,
            number_of_threads);
}

NetState* ParallelExploration::threadedExploration(NetState &ns, Store<void> &myStore,
        Firelist &baseFireList, SimpleProperty* resultProperty,
        int threadNumber, int number_of_threads) {

    SimpleProperty* sp = resultProperty->copy();
    Firelist* myFirelist = baseFireList.createNewFireList(sp);
    /// the search stack
    SearchStack<SimpleStackEntry> stack;
    unsigned int started_stack_at = 0;

    // prepare property
    bool localValue = sp->initProperty(ns);


    if (localValue) {
        // initial marking satisfies property
        // inform all threads that a satisfying has been found
        finished = true;
        // release all semaphores
        for (int i = 0; i < number_of_threads; i++)
            sem_post(restartSemaphore);
        sem_post(transfer_finished_semaphore);
        // return success and the current stack and property
        pthread_mutex_lock(&write_current_back_mutex);
        resultProperty->stack = stack;
        resultProperty->initProperty(ns);
        pthread_mutex_unlock(&write_current_back_mutex);
        // delete the sp&firelist
        delete myFirelist;
        delete sp;
        return &ns;
    }

    // add initial marking to store
    // we do not care about return value since we know that store is empty
    myStore.searchAndInsert(ns,0,threadNumber);

    // get first firelist
    index_t* currentFirelist;
    index_t currentEntry = myFirelist->getFirelist(ns, &currentFirelist);
    //rep->message("Thread %d: create %p", threadNumber, (void*)currentFirelist);

    while (true) // exit when trying to pop from empty stack
    {
        // if one of the threads sets the finished signal this thread is useless and has to abort
        if (finished) {
            // the current firelist it at the moment not on the stack, so will it not be deleted by the stack-delete
            delete[] currentFirelist;
            // delete the sp&firelist
            delete myFirelist;
            delete sp;
            return NULL;
        }
        if (currentEntry-- > 0) {
        	// there is a next transition that needs to be explored in current marking
            // fire this transition to produce new marking in ns
            Transition::fire(ns, currentFirelist[currentEntry]);

            if (myStore.searchAndInsert(ns,0,threadNumber)) {
                // State exists! -->backtracking to previous state
                Transition::backfire(ns, currentFirelist[currentEntry]);
            } else {
                // State does not exist!
                // continue with next transition
                // test whether it is a transition that satisfies the property and if return
                Transition::updateEnabled(ns, currentFirelist[currentEntry]);
                // check current marking for property
                localValue = sp->checkProperty(ns, currentFirelist[currentEntry]);
                // push the transition onto the stack
                new (stack.push()) SimpleStackEntry(currentFirelist, currentEntry);

                if (localValue) {
                	// inform all threads that a satisfying state has been found
                    finished = true;
                    // release all semaphores
                    for (int i = 0; i < number_of_threads; i++)
                        sem_post(restartSemaphore);
                    sem_post(transfer_finished_semaphore);
                    // return success and the current stack and property
                    pthread_mutex_lock(&write_current_back_mutex);
                    resultProperty->stack = stack;
                    resultProperty->initProperty(ns);
                    pthread_mutex_unlock(&write_current_back_mutex);
                    // delete the sp&firelist
                    delete myFirelist;
                    delete sp;
                    return &ns;
                }

                // if possible spare the current transition to give it to an other thread, having explored it's part of the search space completely
                // try the dirty read to make the program more efficient
                // most of the time this will already fail and we have saved the time needed to lock the mutex
                // ... do it only if there are at least two transitions in the firelist left (one for us, and one for the other thread)
                if (currentEntry >= 2 && num_suspended > 0) {
                	// try to lock
                    pthread_mutex_lock(&num_suspend_mutex);
                    //rep->message("(%d) TRANSFER LOCK",threadNumber);
                    if (num_suspended > 0) {
                        // i know that there is an other thread waiting for my data
                        num_suspended--;
                        pthread_mutex_unlock(&num_suspend_mutex);

                        // if the end is reached abort this thread
                        // exclude this from code coverage, as I can not provoke it, but is necessary in very rare occasions
                        // LCOV_EXCL_START
                        if (finished) {
                            // delete the sp&firelist
                            delete myFirelist;
                            delete sp;
                            return NULL;
                        }
                        // LCOV_EXCL_STOP

                        // lock transfer mutex
                        pthread_mutex_lock(&transfer_write_access_mutex);
                        // copy the data for the other thread
                        transfer_stack = stack;
                        //rep->message("(%d) TRANSFER STARTED %p @ %d",threadNumber, transfer_stack.currentchunk, transfer_stack.StackPointer);
                        transfer_netstate = new NetState(ns);
                        transfer_property = sp->copy();

                        // inform the other thread that the data is ready
                        //rep->message("\t\t\t\t\t\t\t\t\t\t(%d) TRANSFER RESTART W %d",threadNumber,foo_baz);
                        sem_post(restartSemaphore);
                        // wait until the transfer has been completed
                        sem_wait(transfer_finished_semaphore);
                        //rep->message("\t\t\t\t\t\t\t\t\t\t(%d) TRANSFER FINISHED",threadNumber);
                        // LCOV_EXCL_START
                        if (finished) {
                            // delete the sp&firelist
                            delete myFirelist;
                            delete sp;
                            pthread_mutex_unlock(&num_suspend_mutex);
                            return NULL;
                        }
                        // LCOV_EXCL_STOP

                        // backfire the current transition to return to original state
                        stack.pop();
                        assert(currentEntry < Net::Card[TR]);
                        Transition::backfire(ns, currentFirelist[currentEntry]);
                        Transition::revertEnabled(ns,currentFirelist[currentEntry]);
                        localValue = sp->updateProperty(ns,currentFirelist[currentEntry]);
                        // go on as nothing happened (i.e. do as if the new marking has been in the store)
                        pthread_mutex_unlock(&transfer_write_access_mutex);
                        continue;
                    }
                    pthread_mutex_unlock(&num_suspend_mutex);
                }
                // Here: current marking does not satisfy property --> continue search
                currentEntry = myFirelist->getFirelist(ns, &currentFirelist);
                //->message("Thread %d: create %p", threadNumber, (void*)currentFirelist);
            } // end else branch for "if state is already in store"
        } else {
            // firing list completed -->close state and return to previous state
            delete[] currentFirelist;
            if (stack.StackPointer == started_stack_at) {
                // have completely processed local initial marking --> state not found in current sub-tree

                // maybe we have to go into an other sub-tree of the state-space
                // first get the counter mutex to be able to count the number of threads currently suspended
                pthread_mutex_lock(&num_suspend_mutex);
                //rep->message("(%d) TRANSFER[R] LOCK",threadNumber);
                num_suspended++;
                pthread_mutex_unlock(&num_suspend_mutex);
                //rep->message("(%d) TRANSFER[R] UNLOCK",threadNumber);
                // then we are the last thread going asleep and so we have to await all the others and tell them that the search is over
                if (num_suspended == number_of_threads) {
                    finished = true;
                    // release all semaphores
                    for (int i = 0; i < number_of_threads; i++)
                        sem_post(restartSemaphore);
                    sem_post(transfer_finished_semaphore);
                    //rep->message("(%d) TRANSFER[R] UNLOCK",threadNumber);
                    // delete the sp&firelist
                    delete myFirelist;
                    delete sp;
                    // there is no such state
                    return NULL;
                }
                sem_wait(restartSemaphore);
                //rep->message("\t\t\t\t\t\t\t\t\t\t(%d) TRANSFER[R] RESTART",threadNumber);
                // if the search has come to an end return without success
                // exclude this from code coverage, as I can not provoke it, but is necessary in very rare occasions
                // delete the firelist
                delete myFirelist;
                delete sp;
                // LCOV_EXCL_START
                if (finished) return NULL;
                // LCOV_EXCL_STOP
                // now we have been signaled because one of the threads is able to give us an part of search space
                // copy the data given by the other thread into the local variables
                stack = transfer_stack;
                sp = transfer_property;
                ns = *transfer_netstate;
                delete transfer_netstate;
                started_stack_at = stack.StackPointer;
                // rebuild
                sp->initProperty(ns);
                myFirelist = baseFireList.createNewFireList(sp);
                // my sender holds the lock that authorizes me to decrease the variable
                //rep->message("(%d) TRANSFER[R] COMPLETED (SP %d == %d) stack: %p", threadNumber, started_stack_at, stack.StackPointer, stack.currentchunk);
                //rep->message("\t\t\t\t\t\t\t\t\t\t(%d) TRANSFER[R] FINISHED",threadNumber);
                sem_post(transfer_finished_semaphore);
                //rep->message("(%d) TRANSFER[R] TFINI",threadNumber);

                // re-initialize the current search state
                currentEntry = myFirelist->getFirelist(ns, &currentFirelist);
                //rep->message("Thread %d: create %p", threadNumber, (void*)currentFirelist);
                continue;
            }
            // if there is still a firelist, which can be popped, do it!
            SimpleStackEntry & s = stack.top();
            currentEntry = s.current;
            currentFirelist = s.fl;
            stack.pop();
            assert(currentEntry < Net::Card[TR]);
            Transition::backfire(ns, currentFirelist[currentEntry]);
            Transition::revertEnabled(ns, currentFirelist[currentEntry]);
            localValue = sp->updateProperty(ns, currentFirelist[currentEntry]);
        }
    }
}

bool ParallelExploration::depth_first(SimpleProperty &property, NetState &ns,
                                      Store<void> &myStore, Firelist &firelist,
                                      int number_of_threads) {
    // copy initial marking into current marking
    //Marking::init();

    runner_thread = (pthread_t*) calloc(number_of_threads, sizeof(pthread_t));
    tpDFSArguments* args = (tpDFSArguments*) calloc(number_of_threads,
                           sizeof(tpDFSArguments));
    for (int i = 0; i < number_of_threads; i++) {
        args[i].baseFireList = &firelist;
        args[i].myStore = &myStore;
        args[i].ns = new NetState(ns);
        args[i].threadNumber = i;
        args[i].number_of_threads = number_of_threads;
        args[i].pexploration = this;
        args[i].resultProperty = &property;
    }
    // init the restart semaphore
    sem_unlink("PErestart");
    restartSemaphore = sem_open("PErestart", O_CREAT, 0600,0);
    sem_unlink("PEtranscom");
    transfer_finished_semaphore = sem_open("PEtranscom", O_CREAT, 0600,0);
    // LCOV_EXCL_START
    if (UNLIKELY(!(long int)restartSemaphore) || UNLIKELY(!(long int)transfer_finished_semaphore)) {
        rep->status("named semaphore could not be created");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP
    int value_prober;
    sem_getvalue(transfer_finished_semaphore,&value_prober);
    // LCOV_EXCL_START
    if (value_prober != 0){
    	rep->status("named semaphore (PEtranscom) has not been created as defined");
    	rep->abort(ERROR_THREADING);
    }
    sem_getvalue(restartSemaphore,&value_prober);
    if (value_prober != 0){
    	rep->status("named semaphore (PErestart) has not been created as defined");
    	rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP


    int mutex_creation_status = 0;
    mutex_creation_status |= pthread_mutex_init(&num_suspend_mutex, NULL);
    mutex_creation_status |= pthread_mutex_init(&transfer_write_access_mutex, NULL);
    mutex_creation_status |= pthread_mutex_init(&write_current_back_mutex,
                             NULL);
    // LCOV_EXCL_START
    if (UNLIKELY(mutex_creation_status)) {
        rep->status("mutexes could not be created");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP
    num_suspended = 0;
    finished = false;

    // create the threads
    for (int i = 0; i < number_of_threads; i++)
        if (UNLIKELY(pthread_create(runner_thread + i, NULL, threadPrivateDFS, args + i))) {
            // LCOV_EXCL_START
            rep->status("threads could not be created");
            rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }

    //// THREADS ARE RUNNING AND SEARCHING

    // wait for all threads to finish
    property.value = false;
    for (int i = 0; i < number_of_threads; i++) {
        void* return_value;
        if (UNLIKELY(pthread_join(runner_thread[i], &return_value))) {
            // LCOV_EXCL_START
            rep->status("threads could not be joined");
            rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }
        if (return_value) {
            property.value = true;
            ns = *(NetState*) return_value;
        }
        delete args[i].ns;
    }

    // clean up all variables needed to make the parallel DFS
    int mutex_destruction_status = 0;
    mutex_destruction_status |= pthread_mutex_destroy(&num_suspend_mutex);
    mutex_destruction_status |= pthread_mutex_destroy(&transfer_write_access_mutex);
    mutex_destruction_status |= pthread_mutex_destroy(&write_current_back_mutex);
    // LCOV_EXCL_START
    if (UNLIKELY(mutex_destruction_status)) {
        rep->status("mutexes could not be destroyed");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP
    int semaphore_destruction_status = 0;
    semaphore_destruction_status |= sem_close(transfer_finished_semaphore);
    semaphore_destruction_status |= sem_unlink("PEtranscom");
    semaphore_destruction_status |= sem_close(restartSemaphore);
    semaphore_destruction_status |= sem_unlink("PErestart");
    // LCOV_EXCL_START
    if (UNLIKELY(semaphore_destruction_status)) {
        rep->status("named semaphore could not be closed and/or unlinked");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

    // free the allocated memory
    free(runner_thread);
    free(args);

    return property.value;
}

bool ParallelExploration::sweepline(SimpleProperty &property, NetState &ns, SweepEmptyStore &myStore, Firelist &myFirelist, int frontNumber, int threadNumber)
{
	Sweep<void> s(property, ns, myStore, myFirelist, frontNumber, threadNumber);
	return s.runThreads();
}

