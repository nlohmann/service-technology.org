// -*- C++ -*-

/*!
 * \file    tarjan.cc
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

#include <vector>
#include <map>
#include <set>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "tarjan.h"

using pnapi::PetriNet;
using pnapi::Marking;
using pnapi::Place;
using pnapi::Transition;
using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::endl;

	/***********************************
	* Implementation of class myPNode *
	***********************************/

/** Standard node constructor. Creates one node for Tarjan's algorithm. Used in constraint building.
*/
myPNode::myPNode() : p(NULL),index(-2),low(-1),instack(false) {}

/** Node destructor.
*/
myPNode::~myPNode() {}

/** Deinitializer. Reverts a node back to the state it had after the constructor call.
	The node is disconnected from any graph.
*/
void myPNode::reset() { index=-2; low=-1; instack=false; nodes.clear(); }

/** Initializer. Reverts a node back to the state it had before the first call of Tarjan's
	algorithm, but after graph construction, i.e. the graph structure stays intact.
*/
void myPNode::init() { if (index>-1) index=-1; low=-1; instack=false; }

	/*********************************
	* Implementation of class Tarjan *
	*********************************/

/** Constructor for a given set of places for which myPNodes are created
	automatically. Yields a structure to compute strongly connected components.
	@param pset The places to be contained in the graph for Tarjan's algorithm.
*/
Tarjan::Tarjan(PetriNet& pn) : net(pn) {
	const set<Place*> pset(pn.getPlaces());
	set<Place*>::const_iterator pit; // walk through the places
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		pton[*pit] = new myPNode(); // create a node
		pton[*pit]->p = *pit; // and make a reference to the place
	}
	st.clear();  // this is the stack for Tarjan's algorithm
	verbose = 0;
}

/** Destructor.
*/
Tarjan::~Tarjan() { // delete all myPNodes:
	map<Place*,myPNode*>::iterator pit;
	for(pit=pton.begin(); pit!=pton.end(); ++pit)
		delete pit->second;
}

/** Modification of Tarjans algorithm for finding strongly connected components.
	We look for strongly connected components of either more than one node
	or of one node with a self-loop. Single nodes without self-loops are disregarded.	
	@param start The root of the graph.
	@param maxdfs Recursive parameter for the depth in the depth-first-search, should be zero initially.
	@param pv Return parameter that will contain the sets of places
		constituting the strongly connected components.
*/
void Tarjan::doTarjan(myPNode* start, int& maxdfs, vector<set<Place*> >& pv) {
	start->index = maxdfs;
	start->low = maxdfs;
	++maxdfs;
	// Tarjans stack, begin with the start node(transition)
	st.push_back(start);
	// marker for "is contained in the stack"
	start->instack = true;
	set<myPNode*>::iterator it; // go through the graph recursively
	for(it=start->nodes.begin(); it!=start->nodes.end(); ++it)
	{
		if ((*it)->index<0) {
			doTarjan(*it,maxdfs,pv);
			if (start->low>(*it)->low) start->low = (*it)->low;
		} else if ((*it)->instack)
			if (start->low>(*it)->index) start->low = (*it)->index;
	}
	if (start->low == start->index) { // an SCC is found
		myPNode* go = NULL;
		set<Place*> pres; // for collecting the places of this SCC
		do {
			go = st.back(); // get the top of the stack
			go->instack = false; // mark it as removed from stack
			st.pop_back(); // and remove it
			pres.insert(go->p); // store the place
		} while (start!=go); // until we find the last element of the SCC
		pv.push_back(pres); // add to the list of SCCs
	}
}

/** Outer loop for Tarjans algorithm, calls the inner algorithm for each
	connected component.
	@param pv Return parameter that will contain the sets of places
		constituting the strongly connected components.
*/
void Tarjan::getComponents(vector<set<Place*> >& pv) {
	map<Place*,myPNode*>::iterator pit;
	int maxdfs = 0;
	// empty the stack for Tarjan
	st.clear();
	for(pit=pton.begin(); pit!=pton.end(); ++pit)
	{
		// call Tarjans algorithm for all nodes that have not been visited.
		if (pit->second->index==-1) 
			doTarjan(pit->second, maxdfs, pv); 
	}
}

/** Add an arc from place to place in the graph for Tarjan's algorithm.
	@param pfrom The source place of the arc.
	@param pto The sink place of the arc.
	@return False if one of the input parameters was a NULL pointer.
*/
bool Tarjan::addArc(Place* pfrom, Place* pto) {
	if (pfrom==NULL || pto==NULL) return false;
	pton[pfrom]->nodes.insert(pton[pto]);
	pton[pfrom]->index = -1;
	pton[pto]->index = -1;
	return true;
}

/** Construct place-based dependency graph from the Petri net.
*/
void Tarjan::buildGraph() {
	const set<Transition*> tset(net.getTransitions());
	set<Transition*>::const_iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		set<pnapi::Arc*>::iterator ait,ait2;
		const set<pnapi::Arc*>& prearcs((*tit)->getPresetArcs());
		const set<pnapi::Arc*>& postarcs((*tit)->getPostsetArcs());
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
			for(ait2=postarcs.begin(); ait2!=postarcs.end(); ++ait2)
				addArc(&((*ait)->getPlace()),&((*ait2)->getPlace()));
	}
}

