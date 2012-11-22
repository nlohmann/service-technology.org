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
 * \date    $Date: 2012-11-20 12:00:00 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.12 $
 */

#include <sstream>
#include "pnapi/pnapi.h"
#include "imatrix.h"
#include "verbose.h"

using pnapi::Arc;
using std::cerr;
using std::endl;
using std::stringstream;

	/*****************************************************
	* Implementation of the methods of the class IMatrix *
	*****************************************************/

/** Constructor
	@param pn The Petri net for which the incidence matrix is to be built.
*/
IMatrix::IMatrix(PetriNet& pn) : verbose(0),petrinet(pn) { 
	// get all transitions and fill the ID<->transition conversion arrays
	set<Transition*> tset(petrinet.getTransitions());
	numtransitions = tset.size();
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		tid[*tit] = idt.size();
		idt.push_back(*tit);
		tname.push_back((*tit)->getName());
	}

	// get all places and fill the ID<->place conversion arrays
	set<Place*> pset(petrinet.getPlaces());
	numplaces = pset.size();
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		pid[*pit] = idp.size();
		idp.push_back(*pit);
		pname.push_back((*pit)->getName());
	}

	// reserve memory for the incidence-related matrices
	mat.resize(numtransitions);
	loop.resize(numtransitions);
	pre.resize(numtransitions);
	post.resize(numtransitions);
	ppre.resize(numplaces);
	ppost.resize(numplaces);

	// get the incidence matrix from the PNAPI
	precompute(); 

	// get the marking from the PNAPI
	pnapi::Marking m(petrinet,false);
	tokens.assign(numplaces,0);
	map<const Place*,unsigned int>::const_iterator mit;
	for(mit=m.begin(); mit!=m.end(); ++mit)
		if (mit->second>0) tokens[pID(const_cast<Place*>(mit->first))]=mit->second;
}

