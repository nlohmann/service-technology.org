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
 * \date    $Date: 2010-08-20 12:00:00 +0200 (Fr, 20. Aug 2010) $
 *
 * \version $Revision: 1.02 $
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

namespace sara {

/*! \brief This class can analyze reachability problems. */
class Reachalyzer {
public:
	/// Constructor
	Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int> cover, Problem& pb, bool verbose, int debug, bool out, int brk, bool passedon);

	/// Destructor
	~Reachalyzer();

	/// Start the reachability analysis
	void start();

	/// Print out the result on stdout
	void printResult(int pbnr);

	/// Get the CPU time used to solve the problem
	clock_t getTime() const;

	/// Get the status after the constructor call or for the final result
	int getStatus();

	/// Check if a solution has been seen earlier already, using a lookup table
	bool solutionSeen(map<Transition*,int>& tv);

	/// Calculate all possible jumps and add them to the JobQueue
	void createJumps(map<Transition*,int>& diff, PartialSolution& ps);

	/// Create the next precalculated jump from the given partial solution
	void nextJump(PartialSolution ps);

	/// Get the maximal trace length after the solutions have been printed
	int getMaxTraceLength() const;
	
	/// Get the sum over all solution lengths
	int getSumTraceLength() const;

	/// Get the number of solutions in the solutions queue
	int numberOfSolutions();

	/// Status
	enum errorMessage {
		SOLUTION_FOUND = 0,
		COUNTEREXAMPLE_FOUND = 1,
		UNSOLVED = 2,
		LPSOLVE_INIT_ERROR = 3,
		LPSOLVE_RESIZE_ERROR = 4,
		LPSOLVE_UPDATE_ERROR = 5,
		LPSOLVE_RUNTIME_ERROR = 6,
		SOLUTIONS_LOST = 7
	};

private:
	/// for timekeeping
	clock_t starttime, endtime;

	/// Status of the solver
	int error;

	/// The JobQueue of past, active, and future jobs
	JobQueue tps;

	/// Possible counterexamples, only valid if no solution can be found
	JobQueue failure;

	/// Solutions of the problem
	JobQueue solutions;

	/// The debug mode (0-3)
	int verbose;

	/// The Verbosity (true=on)
	bool stateinfo;

	/// For checks if stdout has been redirected
	bool out;

	/// Internal flag
	bool solved;

	/// Internal flag
	bool errors;

	/// The initial marking
	Marking m1;

	/// The Petri net
	PetriNet& net;

	/// # of transitions (columns in the marking equation)
	unsigned int cols;

	/// The Object taking care of lp_solve
	LPWrapper lpwrap;

	/// The incidence matrix for the Petri net
	IMatrix im;

	/// A vector of solutions of lp_solve with partial solutions from PathFinder
	map<map<Transition*,int>,vector<PartialSolution> > shortcut;

	/// Debug info for when to stop execution
	int breakafter;

	/// The longest trace length, only valid after solutions have been printed
	int maxsollen;

	/// The sum of all trace lengths, only valid after solutions have been printed
	int sumsollen;

	/// If the problem was passed on from an earlier instance of the PathFinder
	bool passedon;

	/// If the problem was passed on, torealize contains the transition multiset to realize
	map<Transition*,int> torealize;

	/// The problem instance to solve
	Problem& problem;

	/// The number of jobs done so far
	int loops;

	/// if a solution of lp_solve was suboptimal which means solutions can be lost
	bool suboptimal;
};

}

#endif

