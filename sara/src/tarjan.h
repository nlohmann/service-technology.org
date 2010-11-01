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

namespace sara {

/*! \brief Nodes for Tarjan's algorithm used to find new constraints
*/
class myTPNode {
 public:
	/// Constructor for nodes
	myTPNode();

	/// Destructor for nodes
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

	/// Depth counter for Tarjan's algorithm
	int index;

	/// Depth counter for Tarjan's algorithm
	int low;

	/// Flag showing if this node is in the stack at this moment
	bool instack;
};

/** \brief A specialized version of Tarjans algorithm for finding SCCs used for finding new constraints

	This class represents Tarjans algorithm for finding strongly connected components
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

	/// Get the SCCs as pairs of sets of transitions and places
	void getComponents(vector<set<Transition*> >& tv, vector<set<Place*> >& pv);

	/// Apply Tarjans algorithm for finding new constraints
	void doTarjan(myTPNode* start, int& maxdfs, vector<set<Transition*> >& tv, vector<set<Place*> >& pv);

	/// Add an arc to the graph
	bool addArc(Transition* tfrom, Place* pto);

	/// Add an arc to the graph
	bool addArc(Place* pfrom, Transition* tto);

	/// Add an arc to the graph
	bool addArc(Place* pfrom, Place* pto);

	/// Level of verbosity
	int verbose;

private:
	/// Pointers from (non-firable) transitions to their unique nodes in the graph
	map<Transition*,myTPNode*> tton;

	/// Pointers from (undermarked) places to their unique nodes in the graph
	map<Place*,myTPNode*> pton;

	/// A stack for Tarjans algorithm
	vector<myTPNode*> st; 
};

}

#endif

