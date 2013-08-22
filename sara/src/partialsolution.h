// -*- C++ -*-

/*!
 * \file    partialsolution.h
 *
 * \brief   Class PartialSolution
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/12
 *
 * \date    $Date: 2010-08-13 12:00:00 +0200 (Fr, 13. Aug 2010) $
 *
 * \version $Revision: 1.01 $
 */

#ifndef PARTIALSOLUTION_H
#define PARTIALSOLUTION_H

#include <vector>
#include <map>
#include <string>
#include <set>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "imatrix.h"
#include "tarjan.h"

namespace sara {

class PartialSolution;

}

#include "constraint.h"

using pnapi::Marking;
using pnapi::Transition;
using pnapi::Place;
using std::vector;
using std::deque;
using std::map;
using std::string;
using std::set;

namespace sara {

/*! \brief For storing sets of constraints, partial firing sequences, and non-enabled remainders, forming a job

	This class represents partial solutions to the problem if a transition
	vector is realizable.
	Such a partial solution consists of 1) an initially firable sequence,
	2) the marking reached then, 3) A remainder of the transition vector,
	nonfirable, 4) constraints.
*/ 
class PartialSolution {
public:
	/// Minimal constructor with the marking reached
	PartialSolution(Marking& m); 

	/// Constructor with ts=initially firable sequence and remainder rem
	PartialSolution(vector<Transition*>& ts, Marking& m, map<Transition*,int>& rem);

	/// Destructor.
	~PartialSolution(); 

	/// Get the marking of this partial solution
	Marking& getMarking();

	/// Set method for the marking
//	void setMarking(Marking& m);

	/// Get the initially firable sequence
	vector<Transition*>& getSequence();

	/// Set the initially firable sequence
	void setSequence(vector<Transition*> seq);

	/// Get the non-firable remainder of the transition vector
	map<Transition*,int>& getRemains();

	/// Set the full transition vector, including the already fired transitions and remainder
	void setFullVector(map<Transition*,int>& ftv);

	/// Get the full transition vector, including the already fired transitions and remainder
	map<Transition*,int>& getFullVector();

	/// Set one constraint, fails if the exact constraint already exists
	bool setConstraint(const Constraint& c);

	/// Set one failure constraint
	bool setFailureConstraint(const Constraint& c);

	/// Set all constraints at once, existing constraints are overwritten
	void setConstraints(set<Constraint> cv);

	/// Get all constraints
	set<Constraint>& getConstraints();

	/// Get all failure constraints
	set<Constraint>& getFailureConstraints();

	/// Get all constraints
	void clearConstraints();

	/// Set flag about feasibility of the constrained marking equation
	void setFeasibility(bool);

	/// Get flag about feasibility of the constrained marking equation
	bool isFeasible() const;

	/// Print the partial solution
	void show();

	/// Calculate the concrete values in a constraint from given sets of places and transitions and add it to the constraint list
	bool buildMultiConstraint(map<Place*,int>& pmap, int incr, set<Transition*>& forbidden);

	/// Calculate new constraints to enforce firability of so far unenabled transitions
	void buildSimpleConstraints(IMatrix& im);

	/// Check whether this partial solution solves the full problem
	bool isSolved() const;

	/// Tag the partial solution as a full solution
	void setSolved();

	/// Tag constraints as old (not available for failure reasoning)
	void touchConstraints(bool jump);

	/// List all forbidden transitions that are disabled due to undermarking on pset
	set<Transition*> disabledTransitions(map<Place*,int> pset, IMatrix& im);
	
	/// Evaluate the LHS of a constraint under the active marking
	int getActualRHS(const Constraint& c);

	/// Calculate the parikh image of the firing sequence and store it
	map<Transition*,int> calcParikh();

	/// Add a parikh image of a firing sequence to a failure
	void addParikh(map<Transition*,int>& p);

	/// Get all parikh images of firing sequences for this failure
	vector<map<Transition*,int> >& getParikh();

	/// Check if the stored parikh image is less or equal to the given one.
	bool compareVector(map<Transition*,int>& vec);

	/// Get the first jump constraint in the todo list
	Transition* getNextJC(int& val);

	/// Create a set of possible jump constraints and store them in a list
	void setJC(map<Transition*,int>& jumptransitions);

	/// Remove the first jump constraint from the todo list
	void popJC();

	/// Check if the given firing sequence is a permutation of the one in this partial solution
	bool compareSequence(vector<Transition*> seq, map<Transition*,int> rem);

	/// Alter all jump constraints into normal ones
	void transformJumps(map<Transition*,int>& fullvector);

	void reduceUndermarking(IMatrix& im, const set<Place*>& pcomp, map<Place*,int>& red);

//	bool checkConstraintExtension(IMatrix& im);
	bool isExtendable();
	void checkAddedInvariants(IMatrix& im, Marking m0, PartialSolution* father);
	void checkConstraintExtension(IMatrix& im, set<Place*>& hplaces, PartialSolution* father);

private:	
	/// The partial firing sequence
	vector<Transition*> tseq;

	/// The marking after firing the partial firing sequence
	Marking m;

	/// The remaining transitions that could not be fired
	map<Transition*,int> remains;

	/// The constraints for this solution
	set<Constraint> constraints;

	/// The failure constraints for this solution
	set<Constraint> failure;

	/// A vector of parikh images for different sequences, only for failures
	vector<map<Transition*,int> > parikh;

	/// Temporary variables
	vector<set<Transition*> > ttmp;
	vector<set<Place*> > ptmp;
	vector<set<Transition*> > ftmp;

	/// The full vector of transitions to be realized
	map<Transition*,int> fulltv;

	/// Flag marking the marking equation as infeasible
	bool markingEquation;

	/// Flag marking this partial solution as a full solution
	bool fullSolution;

	/// Flag showing that this' partial solutions recent constraints must be extended
	bool extendConstraints;
	set<Place*> noSimpleConstraint;

	/// Map of transitions for creating jump constraints
	map<int,int> jc;

	/// Number of jump constraints we have gone through so far, for job ordering
	int jumpsdone;

	map<Transition*, map<Place*,int> > invgain;
};

}

#endif

