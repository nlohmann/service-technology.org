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
	* Implementation of class myTPNode *
	***********************************/

/** Standard constructor. Creates one node for Tarjan's algorithm.
*/
myTPNode::myTPNode() : t(NULL),p(NULL),index(-2),low(-1),instack(false) { nodes.clear(); }

/** Destructor.
*/
myTPNode::~myTPNode() {}

/** Deinitializer. Reverts a node back to the state it had after the constructor call.
	The node is disconnected from any graph.
*/
void myTPNode::reset() { index=-2; low=-1; instack=false; nodes.clear(); }

/** Initializer. Reverts a node back to the state it had before the first call of Tarjan's
	algorithm, but after graph construction, i.e. the graph structure stays intact.
*/
void myTPNode::init() { if (index>-1) index=-1; low=-1; instack=false; }

	/*********************************
	* Implementation of class Tarjan *
	*********************************/

/** Constructor for a given set of places and transitions for which myTPNodes are created
	automatically. Yields a structure to compute strongly connected components.
	@param tset The transitions to be contained in the graph for Tarjan's algorithm.
	@param pset The places to be contained in the graph for Tarjan's algorithm.
*/
Tarjan::Tarjan(set<Transition*>& tset, set<Place*>& pset) {
	set<Transition*>::iterator it; // walk through the transitions
	for(it=tset.begin(); it!=tset.end(); ++it)
	{
		tton[*it] = new myTPNode(); // create a node
		tton[*it]->t = *it; // set a reference to the transition
		tton[*it]->p = NULL; // but not to a place, or course
	}
	set<Place*>::iterator pit; // walk through the places
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		pton[*pit] = new myTPNode(); // create a node
		pton[*pit]->p = *pit; // and this time make a reference to the place
		pton[*pit]->t = NULL;
	}
	st.clear();  // this is the stack for Tarjan's algorithm
	verbose = 0;
}

/** Destructor.
*/
Tarjan::~Tarjan() { // delete all myTPNodes:
	map<Transition*,myTPNode*>::iterator it;
	for(it=tton.begin(); it!=tton.end(); ++it)
		delete it->second;
	map<Place*,myTPNode*>::iterator pit;
	for(pit=pton.begin(); pit!=pton.end(); ++pit)
		delete pit->second;
}

/** Modification of Tarjans algorithm for finding strongly connected components.
	We look for strongly connected components of either more than one node
	or of one node with a self-loop. Single nodes without self-loops are disregarded.	
	@param start The root of the graph.
	@param maxdfs Recursive parameter for the depth in the depth-first-search, should be zero initially.
	@param tv Return parameter that will contain the sets of transitions
		constituting the strongly connected components.
	@param pv Return parameter that will contain the sets of places
		constituting the strongly connected components. Sets in pv
		and tv with the same index belong to each other.
*/
void Tarjan::doTarjan(myTPNode* start, int& maxdfs, vector<set<Transition*> >& tv, vector<set<Place*> >& pv) {
	start->index = maxdfs;
	start->low = maxdfs;
	++maxdfs;
	// Tarjans stack, begin with the start node(transition)
	st.push_back(start);
	// marker for "is contained in the stack"
	start->instack = true;
	set<myTPNode*>::iterator it; // go through the graph recursively
	for(it=start->nodes.begin(); it!=start->nodes.end(); ++it)
	{
		if ((*it)->index<0) {
			doTarjan(*it,maxdfs,tv,pv);
			if (start->low>(*it)->low) start->low = (*it)->low;
		} else if ((*it)->instack)
			if (start->low>(*it)->index) start->low = (*it)->index;
	}
	if (start->low == start->index) { // an SCC is found
		myTPNode* go = NULL;
//		bool ok = false;
		set<Transition*> tres; // for collecting the transitions of this SCC
		set<Place*> pres; // for collecting the places of this SCC
		do {
			go = st.back(); // get the top of the stack
			go->instack = false; // mark it as removed from stack
			st.pop_back(); // and remove it
			if (go->t) tres.insert(go->t); // it's a transition
			if (go->p) pres.insert(go->p); // it's a place
		} while (start!=go); // until we find the last element of the SCC
		// we are interested in self-loops or components of more than one node:
		if (tres.size()+pres.size()>1 || start->nodes.find(start)!=start->nodes.end())
		{	tv.push_back(tres); pv.push_back(pres); } // add to the list of SCCs
	}
}

