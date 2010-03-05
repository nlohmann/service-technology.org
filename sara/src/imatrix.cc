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
 * \date    $Date: 2009-10-16 12:00:00 +0200 (Fr, 16. Okt 2009) $
 *
 * \version $Revision: -1 $
 */

#include <set>
#include <map>
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "imatrix.h"

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

/** Constructor
	@param pn The Petri net for which the incidence matrix is to be built.
*/
IMatrix::IMatrix(PetriNet& pn) : verbose(0),petrinet(pn) { mat.clear(); loop.clear(); }

/** Calculates or just gets one column of the incidence matrix.
	@param t The transition identifying the column.
	@return A map from places to int representing the row belonging to t. Only places
			with an arc from or to t will be part of the map.
*/
map<Place*,int>& IMatrix::getColumn(Transition& t) {
	map<Transition*,map<Place*,int> >::iterator matit(mat.find(&t));
	if (matit!=mat.end()) return matit->second; // if we calculated this earlier, just return it
	set<Arc*>::iterator ait;
	set<Arc*> arcs = t.getPostsetArcs();
	for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		mat[&t][&((*ait)->getPlace())] = (*ait)->getWeight(); // first get the postset
	arcs = t.getPresetArcs();
	for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
	{
		Place* p = &((*ait)->getPlace());
//		if (mat[&t].find(p)==mat[&t].end()) mat[&t][p]=0; // init
		int preweight = (*ait)->getWeight();
		int loops = (preweight>mat[&t][p] ? mat[&t][p] : preweight); // count the loops on a place
		if (loops>0) loop[&t][p] = loops; // and save this number
		mat[&t][p] -= preweight; // calculate the change the transition produces
	}
	if (verbose>1) {
		map<Place*,int>::iterator pit;
		cerr << "IMatrix Column " << t.getName() << " ";
		for(pit=mat[&t].begin(); pit!=mat[&t].end(); ++pit)
		{
			cerr << pit->first->getName() << ":" << pit->second;
			if (loop[&t].find(pit->first)!=loop[&t].end()) 
				cerr << ":" << loop[&t][pit->first];
			cerr << " ";
		}
		cerr << endl;
	}
	return mat[&t];
}

/** Get the postset of a transition as a map.
	@param t The transition.
	@return The map from places to arc weights.
*/
map<Place*,int> IMatrix::getPostset(Transition& t) {
	map<Place*,int>& col(getColumn(t)); // get the change of the transition
	map<Place*,int>& loop(getLoopColumn(t)); // and its loops
	map<Place*,int> result; // this will be the postset
	map<Place*,int>::iterator it;
//	for(it=loop.begin(); it!=loop.end(); ++it)
//		if (it->second>0) result[it->first] = 0; // init
	for(it=col.begin(); it!=col.end(); ++it)
		if (it->second>0) result[it->first] = it->second; // add the change
	for(it=loop.begin(); it!=loop.end(); ++it)
		if (it->second>0) result[it->first] += it->second; // and add the loops
	return result;
}

/** Get the preset of a transition as a map.
	@param t The transition.
	@return The map from places to arc weights.
*/
map<Place*,int> IMatrix::getPreset(Transition& t) {
	map<Place*,int>& col(getColumn(t)); // get the change of the transition
	map<Place*,int>& loop(getLoopColumn(t)); // and its loops
	map<Place*,int> result; // this will be the preset
	map<Place*,int>::iterator it;
//	for(it=loop.begin(); it!=loop.end(); ++it)
//		if (it->second>0) result[it->first] = 0; // init
	for(it=col.begin(); it!=col.end(); ++it)
		if (it->second<0) result[it->first] = -it->second; // subtract the change
	for(it=loop.begin(); it!=loop.end(); ++it)
		if (it->second>0) result[it->first] += it->second; // and add the loops
	return result;
}

/** Calculate or get one entry from the incidence matrix.
	@param t The transition/column of the incidence matrix.
	@param p The place/row of the incidence matrix.
	@return The number of producing arcs minus the number of consuming arcs.
*/ 
int IMatrix::getEntry(Transition& t, Place& p) {
	return getColumn(t)[&p]; // getColumn calculates the change the first time it is called.
}

/** Calculate or get the number of loops between a place and a transition.
	@param t The transition/column of the incidence matrix.
	@param p The place/row of the incidence matrix.
	@return The minimum of {arc weight from t to p, arc weight from p to t}.
*/
int IMatrix::getLoops(Transition&t, Place& p) {
	getColumn(t); // calculate change and loops if not already done
	if (loop[&t].find(&p)!=loop[&t].end()) return loop[&t][&p]; // check if there are loops so we won't change the map
	else return 0;
}

