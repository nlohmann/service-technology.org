/*!
\file ParallelExploration.cc
\author Gregor
\status new
\brief Evaluates simple property (only SET of states needs to be computed) in parallel.
*/

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sstream>
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


extern Reporter *rep;


/// transfer struct for the start of a parallel search thread
struct tpDFSArguments
{
    /// the store to use (this will be the same of all threads)
    /// the number of the current thread
    int threadNumber;
    /// the exploration object
    ParallelExploration *pexploration;
};

void *ParallelExploration::threadPrivateDFS(void *container)
{
    tpDFSArguments *arguments = (tpDFSArguments *) container;

    return arguments->pexploration->threadedExploration(arguments->threadNumber);
}

NetState *ParallelExploration::threadedExploration(index_t threadNumber)
{

    SimpleProperty *&local_property = thread_property[threadNumber];
    Firelist *local_firelist = global_baseFireList->createNewFireList(local_property);

    SearchStack<SimpleStackEntry> &local_stack = thread_stack[threadNumber];

    NetState &local_netstate = thread_netstate[threadNumber];

    // prepare property
    bool propertyResult = local_property->initProperty(local_netstate);

    if (propertyResult)
    {
        // initial marking satisfies property
        // inform all threads that a satisfying has been found
        finished = true;
        // release all semaphores
        for (int i = 0; i < number_of_threads; i++)
        {
            sem_post(restartSemaphore[i]);
        }

        // return success and the current stack and property
        pthread_mutex_lock(&global_property_mutex);
        global_property->stack.swap(local_stack);
        global_property->initProperty(local_netstate);
        pthread_mutex_unlock(&global_property_mutex);

        delete local_firelist;

        return &local_netstate;
    }

    // add initial marking to store
    // we do not care about return value since we know that store is empty
    global_store->searchAndInsert(local_netstate, 0, threadNumber);

    // get first firelist
    index_t *currentFirelist;
    index_t currentEntry = local_firelist->getFirelist(local_netstate, &currentFirelist);

    while (true) // exit when trying to pop from empty stack
    {
        // if one of the threads sets the finished signal this thread is useless and has to abort
        if (finished)
        {
            // clean up and return
            delete[] currentFirelist;
            delete local_firelist;

            return NULL;
        }
        if (currentEntry-- > 0)
        {
            // there is a next transition that needs to be explored in current marking
            // fire this transition to produce new marking in ns
            Transition::fire(local_netstate, currentFirelist[currentEntry]);

            if (global_store->searchAndInsert(local_netstate, 0, threadNumber))
            {
                // State exists! --> backtrack to previous state and try again
                Transition::backfire(local_netstate, currentFirelist[currentEntry]);
                continue;
            }
            // State does not exist!

            // check whether the new state fulfills the property
            Transition::updateEnabled(local_netstate, currentFirelist[currentEntry]);
            // check current marking for property
            propertyResult = local_property->checkProperty(local_netstate, currentFirelist[currentEntry]);

            if (propertyResult)
            {
                // inform all threads that a satisfying state has been found
                finished = true;
                // release all semaphores
                for (int i = 0; i < number_of_threads; i++)
                {
                    sem_post(restartSemaphore[i]);
                }

                // return success and the current stack and property
                pthread_mutex_lock(&global_property_mutex);
                global_property->stack.swap(local_stack);
                global_property->initProperty(local_netstate);
                pthread_mutex_unlock(&global_property_mutex);

                // clean up and return
                delete[] currentFirelist;
                delete local_firelist;

                return &local_netstate;
            }

            // push the old firelist onto the stack (may be needed to hand over the current state correctly)
            new (local_stack.push()) SimpleStackEntry(currentFirelist, currentEntry);

            // if possible spare the current transition to give it to an other thread, having explored it's part of the search space completely
            // try the dirty read to make the program more efficient
            // most of the time this will already fail and we have saved the time needed to lock the mutex
            // ... do it only if there are at least two transitions in the firelist left (one for us, and one for the other thread)

            if (currentEntry >= 2 && num_suspended > 0)
            {
                // try to lock
                pthread_mutex_lock(&num_suspend_mutex);
                if (num_suspended > 0)
                {
                    // there is another thread waiting for my data, get its thread number
                    index_t reader_thread_number = suspended_threads[--num_suspended];
                    pthread_mutex_unlock(&num_suspend_mutex);

                    // the destination thread is blocked at this point, waiting our data
                    // copy the data for the other thread
                    thread_stack[reader_thread_number] = local_stack;
                    NetState tmp_netstate(local_netstate);
                    thread_netstate[reader_thread_number].swap(tmp_netstate);
                    delete thread_property[reader_thread_number];
                    thread_property[reader_thread_number] = local_property->copy();

                    // inform the other thread that the data is ready
                    sem_post(restartSemaphore[reader_thread_number]);

                    // LCOV_EXCL_START
                    if (finished)
                    {
                        // clean up and return
                        delete local_firelist;

                        return NULL;
                    }
                    // LCOV_EXCL_STOP

                    // backfire the current transition to return to original state
                    local_stack.pop();
                    assert(currentEntry < Net::Card[TR]);
                    Transition::backfire(local_netstate, currentFirelist[currentEntry]);
                    Transition::revertEnabled(local_netstate, currentFirelist[currentEntry]);
                    local_property->updateProperty(local_netstate, currentFirelist[currentEntry]);
                    // go on as nothing happened (i.e. pretend the new marking has already been in the store)
                    continue;
                }
                pthread_mutex_unlock(&num_suspend_mutex);
            }
            // current marking does not satisfy property --> continue search
            // grab a new firelist (old one is already on stack)
            currentEntry = local_firelist->getFirelist(local_netstate, &currentFirelist);

        }
        else
        {
            // firing list completed -->close state and return to previous state
            delete[] currentFirelist;

            if (local_stack.StackPointer)
            {
                // if there is still a firelist, which can be popped, do it!
                SimpleStackEntry &s = local_stack.top();
                currentEntry = s.current;
                currentFirelist = s.fl;
                local_stack.pop();
                assert(currentEntry < Net::Card[TR]);
                Transition::backfire(local_netstate, currentFirelist[currentEntry]);
                Transition::revertEnabled(local_netstate, currentFirelist[currentEntry]);
                propertyResult = local_property->updateProperty(local_netstate, currentFirelist[currentEntry]);
                continue;
            }

            // we have completely processed local initial marking --> state not found in current sub-tree

            // delete firelist (we will get new ones if we start again in a different sub-tree)
            delete local_firelist;

            // maybe we have to go into an other sub-tree of the state-space
            // first get the counter mutex to be able to count the number of threads currently suspended
            pthread_mutex_lock(&num_suspend_mutex);
            suspended_threads[num_suspended++] = threadNumber;
            int local_num_suspended = num_suspended;
            pthread_mutex_unlock(&num_suspend_mutex);

            // if we are the last thread going asleep, we have to wake up all the others and tell them that the search is over
            if (local_num_suspended == number_of_threads)
            {
                finished = true;
                // release all semaphores, wake up all threads
                for (int i = 0; i < number_of_threads; i++)
                {
                    sem_post(restartSemaphore[i]);
                }
                // there is no such state
                return NULL;
            }
            sem_wait(restartSemaphore[threadNumber]);

            // test if result is already known now
            // LCOV_EXCL_START
            if (finished)
            {
                return NULL;
            }
            // LCOV_EXCL_STOP

            // rebuild
            local_property->initProperty(local_netstate);
            local_firelist = global_baseFireList->createNewFireList(local_property);

            // re-initialize the current search state
            currentEntry = local_firelist->getFirelist(local_netstate, &currentFirelist);
        }
    }
}

