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
     \brief evaluate property by dfs. Result true = state found, false = state not found

	Evaluates a given property by standard depth-first-search.
	\param property the property to check
	\param ns The initial state of the net has to be given as a net state object. \n
				If the search has found a state, fulfilling the property this state will be returned in this parameter.
	\param firelist the firelists to use in this search.
			The firelist _must_ be applicable to the given property, else the result of this function may be wrong.\n
			It is not guaranteed that the given firelist will actually be used.
			In the parallel work-mode the given list will just be used as a base list form which all other lists will be generated

	\param number_of_threads will be ignored by the standard seach.
			In the parallel execution mode this number indicates the number of threads to be used for the search
     */
    bool virtual depth_first(SimpleProperty &property, NetState &ns, Store<void> &, Firelist &firelist, int number_of_threads);

    /// evaluate property by bfs. Result true = state found, false = state not found
    bool breadth_first(SimpleProperty &property, NetState &ns, Store<void> &, Firelist &)
    {
        assert(false);
        return false;
    }

    /// evaluate property by random walk without storing states.
    /// Arguments: 1 = number of attempts 2 = depth of attempt 3 = firelist generator
    /// result true = state found, result false = max attempt exhausted without having found state
    /// 1st argument 0 = unlimited number of attempts: no return if property not satisfiable
    bool find_path(SimpleProperty &property, NetState &ns, unsigned int, unsigned int, Firelist &, EmptyStore<void> &, ChooseTransition &);

    virtual ~DFSExploration() {}
};