/** Copy-like Constructor. The net will be copied but the places given
	in the second parameter will be split so that the net itself can be
	split into as many components as it has transitions.
	@param im The original net.
	@param split The vector of places to split.
*/
IMatrix::IMatrix(const IMatrix& im, vector<PlaceID>& split) : petrinet(im.petrinet) {
	// copy the unchanged elements (mainly transition related)
	tid = im.tid;
	idt = im.idt;
	tname = im.tname;
	numtransitions = idt.size();
	tmaxpreset = im.tmaxpreset;
	tmaxpostset = im.tmaxpostset;

	// count the places
	numplaces = 0;

	// get the initial marking
	pnapi::Marking m(petrinet,false);

	PArcs::const_iterator preit,postit;
	
	// now work through the places of the old net
	unsigned int p;
	unsigned int sp(0);
	for(p=0; p<im.idp.size(); ++p)
	{
		// the place will continue to exist even if it is split, so make a copy
		Place* place(im.idp.at(p));
		pid[place] = numplaces; // the new id of the place
		idp.push_back(place);
		tokens.push_back(m[*place]);

		// if this place shouldn't be split, just copy the rest of the data from the old net
		if (sp>=split.size() || p<split.at(sp)) {
			++numplaces;
			ppre.push_back(im.ppre.at(p));
			ppost.push_back(im.ppost.at(p));
			pname.push_back(place->getName());
			continue;
		}
		++sp;

		// make pre and postset empty temporarily
		PArcs nix;
		ppre.push_back(nix);
		ppost.push_back(nix);
		// create a new unique name for the place
		stringstream sstr1;
		sstr1 << place->getName() << "#" << numplaces;
		string name1;
		sstr1 >> name1;
		pname.push_back(name1);

		// remember at which new id we start, so we can later close the loop
		unsigned int tmpplace(numplaces);

		// get pre and postset to obtain the adjacent transitions
		const PArcs& tmppre(im.ppre.at(p));
		const PArcs& tmppost(im.ppost.at(p));
		for(preit=tmppre.begin(),postit=tmppost.begin(); preit!=tmppre.end() || postit!=tmppost.end(); )
		{
			// walk through the transitions connected to p, select the next transition to connect to a new copy of p
			unsigned int preval(0);
			unsigned int postval(0);
			TransitionID t;
			if (postit==tmppost.end()) {
				t = preit->first;
				preval = preit->second;
				++preit;
			} else if (preit==tmppre.end() || preit->first>postit->first) {
				t = postit->first;
				postval = postit->second;
				++postit;
			} else {
				t = preit->first;
				preval = preit->second;
				++preit;
				if (t==postit->first) {
					postval = postit->second;
					++postit;
				}
			}

			// check if this is the last transition to do
			bool close(false);
			if (preit==tmppre.end() && postit==tmppost.end()) close=true;

			// create a new copy of p and connect it
			if (preval>0) {
				idt.push_back(NULL);
				stringstream sstr3;
				sstr3 << "tmp" << numtransitions;
				string name3;
				sstr3 >> name3;
				tname.push_back(name3);
				ppost.back()[numtransitions] = 1;
				if (close) {
					ppre.at(tmpplace)[numtransitions] = 1;
					ppre.at(tmpplace)[t] = preval;
				} else {
					idp.push_back(place);
					tokens.push_back(0);
					ppre.push_back(nix);
					ppost.push_back(nix);
					++numplaces;
					stringstream sstr2;
					sstr2 << place->getName() << "#" << numplaces;
					string name2;
					sstr2 >> name2;
					pname.push_back(name2);
					ppre.back()[numtransitions] = 1;
					ppre.back()[t] = preval;
				}
				++numtransitions;
			}
			if (postval>0) {
				idt.push_back(NULL);
				stringstream sstr3;
				sstr3 << "tmp" << numtransitions;
				string name3;
				sstr3 >> name3;
				tname.push_back(name3);
				ppost.back()[numtransitions] = 1;
				ppost.back()[t] = postval;
				if (close) {
					ppre.at(tmpplace)[numtransitions] = 1;
				} else {
					idp.push_back(place);
					tokens.push_back(0);
					ppre.push_back(nix);
					ppost.push_back(nix);
					++numplaces;
					stringstream sstr2;
					sstr2 << place->getName() << "#" << numplaces;
					string name2;
					sstr2 >> name2;
					pname.push_back(name2);
					ppre.back()[numtransitions] = 1;
				}
				++numtransitions;
			}
		}
		++numplaces;
	}

	// compute mat, loop, pre, post (incidence matrices with p.o.v.: transitions)
	mat.resize(numtransitions);
	loop.resize(numtransitions);
	pre.resize(numtransitions);
	post.resize(numtransitions);
	// go through the place matrices and copy the values
	for(p=0; p<numplaces; ++p)
	{
		PArcs& ppretmp(ppre.at(p));
		for(preit=ppretmp.begin(); preit!=ppretmp.end(); ++preit)
		{
			post.at(preit->first)[p] = preit->second;
			mat.at(preit->first)[p] = preit->second;
		}		
		PArcs& pposttmp(ppost.at(p));
		for(postit=pposttmp.begin(); postit!=pposttmp.end(); ++postit)
		{
			pre.at(postit->first)[p] = postit->second;
			if (postit->second>0 && mat.at(postit->first)[p]>0) {
				if (postit->second<=mat.at(postit->first)[p])
					loop.at(postit->first)[p] = postit->second;
				else loop.at(postit->first)[p] = mat.at(postit->first)[p];
			}
			mat.at(postit->first)[p] -= postit->second;
		}		
	}	

	// compute the maximal pre/postset of a transition
	TransitionID t;
	for(t=0; t<numtransitions; ++t)
	{
		if (tmaxpreset<pre.at(t).size()) tmaxpreset = pre.at(t).size();
		if (tmaxpostset<post.at(t).size()) tmaxpostset = post.at(t).size();
	}

	// do other stuff
	precompute2();
}

