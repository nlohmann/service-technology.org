/*!
\file ParallelExploration.h
\author Gregor
\status new

 \brief Evaluates simple property (only SET of states needs to be computed).
 The actual property is an parameter. The evaluation of the property will be done
 by a parallel exploration of the state space.
*/

#pragma once

#include "Exploration/SearchStack.h"
#include "Exploration/DFSExploration.h"
#include <semaphore.h>
#include <Net/NetState.h>

class Firelist;
class Store;
class EmptyStore;

class ParallelExploration : public DFSExploration
{
    public:
        /// evaluate property by dfs. Result true = state found, false = state not found
        virtual bool depth_first(SimpleProperty &property, NetState &ns, Store &, FireListCreator &firelistcreator, int threadNumber);


        sem_t* restartSemaphore;
        // needs to be a semaphore to be able to simply delete it
        sem_t* transfer_finished_semaphore;
        bool finished;
        // mutex for access to num_suspended AND the two transfer variables
        pthread_mutex_t num_suspend_mutex;
        int num_suspended;

        pthread_mutex_t send_mutex;
        SearchStack transfer_stack;
        NetState* transfer_netstate;
        SimpleProperty* transfer_property;

        // mutex to control writing back to current marking
        pthread_mutex_t write_current_back_mutex;
    protected:


    private:
        pthread_t* runner_thread;
        static void* threadPrivateDFS(void* container);

        // this will either return NULL (no state fullfilling the property has been found) or the witness state itself
        NetState* threadedExploration(NetState &ns, Store &myStore, FireListCreator &fireListCreator, SimpleProperty* resultProperty, int threadNumber, int number_of_threads);
};
