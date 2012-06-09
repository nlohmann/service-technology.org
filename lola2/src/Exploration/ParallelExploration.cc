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
extern Reporter *rep;

struct tpDFSArguments {
	NetState* ns;
	Store* myStore;
	FireListCreator* fireListCreator;
	SimpleProperty* sp;
	SimpleProperty* resultProperty;
	int threadNumber;
	int number_of_threads;
	ParallelExploration* pexploration;
};

void* ParallelExploration::threadPrivateDFS(void* container) {
	NetState* ns = ((tpDFSArguments*) container)->ns;
	Store &myStore = *((tpDFSArguments*) container)->myStore;
	FireListCreator &fireListCreator = *((tpDFSArguments*) container)->fireListCreator;
	SimpleProperty* sp = ((tpDFSArguments*) container)->sp;
	int threadNumber = ((tpDFSArguments*) container)->threadNumber;
	int number_of_threads = ((tpDFSArguments*) container)->number_of_threads;
	SimpleProperty* resultProperty = ((tpDFSArguments*) container)->resultProperty;

	if (((tpDFSArguments*) container)->pexploration->threadedExploration(ns,myStore,fireListCreator, sp, threadNumber, number_of_threads,resultProperty))
		return (void*) 1;
	return (void*) 0;
}


bool ParallelExploration:: threadedExploration(NetState* ns,Store &myStore, FireListCreator &fireListCreator, SimpleProperty* sp, int threadNumber, int number_of_threads, SimpleProperty* resultProperty) {
	Firelist* myFirelist = fireListCreator.createFireList(sp);
	/// the search stack
	SearchStack stack;

	// prepare property
	bool localValue = sp->initProperty(*ns);

	if (localValue) {
		// initial marking satisfies property
		// inform all threads that we have finished
		finished = true;
		// release all semaphores
		for (int i = 0; i < number_of_threads; i++)
			sem_post(&restartSemaphore);
		sem_post(&transfer_finished_mutex);
		// return success
		pthread_mutex_lock(&write_current_back_mutex);
        free(Marking::Current);
        Marking::Current = ns->Current;
        resultProperty->stack = stack;
        pthread_mutex_unlock(&write_current_back_mutex);
		return true;
	}

	// add initial marking to store
	// we do not care about return value since we know that store is empty

	myStore.searchAndInsert(ns, threadNumber);

	// get first firelist
	index_t* currentFirelist;
	index_t currentEntry = myFirelist->getFirelist(ns, &currentFirelist);

	while (true) // exit when trying to pop from empty stack
	{
		// if one of the threads send the finished signal this threadis useless and has to abort
		if (finished)
			return false;
		//printf("GOON %x NS %x\n", container, ns);
		if (currentEntry --> 0) {
			// printf("%d ANA %d\n",threadNumber, currentEntry);
			// there is a next transition that needs to be explored in current marking

			// fire this transition to produce new Marking::Current
			Transition::fire(ns, currentFirelist[currentEntry]);

			if (myStore.searchAndInsert(ns, threadNumber)) {
				// State exists! -->backtracking to previous state
				Transition::backfire(ns, currentFirelist[currentEntry]);
			} else {
				// State does not exist!
				// continue with next transition
				// test whether it is a transition that satisfies the property and if return
				Transition::updateEnabled(ns, currentFirelist[currentEntry]);
				// check current marking for property
				localValue = sp->checkProperty(*ns,currentFirelist[currentEntry]);
				if (localValue) {
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
						sem_post(&restartSemaphore);
					sem_post(&transfer_finished_mutex);
					// return success
					pthread_mutex_lock(&write_current_back_mutex);
			        free(Marking::Current);
			        Marking::Current = ns->Current;
			        resultProperty->stack = stack;
			        pthread_mutex_unlock(&write_current_back_mutex);
			        //printf("%d SUCC\n", threadNumber);
					return true;
				}
				// push the transition onto the stack
				stack.push(currentEntry, currentFirelist);


				// first try the dirty read to make the program more efficient
				// but do it only if there are at least two transitions in the firelist left (one for us, and one for the other thread)
				if (currentEntry >= 2 && num_suspended > 0) {
					//printf("%d TRANSFER\n",threadNumber);
					pthread_mutex_lock (&num_suspend_mutex);
					if (num_suspended > 0) {
						//printf("%d DO TRANSFER\n",threadNumber);
						//  i know that there is an other thread waiting for my data
						num_suspended--;
						pthread_mutex_unlock(&num_suspend_mutex);
						pthread_mutex_lock (&num_suspend_mutex);
						//printf("%d STARTING TRANSFER\n",threadNumber);
						if (finished)
							return false;
						// locked the transfer variables


						// copy the data for the other thread
						transfer_stack = stack;
						transfer_netstate = NetState::createNetStateFromCurrent(ns);
						transfer_property = sp->copy();
						sem_post(&restartSemaphore);
						sem_wait(&transfer_finished_mutex);
						//printf("%d FINISHED TRANSFER %d GRANTING %d:%d\n",threadNumber, stack.StackPointer, currentEntry, currentFirelist[currentEntry]);

						// backfire the current transition to return to original state
						stack.pop(&currentEntry, &currentFirelist);
						assert(currentEntry < Net::Card[TR]);
						Transition::backfire(ns, currentFirelist[currentEntry]);
						Transition::revertEnabled(ns, currentFirelist[currentEntry]);
						localValue = sp->updateProperty(*ns,currentFirelist[currentEntry]);
						// go on as nothing happened

						pthread_mutex_unlock (&num_suspend_mutex);
						continue;
					}
					pthread_mutex_unlock(&num_suspend_mutex);
				}

				//printf("%d DOWN\n", threadNumber);
				// Here: current marking does not satisfy property --> continue search
				currentEntry = myFirelist->getFirelist(ns, &currentFirelist);
			} // end else branch for "if state exists"
		} else {
			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			if (stack.StackPointer == 0) {
				// have completely processed local initial marking --> state not found

				//printf("%d DIE\n",threadNumber);
				// maybe we have to go into an other sub-tree of the state-space
				// first get the counter mutex to be able to count the number of threads currently suspended
				pthread_mutex_lock (&num_suspend_mutex);
				num_suspended++;
				// then we are the last thread going asleep and so we have to await all the others and tell them that the search is over
				if (num_suspended == number_of_threads) {
					//printf("%d FINI\n",threadNumber);
					finished = true;
					// release all semaphores
					for (int i = 0; i < number_of_threads; i++)
						sem_post(&restartSemaphore);
					sem_post(&transfer_finished_mutex);
					pthread_mutex_unlock(&num_suspend_mutex);
					// there is no such state
					return false;
				}
				pthread_mutex_unlock(&num_suspend_mutex);
				//printf("%d AWAIT STACK\n",threadNumber);
				sem_wait(&restartSemaphore);
				//printf("%d STACK RECEIVED\n",threadNumber);
				// if the search has come to an end return without success
				if (finished)
					return false;
				// now we have been signaled because one of the threads is able to give us an part of search space

				delete ns;
				delete sp;

				stack = transfer_stack;
				ns = transfer_netstate;
				sp = transfer_property;
				// rebuild
				sp->initProperty(*ns);
				delete myFirelist;
				myFirelist = fireListCreator.createFireList(sp);
				// my sender holds the lock that authorizes me to decrease the variable
				sem_post(&transfer_finished_mutex);

				// re-initialize the current search state
				currentEntry = myFirelist->getFirelist(ns, &currentFirelist);

				//printf("%d CONT %d\n",threadNumber, currentEntry);
				continue;

			}
			//stack.printTop5(threadNumber,2);
			stack.pop(&currentEntry, &currentFirelist);
			assert(currentEntry < Net::Card[TR]);
			//printf("%d BF %d %d:%d \n",threadNumber, stack.StackPointer, currentEntry, currentFirelist[currentEntry]);
			Transition::backfire(ns, currentFirelist[currentEntry]);
			//printf("%d BFD\n",threadNumber);
			Transition::revertEnabled(ns, currentFirelist[currentEntry]);
			localValue = sp->updateProperty(*ns,currentFirelist[currentEntry]);
		}
	}
}

