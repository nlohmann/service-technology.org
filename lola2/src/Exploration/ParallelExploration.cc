/*!
 \file SimpleProperty.cc
 \author Karsten
 \status approved 23.05.2012

 \brief Evaluates simple property (only SET of states needs to be computed).
 Actual property is virtual, default (base class) is full exploration
 */

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <semaphore.h>
#include <fcntl.h>
#include "Net/Marking.h"
#include "Net/Place.h"
#include "Net/Transition.h"
#include "Net/Net.h"
#include "Exploration/ParallelExploration.h"
#include "Exploration/Firelist.h"
#include "Stores/Store.h"
#include "Stores/EmptyStore.h"
#include "InputOutput/Reporter.h"
#include "cmdline.h"
#include "Stores/ThreadSafeStore.h"
#include "Stores/SIBinStore2.h"

extern gengetopt_args_info args_info;
extern Reporter* rep;

struct tpDFSArguments
{
    NetState* ns;
    Store* myStore;
    FireListCreator* fireListCreator;
    SimpleProperty* resultProperty;
    int threadNumber;
    int number_of_threads;
    ParallelExploration* pexploration;
};

void* ParallelExploration::threadPrivateDFS(void* container)
{
    NetState &ns = *((tpDFSArguments*) container)->ns;
    Store &myStore = *((tpDFSArguments*) container)->myStore;
    FireListCreator &fireListCreator = *((tpDFSArguments*) container)->fireListCreator;
    int threadNumber = ((tpDFSArguments*) container)->threadNumber;
    int number_of_threads = ((tpDFSArguments*) container)->number_of_threads;
    SimpleProperty* resultProperty = ((tpDFSArguments*) container)->resultProperty;

    return ((tpDFSArguments*) container)->pexploration->threadedExploration(ns, myStore, fireListCreator, resultProperty, threadNumber, number_of_threads);
}


