/*!
\file StatePredicateProperty.h
\author Karsten
\status new

\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is virtual, default (base class) is full exploration
*/

#pragma once

#include <Formula/AtomicStatePredicate.h>
#include <Exploration/ParallelSimpleProperty.h>

class Firelist;
class Store;

class StatePredicateProperty: public SimpleProperty
{

    public:
        StatePredicateProperty(StatePredicate*);
        ~StatePredicateProperty();
    private:
        /// prepare for search
        virtual bool initProperty(NetState &ns);

        /// check property in Marking::Current, use after fire. Argument is transition just fired.
        virtual bool checkProperty(NetState &ns,index_t);

        /// check property in Marking::Current, use after backfire. Argument is transition just backfired.
        virtual bool updateProperty(NetState &ns,index_t);

        // the actual formula to be verified;
        StatePredicate* predicate;

        // for each transition t, number of state predicates that need to be checked
        // when t is fired
        index_t* cardChanged;

        // for each transition t, an array with all state predicates that
        // need to be checked
        // when t is fired
        AtomicStatePredicate*** changedPredicate;

        // changedSum[t][i] is the difference that t causes in the formal sum of
        // state predicate changedPredicate[t][i]
        int** changedSum;
};
