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
 * \date    $Date: 2012-06-12 14:47:01 +0200 (Di, 12. Jun 2012) $
 *
 * \version $Revision: 1.10 $
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
#include <time.h>

using pnapi::Marking;
using pnapi::Place;
using pnapi::Transition;
using std::vector;
using std::deque;
using std::map;
using std::set;
using std::string;

namespace sara {

struct SThread;

/** Shorthand types for conflict&dependency tables */
//typedef map<Transition*,set<Transition*> > Cft;
//typedef map<Transition*,set<Transition*> >::iterator Cftit;

/** \brief The class responsible for realizing a transition vector into a firable firing sequence
	if it exists.
*/
class PathFinder {

public:

	/// Constructor with initial marking m, transition vector tv, # of transitions col, initialized job queues past, tps, sol, and fail, and incidence matrix im
	PathFinder(Marking& m, map<Transition*,int>& tv, JobQueue& past, JobQueue& tps, JobQueue& sol, JobQueue& fail, IMatrix& im, map<map<Transition*,int>,vector<PartialSolution> >& lookup, PartialSolution* excl);

	/// Destructor.
	~PathFinder();

	/// The algorithm to check realizability
	bool recurse(unsigned int tid);

	/// If a solution has been found, this method will print it to stdout.
	void printSolution(PartialSolution* ps);

	/// If only minimal paths without repeating markings are sought
	void setMinimize();

	/// Wait for other threads to finish their jobs
	bool waitForThreads(unsigned int rootthread, bool solution);

	/// Level of verbosity
	int verbose;

	/// If the problem was passed on from an earlier instance of the PathFinder
	bool passedon;

	/// If the problem was passed on, torealize contains the transition multiset to realize
	map<Transition*,int> torealize;

	/*! \brief Graph structure for stubborn set method (for finding strongly connected components).
	*/
	class myNode {
	 public:
		myNode();
		~myNode();
		void reset();
		vector<int> nodes; // successor nodes
		Transition* t;
		int index, low;
		bool instack;
	};

	// some thread functions need to access private elements in PathFinder
	friend void* concurrentPathFinder(void* arg);
	friend bool assignHelper(unsigned int tid);
	friend void initThread(unsigned int rtnr, PartialSolution* ps, Marking& m, map<Transition*,int>& tv, PathFinder& pf);

private:
	// the initial marking of the problem
	Marking& m0;

	/// A multiset of transitions to be realized
	map<Transition*,int> fulltvector;

	/// A JobQueue for comparisons with past jobs, will not be changed
	JobQueue& past;

	/// The JobQueue for recurse
	JobQueue& tps;

	/// The solutions queue
	JobQueue& solutions;

	/// The failure queue
	JobQueue& failure;

	/// The incidence matrix of the Petri net
	IMatrix& im;

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
	
	/// A flag telling all threads that the problem has been solved
	bool solved;

	/// The size of fulltvector (multiset count)
	int tvsize;

	/// The father job which should be excluded from comparisons with past jobs
	PartialSolution* excl;

	/// Calculate the conflicts and dependencies for a given transition
	void conflictTable(SThread*, Transition*);

	/// Calculate one place that is a reason for nonfirability of a transition
	Place* hinderingPlace(SThread*, Transition&);

	/// Calculate from the list of transitions to be fired those transitions that increment the token count on a place
	void requiredTransitions(SThread*, Place&);

	/// Calculate a stubborn set of enabled transitions from the conflict graph
	vector<Transition*>& getSZK(SThread*);

	/// Implementation of Tarjans algorithm for the conflict graph (with some optimization)
	bool doTarjan(SThread*, myNode*, vector<Transition*>&, int&, vector<Transition*>&);

	/// Check if a diamond is about to be closed during recursion
	bool checkForDiamonds(SThread*);

	/// Checks whether a T-invariant can bring a transition closer to enabledness
	bool checkIfClosingIn(SThread*, int start);

	/// Compare two multisets of transitions for componentwise inclusion
	bool isSmaller(map<Transition*,int>& m1, map<Transition*,int>& m2);
	
};

}

#endif

