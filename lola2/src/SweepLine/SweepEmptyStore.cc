/*
\file SweepEmptyStore.cc
\author Harro
\status new

\brief List of Stores for persistent markings in the SweepLine method

This class realizes one store in a list of stores, sorted by progress
measure and containing known persistent markings, either old or new ones.
*/

#include <SweepLine/SweepEmptyStore.h>

/*!
 * \brief Constructor for one store in a list of stores
 */
SweepEmptyStore::SweepEmptyStore() : EmptyStore<void>(1)
{
}

/*!
 * \brief Increment the total number of markings
 */
void SweepEmptyStore::incrementMarkings()
{
    ++(this->markings[0]);
}

/*!
 * \brief Increment the total number of edges
 */
void SweepEmptyStore::incrementCalls()
{
    ++(this->calls[0]);
}

/*!
 * \brief Set the total number of markings
 * \param The number of markings
 */
void SweepEmptyStore::setMarkings(int64_t count)
{
    this->markings[0] = count;
}

/*!
 * \brief Set the total number of calls
 * \param The number of calls
 */
void SweepEmptyStore::setCalls(int64_t count)
{
    this->calls[0] = count;
}

