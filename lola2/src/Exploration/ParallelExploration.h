/*!
\file SimpleProperty.h
\author Karsten
\status new

\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is virtual, default (base class) is full exploration
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
        virtual bool depth_first(SimpleProperty& property, Store &, FireListCreator &firelistcreator, int threadNumber);


        sem_t restartSemaphore;
        bool finished;
        // mutex for access to num_suspended AND the two transfer variables
        pthread_mutex_t num_suspend_mutex;
        int num_suspended;

        pthread_mutex_t send_mutex;
        sem_t transfer_finished_mutex;
        SearchStack transfer_stack;
        NetState* transfer_netstate;
        SimpleProperty* transfer_property;

        // mutex to control writing back to current marking
        pthread_mutex_t write_current_back_mutex;
    protected:


    private:
        pthread_t* runner_thread;
        static void* threadPrivateDFS(void* container);

        bool threadedExploration(NetState* ns,Store &myStore, FireListCreator &fireListCreator, SimpleProperty* sp, int threadNumber, int number_of_threads, SimpleProperty* resultProperty);
};
