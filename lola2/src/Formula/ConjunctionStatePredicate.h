/*!
\file ConjunctionStatePredicate.h
\author Karsten
\status new

\brief class definition for conjunction state predicates
*/

#pragma once

#include "Formula/StatePredicate.h"

/// A conjunction state predicate is a AND formula with multiple children that assigns a Boolean value to Marking::Current
class ConjunctionStatePredicate : public StatePredicate
{
    public:
	/// arg is number of subformulas
	ConjunctionStatePredicate(unsigned int);

	/// adds i-th subformula
	void addSub(unsigned int i, StatePredicate * f);

	virtual index_t getUpSet(index_t * stack, bool * onstack); 
                                 
	/// If value of this changes, the parent formula is 
	/// triggered for updating. This means that updating is started at the leafs of the formula tree.
	/// Parts of the formula that did not change are not examined.
	/// The parameter is the position of this in the parent's subformula list

	virtual void updateTF(index_t);
	virtual void updateFT(index_t);

	/// evaluates a formula, e.g. upon initialization. Evaluation starts top/down, so the whole formula is 
	/// examined.
	virtual void evaluate();

	StatePredicate * parent;

private:
	/// the list of subformulas
	StatePredicate ** sub;
	
	/// The number of subformulas;
	unsigned int cardSub;

	/// The number of satisfied subformulas
	unsigned int cardSat;
};
