// -*- C++ -*-

/*!
 * \file    tarjan.h
 *
 * \brief   Class Tarjan
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2011/03/14
 *
 * \date    $Date: 2011-03-14 23:00:00 +0200 (Mo, 14. Mar 2011) $
 *
 * \version $Revision: 1.1 $
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


/*! \brief Nodes for Tarjan's algorithm used to find new constraints
*/
class myPNode {
 public:
	/// Constructor for nodes
	myPNode();

	/// Destructor for nodes
	~myPNode();

	/// Deinitializer
	void reset();

	/// Initializer
	void init();

	/// Successor nodes in the graph
	set<myPNode*> nodes;

	/// Pointer to the place, if this node belongs to a place
	Place* p;

	/// Depth counter for Tarjan's algorithm
	int index;

	/// Depth counter for Tarjan's algorithm
	int low;

	/// Flag showing if this node is in the stack at this moment
	bool instack;
};

/** \brief A specialized version of Tarjans algorithm for finding SCCs

	This class represents Tarjans algorithm for finding strongly connected components.
	It operates on a graph structure built from myPNodes.
*/
class Tarjan {

public:
	/// Standard constructor
	Tarjan(PetriNet& pn);

	/// Destructor.
	~Tarjan();

	/// Get the SCCs as sets of places
	void getComponents(vector<set<Place*> >& pv);

	/// Apply Tarjans algorithm (inner section)
	void doTarjan(myPNode* start, int& maxdfs, vector<set<Place*> >& pv);

	/// Add an arc to the graph
	bool addArc(Place* pfrom, Place* pto);

	/// Construct a place-only dependency graph from the Petri net
	void buildGraph();

	/// Level of verbosity
	int verbose;

private:
	/// Pointers from places to their unique nodes in the graph
	map<Place*,myPNode*> pton;

	/// A stack for Tarjans algorithm
	vector<myPNode*> st; 

	/// The Petri net on which the graph is based
	PetriNet& net;
};


#endif

