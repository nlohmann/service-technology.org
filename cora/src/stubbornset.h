// -*- C++ -*-

/*!
 * \file    stubbornset.h
 *
 * \brief   Class StubbornSet
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/06/01
 *
 * \date    $Date: 2010-06-01 14:47:01 +0200 (Di, 1. Jun 2010) $
 *
 * \version $Revision: -1 $
 */

#ifndef STUBBORNSET_H
#define STUBBORNSET_H

#include <vector>
#include <map>
#include <set>
#include <string>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "imatrix.h"
#include "extmarking.h"

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

/** \brief The class responsible for finding a stubborn set.
*/
class StubbornSet {

public:
	/// Constructor with ordered list of all transitions tlist, goal marking goal, and incidence matrix im
	StubbornSet(vector<Transition*> tlist, Marking& goal, IMatrix& im, bool cov);

	/// Destructor.
	~StubbornSet();

	/// The algorithm to compute a stubborn set
	vector<Transition*> compute(ExtMarking& m);

	/// Sort a set of transitions(arcs) by the distance of the node they point to to the goal
	vector<Transition*> greedySort(set<Transition*> stubbornset);

	/// Get the goal marking
	Marking& getGoal();

private:
	/*! \brief Graph structure for stubborn set method (for finding strongly connected components).
	*/
	class Node {
	 public:
		Node();
		~Node();
		void reset();
		set<int> nodes;
		Transition* t;
		int index, low;
		bool instack;
	};

	/// The active marking
	ExtMarking m0;

	/// The final marking
	Marking aim;

	/// A conflict and dependency table for stubborn set search
	Cft cftab;

	/// Pointers from transitions to their unique nodes in the conflict graph
	vector<Node*> tton;

	/// A stack for Tarjans algorithm
	vector<Transition*> st; 

	/// An ordering of all transitions
	vector<Transition*> tord;

	/// Reverse of the ordering tord of all transitions
	map<Transition*,int> revtord;

	/// The incidence matrix of the Petri net
	IMatrix& im;

	/// Coverability or Reachability?
	bool cover;

	/// Calculate the conflicts and dependencies for a given set of transitions
	void conflictTable(Transition* tstart);

	/// Calculate one place that is a reason for nonfirability of a transition
	Place* hinderingPlace(Transition&);

	/// Calculate from the list of transitions to be fired those transitions that increment the token count on a place
	set<Transition*> requiredTransitions(Place&);

	/// Calculate a stubborn set of enabled transitions from the conflict graph
	set<Transition*> getSZK();

	/// Implementation of Tarjans algorithm for the conflict graph (with some optimization)
	bool doTarjan(Node*, set<Transition*>&, int&);

	/// Obtain the starting set of transitions for the stubborn set method
	set<Transition*> changesMarkingOn(Place* p, bool lower);
};

#endif