/** Called by constructor. Computes all the necessary internal relations
	and values.
*/
void IMatrix::precompute() {
	// for counting the maximal pre/postset size
	tmaxpreset = 0;
	tmaxpostset = 0;

	TransitionID id;
	// go through all transitions
	for(id=0; id<idt.size(); ++id)
	{
		// for the postset of this transition:
		set<Arc*>::iterator ait;
		set<Arc*> arcs = idt.at(id)->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// get the connected places with their arc weight
			Place* p = &((*ait)->getPlace());
			PlaceID id2(pid[p]);
			int postweight = (*ait)->getWeight();

			// calculate the change on p when firing the transition
			mat.at(id)[id2] = postweight; // first get the postset
			// and save the weight for the transition as well as the place
			post.at(id)[id2] = postweight;
			ppre.at(id2)[id] = postweight;
		}

		// now for the preset of the transition:
		arcs = idt.at(id)->getPresetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// get the connected places with their arc weight
			Place* p = &((*ait)->getPlace());
			PlaceID id2(pid[p]);
			int preweight = (*ait)->getWeight();

			// calculate the number of direct loops between p and the transition
			int loops = (preweight>mat.at(id)[id2] ? mat.at(id)[id2] : preweight); // count the loops on a place
			if (loops>0) loop.at(id)[id2] = loops; // and save this number
			// continue calculating the change of firing the transition on p			
			mat.at(id)[id2] -= preweight; // calculate the change the transition produces
			// and save the weight for the transition as well as the place
			pre.at(id)[id2] = preweight;
			ppost.at(id2)[id] = preweight;
		}

		// adapt the maximal pre/postset size
		unsigned int tmp(idt.at(id)->getPreset().size());
		if (tmp>tmaxpreset) tmaxpreset=tmp;
		tmp = (idt.at(id)->getPostset().size());
		if (tmp>tmaxpostset) tmaxpostset=tmp;
	}

	// do other stuff
	precompute2();
}

void IMatrix::precompute2() {
	PlaceID id;
	
	// run through the places to find the maximal pre/postsets
	pmaxpreset = 0;
	pmaxpostset = 0;
	for(id=0; id<idp.size(); ++id)
	{
		unsigned int tmp(idp.at(id)->getPreset().size());
		if (tmp>pmaxpreset) pmaxpreset=tmp;
		tmp = (idp.at(id)->getPostset().size());
		if (tmp>pmaxpostset) pmaxpostset=tmp;
	}
	maxconflict=tmaxpreset*pmaxpostset;
	if (maxconflict>numtransitions) maxconflict=numtransitions;

	// compute the siphon and trap implications (like "if p is in the siphon also q or r or s must be in the siphon ...")
	for(id=0; id<idp.size(); ++id)
	{
		vector<set<PlaceID> > vs;
		PArcs::iterator ait;
		for(ait=ppre.at(id).begin(); ait!=ppre.at(id).end(); ++ait)
		{
			set<PlaceID> s;
			TArcs::iterator ait2;
			for(ait2=pre.at(ait->first).begin(); ait2!=pre.at(ait->first).end(); ++ait2)
				s.insert(ait2->first);
			vs.push_back(s);
		}
		simpl.push_back(vs);
		vs.clear();
		for(ait=ppost.at(id).begin(); ait!=ppost.at(id).end(); ++ait)
		{
			set<PlaceID> s;
			TArcs::iterator ait2;
			for(ait2=post.at(ait->first).begin(); ait2!=post.at(ait->first).end(); ++ait2)
				s.insert(ait2->first);
			vs.push_back(s);
		}
		timpl.push_back(vs);
	}
}

/** Get the underlying Petri net.
	@return The Petri net,
*/
const PetriNet& IMatrix::getNet() const { return petrinet; }

/** Get the ID of a transition.
	@param t The transition.
	@return The transition's ID.
*/
TransitionID IMatrix::tID(const Transition* t) const { 
	const map<const Transition*,TransitionID>::const_iterator tit(tid.find(t));
	if (tit!=tid.end()) return tit->second;
	abort(81,"no TransitionID for transition pointer");
	return 0;
}

/** Get the ID of a place.
	@param p The place.
	@return The place' ID.
*/
PlaceID IMatrix::pID(const Place* p) const {
	const map<const Place*,PlaceID>::const_iterator pit(pid.find(p));
	if (pit!=pid.end()) return pit->second;
	abort(82,"no PlaceID for place pointer");
	return 0;
}

/** Get the transition belonging to an ID.
	@param t The transition's ID.
	@return The transition.
*/
Transition* IMatrix::tPTR(TransitionID t) const { 
	if (t<numtransitions) return idt.at(t); 
	return NULL;
}

