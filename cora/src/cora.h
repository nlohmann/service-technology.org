// -*- C++ -*-

/*!
 * \file    cora.h
 *
 * \brief   Class Cora
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/07/30
 *
 * \date    $Date: 2010-07-30 12:00:00 +0200 (Fr, 30. Jul 2010) $
 *
 * \version $Revision: 0.3 $
 */

#ifndef CORA_H
#define CORA_H

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <deque>
#include <pnapi/pnapi.h>
#include "cnode.h"
#include "covergraph.h"
#include "extmarking.h"
#include "imatrix.h"
#include "pnloader.h"
#include "rnode.h"
#include "stubbornset.h"

#include "verbose.h"

using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::deque;
using pnapi::PetriNet;
using pnapi::Transition;
using pnapi::Place;
using pnapi::Marking;

/*! \brief This class defines the main functionality. */
class Cora {
public:
	/// Standard constructor.
	Cora(PetriNet& net, Marking& m0, const vector<Place*>& porder, const vector<Transition*>& torder);

	/// Destructor.
	~Cora();

	/// set debug mode
	void setDebug(bool d);

	/// Find a firable path to reach a given marking
	deque<Transition*> findReach(Marking& m);

	/// Find a firable path to cover a given marking
	deque<Transition*> findCover(Marking& m);

	/// Find a firable path to the same node as a given path
	deque<Transition*> findPath(vector<Transition*>& path);

	/// Find a firable path to the same node as a given path
	void buildFullGraph();

	/// Print the coverability graph
	void printGraph(bool root);

	/// Print the number of paths searched
	void printStatus(int mode);

	/// The problems that can be solved
	enum CoraProblems {
		PATH = 0,
		REACH = 1,
		COVER = 2
	};

private:
	/// The Petri net
	PetriNet* pn;

	/// The initial marking of the Petri net
	Marking* init;

	/// An (arbitrary) ordering of all places of the net
	vector<Place*> pord;

	/// An (arbitrary) ordering of all transitions of the net
	vector<Transition*> tord;

	/// The incidence matrix of the net
	IMatrix im;

	/// The final marking of the net that should be reached or covered
	Marking* final;

	/// The final marking as an extended version
	ExtMarking* final2;

	/// Pointer to the stubborn set algorithm if used
	StubbornSet* stubset;

	/// The coverability graph that is built (possibly partially)
	CoverGraph* cg;

	/// Aim node when searching for a path
	CNode* aim;

	/// Number of paths investigated
	int loops;

	/// Debug flag
	bool debug;

	/// The outer solution algorithm
	deque<Transition*> solve(int mode);

	/// If the full coverability graph has been built
	bool full;
};

#endif

