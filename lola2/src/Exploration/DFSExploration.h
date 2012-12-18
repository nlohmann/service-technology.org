/*!
\file DFSExploration.h
\author Karsten
\status new

\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is a parameter of the constructor
*/

#pragma once

#include <config.h>
#include <Exploration/SearchStack.h>
#include <Exploration/ChooseTransition.h>
#include <Exploration/SimpleProperty.h>
#include <Net/NetState.h>
#include <Exploration/Firelist.h>
#include <Stores/Store.h>
#include <Stores/EmptyStore.h>
#include <SweepLine/SweepEmptyStore.h>


class Firelist;
//class Store;
//class EmptyStore;


/*!
\author Karsten
\status new

\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is a parameter of the constructor
*/
class DFSExploration
{
public:
    /*!
     \brief evaluate property by dfs.

	Evaluates a given property by standard depth-first-search.
	The result will be
	- true, if a marking fulfilling the property was found
	- false, if all markings have been explored and no such state was found
	- no termination (in time), if the state space is too big and no 'good' marking was found

	\param property the property to check
	\param ns The initial state of the net has to be given as a net state object. \n
				If the search has found a state, fulfilling the property this state will be returned in this parameter.
	\param store the store to be used. The selection of the store may greatly influence the performance of the program
	\param firelist the firelists to use in this search.
			The firelist _must_ be applicable to the given property, else the result of this function may be wrong.\n
			It is not guaranteed that the given firelist will actually be used.
			In the parallel work-mode the given list will just be used as a base list form which all other lists will be generated
	\param number_of_threads will be ignored by the standard seach.
			In the parallel execution mode this number indicates the number of threads to be used for the search
     */
    bool virtual depth_first(SimpleProperty &property, NetState &ns, Store<void> &store, Firelist &firelist, int number_of_threads);

    /*!
    \brief evaluate property by bfs. Result true = state found, false = state not found

    This function is currently not implemented. Default behavior is a call to

    	assert(false);

    The signature of this function may also be altered in the future.
    */
    bool breadth_first(SimpleProperty &property, NetState &ns, Store<void> &, Firelist &)
    {
        assert(false);
        return false;
    }

    /*!
    \brief evaluate property by random walk without storing states.

    The result will be
    - true, if the state was found
    - false, if max attempt(with maxdepth) exhausted without having found state

	\param property the property for which a path should be found
	\param ns the net state of the initial marking
    \param attempts number of attempts. An argument value of 0 will lead to an unlimited number of attempts:
    the function will not return if the property not satisfiable
    \param maxdepth maximal depth of each attempt
    \param myFirelist the object used to generate the actual firelists, this must correspond with given property
    \param e the user have to given an instance of the empty store
    \param c ChooseTransition object to determine which enabled transition to fire.
    */
    bool find_path(SimpleProperty &property, NetState &ns, unsigned int attempts, unsigned int maxdepth, Firelist &, EmptyStore<void> &e, ChooseTransition &c);

    /*!
    \brief evaluate property by sweepline method.
    \param property the property for which a witness state should be found
    \param ns the net state of the initial marking
    \param myStore A dummy store for counting markings
    \param myFirelist the object used to generate the actual firelists, this must correspond with given property
    \param number_of_threads will be ignored by the standard search.
           In the parallel execution mode this number indicates the number of threads to be used for the search
    \return if the state was found
    */
    bool sweepline(SimpleProperty &property, NetState &ns, SweepEmptyStore &myStore, Firelist &firelist, int number_of_threads);

    virtual ~DFSExploration() {}
};
