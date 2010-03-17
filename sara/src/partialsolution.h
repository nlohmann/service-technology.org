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
 * \date    $Date: 2009-10-12 12:00:00 +0200 (Mo, 12. Okt 2009) $
 *
 * \version $Revision: -1 $
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

class PartialSolution;

#include "constraint.h"

using pnapi::Marking;
using pnapi::Transition;
using pnapi::Place;
using std::vector;
using std::deque;
using std::map;
using std::string;
using std::set;

/** This class represents partial solutions to the problem if a transition
	vector is realizable.
	Such a partial solution consists of 1) an initially firable sequence,
	2) the marking reached then, 3) A remainder of the transition vector,
	nonfirable, 4) constraints.
*/ 
class PartialSolution {
public:
	/// minimal constructor with reached marking and number of transitions in the Petri net
	PartialSolution(Marking& m); 

	/// constructor with ts=initially firable sequence and remainder rem
	PartialSolution(vector<Transition*>& ts, Marking& m, map<Transition*,int>& rem);

	/// destructor
	~PartialSolution(); 

	/// marking
	Marking& getMarking();

	/// set method for the marking
	void setMarking(Marking& m);

	/// the initially firable sequence
	vector<Transition*>& getSequence();

	/// set the initially firable sequence
	void setSequence(vector<Transition*> seq);

	/// the remainder of the transition vector
	map<Transition*,int>& getRemains();

	void setFullVector(map<Transition*,int>& ftv);
	map<Transition*,int>& getFullVector();

	/// sets one constraint, fails if the exact constraint already exists
	bool setConstraint(const Constraint& c);

	/// set one failure constraint
	bool setFailureConstraint(const Constraint& c);

	/// get the right hand side of a constraint (or -1 on failure)
	int findRHS(Constraint& c);

	/// remove all constraints
	void clearConstraints();

	/// remove all failure constraints
	void clearFailureConstraints();

	/// set all constraints at once, existing constraints are overwritten
	void setConstraints(set<Constraint> cv);

	/// get all constraints
	set<Constraint>& getConstraints();

	/// get all failure constraints
	set<Constraint>& getFailureConstraints();

	/// sets flag about feasibility of the constrained marking equation
	void setFeasibility(bool);

	/// flag about feasibility of the constrained marking equation
	bool isFeasible();

	/// prints the partial solution
	void show();
/*
	/// calculate on which places the actual marking is less than the final marking and by which amount.
	map<Place*,int> underFinalMarking(IMatrix& im);

	/// calculates the number of effectively produced tokens for each transition in the partial firing sequence and adds them all up
	map<Place*,int> produce();
*/
	/// checks if the partial firing sequence is "better" than a given one (tv)
	bool betterSequenceThan(vector<Transition*> tv, Marking &m0, map<Transition*,int>& rem);

	// calculate constraints to make unfirable circles firable or to prevent them
//	void calcCircleConstraints(IMatrix& im, Marking& m0);

	/// calculates the concrete values in a constraint from given sets of places and transitions and adds it to the constraint list
	bool buildMultiConstraint(map<Place*,int>& pmap, int incr, set<Transition*>& forbidden);

//	int checkUndermarking(Marking& m, set<Transition*>& tset, set<Place*>& pset, set<Place*>& dead, IMatrix& im);

	/// calculates new constraints to enforce firability of so far unenabled transitions
	void buildSimpleConstraints(IMatrix& im);

/*
	set<Place*> getPlacesForZSK(const set<Transition*>& tset, IMatrix& im);
	map<Place*,int> findPlacesAfter(set<Transition*>& szk, IMatrix& im);
	set<Place*> findPlacesIn(set<Transition*>& szk, IMatrix& im);
	set<Place*> findPlacesDead(set<Transition*>& szk, IMatrix& im);
*/
	/// check whether this partial solution solves the full problem
	bool isSolved();

	/// mark the partial solution as a full solution
	void setSolved();

	/// mark all constraints as old (not available for failure reasoning)
	void touchConstraints();

	/// list all forbidden transitions that are disabled due to undermarking on pset
	set<Transition*> disabledTransitions(map<Place*,int> pset, IMatrix& im);
	
	/// Evaluate the LHS of a constraint under the active marking
	int getActualRHS(const Constraint& c);

	/// Calculate the parikh image of the firing sequence and store it
	map<Transition*,int> calcParikh();

	/// Add a parikh image of a firing sequence to a failure
	void addParikh(map<Transition*,int>& p);

	/// Get all parikh images of firing sequences for this failure
	vector<map<Transition*,int> >& getParikh();

private:	
	/// the partial firing sequence
	vector<Transition*> tseq;

	/// the marking after firing the partial firing sequence
	Marking m;

	/// the remaining transitions that could not be fired
	map<Transition*,int> remains;

	/// the constraints for this solution
	set<Constraint> constraints,failure;

	/// vector of parikh images for different sequences, only for failures
	vector<map<Transition*,int> > parikh;

	/// temporary variables
	vector<set<Transition*> > ttmp;
	vector<set<Place*> > ptmp;
	vector<set<Transition*> > ftmp;

	/// The full vector of transitions to be realized
	map<Transition*,int> fulltv;

	/// flag marking the marking equation as infeasible
	bool markingEquation;

	/// flag marking this partial solution as a full solution
	bool fullSolution;
};

#endif
