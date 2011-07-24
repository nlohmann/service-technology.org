// -*- C++ -*-

/*!
 * \file    inveqrel.cc
 *
 * \brief   Class InvEqRel
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2011/07/13
 *
 * \date    $Date: 2011-07-13 14:47:01 +0200 (Mi, 13. Jul 2011) $
 *
 * \version $Revision: 1.0 $
 */

#include <vector>
#include <map>
#include <set>

#include "pnapi/pnapi.h"
#include "inveqrel.h"

using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using pnapi::Node;
using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::endl;

	/***********************************
	* Implementation of class InvEqRel *
	***********************************/

/** Constructor for a given Petri net.
	Yields a structure to compute the equivalence classes.
	@param pn The Petri net for which the equivalence classes of places are to be determined.
	@param ntype Whether the objects are places (true) or transitions (false).
	@param pricheck Optimize for less calls to lp_solve, induces some overhead.
*/
InvEqRel::InvEqRel(PetriNet& pn, bool ntype, bool pricheck) : start(0),net(pn),prejoin(0),nodetype(ntype),finv(false) {
	set<Place*> pset(pn.getPlaces());
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		places.insert(static_cast<Node*>(*pit));
	set<Transition*> tset(pn.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
		transitions.insert(static_cast<Node*>(*tit));
	set<Node*> nset(nodetype?places:transitions);
	// the finer relation `above' separates non-equivalent nodes by putting them into different sets
	above.push_back(nset);
	set<Node*> empty;
	set<Node*>::iterator nit;
	for(nit=nset.begin(); nit!=nset.end(); ++nit)
	{
		// `toclass' points each node to the correct set in `above'
		toclass[*nit]=0;
		// the coarser relation `below' keeps equivalent nodes in a single set
		below[*nit]=empty;
		// a union-find algorithm is used to obtain the class where a node is in `below'
		unionfind.push_back(-1);
		nmap[*nit]=static_cast<int>(nvec.size());
		nvec.push_back(*nit);
	}
	if (pricheck) initPriorityChecks();
}

/** Destructor.
*/
InvEqRel::~InvEqRel() { }

/** Makes two nodes equivalent.
	@param n1 The first node.
	@param n2 The second node.
	@return If the operation was successful.
*/
bool InvEqRel::join(Node* n1, Node* n2) {
	// get the class of n1 and check whether it is an equivalence class at all
	n1 = findClass(n1);
	if (below.find(n1)==below.end()) return false;
	// same for n2, and check if it's in the same class as n1 already
	n2 = findClass(n2);
	if (n1==n2) return false;
	if (below.find(n2)==below.end()) return false;
	// get the position of n2 in the finer relation `above'
	unsigned int point(toclass[n2]);
	// and compare it to that of n1 -- inequality means n1 and n2 cannot be equivalent
	if (toclass[n1]!=point) return false;
	// add the class of n2 in the coarser relation `below' to that of n1
	set<Node*> nset(below[n2]);
	set<Node*>::iterator nit;
	for(nit=nset.begin(); nit!=nset.end(); ++nit)
		below[n1].insert(*nit);
	// and add n2 itself to that class
	below[n1].insert(n2);
	// delete n2 and its class from `below'
	below.erase(n2);
	// and put n2 into the union-find-tree
	unionfind[nmap[n2]]=nmap[n1];
	// delete n2 from the finer relation -- we do not keep equivalent entries here
	above[point].erase(n2);
	toclass.erase(n2);
	return true;
}

/** Makes two nodes equivalent in the pre-lp_solve phase. Calls to this function are counted. 
	@param n1 The first node.
	@param n2 The second node.
	@return If the operation was successful.
*/
bool InvEqRel::pjoin(Node* n1, Node* n2) {
	if (join(n1,n2)) { ++prejoin; return true; }
	return false;
}

/** Detects from a semipositive invariant which nodes can never be equivalent.
	@param inv The semipositive invariant.
*/
void InvEqRel::split(map<Node*,int> inv) {
	// we split up each class in the finer `above' relation into two subsets:
	// one containing all the nodes with zero entries in inv, the other
	// containing those with non-zero entries.
	set<Node*>::iterator nit;
	unsigned int end(static_cast<unsigned int>(above.size()));
	// sets before `start' are singletons or empty, we don't need to check there
	// sets after `end' are those just created -- they already have been split
	for(unsigned int i=start; i<end; ++i)
	{
		set<Node*> full(above[i]);
		while (!full.empty()) 
		{
			// don't split if there is nothing to split
			if (full.size()<2) { full.clear(); continue; }
			set<Node*> nset;
			// entry of the first element in inv
			int cmp(inv[*(full.begin())]);
			// get all nodes with same entry in inv as the first element
			for(nit=full.begin(); nit!=full.end(); ++nit)
				if (inv[*nit]==cmp) nset.insert(*nit);
			// if all or no nodes have to be split away, there is nothing to do
			if (nset.size()==full.size() || nset.empty()) 
				{ full.clear(); continue; }
			// remove the collected nodes from the active set
			for(nit=nset.begin(); nit!=nset.end(); ++nit) 
			{
				above[i].erase(*nit);
				full.erase(*nit);
				// and also adapt the pointers to the new set (to be created) 
				toclass[*nit]=static_cast<unsigned int>(above.size());
			}
			// add the new set to the end of the vector
			above.push_back(nset);
			if (!finv) full.clear(); // only differentiate once, by support
		}
	}
}

/** Obtain a vector containing the equivalence relation as far as it has been computed.
	@param approx This parameter decides if we get the finer (false) or the coarser 
		(true) version of the relation.
		If the computation is completed, both will coincide.
	@return A vector of equivalence classes of nodes.
*/
vector<set<Node*> > InvEqRel::getClasses(bool approx) {
	vector<set<Node*> > res;
	// in case we want the coarser relation ... 
	if (approx)
	{
		// we just pick up the classes in `below' of known-to-be-equivalent nodes
		map<Node*,set<Node*> >::iterator mit;
		for(mit=below.begin(); mit!=below.end(); ++mit)
		{
			res.push_back(mit->second);
			// we need to add the representative of the class, since
			// it isn't contained in the class itself
			res[res.size()-1].insert(mit->first);
		}
		return res;
	}
	// in case we want the finer relation, we go through `above'
	for(unsigned int i=0; i<above.size(); ++i)
	{
		// we check for empty sets (though they won't occur if the programming is correct)
		if (above[i].size()==0) continue;
		set<Node*> nset;
		set<Node*>::iterator nit;
		// we go through each set of nodes, but note that there are no
		// known-to-be-equivalent nodes in these sets
		for(nit=above[i].begin(); nit!=above[i].end(); ++nit)
		{
			// therefore we do not only need the places itself
			nset.insert(*nit);
			set<Node*>::iterator nit2;
			// but also all nodes known to be equivalent, as given by `below'
			for(nit2=below[*nit].begin(); nit2!=below[*nit].end(); ++nit2)
				nset.insert(*nit2);
		}
		// we picked up a set of nodes that are pairwise either equivalent
		// or undecided
		res.push_back(nset);
	}
	return res;
}

/** Obtain two nodes for which equivalence is undecided so far.
	@param n1 Empty pointer, to be filled with the first node.
	@param n2 Empty pointer, to be filled with the second node.
	@return If two nodes could be obtained. (Otherwise the equivalence relation has been fully computed.)
*/
bool InvEqRel::getUndecided(Node*& n1, Node*& n2) { 
	if (findPriorityCheck(n1,n2)) return true;
	// increment that start pointer to the finer relation `above' to the first interesting class
	while (start<above.size() && above[start].size()<2) ++start;
	// if there is none, we are done
	if (start>=above.size()) return false;
	// otherwise, get the first two nodes in this class
	set<Node*>::iterator nit(above[start].begin());
	n1 = *nit;
	++nit;
	n2 = *nit;
	return true;
}

/** Compute some simple equivalences that can be found by looking at the local structure
	of the net, without using lp_solve.
	@param fullinv If node must have the same weight in invariants for the equivalence to hold.
*/
void InvEqRel::simpleEquivalences(bool fullinv) {
	// save the parameter
	finv = fullinv;
	// we check for every node of the other type ...
	set<Node*> nset(nodetype?transitions:places);
	set<Node*>::iterator nit;
	for(nit=nset.begin(); nit!=nset.end(); ++nit)
	{
		set<pnapi::Arc*> arcs1((*nit)->getPresetArcs());
		set<pnapi::Arc*> arcs2((*nit)->getPostsetArcs());
		set<pnapi::Arc*>::iterator ait;
		// if it has an empty preset
		if (arcs1.empty())
		{
			if (arcs2.empty()) continue;
			// the nodes in the postset have weight zero in every invariant
			ait = arcs2.begin();
			// so, select a node from the postset as class representative
			Node* n(&((*ait)->getTargetNode()));
			// and put all postset nodes into that class, as they are eqivalent
			for(++ait; ait!=arcs2.end(); ++ait)
				pjoin(n,&((*ait)->getTargetNode()));
		} else if (arcs2.empty()) {
			// if it has an empty postset, do the same for the preset
			ait = arcs1.begin();
			Node* n(&((*ait)->getSourceNode()));
			for(++ait; ait!=arcs1.end(); ++ait)
				pjoin(n,&((*ait)->getSourceNode()));
		} else if (arcs1.size()==1 && arcs2.size()==1) {
			// if the intermediate node has singleton pre- and postsets,
			// the two nodes occur in all invariant with the same weight,
			// so make them equivalent
			Node* n1(&((*(arcs1.begin()))->getSourceNode()));
			Node* n2(&((*(arcs2.begin()))->getTargetNode()));
			if (!fullinv || (*(arcs1.begin()))->getWeight()==(*(arcs2.begin()))->getWeight())
				pjoin(n1,n2);
		}
	}
}

/** Find the representative of the class containing a given node, in the coarser relation `below'.
	@param n The node for which the representative is sought.
	@return The representative (or NULL in case of an error).
*/
Node* InvEqRel::findClass(Node* n) {
	int j(findClassNum(n));
	return (j<0 ? NULL : nvec[j]);
}

/** Compute the ID of the class containing a given node, in the coarser relation `below'.
	@param n The node for which the representative is sought.
	@return The number appointed to the class.
*/
int InvEqRel::findClassNum(Node* n) {
	map<Node*,int>::iterator hit(nmap.find(n));
	if (hit==nmap.end()) return -1;
	int j(hit->second);
	while (unionfind[j]>=0) j=unionfind[j];
	return j;
}

/** Get the number of equivalences found prior to using lp_solve.
	@return The number of equivalences found.
*/
unsigned int InvEqRel::preJoinsDone() const { return prejoin; }

/** Initialize the list of priority checks. These checks may reduce
	the number of calls to lp_solve.
*/
void InvEqRel::initPriorityChecks() {
	set<Node*> nset(nodetype?transitions:places);
	set<Node*>::iterator nit;
	for(nit=nset.begin(); nit!=nset.end(); ++nit)
	{
		set<pnapi::Arc*> prearcs((*nit)->getPresetArcs());
		set<pnapi::Arc*> postarcs((*nit)->getPostsetArcs());
		if (prearcs.empty() || postarcs.empty()) continue;
		bool which(prearcs.size()==1);
		if ((which) ^ (postarcs.size()!=1)) continue;
		set<Node*> nset2;
		set<pnapi::Arc*>::iterator ait;
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
			if (which) ppnode.push_back(&((*ait)->getSourceNode()));
			else nset2.insert(&((*ait)->getSourceNode()));
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
			if (!which) ppnode.push_back(&((*ait)->getTargetNode()));
			else nset2.insert(&((*ait)->getTargetNode()));
		ppcheck.push_back(nset2);
	}
}

/** If there are priority checks to be done, get one of them.
	@param n1 The first node of the pair to be checked. 
	@param n2 The second node of the pair to be checked.
	@return If there are checks to be done. If not, n1 and n2 remain unchanged.
*/
bool InvEqRel::findPriorityCheck(Node*& n1, Node*& n2) {
	while (!ppcheck.empty())
	{
		set<Node*> nset(ppcheck.back());
		set<Node*>::iterator nit(nset.begin());
		Node* eqclass1(findClass(*nit));
		Node* eqclass2(NULL);
		unsigned int clss(toclass[eqclass1]);
		bool destroy(false);
		for(++nit; nit!=nset.end(); ++nit)
		{
			eqclass2 = findClass(*nit);
			if (clss!=toclass[eqclass2]) { destroy=true; break; }
			if (eqclass1!=eqclass2) break; 
			ppcheck.back().erase(*nit);
		}
		if (ppcheck.back().size()<2 || destroy)
		{
			if (!destroy) pjoin(eqclass1,ppnode.back());
			ppcheck.pop_back();
			ppnode.pop_back();
			continue;
		}
		n1 = eqclass1;
		n2 = eqclass2;
		return true;
	}
	return false;
}

