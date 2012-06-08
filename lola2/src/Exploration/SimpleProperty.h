/*!
\file SimpleProperty.h
\author Karsten
\status new

\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is virtual, default (base class) is full exploration
*/

#pragma once

#include <config.h>
#include <Exploration/SearchStack.h>
#include <Exploration/ChooseTransition.h>
#include <Net/NetState.h>
#include <Exploration/Firelist.h>

class Firelist;
class Store;
class EmptyStore;

class SimpleProperty
{
    public:
        /// evaluate property by dfs. Result true = state found, false = state not found
        bool virtual depth_first(Store &, FireListCreator&, int threadNumber);

        /// evaluate property by bfs. Result true = state found, false = state not found
        bool breadth_first(Store &, Firelist &)
        {
            assert(false);
            return false;
        }

        /// evaluate property by random walk without storing states.
        /// Arguments: 1 = number of attempts 2 = depth of attempt 3 = firelist generator
        /// result true = state found, result false = max attempt exhausted without having found state
        /// 1st argument 0 = unlimited number of attempts: no return if property not satisfiable
        bool find_path(unsigned int, unsigned int, Firelist &, EmptyStore &, ChooseTransition &);

        /// the search stack
        SearchStack stack;
        virtual ~SimpleProperty() {}

    public:
        /// value of property in current state
        bool value;

    public:
        /// prepare for search
        virtual bool initProperty(NetState &ns);

        /// check property in Marking::Current, use after fire. Argument is transition just fired.
        virtual bool checkProperty(NetState &ns, index_t) {return false;}

        /// check property in Marking::Current, use after backfire. Argument is transition just backfired.
        virtual bool updateProperty(NetState &ns, index_t) {return false;}
};