/** Get the place belonging to an ID.
	@param p The place' ID.
	@return The place.
*/
Place* IMatrix::pPTR(PlaceID p) const { 
	if (p<numplaces) return idp.at(p); 
	return NULL;
}

/** Get the transition name belonging to an ID.
	@param t The transition's ID.
	@return The transition's name.
*/
string IMatrix::tName(TransitionID t) const { 
	if (t<numtransitions) return tname.at(t); 
	return "";
}

/** Get the place name belonging to an ID.
	@param p The place' ID.
	@return The place' name.
*/
string IMatrix::pName(PlaceID p) const { 
	if (p<numplaces) return pname.at(p); 
	return "";
}

/** Get one column of the incidence matrix.
	@param t The transition identifying the column.
	@return A map from places to int representing the column belonging to t. Only places
			with an arc from or to t will be part of the map. A zero entry shows a place
			with the same positive weight for both arc directions.
*/
const TArcs& IMatrix::getColumn(TransitionID t) const { return mat.at(t); }

/** Get the postset of a transition as a map. Places not contained in the postset will not appear in the map.
	@param t The transition.
	@return The map from places to arc weights.
*/
const TArcs& IMatrix::getTPostset(TransitionID t) const { return post.at(t); }

/** Get the postset of a place as a map. Transitions not contained in the postset will not appear in the map.
	@param p The place.
	@return The map from transitions to arc weights.
*/
const PArcs& IMatrix::getPPostset(PlaceID p) const { return ppost.at(p); }

/** Get the preset of a transition as a map. Places not in the preset will not appear in the map.
	@param t The transition.
	@return The map from places to arc weights.
*/
const TArcs& IMatrix::getTPreset(TransitionID t) const { return pre.at(t); }

/** Get the preset of a place as a map. Transitions not contained in the preset will not appear in the map.
	@param p The place.
	@return The map from transitions to arc weights.
*/
const PArcs& IMatrix::getPPreset(PlaceID p) const { return ppre.at(p); }

/** Get one entry from the incidence matrix.
	@param t The transition/column of the incidence matrix.
	@param p The place/row of the incidence matrix.
	@return The number of producing arcs minus the number of consuming arcs.
*/ 
int IMatrix::getEntry(TransitionID t, PlaceID p) const { 
	TArcs::const_iterator pit(mat.at(t).find(p));
	return (pit!=mat.at(t).end() ? pit->second : 0); // check if there is an arc so we won't change the map
}

/** Get the number of loops between a place and a transition.
	@param t The transition/column of the incidence matrix.
	@param p The place/row of the incidence matrix.
	@return The minimum of {arc weight from t to p, arc weight from p to t}.
*/
int IMatrix::getLoops(TransitionID t, PlaceID p) const {
	TArcs::const_iterator pit(loop.at(t).find(p));
	return (pit!=loop.at(t).end() ? pit->second : 0); // check if there are loops so we won't change the map
}

/** Get the number of arcs from a transition to a place.
	@param t The transition/column of the incidence matrix.
	@param p The place/row of the incidence matrix.
	@return The number of arcs from t to p.
*/ 
int IMatrix::getTPWeight(TransitionID t, PlaceID p) const {
	TArcs::const_iterator pit(post.at(t).find(p));
	return (pit!=post.at(t).end() ? pit->second : 0); // check if there is a place so we won't change the map
}

/** Get the number of arcs from a place to a transition.
	@param p The place/row of the incidence matrix.
	@param t The transition/column of the incidence matrix.
	@return The number of arcs from p to t.
*/ 
int IMatrix::getPTWeight(PlaceID p, TransitionID t) const {
	TArcs::const_iterator pit(pre.at(t).find(p));
	return (pit!=pre.at(t).end() ? pit->second : 0); // check if there is a place so we won't change the map
}

/** Calculate the loops at some transition.
	@param t The transition.
	@return A map from all places with loops at t to the (lesser) arc weight in the loop.
*/
const TArcs& IMatrix::getLoopColumn(TransitionID t) const { return loop.at(t); }

