// -*- C++ -*-

/*!
 * \file    reachalyzer.h
 *
 * \brief   Class Reachalyzer
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/21
 *
 * \date    $Date: 2010-03-01 12:00:00 +0200 (Mo, 1. Mar 2010) $
 *
 * \version $Revision: -1 $
 */

#ifndef REACHALYZER_H
#define REACHALYZER_H

#include <vector>
#include <time.h>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "lpwrapper.h"
#include "jobqueue.h"

using pnapi::PetriNet;
using pnapi::Marking;
using std::vector;

/*! This class can analyze reachability problems. */
class Reachalyzer {
public:
/* unused
	// Constructor with a Petri net, initial and final marking, and debug level (0-3)
	Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, Problem& pb, bool verbose, int debug, bool out, int brk);
*/
	/// Constructor for partially testing coverability instead of exact reachability
	Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int> cover, Problem& pb, bool verbose, int debug, bool out, int brk, bool passedon);

	/// Destructor
	~Reachalyzer();

	/// Starts the reachability analysis
	void start();

	/// Prints out the result on stdout
	void printResult();

	/// Gets the CPU time used to solve the problem
	clock_t getTime();

	/// Gets the status after the constructor call or for the final result
	int getStatus();

/*
	// Gets the solution (if status==SOLUTION_FOUND) or one possible counterexample (if status==COUNTEREXAMPLE_FOUND)
	PartialSolution* getSolution();

	// Returns all possible counterexamples (if status==COUNTEREXAMPLE_FOUND)
	JobQueue& getFailureReasons();

	void extendTransitionOrder(map<Transition*,int> fullv, vector<Transition*>& ord, vector<Transition*> fseq);
*/

	/// Check if a solution has been seen earlier already, using a lookup table
	bool solutionSeen(map<Transition*,int>& tv);

	/// Calculate all possible jumps and add them to the JobQueue
	void createJumps(map<Transition*,int>& diff, PartialSolution& ps);

	/// Create the next precalculated jump from the given partial solution
	void nextJump(PartialSolution& ps);

	/// returns the maximal trace length after the solutions have been printed
	int getMaxTraceLength();

	/// Status
	enum errorMessage {
		SOLUTION_FOUND = 0,
		COUNTEREXAMPLE_FOUND = 1,
		UNSOLVED = 2,
		LPSOLVE_INIT_ERROR = 3,
		LPSOLVE_RESIZE_ERROR = 4,
		LPSOLVE_UPDATE_ERROR = 5,
		LPSOLVE_RUNTIME_ERROR = 6
	};

private:
	/// for timekeeping
	clock_t starttime, endtime;

	/// status
	int error;

	/// partial solutions vector, may contain a full solution as the last entry
	JobQueue tps;

	/// possible counterexamples, only valid if no solution can be found
	JobQueue failure;

	/// solutions of the problem
	JobQueue solutions;

	/// level of verbosity(stateinfo) and debug mode(verbose)
	int verbose;
	bool stateinfo;

	/// for checks if stdout has been redirected
	bool out;

	/// internal flags
	bool solved, errors;

	/// the initial marking
	Marking m1;

	/// the Petri net
	PetriNet& net;

	/// # of transitions (columns in the marking equation)
	unsigned int cols;

	/// object taking care of lp_solve
	LPWrapper lpwrap;

	/// the incidence matrix for the Petri net
	IMatrix im;

	/// vector of solutions of lp_solve with partial solutions from PathFinder
	map<map<Transition*,int>,vector<PartialSolution> > shortcut;

	/// debug info for when to stop execution
	int breakafter;

	/// longest trace length, only valid after solutions have been printed
	int maxsollen;

	/// If the problem was passed on from an earlier instance of the PathFinder
	bool passedon;

	/// If the problem was passed on, torealize contains the transition multiset to realize
	map<Transition*,int> torealize;
};

#endif

