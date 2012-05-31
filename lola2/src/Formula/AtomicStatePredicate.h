/*!
\file AtomicStatePredicate.h
\author Karsten
\status new

\brief class definition for atomic state predicates
*/

#pragma once

#include <Formula/StatePredicate.h>

/*!
A state predicate is a formula that assigns a Boolean value to
Marking::Current An atomic predicate compares a formal sum of places with a
constant The general shape is: \f$ k_1 \cdot p_1 + \cdots + k_n \cdot p_n \leq k \f$
*/
class AtomicStatePredicate : public StatePredicate
{
    public:
        virtual ~AtomicStatePredicate();

        /// creates a state predicate with a formal sum of #arg1 places with positive factor,
        /// #arg2 places with negative factor, and constant #arg3
        /// particular places are added using addPos and addNeg
        AtomicStatePredicate(index_t, index_t, int);

        /// adds a place (arg2) with positive factor (arg3) at position arg1
        void addPos(index_t, index_t, capacity_t);

        /// adds a place (arg2) with negative factor (arg3) at position arg1
        void addNeg(index_t, index_t, capacity_t);

        virtual index_t getUpSet(index_t* stack, bool* onstack);

        /// These will only be called for non-atomic formulas.
	// LCOV_EXCL_START
        virtual void updateTF(index_t) {}
        virtual void updateFT(index_t) {}
	// LCOV_EXCL_STOP


        /// updates the value of this sub formula. If value of this changes, it needs to be propagated to its parent
        /// The parameter is the change in the formal sum
        /// k_1 p_1 + ... + k_n p_n between the previously considered marking and the current marking.
        /// Having a precomputed value for this change, evaluation of the formula is accelerated
        void update(int);

        /// evaluates a formula, e.g. upon initialization. Evaluation starts top/down, so the whole formula is
        /// examined. Evaluation is done w.r.t. Marking::Current
        virtual void evaluate();

        void setUpSet();

        /// lists place p_i indices that occur with positive multiplicity k_i
        index_t* posPlaces;

        /// lists place p_i indices that occur with negative multiplicity k_i
        index_t* negPlaces;

        /// lists multiplicities  k_i of places in posPlaces
        capacity_t* posMult;

        /// lists multiplicities  k_i of places in negPlaces
        capacity_t* negMult;

        /// the number of pos entries
        index_t cardPos;

        /// the number of negative entries
        index_t cardNeg;

        /// the threshold k
        int threshold;

        /// The current value of the formal sum k_1 p_1 + ... + k_n p_n
        int sum;

        /// The up set of this formula
        index_t* up;

        /// The size of the up set
        index_t cardUp;

        /// counts atomic subformulas
        virtual index_t countAtomic();

        /// collects atomic subformulas; array must be malloced beforehand
        /// result is number of inserted elements
        virtual index_t collectAtomic(AtomicStatePredicate**);
        virtual void consistency();
};