/** Calculate the marking previous to firing a transition.
	@param m The marking. The method changes this marking by reverse-firing t. No checks
		are done if this reverse firing is possible, i.e. the resulting marking may be invalid.
	@param t The transition for which the effects should be reversed.
*/
void IMatrix::predecessor(map<PlaceID,int>& m, TransitionID t) const
{
	const TArcs& pmap(getColumn(t)); // get the change
	TArcs::const_iterator pit;
	for(pit=pmap.begin(); pit!=pmap.end(); ++pit)
		m[pit->first] -= pit->second; // and subtract it from the marking
	// note: there may be zero entries in a marking afterwards. Comparison of two
	// markings has to reflect this possibility.
}

/** Calculate the successor of a marking after firing a transition.
	@param m The marking. The method changes this marking by firing t, even if t is not enabled.
		In this case, the marking may become invalid.
	@param t The transition to be fired.
*/
void IMatrix::successor(map<PlaceID,int>& m, TransitionID t) const
{
	const TArcs& pmap(getColumn(t)); // get the change
	TArcs::const_iterator pit;
	for(pit=pmap.begin(); pit!=pmap.end(); ++pit)
		m[pit->first] += pit->second; // and add it to the marking
	// note: there may be zero entries in a marking afterwards. Comparison of two
	// markings has to reflect this possibility.
}

