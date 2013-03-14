/*!
\file SweepListStore.h
\author Harro
\status new
\brief List of Stores for persistent markings in the SweepLine method

This class realizes one store in a list of stores, sorted by progress
measure and containing known persistent markings, either old or new ones. 
*/

#pragma once
#include <Core/Dimensions.h>
#include <SweepLine/SweepStore.h>

/*!
\brief List of Stores for persistent markings in the SweepLine method

This class realizes one store in a list of stores, sorted by progress
measure and containing known persistent markings, either old or new ones. 
*/
template <class T>
class SweepListStore
{
public:
	/// constructor for a single element of a list
	SweepListStore(index_t nr_of_threads = 1);

	/// constructor for a list of stores of given length
	SweepListStore(index_t nr_of_threads, index_t size);

	/// destructor
	~SweepListStore();

	/// check if there is a next store in the list and return it (or NULL)
	SweepListStore<T>* checkNext();

	/// check for and possibly create a next store in the list
	SweepListStore<T>* getNext();

	/// set the next store in the list
	void setNext(SweepListStore<T>* sls);

	/// the actual store for this list element
	SweepStore<T> store;

private:
	/// the next store in the list
	SweepListStore<T>* next;

	/// maximal number of threads to access this store simultaneously
	index_t nr_of_threads;
};

#include <SweepLine/SweepListStore.inc>
