/*!
\file ParallelExploration.h
\author Gregor
\status new

\brief parallel of the simple-property exploration
*/

#pragma once

#include "Exploration/SearchStack.h"
#include "Exploration/DFSExploration.h"
#include <semaphore.h>
#include <Net/NetState.h>
#include <Stores/Store.h>
#include <Stores/EmptyStore.h>
#include <SweepLine/SweepEmptyStore.h>


class Firelist;


/*!
\brief Evaluates simple property (only SET of states needs to be computed).


The actual property is an parameter.


 */
class ParallelExploration : public DFSExploration
{
public:
    /*!
     \brief evaluate property by parallel-pseude-dfs.

    Evaluates a given property by a parallel pseudo-depth-first-search.
    The result will be
    - true, if a marking fulfilling the property was found
    - false, if all markings have been explored and no such state was found
    - no termination (in time), if the state space is too big and no 'good' marking was found

    This is just a wrapper function, which starts the threads

    Parallel-Pseudo-DFS
    =========================
    The evaluation of the property will be done by a parallel exploration of the state space.
    Therefore \f$n\f$ threads will be started with the initial marking.\n
    The store used in the search has to be thread-safe, if it is not this function may crash or return wrong results.
    It would be preferable if the store would give the absolute correct answer, but this is not necessary.
    Absolute correct here means, that if a thread has already inserted a marking, all other threads will get the answer "marking already present".
    It is possible for the store to give in this case an incorrect answer, but this will slow down the search.\n
    If the search of an thread has ended, that means there are no successor-markings which have not already been visited by this or any other thread, the search will be restarted.
    If so, an other thread will spare one of its transitions its current firelist, but only if this would lead to a new state, and the spearing-thread will have one other transition left.
    (Assuming there is not always only one transition to fire, but then parallelization is useless)
    The state resulting in firing this transition will be transfered to the other thread, which will continue the search at this position.\n
    All threads will poll a variable to be able to abort if one of the threads has found a solution.

    Argument description
    -------------------------
    \param property the property to check
    \param ns The initial state of the net has to be given as a net state object. \n
    			If the search has found a state, fulfilling the property this state will be returned in this parameter.
    \param store the store to be used. The selection of the store may greatly influence the performance of the program.\n
    	It is __imperative__ to use a thread-safe store.
    \param firelist the firelists to use in this search.
    		The firelist _must_ be applicable to the given property, else the result of this function may be wrong.\n
    		It is not guaranteed that the given firelist will actually be used.
    		In the parallel work-mode the given list will just be used as a base list form which all other lists will be generated
    \param This parameter indicates the number of threads to be used for the search
     */
    virtual bool depth_first(SimpleProperty &property, NetState &ns, Store<void> &store,
                             Firelist &firelist, int threadNumber);

    /*!
    \brief distributed evaluation of the property by the sweepline method.
    \param property the property for which a witness state should be found
    \param ns the net state of the initial marking
    \param myStore A dummy store for counting markings
    \param myFirelist the object used to generate the actual firelists, this must correspond with given property
    \param number_of_threads indicates the number of threads to be used for the search
    \return if a witness state was found
    */
    bool sweepline(SimpleProperty &property, NetState &ns, SweepEmptyStore &myStore, Firelist &firelist,
                   int number_of_fronts, int number_of_threads);

    virtual ~ParallelExploration() {}
protected:


private:
    /// semaphore used to signal that a transition is able to be spared, one for each thread
    sem_t **restartSemaphore;

    /// if true one of the threads has found a marking, which satisfies the property
    bool finished;

    /// mutex to access the num_suspended variable
    pthread_mutex_t num_suspend_mutex;
    /// number of threads currently suspended
    int num_suspended;
    /// array of suspended thread
    index_t *suspended_threads;

    /// search stacks, one for each thread
    SearchStack<SimpleStackEntry> *thread_stack;
    /// NetStates, one for each thread
    NetState *thread_netstate;
    /// goal properties, one for each thread
    SimpleProperty **thread_property;

    SimpleProperty *global_property;
    /// initial firelist, all used firelists will be created from this one.
    Firelist *global_baseFireList;
    Store<void> *global_store;

    /// mutex to control writing to current marking varible, which contains the result of the parallel search
    pthread_mutex_t global_property_mutex;

    /// array containing the threads actually used for the parallel exploration
    pthread_t *threads;
    int number_of_threads;

    /*!
     \brief man-in-the-middle function to transform the thread-call into a usable function

     The thread-interface of C/C++ allows only functions with the signature

     	 void* f(void*)

     to be used as entry-function for a new thread.
     This method transforms this call into a call to the `threadedExploration`-function having better and structured arguments.

     \param container a simple struct-container to all parameters
     \return a pointer to a net-state fulfilling the property or null.
     	 Only this value will be determined to decide, whether the search has found something or not.
     */
    static void *threadPrivateDFS(void *container);

    /*!
    \brief parallel execution of the DFS

    This is the inner function that actually does the parallel DFS.
    An algorithmic description can be found in the description of the `depth_first`-function.

    \param threadNumber the number of the thread for which this function is called
    \param number_of_threads the total number of threads used

    \return this will either return NULL (no state fulfilling the property has been found by this thread) or the witness state itself
    */
    NetState *threadedExploration(index_t threadNumber);
};
