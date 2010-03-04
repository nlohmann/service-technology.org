// -*- C++ -*-

/*!
 * \file    tarjan.h
 *
 * \brief   Class Tarjan
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/16
 *
 * \date    $Date: 2009-10-16 14:47:01 +0200 (Fr, 16. Okt 2009) $
 *
 * \version $Revision: -1 $
 */

#ifndef TARJAN_H
#define TARJAN_H

#include <vector>
#include <map>
#include <set>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif

using pnapi::PetriNet;
using pnapi::Marking;
using pnapi::Place;
using pnapi::Transition;
using std::vector;
using std::map;
using std::set;

/** Graph for stubborn set method (for finding strongly connected components).
*/
class myTPNode {
 public:
	/// Constructor
	myTPNode();

	/// Destructor
	~myTPNode();

	/// Deinitializer
	void reset();

	/// Initializer
	void init();

	/// Successor nodes in the graph
	set<myTPNode*> nodes;

	/// Pointer to the transition, if this node belongs to a transition
	Transition* t;

	/// Pointer to the place, if this node belongs to a place
	Place* p;

	/// Depth counters for Tarjan's algorithm
	int index, low;

	/// Flag showing if this node is in the stack at this moment
	bool instack;
};

/** This class represents Tarjan's algorithm for finding strongly connected components
	used to determine which places in a Petri net need more tokens. It operates on a
	graph structure built from myTPNodes which should contain undermarked places and
	non-firable transitions.
*/
class Tarjan {

public:
	/// Standard constructor
	Tarjan(set<Transition*>& tset, set<Place*>& pset);

	/// Destructor.
	~Tarjan();

	/// returns a stubborn set of enabled transitions from the conflict graph
	void getComponents(vector<set<Transition*> >& tv, vector<set<Place*> >& pv);

	/// implements Tarjans algorithm for the conflict graph (with some optimization)
	void doTarjan(myTPNode* start, int& maxdfs, vector<set<Transition*> >& tv, vector<set<Place*> >& pv);

	/// Add an arc to the graph
	bool addArc(Transition* tfrom, Transition* tto);

	/// Add an arc to the graph
	bool addArc(Transition* tfrom, Place* pto);

	/// Add an arc to the graph
	bool addArc(Place* pfrom, Transition* tto);

	/// Add an arc to the graph
	bool addArc(Place* pfrom, Place* pto);

	/// Remove an arc from the graph
	bool removeArc(Transition* tfrom, Place* pto);

	/// Level of verbosity
	int verbose;

private:
	/// pointers from (non-firable) transitions to their unique nodes in the graph
	map<Transition*,myTPNode*> tton;

	/// pointers from (undermarked) places to their unique nodes in the graph
	map<Place*,myTPNode*> pton;

	/// stack for Tarjans algorithm
	vector<myTPNode*> st; 
};

#endif
