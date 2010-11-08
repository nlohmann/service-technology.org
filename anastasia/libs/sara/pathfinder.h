// -*- C++ -*-

/*!
 * \file    pathfinder.h
 *
 * \brief   Class PathFinder
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/06
 *
 * \date    $Date: 2009-10-06 14:47:01 +0200 (Mi, 6. Okt 2009) $
 *
 * \version $Revision: -1 $
 */

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <map>
#include <set>
#include <string>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "partialsolution.h"
#include "imatrix.h"
#include "jobqueue.h"

using pnapi::Marking;
using pnapi::Place;
using pnapi::Transition;
using std::vector;
using std::deque;
using std::map;
using std::set;
using std::string;

namespace sara {

/** Shorthand types for conflict&dependency tables */
typedef map<Transition*,set<Transition*> > Cft;
typedef map<Transition*,set<Transition*> >::iterator Cftit;

/** \brief The class responsible for realizing a transition vector into a firable firing sequence
	if it exists.
*/
class PathFinder {

public:

	/// Constructor with initial marking m, transition vector tv, # of transitions col, initialized job queues tps, sol, and fail, and incidence matrix im
	PathFinder(Marking& m, map<Transition*,int>& tv, int col, JobQueue& tps, JobQueue& sol, JobQueue& fail, IMatrix& im, map<map<Transition*,int>,vector<PartialSolution> >& lookup);

	/// Destructor.
	~PathFinder();

	/// The algorithm to check realizability
	bool recurse();

	/// If a solution has been found, this method will print it to stdout.
	void printSolution(PartialSolution* ps);

	/// Only minimal paths without repeating markings are sought
	void setMinimize();

	/// Level of verbosity
	int verbose;

	/// If the problem was passed on from an earlier instance of the PathFinder
	bool passedon;

	/// If the problem was passed on, torealize contains the transition multiset to realize
	map<Transition*,int> torealize;

private:
	/*! \brief Graph structure for stubborn set method (for finding strongly connected components).
	*/
	class myNode {
	 public:
		myNode();
		~myNode();
		void reset();
		set<int> nodes;
		Transition* t;
		int index, low;
		bool instack;
	};

	/// The active marking
	Marking m0;

	/// A stack of markings for recursion, [0]=initial marking
	vector<Marking> mv;

	/// A multiset of transitions to be realized
	map<Transition*,int> fulltvector;

	/// The remaining transitions to be fired
	map<Transition*,int> rec_tv;

	/// A stack for firing sequences in recursion, [size-1]=last fired transition
	vector<Transition*> fseq;

	/// The transitions to check against for diamonds while recursing, [size-1]=the active transition
	vector<set<Transition*> > stubsets;

	/// A conflict and dependency table for stubborn set search
	Cft cftab;

	/// Pointers from transitions to their unique nodes in the conflict graph
	vector<myNode*> tton;

	/// A stack for Tarjans algorithm
	vector<Transition*> st; 

	/// The column number for the marking equation (# of transitions)
	int cols;

	/// The JobQueue for recurse
	JobQueue& tps;

	/// The solutions queue
	JobQueue& solutions;

	/// The failure queue
	JobQueue& failure;

	/// The incidence matrix of the Petri net
	IMatrix& im;

	/// counter for the leafs reached by the recursive function (just for protocol)
	int pos;

	/// List of solutions of lp_solve so far, inherited from Reachalyzer
	map<map<Transition*,int>,vector<PartialSolution> >& shortcut;

	/// Counter of all recursion steps for the progress indicator
	int recsteps;

	/// Maximal shortcut size (redefinable per command line switch)
	int shortcutmax;

	/// Temporary firing sequence pool for complex diamond checks
	vector<vector<Transition*> > fpool;

	/// A flag stating that this PathFinder should only find minimal paths (non-repeating markings)
	bool minimize;

	/// Calculate the conflicts and dependencies for a given transition
	void conflictTable(Transition*);

	/// Calculate one place that is a reason for nonfirability of a transition
	Place* hinderingPlace(Transition&);

	/// Calculate from the list of transitions to be fired those transitions that increment the token count on a place
	set<Transition*> requiredTransitions(Place&);

	/// Calculate a stubborn set of enabled transitions from the conflict graph
	set<Transition*> getSZK();

	/// Implementation of Tarjans algorithm for the conflict graph (with some optimization)
	bool doTarjan(myNode*, set<Transition*>&, int&);

	/// Check if a diamond is about to be closed during recursion
	bool checkForDiamonds();

	/// Checks whether a T-invariant can bring a transition closer to enabledness
	bool checkIfClosingIn(int start);

	/// Compare two multisets of transitions for componentwise inclusion
	bool isSmaller(map<Transition*,int>& m1, map<Transition*,int>& m2);

};

}

#endif

