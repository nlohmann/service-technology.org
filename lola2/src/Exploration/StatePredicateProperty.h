/*!
\file StatePredicateProperty.h
\author Karsten
\status new

\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is virtual, default (base class) is full exploration
*/

#pragma once

#include "Formula/AtomicStatePredicate.h"
#include "Exploration/SimpleProperty.h"

class Firelist;
class Store;

class StatePredicateProperty: public SimpleProperty
{

    public:
        StatePredicateProperty(StatePredicate*);
        ~StatePredicateProperty();
    private:
        /// prepare for search
        virtual void initProperty();

        /// check property in Marking::Current, use after fire. Argument is transition just fired.
        virtual void checkProperty(index_t);

        /// check property in Marking::Current, use after backfire. Argument is transition just backfired.
        virtual void updateProperty(index_t);

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
        index_t** changedSum;
};
