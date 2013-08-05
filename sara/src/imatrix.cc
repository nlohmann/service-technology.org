// -*- C++ -*-

/*!
 * \file    imatrix.cc
 *
 * \brief   Class IMatrix
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/14/07
 *
 * \date    $Date: 2012-06-13 12:00:00 +0200 (Mi, 13. Jun 2012) $
 *
 * \version $Revision: 1.10 $
 */

#include <set>
#include <map>
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "imatrix.h"
#include "sthread.h"
#include <pthread.h>

using pnapi::PetriNet;
using pnapi::Transition;
using pnapi::Place;
using pnapi::Marking;
using pnapi::Arc;
using std::set;
using std::map;
using std::vector;
using std::cerr;
using std::endl;

	/*****************************************************
	* Implementation of the methods of the class IMatrix *
	*****************************************************/

namespace sara {

/** Constructor
	@param pn The Petri net for which the incidence matrix is to be built.
*/
IMatrix::IMatrix(PetriNet& pn) : verbose(0),petrinet(pn) { precompute(); }

/** Called by constructor. Computes all the necessary internal relations
	and values.
*/
void IMatrix::precompute() {
	// for iteration over the transitions of the Petri net
	set<Transition*> tset(petrinet.getTransitions());
	set<Transition*>::iterator tit;
	numtransitions = tset.size();

	// for counting the maximal pre/postset size
	tmaxpreset = 0;
	tmaxpostset = 0;

	// go through all transitions
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		// for the postset of this transition:
		set<Arc*>::iterator ait;
		set<Arc*> arcs = (*tit)->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// get the connected places with their arc weight
			Place* p = &((*ait)->getPlace());
			int postweight = (*ait)->getWeight();

			// calculate the change of firing the transition on p
			mat[*tit][p] = postweight; // first get the postset
			// and save the weight for the transition as well as the place
			post[*tit][p] = postweight;
			ppre[p][*tit] = postweight;
		}

		// now for the preset of the transition:
		arcs = (*tit)->getPresetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// get the connected places with their arc weight
			Place* p = &((*ait)->getPlace());
			int preweight = (*ait)->getWeight();

			// calculate the number of direct loops between p and the transition
			int loops = (preweight>mat[*tit][p] ? mat[*tit][p] : preweight); // count the loops on a place
			if (loops>0) loop[*tit][p] = loops; // and save this number
			// continue calculating the change of firing the transition on p			
			mat[*tit][p] -= preweight; // calculate the change the transition produces
			// and save the weight for the transition as well as the place
			pre[*tit][p] = preweight;
			ppost[p][*tit] = preweight;
		}

		// adapt the maximal pre/postset size
		unsigned int tmp((*tit)->getPreset().size());
		if (tmp>tmaxpreset) tmaxpreset=tmp;
		tmp = ((*tit)->getPostset().size());
		if (tmp>tmaxpostset) tmaxpostset=tmp;
	}

	// run through the places to find the maximal pre/postsets
	set<Place*> pset(petrinet.getPlaces());
	set<Place*>::iterator pit;
	numplaces = pset.size();
	pmaxpreset = 0;
	pmaxpostset = 0;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		unsigned int tmp((*pit)->getPreset().size());
		if (tmp>pmaxpreset) pmaxpreset=tmp;
		tmp = ((*pit)->getPostset().size());
		if (tmp>pmaxpostset) pmaxpostset=tmp;
	}
	maxconflict=tmaxpreset*pmaxpostset;
	if (maxconflict>numtransitions) maxconflict=numtransitions;
}

/** Get one column of the incidence matrix.
	@param t The transition identifying the column.
	@return A map from places to int representing the column belonging to t. Only places
			with an arc from or to t will be part of the map. A zero entry shows a place
			with the same positive weight for both arc directions.
*/
map<Place*,int>& IMatrix::getColumn(Transition& t) { return mat[&t]; }

/** Get the postset of a transition as a map. Places not contained in the postset will not appear in the map.
	@param t The transition.
	@return The map from places to arc weights.
*/
map<Place*,int>& IMatrix::getPostset(Transition& t) { return post[&t]; }

/** Get the postset of a place as a map. Transitions not contained in the postset will not appear in the map.
	@param p The place.
	@return The map from transitions to arc weights.
*/
map<Transition*,int>& IMatrix::getPostset(Place& p) { return ppost[&p]; }