/** Outer loop for Tarjans algorithm, calls the inner algorithm for each
	connected component.
	@param tv Return parameter that will contain the sets of transitions
		constituting the strongly connected components.
	@param pv Return parameter that will contain the sets of places
		constituting the strongly connected components. Sets in pv
		and tv with the same index belong to each other.
*/
void Tarjan::getComponents(vector<set<Transition*> >& tv, vector<set<Place*> >& pv) {
	map<Transition*,myTPNode*>::iterator git;
	map<Place*,myTPNode*>::iterator pit;
/*	cerr << "TJ: ";
	for(git=tton.begin(); git!=tton.end(); ++git)
	if (git->second->index>-2)
	{
		cerr << git->first->getName() << ":";
		set<myTPNode*>::iterator nit;
		for(nit=git->second->nodes.begin(); nit!=git->second->nodes.end(); ++nit)
			if ((*nit)->p) cerr << (*nit)->p->getName() << " ";
		cerr << "|";
	}
	for(pit=pton.begin(); pit!=pton.end(); ++pit)
	if (pit->second->index>-2)
	{
		cerr << pit->first->getName() << ":";
		set<myTPNode*>::iterator nit;
		for(nit=pit->second->nodes.begin(); nit!=pit->second->nodes.end(); ++nit)
			if ((*nit)->t) cerr << (*nit)->t->getName() << " ";
		cerr << "|";
	}	
	cerr << endl;
*/
	int maxdfs = 0;
	// empty the stack for Tarjan
	st.clear();
	for(git=tton.begin(); git!=tton.end(); ++git)
	{
		// call Tarjans algorithm for all nodes that have not been visited.
		if (git->second->index==-1) 
			doTarjan(git->second, maxdfs, tv, pv); 
	}
	for(pit=pton.begin(); pit!=pton.end(); ++pit)
	{
		// call Tarjans algorithm for all nodes that have not been visited.
		if (pit->second->index==-1) 
			doTarjan(pit->second, maxdfs, tv, pv); 
	}
/*	for(int i=0; i<tv.size(); ++i)
	{
		cerr << "TJ: ";
		set<Transition*>::iterator tit;
		for(tit=tv[i].begin(); tit!=tv[i].end(); ++tit)
			cerr << (*tit)->getName() << " ";
		cerr << ":";
		set<Place*>::iterator pit;
		for(pit=pv[i].begin(); pit!=pv[i].end(); ++pit)
			cerr << (*pit)->getName() << " ";
		cerr << endl;
	}
*/
}

/** Add an arc from transition to transition in the graph for Tarjan's algorithm.
	@param tfrom The source transition of the arc.
	@param tto The sink transition of the arc.
	@return False if one of the input parameters was a NULL pointer.
*/
bool Tarjan::addArc(Transition* tfrom, Transition* tto) {
	if (tfrom==NULL || tto==NULL) return false;
	tton[tfrom]->nodes.insert(tton[tto]);
	tton[tfrom]->index = -1;
	tton[tto]->index = -1;
	return true;
}

/** Add an arc from transition to place in the graph for Tarjan's algorithm.
	@param tfrom The source transition of the arc.
	@param pto The sink place of the arc.
	@return False if one of the input parameters was a NULL pointer.
*/
bool Tarjan::addArc(Transition* tfrom, Place* pto) {
	if (tfrom==NULL || pto==NULL) return false;
	tton[tfrom]->nodes.insert(pton[pto]);
	tton[tfrom]->index = -1;
	pton[pto]->index = -1;
	return true;
}

/** Add an arc from place to transition in the graph for Tarjan's algorithm.
	@param pfrom The source place of the arc.
	@param tto The sink transition of the arc.
	@return False if one of the input parameters was a NULL pointer.
*/
bool Tarjan::addArc(Place* pfrom, Transition* tto) {
	if (pfrom==NULL || tto==NULL) return false;
	pton[pfrom]->nodes.insert(tton[tto]);
	pton[pfrom]->index = -1;
	tton[tto]->index = -1;
	return true;
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

/** Remove an arc from transition to place in the graph for Tarjan's algorithm.
	@param tfrom The source transition of the arc.
	@param pto The sink place of the arc.
	@return False if one of the input parameters was a NULL pointer.
*/
bool Tarjan::removeArc(Transition* tfrom, Place* pto) {
	if (tfrom==NULL || pto==NULL) return false;
	tton[tfrom]->nodes.erase(pton[pto]);
	tton[tfrom]->index = -1;
	pton[pto]->index = -1;
	return true;
}