bool ParallelExploration::depth_first(SimpleProperty &property, NetState &ns,
                                      Store<void> &myStore, Firelist &firelist,
                                      int _number_of_threads)
{

    // allocate space for threads
    threads = new pthread_t[_number_of_threads]();

    global_property = &property;
    global_store = &myStore;
    global_baseFireList = &firelist;
    number_of_threads = _number_of_threads;

    // allocate space for thread arguments
    tpDFSArguments *args = new tpDFSArguments[number_of_threads]();
    for (int i = 0; i < number_of_threads; i++)
    {
        args[i].threadNumber = i;
        args[i].pexploration = this;
    }

    // allocate and initialize thread local information
    thread_netstate = new NetState[number_of_threads];
    thread_property = new SimpleProperty*[number_of_threads];
    thread_stack = new SearchStack<SimpleStackEntry>[number_of_threads];
    for (int i = 0; i < number_of_threads; i++)
    {
        NetState tmp_netstate(ns);
        thread_netstate[i].swap(tmp_netstate); // copy and swap
        thread_property[i] = property.copy();
    }

    // allocate restart semaphore names (named semaphores used for MacOS compatibility)
    std::string *restartSemaphoreName = new std::string[number_of_threads];
    for (int i = 0; i < number_of_threads; i++)
    {
        std::stringstream out;
        out << "PErestart" << i;
        restartSemaphoreName[i] = out.str();
    }

    // try to clear restart semaphore first
    for (int i = 0; i < number_of_threads; i++)
    {
        sem_unlink(restartSemaphoreName[i].c_str());
    }

    // init the restart semaphore
    restartSemaphore = new sem_t*[number_of_threads];
    int sem_fail = 0;
    int value_prober;
    for (int i = 0; i < number_of_threads; i++)
    {
        restartSemaphore[i] = sem_open(restartSemaphoreName[i].c_str(), O_CREAT, 0600, 0);
        sem_fail |= (!(long int)restartSemaphore[i]);
        sem_getvalue(restartSemaphore[i], &value_prober);
        if (value_prober != 0)
        {
            rep->status("named semaphore (PErestart) has not been created as defined");
            sem_wait(restartSemaphore[i]); // hack: decrement semaphore in order to eventually eliminate the problem
            rep->abort(ERROR_THREADING);
        }
    }
    // LCOV_EXCL_START
    if (UNLIKELY(sem_fail))
    {
        rep->status("named semaphores could not be created");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

    // initialize mutexes
    int mutex_creation_status = 0;
    mutex_creation_status |= pthread_mutex_init(&num_suspend_mutex, NULL);
    mutex_creation_status |= pthread_mutex_init(&global_property_mutex, NULL);
    // LCOV_EXCL_START
    if (UNLIKELY(mutex_creation_status))
    {
        rep->status("mutexes could not be created");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

    // initialize thread intercommunication data structures
    num_suspended = 0;
    suspended_threads = new index_t[number_of_threads];
    finished = false;

    // start the threads
    for (int i = 0; i < number_of_threads; i++)
        if (UNLIKELY(pthread_create(threads + i, NULL, threadPrivateDFS, args + i)))
        {
            // LCOV_EXCL_START
            rep->status("threads could not be created");
            rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }

    //// THREADS ARE RUNNING AND SEARCHING

    // wait for all threads to finish
    property.value = false;
    for (int i = 0; i < number_of_threads; i++)
    {
        void *return_value;
        if (UNLIKELY(pthread_join(threads[i], &return_value)))
        {
            // LCOV_EXCL_START
            rep->status("threads could not be joined");
            rep->abort(ERROR_THREADING);
            // LCOV_EXCL_STOP
        }
        if (return_value)
        {
            property.value = true;
            ns = *(NetState *) return_value;
        }
    }

    // clean up mutexes
    int mutex_destruction_status = 0;
    mutex_destruction_status |= pthread_mutex_destroy(&num_suspend_mutex);
    mutex_destruction_status |= pthread_mutex_destroy(&global_property_mutex);
    // LCOV_EXCL_START
    if (UNLIKELY(mutex_destruction_status))
    {
        rep->status("mutexes could not be destroyed");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

    // clean up semaphores
    int semaphore_destruction_status = 0;
    for (int i = 0; i < number_of_threads; i++)
    {
        semaphore_destruction_status |= sem_close(restartSemaphore[i]);
        semaphore_destruction_status |= sem_unlink(restartSemaphoreName[i].c_str());
    }
    // LCOV_EXCL_START
    if (UNLIKELY(semaphore_destruction_status))
    {
        rep->status("named semaphore could not be closed and/or unlinked");
        rep->abort(ERROR_THREADING);
    }
    // LCOV_EXCL_STOP

    // clean up thread intercommunication data structures
    for (int i = 0; i < number_of_threads; i++)
    {
        delete thread_property[i];
    }
    delete[] suspended_threads;
    delete[] thread_netstate;
    delete[] thread_property;
    delete[] thread_stack;

    // clean up thread arguments
    delete[] args;

    // free the allocated memory
    delete[] restartSemaphoreName;
    delete[] restartSemaphore;
    delete[] threads;

    return property.value;
}

bool ParallelExploration::sweepline(SimpleProperty &property, NetState &ns,
                                    SweepEmptyStore &myStore, Firelist &myFirelist, int frontNumber, int threadNumber)
{
    Sweep<void> s(property, ns, myStore, myFirelist, frontNumber, threadNumber);
    return s.runThreads();
}