/** Get the preset of a transition as a map. Places not in the preset will not appear in the map.
	@param t The transition.
	@return The map from places to arc weights.
*/
map<Place*,int>& IMatrix::getPreset(Transition& t) { return pre[&t]; }

/** Get the preset of a place as a map. Transitions not contained in the preset will not appear in the map.
	@param p The place.
	@return The map from transitions to arc weights.
*/
map<Transition*,int>& IMatrix::getPreset(Place& p) { return ppre[&p]; }

/** Get one entry from the incidence matrix.
	@param t The transition/column of the incidence matrix.
	@param p The place/row of the incidence matrix.
	@return The number of producing arcs minus the number of consuming arcs.
*/ 
int IMatrix::getEntry(Transition& t, Place& p) { 
	map<Place*,int>::iterator pit(mat[&t].find(&p));
	return (pit!=mat[&t].end() ? pit->second : 0); // check if there is an arc so we won't change the map
}

/** Get the number of loops between a place and a transition.
	@param t The transition/column of the incidence matrix.
	@param p The place/row of the incidence matrix.
	@return The minimum of {arc weight from t to p, arc weight from p to t}.
*/
int IMatrix::getLoops(Transition&t, Place& p) {
	map<Place*,int>::iterator pit(loop[&t].find(&p));
	return (pit!=loop[&t].end() ? pit->second : 0); // check if there are loops so we won't change the map
}

/** Get the number of arcs from a transition to a place.
	@param t The transition/column of the incidence matrix.
	@param p The place/row of the incidence matrix.
	@return The number of arcs from t to p.
*/ 
int IMatrix::getWeight(Transition& t, Place& p) {
	map<Place*,int>::iterator pit(post[&t].find(&p));
	return (pit!=post[&t].end() ? pit->second : 0); // check if there is a place so we won't change the map
}

/** Get the number of arcs from a place to a transition.
	@param p The place/row of the incidence matrix.
	@param t The transition/column of the incidence matrix.
	@return The number of arcs from p to t.
*/ 
int IMatrix::getWeight(Place& p, Transition& t) {
	map<Place*,int>::iterator pit(pre[&t].find(&p));
	return (pit!=pre[&t].end() ? pit->second : 0); // check if there is a place so we won't change the map
}

/* Calculate the loops at some transition.
	@param t The transition.
	@return A map from all places with loops at t to the (lesser) arc weight in the loop.
*/
//map<Place*,int>& IMatrix::getLoopColumn(Transition&t) {	return loop[&t]; }

/** Calculate the marking previous to firing a transition.
	@param m The marking. The method changes this marking by reverse-firing t. No checks
		are done if this reverse firing is possible, i.e. the resulting marking may be invalid.
	@param t The transition for which the effects should be reversed.
*/
void IMatrix::predecessor(Marking& m, Transition &t)
{
	map<Place*,int>& pmap(getColumn(t)); // get the change
	map<Place*,int>::iterator pit;
	for(pit=pmap.begin(); pit!=pmap.end(); ++pit)
		m[*(pit->first)] -= pit->second; // and subtract it from the marking
	// note: there may be zero entries in a marking afterwards. Comparison of two
	// markings has to reflect this possibility.
}

/** Calculate the successor of a marking after firing a transition.
	@param m The marking. The method changes this marking by firing t, even if t is not enabled.
		In this case, the marking may become invalid.
	@param t The transition to be fired.
*/
void IMatrix::successor(Marking& m, Transition &t)
{
	map<Place*,int>& pmap(getColumn(t)); // get the change
	map<Place*,int>::iterator pit;
	for(pit=pmap.begin(); pit!=pmap.end(); ++pit)
		m[*(pit->first)] += pit->second; // and add it to the marking
	// note: there may be zero entries in a marking afterwards. Comparison of two
	// markings has to reflect this possibility.
}