bool ParallelExploration::depth_first(SimpleProperty& property, Store &myStore, FireListCreator &firelistcreator, int number_of_threads) {
	// copy initial marking into current marking
	memcpy(Marking::Current, Marking::Initial, Net::Card[PL] * SIZEOF_INDEX_T);
	Marking::HashCurrent = Marking::HashInitial;

	runner_thread = (pthread_t*) calloc(number_of_threads,
			sizeof(pthread_t));
	tpDFSArguments* args = (tpDFSArguments*) calloc(number_of_threads,
			sizeof(tpDFSArguments));
	for (int i = 0; i < number_of_threads; i++) {
		args[i].fireListCreator = &firelistcreator;
		args[i].myStore = &myStore;
		args[i].sp = property.copy();
		args[i].ns = NetState::createNetStateFromCurrent();
		args[i].threadNumber = i;
		args[i].number_of_threads = number_of_threads;
		args[i].pexploration = this;
		args[i].resultProperty = &property;
	}
	// init the restart semaphore
	sem_init(&restartSemaphore, 0, 0);
	sem_init(&transfer_finished_mutex, 0,0);
	pthread_mutex_init(&num_suspend_mutex, NULL);
	pthread_mutex_init(&send_mutex, NULL);
	pthread_mutex_init(&write_current_back_mutex,NULL);
	num_suspended = 0;
	finished  =false;

	for (int i = 0; i < number_of_threads; i++)
		pthread_create(runner_thread + i, NULL, threadPrivateDFS, args + i);

	bool found = false;
	for (int i = 0; i < number_of_threads; i++) {
		void* return_value;
		pthread_join(runner_thread[i], &return_value);
		found |= (bool) return_value;
	}

	pthread_mutex_destroy(&num_suspend_mutex);
	pthread_mutex_destroy(&send_mutex);
	pthread_mutex_destroy(&write_current_back_mutex);

	// finalize the store
	myStore.finalize();
	property.value = found;
	return found;
}
