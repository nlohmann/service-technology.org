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

/** Shorthand types for conflict&dependency tables */
typedef map<Transition*,set<Transition*> > Cft;
typedef map<Transition*,set<Transition*> >::iterator Cftit;

/** The class responsible for realizing a transition vector into a firable firing sequence
	if it exists.
*/
class PathFinder {

public:

	/// Constructor with initial marking m, transition vector tv, # of transitions col, initialized partial solutions tps, and incidence matrix im
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

private:
	/** Graph for stubborn set method (for finding strongly connected components).
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

	/// active marking
	Marking m0;

	/// stack of markings for recursion, [0]=initial marking
	vector<Marking> mv;

	/// multiset of transitions to be realized
	map<Transition*,int> fulltvector;

	/// remaining transitions to be fired
	map<Transition*,int> rec_tv;

	/// stack for firing sequence in recursion, [size-1]=last fired transition
	vector<Transition*> fseq;

	/// transitions to check against for diamonds while recursing, [size-1]=the active transition
	vector<set<Transition*> > stubsets;

	/// conflict and dependency table for stubborn set search
	Cft cftab;

	/// pointers from transition to their unique nodes in the conflict graph
	vector<myNode*> tton;

	/// stack for Tarjans algorithm
	vector<Transition*> st; 

	/// Column number for linear system (# of transitions)
	int cols;

	/// JobQueue of recurse:
	JobQueue& tps;

	/// Solutions queue:
	JobQueue& solutions;

	/// Failure queue:
	JobQueue& failure;

	/// Incidence matrix of the Petri net
	IMatrix& im;

	/// counts the leafs reached by the recursive function (just for protocol)
	int pos;

	/// inherited from Reachalyzer
	map<map<Transition*,int>,vector<PartialSolution> >& shortcut;

	/// counts all recursion steps for progress indicator
	int recsteps;

	/// maximal shortcut size (redefinable per command line switch)
	unsigned int shortcutmax;

	/// temporary firing sequence pool for complex diamond checks
	vector<vector<Transition*> > fpool;

	/// flag stating that this PathFinder should only find minimal paths (non-repeating markings)
	bool minimize;

	/// calculates the conflicts and dependencies for a given transition
	void conflictTable(Transition*);

	/// calculates one place that is a reason for nonfirability of a transition
	Place* hinderingPlace(Transition&);

	/// calculates from the list of transitions to be fired those transitions that increment the token count on a place
	set<Transition*> requiredTransitions(Place&);

	/// returns a stubborn set of enabled transitions from the conflict graph
	set<Transition*> getSZK();

	/// implements Tarjans algorithm for the conflict graph (with some optimization)
	bool doTarjan(myNode*, set<Transition*>&, int&);

	/// checks if a diamond is about to be closed during recursion
	bool checkForDiamonds();

//	vector<Transition*> calcOrder(set<Transition*> tset);

};

#endif