/** Compares the effect of two Transitions t1 and t2; if fired under
	the same marking the result on some place p is lower for t1
	than for t2, the place p is in the resulting output set.
	@param thr The calling thread (for reuse of local memory).
	@param t1 Anchor transition.
	@param t2 The transition to compare t1 to.
	@return The set of places that are marked lower by t1 than by t2.
*/
vector<Place*>& IMatrix::compareOutput(SThread* thr, Transition& t1, Transition& t2)
{
	// the resulting set of places, to be built
	vector<Place*>& result(thr->psets);
	result.clear();

	// get the incidence columns of t1 and t2
	map<Place*,int>& t1map(getColumn(t1));
	map<Place*,int>& t2map(getColumn(t2));
	map<Place*,int>::iterator it;
//	result.reserve(t1map.size()+t2map.size());

	// check all places connected with t1
	for(it=t1map.begin(); it!=t1map.end(); ++it)
	{
		Place* p(it->first);
		map<Place*,int>::iterator t2it(t2map.find(p));
		// if the place is connected to t1 and t2
		if (t2it!=t2map.end())
		{
			// if t1 yields less tokens on p than t2
			if (it->second<t2it->second) result.push_back(p); 
		} else {
			// if t1 effectively consumes tokens from p (which has no connection to t2)
			if (it->second<0) result.push_back(p);
		}		
	}
	// check those places connected with t2 that have no connection with t1
	for(it=t2map.begin(); it!=t2map.end(); ++it)
	{
		Place* p(it->first);
		// if there is no connection to t1
		if (t1map.find(p)==t1map.end())
		{
			// if t2 effectively produces tokens on p (and thus t1 yields less)
			if (it->second>0) result.push_back(p); 
		} 
	}
	return result;
}

/** Checks whether a firing sequence is activated under a marking
	disregarding all places not in a given restriction set. On places outside the set
	the marking is automatically assumed to contain infinitely may tokens (independent
	from the actual value).
	@param m The marking AFTER the firing sequence.
	@param tv The transition vector containing the firing sequence
	@param restriction The set of places which should be tested for undermarking.
		All other places are regarded as having an arbitrary high number of tokens available.
	@return Whether the firing sequence is reverse-firable under m regarding the restriction set.
*/
bool IMatrix::checkRestrictedActivation(Marking& m, vector<Transition*>& tv, vector<Place*>& restriction)
{
	// go through all places in the restriction
	for(unsigned int i=0; i<restriction.size(); ++i)
	{
		Place* p(restriction[i]);
		int tokens(m[*p]);
		for(int j=tv.size()-1; j>=0; --j)
		{
			tokens -= getWeight(*(tv[j]),*p);
			if (tokens<0) return false;
			tokens += getWeight(*p,*(tv[j]));
		}
	}
	return true;	
}

/** Calculate the change in the marking a multiset of transitions will bring upon firing.
	No check for realizability of the multiset is performed.
	@param fv The multiset of transitions.
	@return The token change.
*/
map<Place*,int> IMatrix::getChange(map<Transition*,int>& fv) {
	map<Place*,int> result; // this will be the resulting map, it may contain zero entries
	map<Transition*,int>::iterator it;
	for(it=fv.begin(); it!=fv.end(); ++it) // go through all transitions to fire
	{
		map<Place*,int> col(getColumn(*(it->first))); // use the change
		map<Place*,int>::iterator pit;
		for(pit=col.begin(); pit!=col.end(); ++pit) // go through all places with changed token numbers 
			result[pit->first]+=(pit->second)*(it->second); // and multiply it with the weight of the transition
	}
	return result;
}

/** Get the maximal size of presets of places.
	@return The maximal size.
*/
unsigned int IMatrix::pMaxPreset() { return pmaxpreset; }

/* Get the maximal size of postsets of places.
	@return The maximal size.
*/
//unsigned int IMatrix::pMaxPostset() { return pmaxpostset; }

/* Get the maximal size of presets of transitions.
	@return The maximal size.
*/
//unsigned int IMatrix::tMaxPreset() { return tmaxpreset; }

/* Get the maximal size of postsets of transitions.
	@return The maximal size.
*/
//unsigned int IMatrix::tMaxPostset() { return tmaxpostset; }

/* Get the maximal number of conflicting transitions for a single transition.
	@return An overaproximation of the maximal number of conflicting transitions.
*/
//unsigned int IMatrix::maxConflict() { return maxconflict; }

/** Get the number of places in the Petri net.
	@return The number of places.
*/
unsigned int IMatrix::numPlaces() { return numplaces; }

/** Get the number of transitions in the Petri net.
	@return The number of places.
*/
unsigned int IMatrix::numTransitions() { return numtransitions; }

} // end namespace sara
