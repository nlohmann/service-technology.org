// -*- C++ -*-

/*!
 * \file    witness.h
 *
 * \brief   Class Witness
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/11/03
 *
 * \date    $Date: 2010-11-03 11:22:00 +0200 (Mi, 03. Nov 2010) $
 *
 * \version $Revision: 1.0 $
 */

#ifndef WITNESS_H
#define WITNESS_H

#include <set>
#include <vector>
#include <map>
#include <iostream>
#include <pnapi/pnapi.h>
#include "formula.h"
#include "verbose.h"
#include <sara/sara_lib.h>

extern bool flag_verbose;

using std::set;
using std::vector;
using std::map;
using std::ostream;
using std::cout;
using std::endl;
using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using pnapi::Node;
using pnapi::Marking;



/*! \brief A node of of a siphon dependency graph to find a witness. */
class WNode {
public:
	/// Standard constructor (initial node)
	WNode();

	/// Standard constructor
	WNode(Place* p);

	/// Destructor
	~WNode();

	/// Compute a circuit-free allocation for the siphon
	void computeAllocation();

	/// Check if a transition is node-activated
	bool isActivated(map<Place*,unsigned int>& m, map<Place*,unsigned int>& tpre);

	/// Check if a transition is node-activated
	set<Transition*> getActivated(map<Place*,unsigned int>& m, map<Transition*,map<Place*,unsigned int> >& pre);

	/// Compute the postset of the siphon
	void computeSiphonPostset();

	/// Check if the maximal trap contains a token
	bool checkMarkedMaxTrap(map<Place*,unsigned int>& m);

	/// Create an edge from one node to another
	void createArc(Transition* t, Place* p, WNode& w);

	/// Remove all outgoing arcs
	void removeOutArcs();

	/// Remove all incoming arcs
	void removeInArcs();

	/// checks whether a token is produced on the defining (set of) place(s)
	bool producesDefiningToken(Transition& t);

	// Print a node
	//void show(ostream& out);

	/// The original place (NULL for initial node)
	Place* P;

	/// The siphon belonging to this node
	set<Place*> siphon;

	/// The maximal trap belonging to the siphon
	set<Place*> maxtrap;

	/// The edges going forward (to node)
	map<Transition*,WNode*> out;

	/// The edges going forward (place label)
	map<Transition*,Place*> outP;

	/// The edges coming in
	map<WNode*,map<Transition*,Place*> > in;

	/// The circuit-free allcoation for the siphon
	map<Place*,Transition*> alloc;

	/// Postset of the siphon, with a place from the siphon in the transitions preset
	map<Transition*,Place*> postsiphon;
};

/*! \brief A witness against liveness via Commoner-Hack in form of a siphon dependency graph. */
class Witness {
public:
	/// Standard constructor.
	Witness(PetriNet& net);

	/// Destructor.
	~Witness();

	/// A initial siphon without a marked trap (will be tested)
	void initSWOMT(const set<Place*>& swomt);

	/// Compute new minimal siphon and unmarked maxtrap
	bool computeSWOMT(WNode& wn);

	// Print all nodes
	//void show(ostream& out);

	/// Check if a transition is activated under the current marking
	bool isActivated(Transition* t);

	/// Fire a transition
	void fireTransition(Transition* t);

	/// Compute the siphon dependency graph
	set<Place*> computeGraph(vector<Transition*>& seq);

	/// Adapt the graph
	int adaptGraph(WNode& w, set<WNode*>& done);

	/// Call to the external tool Sara to solve the problem of enabling t
	vector<Transition*> callSara(Transition* t);

private:
	/// The Petri net
	PetriNet& pn;

	/// The active marking (starting with the initial marking)
	map<Place*,unsigned int> am;

	/// Formula for siphon and maxtrap, "unmarked" missing
	Formula formula;

	/// Set variable of the siphon in the formula
	setVar X;

	/// Set variable of the maxtrap in the formula
	setVar Y;

	/// The nodes of the graph, one node per place not in the initial siphon
	map<Place*,WNode*> node;

	/// The initial node
	WNode init;

	/// The computed firing sequence
	vector<Transition*> fseq;

	/// The "emptied" siphon
	set<Place*> res;

	/// The preset of transitions, abbreviation
	map<Transition*, map<Place*,unsigned int> > pre;

	/// The effective change when firing a transition
	map<Transition*, map<Place*,int> > change;

	/// The set of activated transition under the current marking
	set<Transition*> tact;
};

#endif

