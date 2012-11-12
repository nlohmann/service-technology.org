/*!
 \file StatePredicateProperty.h
 \author Karsten
 \status new

 \brief represents a property to be represented by a predicate
 */

#pragma once

#include <Formula/AtomicStatePredicate.h>
#include <Exploration/SimpleProperty.h>
#include <Stores/Store.h>


class Firelist;
//class Store;


/*!
\brief represents a property to be represented by a predicate

This is a property based on a state-predicate, which makes statements about a marking.
Incremental update of the property is possible.
*/
class StatePredicateProperty: public SimpleProperty
{

public:
    StatePredicateProperty(StatePredicate*);
    virtual ~StatePredicateProperty();
    StatePredicateProperty() {}
    /// prepare for search
    virtual bool initProperty(NetState &ns);

    /// check property in Marking::Current, use after fire. Argument is transition just fired.
    virtual bool checkProperty(NetState &ns, index_t);

    /// check property in Marking::Current, use after backfire. Argument is transition just backfired.
    virtual bool updateProperty(NetState &ns, index_t);

    /// return the predicate used to evaluate the property
    StatePredicate* getPredicate(){return predicate;}

private:

    /// the actual formula to be verified;
    StatePredicate* predicate;

    /// for each transition t, number of state predicates that need to be checked when t is fired
    index_t* cardChanged;

    /// for each transition t, an array with all state predicates that need to be checked when t is fired
    AtomicStatePredicate*** changedPredicate;

    /// changedSum[t][i] is the difference that t causes in the formal sum of state predicate changedPredicate[t][i]
    int** changedSum;

    /// create a copy of the property, needed in parallel exploration
    virtual SimpleProperty* copy();
};
