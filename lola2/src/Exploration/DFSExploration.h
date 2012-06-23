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

class Firelist;
class Store;
class EmptyStore;

class DFSExploration
{
    public:
        /// evaluate property by dfs. Result true = state found, false = state not found
        bool virtual depth_first(SimpleProperty &property, NetState &ns, Store &, FireListCreator &, int threadNumber);

        /// evaluate property by bfs. Result true = state found, false = state not found
        bool breadth_first(SimpleProperty &property, NetState &ns, Store &, Firelist &)
        {
            assert(false);
            return false;
        }

        /// evaluate property by random walk without storing states.
        /// Arguments: 1 = number of attempts 2 = depth of attempt 3 = firelist generator
        /// result true = state found, result false = max attempt exhausted without having found state
        /// 1st argument 0 = unlimited number of attempts: no return if property not satisfiable
        bool find_path(SimpleProperty &property, NetState &ns, unsigned int, unsigned int, Firelist &, EmptyStore &, ChooseTransition &);

        virtual ~DFSExploration() {}
};
