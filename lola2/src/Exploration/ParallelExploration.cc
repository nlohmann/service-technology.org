/*!
 \file ParallelExploration.cc
 \author Gregor
 \status new

 \brief Evaluates simple property (only SET of states needs to be computed).
 		The actual property is an parameter.
 		The evaluation of the property will be done by a parallel exploration of the state space.
 		Therefore n threads will be started with the initial marking. The given store has to be thread-safe. If it is not this search may crash.
		It would be preferable if the store would give the absolute correct answer, but this is not necessary.
		If the search of an thread has ended (because there are no successor-markings, that have not been visited by none of the threads[Meaning: other threads can cut parts of the search-space of other threads]), the search will be restarted.
		If so, an other thread will spare one of its transitions currently in the current firelist, but only if this would lead to a new state, and the spearing-thread will have one other transition left. (Assuming there is not always only one transition to fire, but then parallelization is useless)
		The state resulting in firering this transition will be transfered to the other thread, which will continue the search at this position.
		All threads will poll a variable to be able to abort if one of the threads has found a solution.
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
#include <InputOutput/Reporter.h>
#include <cmdline.h>
#include <InputOutput/Reporter.h>

extern gengetopt_args_info args_info;
extern Reporter* rep;

struct tpDFSArguments {
    NetState* ns;
    Store<void>* myStore;
    FireListCreator* fireListCreator;
    SimpleProperty* resultProperty;
    int threadNumber;
    int number_of_threads;
    ParallelExploration* pexploration;
};

void* ParallelExploration::threadPrivateDFS(void* container) {
    tpDFSArguments* arguments = (tpDFSArguments*) container;
    NetState &ns = *arguments->ns;
    Store<void> &myStore = *arguments->myStore;
    FireListCreator &fireListCreator = *arguments->fireListCreator;
    int threadNumber = arguments->threadNumber;
    int number_of_threads = arguments->number_of_threads;
    SimpleProperty* resultProperty = arguments->resultProperty;

    return arguments->pexploration->threadedExploration(ns,
            myStore, fireListCreator, resultProperty, threadNumber,
            number_of_threads);
}

NetState* ParallelExploration::threadedExploration(NetState &ns, Store<void> &myStore,
        FireListCreator &fireListCreator, SimpleProperty* resultProperty,
        int threadNumber, int number_of_threads) {
    SimpleProperty* sp = resultProperty->copy();
    Firelist* myFirelist = fireListCreator.createFireList(sp);
    /// the search stack
    SearchStack<SimpleStackEntry> stack;

    // prepare property
    bool localValue = sp->initProperty(ns);

    if (localValue) {
        // initial marking satisfies property
        // inform all threads that we have finished
        finished = true;
        // release all semaphores
        for (int i = 0; i < number_of_threads; i++)
            sem_post(restartSemaphore);
        sem_post(transfer_finished_semaphore);
        // return success
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

    while (true) // exit when trying to pop from empty stack
    {
        // if one of the threads sends the finished signal this thread is useless and has to abort
        if (finished) {
            // the current firelist it at the moment not on the stack, so will not be deleted by the stack
            delete[] currentFirelist;
            // delete the sp&firelist
            delete myFirelist;
            delete sp;
            return NULL;
        }
        if (currentEntry-- > 0) {
            // there is a next transition that needs to be explored in current marking

            // fire this transition to produce new Marking::Current
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
                localValue = sp->checkProperty(ns,
                                               currentFirelist[currentEntry]);
                if (localValue) {
                    // current  marking satisfies property
                    // push put current transition on stack
                    // this way, the stack contains ALL transitions
                    // of witness path
                    SimpleStackEntry * s = stack.push();
                    s = new (s) SimpleStackEntry(currentFirelist, currentEntry);
                    // end the DFS
                    // inform all threads that we have finished
                    finished = true;
                    // release all semaphores
                    for (int i = 0; i < number_of_threads; i++)
                        sem_post(restartSemaphore);
                    sem_post(transfer_finished_semaphore);
                    // return success
                    pthread_mutex_lock(&write_current_back_mutex);
                    resultProperty->stack = stack;
                    resultProperty->initProperty(ns);
                    pthread_mutex_unlock(&write_current_back_mutex);
                    // delete the sp&firelist
                    delete myFirelist;
                    delete sp;
                    return &ns;
                }
                // push the transition onto the stack
                SimpleStackEntry * s = stack.push();
                s = new (s) SimpleStackEntry(currentFirelist, currentEntry);

                // first try the dirty read to make the program more efficient
                // but do it only if there are at least two transitions in the firelist left (one for us, and one for the other thread)
                if (currentEntry >= 2 && num_suspended > 0) {
                    pthread_mutex_lock(&num_suspend_mutex);
                    if (num_suspended > 0) {
                        //  i know that there is an other thread waiting for my data
                        num_suspended--;
                        pthread_mutex_unlock(&num_suspend_mutex);
                        pthread_mutex_lock(&num_suspend_mutex);
                        // if the end is reached abort this thread
                        // exclude this from code coverage, as I can not provoke it, but is necessary in very rare occasions
                        // LCOV_EXCL_START
                        if (finished) {
                            // delete the firelist
                            delete myFirelist;
                            delete sp;
                            return NULL;
                        }
                        // LCOV_EXCL_STOP

                        // copy the data for the other thread
                        transfer_stack = stack;
                        transfer_netstate = new NetState(ns);
                        transfer_property = sp->copy();

                        sem_post(restartSemaphore);
                        sem_wait(transfer_finished_semaphore);

                        // backfire the current transition to return to original state
                        SimpleStackEntry & s = stack.top();
                        currentEntry = s.current;
                        currentFirelist = s.fl;
                        stack.pop();
                        assert(currentEntry < Net::Card[TR]);
                        Transition::backfire(ns, currentFirelist[currentEntry]);
                        Transition::revertEnabled(ns,
                                                  currentFirelist[currentEntry]);
                        localValue = sp->updateProperty(ns,
                                                        currentFirelist[currentEntry]);
                        // go on as nothing happened
                        pthread_mutex_unlock(&num_suspend_mutex);
                        continue;
                    }
                    pthread_mutex_unlock(&num_suspend_mutex);
                }

                // Here: current marking does not satisfy property --> continue search
                currentEntry = myFirelist->getFirelist(ns, &currentFirelist);
            } // end else branch for "if state exists"
        } else {
            // firing list completed -->close state and return to previous state
            delete[] currentFirelist;
            if (stack.StackPointer == 0) {
                // have completely processed local initial marking --> state not found in current sub-tree

                // maybe we have to go into an other sub-tree of the state-space
                // first get the counter mutex to be able to count the number of threads currently suspended
                pthread_mutex_lock(&num_suspend_mutex);
                num_suspended++;
                // then we are the last thread going asleep and so we have to await all the others and tell them that the search is over
                if (num_suspended == number_of_threads) {
                    finished = true;
                    // release all semaphores
                    for (int i = 0; i < number_of_threads; i++)
                        sem_post(restartSemaphore);
                    sem_post(transfer_finished_semaphore);
                    pthread_mutex_unlock(&num_suspend_mutex);
                    // delete the sp&firelist
                    delete myFirelist;
                    delete sp;
                    // there is no such state
                    return false;
                }
                pthread_mutex_unlock(&num_suspend_mutex);
                sem_wait(restartSemaphore);
                // if the search has come to an end return without success
                // exclude this from code coverage, as I can not provoke it, but is necessary in very rare occasions
                // LCOV_EXCL_START
                if (finished) {
                    // delete the firelist
                    delete myFirelist;
                    delete sp;
                    return NULL;
                }
                // LCOV_EXCL_STOP


                // now we have been signaled because one of the threads is able to give us an part of search space
                delete sp;
                delete myFirelist;
                stack = transfer_stack;
                sp = transfer_property;
                ns = *transfer_netstate;
                delete transfer_netstate;
                // rebuild
                sp->initProperty(ns);
                myFirelist = fireListCreator.createFireList(sp);
                // my sender holds the lock that authorizes me to decrease the variable
                sem_post(transfer_finished_semaphore);

                // re-initialize the current search state
                currentEntry = myFirelist->getFirelist(ns, &currentFirelist);

                continue;

            }
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
                                      Store<void> &myStore, FireListCreator &firelistcreator,
                                      int number_of_threads) {
    // copy initial marking into current marking
    //Marking::init();

    runner_thread = (pthread_t*) calloc(number_of_threads, sizeof(pthread_t));
    tpDFSArguments* args = (tpDFSArguments*) calloc(number_of_threads,
                           sizeof(tpDFSArguments));
    for (int i = 0; i < number_of_threads; i++) {
        args[i].fireListCreator = &firelistcreator;
        args[i].myStore = &myStore;
        args[i].ns = new NetState(ns);
        args[i].threadNumber = i;
        args[i].number_of_threads = number_of_threads;
        args[i].pexploration = this;
        args[i].resultProperty = &property;
    }
    // init the restart semaphore
    sem_unlink("PErestart");
    restartSemaphore = sem_open("PErestart", O_CREAT, 0600,
                                0);
    transfer_finished_semaphore = sem_open("PEtranscom", O_CREAT, 0600,
                                           0);
    // LCOV_EXCL_START
    if (UNLIKELY(!(long int)restartSemaphore)) {
        rep->status("named semaphore could not be created");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

    int mutex_creation_status = 0;
    mutex_creation_status |= pthread_mutex_init(&num_suspend_mutex, NULL);
    mutex_creation_status |= pthread_mutex_init(&send_mutex, NULL);
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
    mutex_destruction_status |= pthread_mutex_destroy(&send_mutex);
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