/** Calculate the loops at some transition.
	@param t The transition.
	@return A map from all place with loops at t to the (lesser) arc weight in the loop.
*/
map<Place*,int>& IMatrix::getLoopColumn(Transition&t) {
	getColumn(t); // calculate change and loops if not already done
	return loop[&t];
}

/** Calculate the marking before firing a transition.
	@param m The marking. The method changes this marking by reverse-firing t.
	@param t The transition whose effetcs should be reversed.
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

/** Calculate the successor of marking after firing a transition.
	@param m The marking. The method changes this marking by firing t.
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
	@param t1 Anchor transition.
	@param t2 The transition to compare t1 to.
	@return The set of places that is marked lower by t1 than by t2.
*/
set<Place*> IMatrix::compareOutput(Transition& t1, Transition& t2)
{
	// the resulting set of places, to be built
	set<Place*> result;
	result.clear();
	// get the incidence columns of t1 and t2
	map<Place*,int>& t1map(getColumn(t1));
	map<Place*,int>& t2map(getColumn(t2));
	map<Place*,int>::iterator it;
	// check all places connected with t1
	for(it=t1map.begin(); it!=t1map.end(); ++it)
	{
		Place* p = it->first;
		// if the place is connected to t1 and t2
		if (t2map.find(p)!=t2map.end())
		{
			// if t1 yields less tokens on p than t2
			if (it->second<t2map[p]) result.insert(p); 
		} else {
			// if t1 effectively consumes tokens from p (which has no connection to t2)
			if (it->second<0) result.insert(p);
		}		
	}
	// check those places connected with t2 that have no connection with t1
	for(it=t2map.begin(); it!=t2map.end(); ++it)
	{
		Place* p = it->first;
		// if there is no connection to t1
		if (t1map.find(p)==t1map.end())
		{
			// if t2 effectively produces tokens on p (and thus t1 yields less)
			if (it->second>0) result.insert(p); 
		} 
	}
	return result;
}

/** Checks whether a firing sequence is activated under a marking m
	disregarding all places not in a given restriction set.
	@param m The marking.
	@param tv The transition vector containing the firing sequence
	@param restriction The set of places which should be tested for undermarking.
		All other places are regarded as having an arbitrary high number of tokens available.
	@return Whether the firing sequence is firable under m regarding the restriction set.
*/
bool IMatrix::checkRestrictedActivation(Marking& m, vector<Transition*>& tv, set<Place*>& restriction)
{
	set<Place*>::iterator pit;
	// go through all places in the restrriction
	for(pit=restriction.begin(); pit!=restriction.end(); ++pit)
	{
		int tokens = m[**pit];
		for(unsigned int i=0; i<tv.size(); ++i)
		{
			int entry = getEntry(*(tv[i]),**pit);
			int loops = getLoops(*(tv[i]),**pit);
			// consume the loops
			tokens -= loops;
			// consume the "normal" consumption
			if (entry<0) tokens += entry; 
			// if the token number goes below zero after consumption, the sequence is not firable
			if (tokens<0) return false;
			// return the tokens consumed by loops.
			tokens += loops;
			// produce what the transition produces
			if (entry>0) tokens += entry;
		}
	}
	if (verbose>2) {
		cerr << "DIAMOND: " << tv.at(tv.size()-1)->getName() << " ";
		for(unsigned int i=1; i<tv.size()-1; ++i) cerr << tv[i]->getName() << " ";
		cerr << tv.at(0)->getName() << endl;
	}
	return true;	
}

/** Calculate the change in the marking a multiset of transitions will bring upon firing.
	@param fv The multiset of transitions.
	@return The token change.
*/
map<Place*,int> IMatrix::getChange(map<Transition*,int>& fv) {
	map<Place*,int> result; // this will be the resulting map, it may contain zero entries
	result.clear();
	map<Transition*,int>::iterator it;
	for(it=fv.begin(); it!=fv.end(); ++it) // go through all transitions to fire
	{
		map<Place*,int> col(getColumn(*(it->first))); // use the change
		map<Place*,int>::iterator pit;
		for(pit=col.begin(); pit!=col.end(); ++pit) // go through all places with changed token numbers 
		{
//			if (result.find(pit->first)!=result.end())
				result[pit->first]+=(pit->second)*(it->second); // and multiply it with the weight of the transition
//			else result[pit->first]=(pit->second)*(it->second);
		}
	}
	return result;
}