NetState* ParallelExploration::threadedExploration(NetState &ns, Store &myStore, FireListCreator &fireListCreator, SimpleProperty* resultProperty, int threadNumber, int number_of_threads)
{
	SimpleProperty* sp = resultProperty->copy();
    Firelist* myFirelist = fireListCreator.createFireList(sp);
    /// the search stack
    SearchStack stack;

    // prepare property
    bool localValue = sp->initProperty(ns);

    if (localValue)
    {
        // initial marking satisfies property
        // inform all threads that we have finished
        finished = true;
        // release all semaphores
        for (int i = 0; i < number_of_threads; i++)
            sem_post(restartSemaphore);
        pthread_mutex_unlock(&transfer_finished_mutex);
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

    myStore.searchAndInsert(ns, threadNumber);

    // get first firelist
    index_t* currentFirelist;
    index_t currentEntry = myFirelist->getFirelist(ns, &currentFirelist);

    while (true) // exit when trying to pop from empty stack
    {
        // if one of the threads sends the finished signal this thread is useless and has to abort
        if (finished)
        {
        	// the current firelist it at the moment not on the stack, so will not be deleted by the stack
        	delete[] currentFirelist;
            // delete the sp&firelist
            delete myFirelist;
            delete sp;
            return NULL;
        }
        //printf("GOON %x NS %x\n", container, ns);
        if (currentEntry -- > 0)
        {
            // printf("%d ANA %d\n",threadNumber, currentEntry);
            // there is a next transition that needs to be explored in current marking

            // fire this transition to produce new Marking::Current
            Transition::fire(ns, currentFirelist[currentEntry]);

            if (myStore.searchAndInsert(ns, threadNumber))
            {
                // State exists! -->backtracking to previous state
                Transition::backfire(ns, currentFirelist[currentEntry]);
            }
            else
            {
                // State does not exist!
                // continue with next transition
                // test whether it is a transition that satisfies the property and if return
                Transition::updateEnabled(ns, currentFirelist[currentEntry]);
                // check current marking for property
                localValue = sp->checkProperty(ns, currentFirelist[currentEntry]);
                if (localValue)
                {
                    // current  marking satisfies property
                    // push put current transition on stack
                    // this way, the stack contains ALL transitions
                    // of witness path
                    stack.push(currentEntry, currentFirelist);
                    // end the DFS
                    // inform all threads that we have finished
                    finished = true;
                    // release all semaphores
                    for (int i = 0; i < number_of_threads; i++)
                        sem_post(restartSemaphore);
                    pthread_mutex_unlock(&transfer_finished_mutex);
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
                stack.push(currentEntry, currentFirelist);


                // first try the dirty read to make the program more efficient
                // but do it only if there are at least two transitions in the firelist left (one for us, and one for the other thread)
                if (currentEntry >= 2 && num_suspended > 0)
                {
                    pthread_mutex_lock(&num_suspend_mutex);
                    if (num_suspended > 0)
                    {
                        //  i know that there is an other thread waiting for my data
                        num_suspended--;
                        pthread_mutex_unlock(&num_suspend_mutex);
                        pthread_mutex_lock(&num_suspend_mutex);
                        // if the end is reached abort this thread
                        if (finished){
                            // delete the sp&firelist
                            delete myFirelist;
                            delete sp;
                            return NULL;
                        }

                        // copy the data for the other thread
                        transfer_stack = stack;
                        transfer_netstate = NetState::createNetStateFromCurrent(ns);
                        transfer_property = sp->copy();

                        sem_post(restartSemaphore);
                        pthread_mutex_lock(&transfer_finished_mutex);

                        // backfire the current transition to return to original state
                        stack.pop(&currentEntry, &currentFirelist);
                        assert(currentEntry < Net::Card[TR]);
                        Transition::backfire(ns, currentFirelist[currentEntry]);
                        Transition::revertEnabled(ns, currentFirelist[currentEntry]);
                        localValue = sp->updateProperty(ns, currentFirelist[currentEntry]);
                        // go on as nothing happened
                        pthread_mutex_unlock(&num_suspend_mutex);
                        continue;
                    }
                    pthread_mutex_unlock(&num_suspend_mutex);
                }

                //printf("%d DOWN\n", threadNumber);
                // Here: current marking does not satisfy property --> continue search
                currentEntry = myFirelist->getFirelist(ns, &currentFirelist);
            } // end else branch for "if state exists"
        }
        else
        {
            // firing list completed -->close state and return to previous state
            delete[] currentFirelist;
            if (stack.StackPointer == 0)
            {
                // have completely processed local initial marking --> state not found in current sub-tree

                // maybe we have to go into an other sub-tree of the state-space
                // first get the counter mutex to be able to count the number of threads currently suspended
                pthread_mutex_lock(&num_suspend_mutex);
                num_suspended++;
                // then we are the last thread going asleep and so we have to await all the others and tell them that the search is over
                if (num_suspended == number_of_threads)
                {
                    finished = true;
                    // release all semaphores
                    for (int i = 0; i < number_of_threads; i++)
                        sem_post(restartSemaphore);
                    pthread_mutex_unlock(&transfer_finished_mutex);
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
                if (finished)
                {
                    // delete the sp&firelist
                    delete myFirelist;
                    delete sp;
                    return NULL;
                }
                // now we have been signaled because one of the threads is able to give us an part of search space

                delete sp;
                delete myFirelist;
                stack = transfer_stack;
                sp = transfer_property;
                ns.copyNetState(*transfer_netstate);
                delete transfer_netstate;
                // rebuild
                sp->initProperty(ns);
                myFirelist = fireListCreator.createFireList(sp);
                // my sender holds the lock that authorizes me to decrease the variable
                pthread_mutex_unlock(&transfer_finished_mutex);

                // re-initialize the current search state
                currentEntry = myFirelist->getFirelist(ns, &currentFirelist);

                continue;

            }
            stack.pop(&currentEntry, &currentFirelist);
            assert(currentEntry < Net::Card[TR]);
            Transition::backfire(ns, currentFirelist[currentEntry]);
            Transition::revertEnabled(ns, currentFirelist[currentEntry]);
            localValue = sp->updateProperty(ns, currentFirelist[currentEntry]);
        }
    }
}



bool ParallelExploration::depth_first(SimpleProperty &property, NetState &ns, Store &myStore, FireListCreator &firelistcreator, int number_of_threads)
{
    // copy initial marking into current marking
    //Marking::init();

    runner_thread = (pthread_t*) calloc(number_of_threads,
                                        sizeof(pthread_t));
    tpDFSArguments* args = (tpDFSArguments*) calloc(number_of_threads,
                                                    sizeof(tpDFSArguments));
    for (int i = 0; i < number_of_threads; i++)
    {
        args[i].fireListCreator = &firelistcreator;
        args[i].myStore = &myStore;
        args[i].ns = NetState::createNetStateFromCurrent(ns);
        args[i].threadNumber = i;
        args[i].number_of_threads = number_of_threads;
        args[i].pexploration = this;
        args[i].resultProperty = &property;
    }
    // init the restart semaphore
    restartSemaphore = sem_open("ParallelExploration_restartSem", O_CREAT, 0600, 0);
    pthread_mutex_init(&transfer_finished_mutex, NULL);
    pthread_mutex_init(&num_suspend_mutex, NULL);
    pthread_mutex_init(&send_mutex, NULL);
    pthread_mutex_init(&write_current_back_mutex, NULL);
    pthread_mutex_lock(&transfer_finished_mutex);
    num_suspended = 0;
    finished  = false;


    // create the threads
    for (int i = 0; i < number_of_threads; i++)
    {
        pthread_create(runner_thread + i, NULL, threadPrivateDFS, args + i);
    }

    //// THREADS ARE RUNNING AND SEARCHING


    // wait for all threads to finish
    property.value = false;
    for (int i = 0; i < number_of_threads; i++)
    {
        void* return_value;
        pthread_join(runner_thread[i], &return_value);
        if (return_value)
        {
            property.value = true;
            ns.copyNetState(*(NetState*)return_value);
        }
        delete args[i].ns;
    }

    // clean up all variables needed to make the parallel DFS
    pthread_mutex_destroy(&num_suspend_mutex);
    pthread_mutex_destroy(&send_mutex);
    pthread_mutex_destroy(&write_current_back_mutex);
    pthread_mutex_destroy(&transfer_finished_mutex);
    sem_close(restartSemaphore);
    sem_unlink("ParallelExploration_restartSem");
    free(runner_thread);
    free(args);

    // finalize the store to thave the corrent number of markings to report
    myStore.finalize();
    return property.value;
}
