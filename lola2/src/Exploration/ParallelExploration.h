/*!
\file ParallelExploration.h
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
        SearchStack<SimpleStackEntry> transfer_stack;
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
