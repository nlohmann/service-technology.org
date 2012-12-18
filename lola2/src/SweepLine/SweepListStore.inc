/*
\file SweepListStore.inc
\author Harro
\status new

\brief List of Stores for persistent markings in the SweepLine method

This class realizes one store in a list of stores, sorted by progress
measure and containing known persistent markings, either old or new ones. 
*/

#include <cstdlib>
#include <SweepLine/SweepListStore.h>
#include <iostream>
using std::cout;
using std::endl;

/*! 
 * \brief Constructor for one store in a list of stores
 */
template <class T>
SweepListStore<T>::SweepListStore() : next(NULL) {
//cout << "list elt=" << this << endl;
}

/*! 
 * \brief Constructor for a list of stores, returns pointer to the first store
 * \param size The number of stores in the list
 */
template <class T>
SweepListStore<T>::SweepListStore(index_t size) : next(size>0 ? new SweepListStore(size-1) : NULL) {
//cout << "list elt=" << this << endl;
}

/*!
 * \brief Destructor for the list starting at this store
 */
template <class T>
SweepListStore<T>::~SweepListStore()
{
//cout << "list free=" << this << endl;
	delete next;
}

/*!
 * \brief Check if there exists a store after this one in the list
 * \return The next store if it exists, NULL otherwise
 */
template <class T>
SweepListStore<T>* SweepListStore<T>::checkNext()
{
	return next;
}

/*!
 * \brief Check if there exists a store after this one in the list
 *        and create one if it does not exists
 * \return The next store in the list, possibly just created
 */
template <class T>
SweepListStore<T>* SweepListStore<T>::getNext()
{
	if (!next) next = new SweepListStore<T>(); 
	return next;
}

/*!
 * \brief Manually set the next store in the list
 * \param sls The store that should be the next after this one
 */
template <class T>
void SweepListStore<T>::setNext(SweepListStore<T>* sls)
{
	next = sls;
}