/** Compares the effect of two Transitions t1 and t2; if fired under
	the same marking the result on some place p is lower for t1
	than for t2, the place p is in the resulting output set.
	@param t1 Anchor transition.
	@param t2 The transition to compare t1 to.
	@return The set of places that are marked lower by t1 than by t2.
*/
vector<PlaceID> IMatrix::compareOutput(TransitionID t1, TransitionID t2) const
{
	// the resulting set of places, to be built
	vector<PlaceID> result;

	// get the incidence columns of t1 and t2
	const TArcs& t1map(getColumn(t1));
	const TArcs& t2map(getColumn(t2));
	TArcs::const_iterator it;
	result.reserve(t1map.size()+t2map.size());

	// check all places connected with t1
	for(it=t1map.begin(); it!=t1map.end(); ++it)
	{
		TArcs::const_iterator t2it(t2map.find(it->first));
		// if the place is connected to t1 and t2
		if (t2it!=t2map.end())
		{
			// if t1 yields less tokens on the place than t2
			if (it->second<t2it->second) result.push_back(it->first); 
		} else {
			// if t1 effectively consumes tokens from the place (which has no connection to t2)
			if (it->second<0) result.push_back(it->first);
		}		
	}
	// check those places connected with t2 that have no connection with t1
	for(it=t2map.begin(); it!=t2map.end(); ++it)
	{
		// if there is no connection to t1
		if (t1map.find(it->first)==t1map.end())
		{
			// if t2 effectively produces tokens on the place (and thus t1 yields less)
			if (it->second>0) result.push_back(it->first); 
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
bool IMatrix::checkRestrictedActivation(const Marking& m, const FiringSequence& tv, const vector<PlaceID>& restriction) const
{
	// go through all places in the restriction
	for(unsigned int i=0; i<restriction.size(); ++i)
	{
		PlaceID p(restriction.at(i));
		int tokens(0);
		if (m.find(p)!=m.end()) tokens = m.find(p)->second;
		for(int j=tv.size()-1; j>=0; --j)
		{
			tokens -= getTPWeight(tv.at(j),p);
			if (tokens<0) return false;
			tokens += getPTWeight(p,tv.at(j));
		}
	}
	return true;	
}

/** Calculate the change in the marking a multiset of transitions will bring upon firing.
	No check for realizability of the multiset is performed.
	@param fv The multiset of transitions.
	@return The token change.
*/
Marking IMatrix::getChange(const FiringVector& fv) const {
	Marking result; // this will be the resulting map, it may contain zero entries
	FiringVector::const_iterator it;
	for(it=fv.begin(); it!=fv.end(); ++it) // go through all transitions to fire
	{
		const TArcs& col(getColumn(it->first)); // use the change
		TArcs::const_iterator pit;
		for(pit=col.begin(); pit!=col.end(); ++pit) // go through all places with changed token numbers 
			result[pit->first]+=(pit->second)*(it->second); // and multiply it with the weight of the transition
	}
	return result;
}

/** Get the maximal size of presets of places.
	@return The maximal size.
*/
unsigned int IMatrix::pMaxPreset() const { return pmaxpreset; }

/** Get the maximal size of postsets of places.
	@return The maximal size.
*/
unsigned int IMatrix::pMaxPostset() const { return pmaxpostset; }

/** Get the maximal size of presets of transitions.
	@return The maximal size.
*/
unsigned int IMatrix::tMaxPreset() const { return tmaxpreset; }

/** Get the maximal size of postsets of transitions.
	@return The maximal size.
*/
unsigned int IMatrix::tMaxPostset() const { return tmaxpostset; }

/** Get the maximal number of conflicting transitions for a single transition.
	@return An overaproximation of the maximal number of conflicting transitions.
*/
unsigned int IMatrix::maxConflict() const { return maxconflict; }

/** Get the number of places in the Petri net.
	@return The number of places.
*/
unsigned int IMatrix::numPlaces() const { return numplaces; }

/** Get the number of transitions in the Petri net.
	@return The number of places.
*/
unsigned int IMatrix::numTransitions() const { return numtransitions; }

/** Get the implications for a place being contained in a siphon.
	@param p The ID of the place.
	@return A conjunction of choices. Select one place ID from each set in the vector.
*/
const vector<set<PlaceID> >& IMatrix::getSiphonImplication(PlaceID p) const {
	return simpl.at(p);
}

/** Get the implications for a place being contained in a trap.
	@param p The ID of the place.
	@return A conjunction of choices. Select one place ID from each set in the vector.
*/
const vector<set<PlaceID> >& IMatrix::getTrapImplication(PlaceID p) const {
	return timpl.at(p);
}

/** Get all places that need to be split if a place must not have more than a given number of neighbor transitions.
	@param size The maximal number of neighbor transitions for any given place.
	@return A vector of place that must be split.
*/
vector<PlaceID> IMatrix::placesToSplit(unsigned int size) const {
	vector<PlaceID> result;
	if (size==0) return result;
	for(PlaceID p=0; p<numplaces; ++p)
	{
		unsigned int presize(ppre.at(p).size());
		unsigned int postsize(ppost.at(p).size());
		if (presize>1 || postsize>1)
			if (presize+postsize>size)
				result.push_back(p);
	}
	return result;
}

/** Get all transitions that belong to the same component as the given one.
	@param t A transition inside the selected component.
	@return A vector of all transitions in the same component.
*/
vector<TransitionID> IMatrix::componentTransitions(TransitionID t) const {
	vector<TransitionID> result;
	set<TransitionID> tset;
	if (t>=numTransitions() || tPTR(t)==NULL) return result;
	result.push_back(t);
	tset.insert(t);
	unsigned int pos(0);

	while (pos<result.size())
	{
		TransitionID act(result[pos++]);
//		tset.insert(act);

		TArcs::const_iterator pit;
		PArcs::const_iterator tit;
		for(pit=pre.at(act).begin(); pit!=pre.at(act).end(); ++pit)
			for(tit=ppost.at(pit->first).begin(); tit!=ppost.at(pit->first).end(); ++tit)
				if (tset.find(tit->first)==tset.end()) { tset.insert(tit->first); result.push_back(tit->first); }
		for(pit=post.at(act).begin(); pit!=post.at(act).end(); ++pit)
			for(tit=ppre.at(pit->first).begin(); tit!=ppre.at(pit->first).end(); ++tit)
				if (tset.find(tit->first)==tset.end()) { tset.insert(tit->first); result.push_back(tit->first); }
	}
	return result;
}

/** Get the initial marking on a given place.
	@param p The ID of the place.
	@return The number of tokens initially on this place.
*/
unsigned int IMatrix::marking(PlaceID p) const { return tokens.at(p); }

/** Get all places that have tokens initially.
	@return A vector of all places (IDs) initially marked.
*/
vector<PlaceID> IMatrix::getMarked() const {
	vector<PlaceID> res;
	for(PlaceID p=0; p<numplaces; ++p)
		if (tokens.at(p)>0) res.push_back(p);
	return res;
}

