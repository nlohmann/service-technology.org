// -*- C++ -*-

/*!
 * \file    partialsolution.cc
 *
 * \brief   Class PartialSolution
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/12
 *
 * \date    $Date: 2010-03-01 12:00:00 +0200 (Mo, 1. Mar 2010) $
 *
 * \version $Revision: -1 $
 */

#include <vector>
#include <map>
#include <string>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "partialsolution.h"
#include "tarjan.h"

using pnapi::Marking;
using pnapi::Transition;
using pnapi::Place;
//using pnapi::Arc;
using pnapi::Node;
using std::vector;
using std::deque;
using std::map;
using std::string;
using std::cerr;
using std::endl;

extern vector<Transition*> transitionorder;
extern map<Transition*,int> revtorder;

	/*************************************************************
	* Implementation of the methods of the class PartialSolution *
	*************************************************************/

/** Constructor for the first partial solution (no output from lp_solve yet).
	@param m1 The initial marking.
*/ 
PartialSolution::PartialSolution(Marking& m1) : m(m1),markingEquation(false) {
	fullSolution = false;
} 

/** Constructor for later partial solutions.
	@param ts The partial firing sequence.
	@param m1 The marking reached after the partial firing sequence.
	@param rvec The remaining (nonfirable) transitions. 
*/
PartialSolution::PartialSolution(vector<Transition*>& ts,Marking& m1,map<Transition*,int>& rvec) : tseq(ts),m(m1),remains(rvec),markingEquation(false) {
	fullSolution = false;
}

/** Destructor.
*/
PartialSolution::~PartialSolution() {} 

/** Get method for the marking.
	@return The marking reached after the partial firing sequence.
*/
Marking& PartialSolution::getMarking() { return m; }

/** Set the marking reached after firing the partial firing sequence.
	@param m1 The Marking.
*/
void PartialSolution::setMarking(Marking& m1) { m=m1; }

/** Get method for the partial firing sequence.
	@return The partial firing sequence.
*/
vector<Transition*>& PartialSolution::getSequence() { return tseq; }

/** Set method for the partial firing sequence.
	@param The partial firing sequence.
*/
void PartialSolution::setSequence(vector<Transition*> seq) { tseq=seq; }

/** Get method for the remainder of non-firable transitions.
	@return The remainder.
*/
map<Transition*,int>& PartialSolution::getRemains() { return remains; }

/** Set the vector of the full solution, i.e. firing sequence plus remainder.
	@param ftv The solution as calculated by lp_solve.
*/
void PartialSolution::setFullVector(map<Transition*,int>& ftv) { fulltv=ftv; }

/** Get the vector of the full solution, i.e. firing sequence plus remainder.
	@return The solution as calculated by lp_solve.
*/
map<Transition*,int>& PartialSolution::getFullVector() { return fulltv; }

/** Sets a single constraint.
	@param c A constraint.
	@return False, if the constraint was already in the set of constraints of the partial solution.
		True, if the constraint is new or the right hand side has changed.
*/
bool PartialSolution::setConstraint(const Constraint& c) {
	set<Constraint>::iterator cit;
	for(cit=constraints.begin(); cit!=constraints.end(); ++cit)
		if ((*cit).compare(c)!=0) break;
	if (cit!=constraints.end())
	{
		if (!c.isJump())
		{
			if (cit->getRHS()>=c.getRHS()) return false;
			const_cast<Constraint*>(&(*cit))->setRHS(c.getRHS());
			const_cast<Constraint*>(&(*cit))->setRecent(true);
		} else {
			if (cit->getRHS()<=c.getRHS()) return false;
			const_cast<Constraint*>(&(*cit))->setRHS(c.getRHS());
			const_cast<Constraint*>(&(*cit))->setRecent(true);
		}
	}
	else constraints.insert(c);
	return true;
}

/** Sets a single failure constraint.
	@param c A constraint.
	@return False, if the constraint was already in the set of failure constraints of the partial solution.
		True, if the constraint is new or the right hand side has changed.
*/
bool PartialSolution::setFailureConstraint(const Constraint& c) {
	failure.insert(c);
	return true;
}

/* Checks if there is a constraint in the partial solution with at most a differing right hand side.
	@param c The constraint to be checked.
	@return The right hand side of the constraint in the partial solution if a match was found,
		-1 otherwise.
int PartialSolution::findRHS(Constraint& c) {
	if (constraints.size()==0) return -1;
	set<Constraint>::iterator cit;
	for(cit=constraints.begin(); cit!=constraints.end(); ++cit)
		if (cit->compare(c)!=0) break;
	if (cit!=constraints.end()) return cit->getRHS();
	else return -1;
}
*/

/* Removes all constraints from the partial solution.
void PartialSolution::clearConstraints() { 
	constraints.clear(); 
}
*/

/* Removes all failure constraints from the partial solution.
void PartialSolution::clearFailureConstraints() { 
	failure.clear(); 
}
*/

/** Sets all constraints of the partial solution at once, existing constraints are deleted.
	@param cv The new constraints.
*/
void PartialSolution::setConstraints(set<Constraint> cv) { constraints = cv; }

/** Gets all constraints in the partial solution.
	@return The constraints.
*/
set<Constraint>& PartialSolution::getConstraints() { return constraints; }

/** Gets all failed constraints in the partial solution.
	@return The constraints.
*/
set<Constraint>& PartialSolution::getFailureConstraints() { return failure; }

/** Sets the flag for feasibility of the marking equation. The flag is not set automatically!
	@param mef True, if the marking equation is feasible. 
*/
void PartialSolution::setFeasibility(bool mef) { markingEquation=mef; }

/** Check if the flag for feasibility of the marking equation is set.
	@return The flag.
*/
bool PartialSolution::isFeasible() { return markingEquation; }

/** Prints the partial solution on cerr for debugging purposes.
*/
void PartialSolution::show() {
	cerr << "SEQ:  ";
	for(unsigned int j=0; j<getSequence().size(); j++)
		cerr << getSequence().at(j)->getName() << " ";
	cerr << endl;
	map<const Place*,unsigned int>::const_iterator mit;
	cerr << "Marking: ";
	for(mit=getMarking().begin(); mit!=getMarking().end(); ++mit)
	{
		if (mit->second>0) cerr << mit->first->getName() << ":" << mit->second << " "; 
	}
	cerr << endl;
	cerr << "Remainder: ";
	map<Transition*,int>::iterator vit;
	for(vit=getRemains().begin(); vit!=getRemains().end(); vit++)
		if (vit->second>0)
			cerr << vit->first->getName() << ":" << vit->second << " ";
	cerr << endl;
	cerr << "Constraints: ";
	set<Constraint>::iterator cit;
	for(cit=constraints.begin(); cit!=constraints.end(); ++cit)
		cit->showConstraint(cerr);
	cerr << endl;
}

/* For those places where we have less tokens at the moment than in the final marking,
	calculate the amount missing.
	@param im Incidence matrix.
	@return A map from those places to the number of missing tokens.
map<Place*,int> PartialSolution::underFinalMarking(IMatrix& im) {
	map<Place*,int> change(im.getChange(getRemains()));
	map<Place*,int> result;
	result.clear();
	map<Place*,int>::iterator pit;
	for(pit=change.begin(); pit!=change.end(); ++pit)
		if (pit->second>0) result[pit->first] = pit->second;
	return result;
}
*/

/* For each place for all transitions from the partial firing sequence that effectively produce tokens on this
	place, the sum of the effects of the transitions is calculated.
	@return A map from places to positive token effects.
map<Place*,int> PartialSolution::produce() {
	map<Place*,int> prod;
	for(unsigned int i=0; i<tseq.size(); ++i)
	{
		set<pnapi::Arc*> as(tseq[i]->getPostsetArcs());
		set<pnapi::Arc*>::iterator ait;
		for(ait=as.begin(); ait!=as.end(); ++ait)
		{
			Place& p = (*ait)->getPlace();
			pnapi::Arc* a = m.getPetriNet().findArc(p,*(tseq[i]));
			int aw = (*ait)->getWeight();
			if (a) aw -= a->getWeight();
			if (aw<0) aw=0;
			prod[&p] += aw;
		}
	}
	return prod;
}
*/

/** Compares the partial solution to a given sequence, marking, and remainder.
	@param tv A transition vector to compare with the partial firing sequence.
	@param m0 An initial marking.
	@param rem A remainder of non-firable transitions.
	@return If the partial solution is "better" than the parameters.
*/
bool PartialSolution::betterSequenceThan(vector<Transition*> tv, Marking& m0, map<Transition*,int>& rem) {
	// the new firing sequence should be longer than the old one
	if (tseq.size()>tv.size()) return true;
	// it should not reach the same marking and have the same nonfirable transitions as the old one
	if (rem!=remains) return false;
	if (m==m0) return true;
	return false;
}

/*
void PartialSolution::calcCircleConstraints(IMatrix& im, Marking& m0) {
	set<Transition*> tset = m.getPetriNet().getTransitions();
	set<Place*> pset;
	Tarjan tj(tset,pset);
	map<Transition*,int>::iterator rit;
	for(rit=getRemains().begin(); rit!=getRemains().end(); ++rit)
		if (rit->second>0) {
			map<Place*,int>& postset(im.getColumn(*(rit->first)));
			map<Place*,int>::iterator pit;
			for(pit=postset.begin(); pit!=postset.end(); ++pit)
			{
				set<Arc*> arcs(pit->first->getPostsetArcs());
				set<Arc*>::iterator ait;
				for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
				{
					Transition* t(&(*ait)->getTransition());
					if (getRemains().find(t)!=getRemains().end() && getRemains()[t]>0) 
						tj.addArc(rit->first,t);
				}
			}
		}
	vector<set<Transition*> > tcomp;
	vector<set<Place*> > pcomp;
	tj.getComponents(tcomp,pcomp);
	for(int i=0; i<tcomp.size(); ++i)
	{
//		vector<set<Transition*> > tv;
//		vector<set<Place*> > pv;
//		cerr << "MX TCOMP: ";
//		set<Transition*>::iterator xtit;
//		for(xtit=tcomp[i].begin(); xtit!=tcomp[i].end(); ++xtit)
//			cerr << (*xtit)->getName() << " ";
//		cerr << " P: ";
//
		set<Place*> ptcomp(getPlacesForZSK(tcomp[i],im));
//		set<Place*>::iterator xpit;
//		for(xpit=ptcomp.begin(); xpit!=ptcomp.end(); ++xpit)
//			cerr << (*xpit)->getName() << " ";
//		cerr << endl;
//
		ttmp.push_back(tcomp[i]);
		ptmp.push_back(ptcomp);
		ftmp.push_back(tcomp[i]);
	}
	for(int i=0; i<ttmp.size(); ++i)
	{
		// if there are undermarked places markable only by the component, it has to be made firable
		set<Place*> deadp(findPlacesDead(ttmp[i],im));
		if (!deadp.empty()) ptmp[i] = findPlacesIn(ttmp[i],im);

//		cerr << "MX TTMP/PTMP: ";
//		set<Transition*>::iterator xtit;
//		for(xtit=ttmp[i].begin(); xtit!=ttmp[i].end(); ++xtit)
//			cerr << (*xtit)->getName() << " ";
//		set<Place*>::iterator xpit;
//		for(xpit=ptmp[i].begin(); xpit!=ptmp[i].end(); ++xpit)
//			cerr << (*xpit)->getName() << " ";
//		cerr << endl;
//
		int um = checkUndermarking(m0,ttmp[i],ptmp[i],deadp,im);
//		cerr << "UM: " << um << endl;
		if (um<=0) continue;
//		if (um>1) um=1; // increment should be limited to one, but this doesn't work!

		map<Place*,int> posmap,loopmap;
		set<Transition*>::iterator sit;
		for(sit=ttmp[i].begin(); sit!=ttmp[i].end(); ++sit)
		{
			map<Place*,int> post = im.getColumn(**sit);
			map<Place*,int> loops = im.getLoopColumn(**sit);
			int cnt = count(tseq.begin(),tseq.end(),*sit)+getRemains()[*sit];
			map<Place*,int>::iterator mit;
			for(mit=post.begin(); mit!=post.end(); ++mit)
				if (ptmp[i].find(mit->first)!=ptmp[i].end()) 
					if (post[mit->first]>0) {
						if (posmap.find(mit->first)!=posmap.end()) 
							posmap[mit->first] += mit->second * cnt;
						else posmap[mit->first] = mit->second * cnt; // init
					}
			for(mit=loops.begin(); mit!=loops.end(); ++mit)
				if (ptmp[i].find(mit->first)!=ptmp[i].end()) {
					if (loopmap.find(mit->first)==loopmap.end()) loopmap[mit->first]=0; //init
					if (loops[mit->first]>0 && loopmap[mit->first]<loops[mit->first]+post[mit->first])
						loopmap[mit->first]=loops[mit->first]+post[mit->first];
				}
		}		
		map<Place*,int>::iterator mit;
		for(mit=loopmap.begin(); mit!=loopmap.end(); ++mit)
		{
			if (posmap.find(mit->first)==posmap.end()) posmap[mit->first]=0; // init
			if (mit->second>0 && posmap[mit->first]<mit->second) posmap[mit->first]=mit->second;
		}
		if (!buildMultiConstraint(posmap,um,ftmp[i])) ; //cerr << "TJ: ERROR" << endl;
	}
}
*/

/** Constructs a new constraint from a given weighted set of places and forbidden transitions and
	adds it to the partial solution.
	@param pmap The places (weighted) that need additional tokens.
	@param incr The number of additional tokens needed.
	@param forbidden A set of transitions that is not allowed to deliver these tokens.
	@param im The incidence matrix of the Petri net.
	@return Whether the constraint is non-trivial and has thus been added.
*/
bool PartialSolution::buildMultiConstraint(map<Place*,int>& pmap, int incr, set<Transition*>& forbidden) {
	Constraint c;
	map<Place*,int>::iterator pit;
	for(pit=pmap.begin(); pit!=pmap.end(); ++pit)
		c.addPlace(*(pit->first),pit->second);
	set<Transition*>::iterator tit;
	for(tit=forbidden.begin(); tit!=forbidden.end(); ++tit)
		if (c.checkSubTransition(**tit)) c.addSubTransition(**tit);
	map<Transition*,int>& cs(c.calcConstraint());
	int allprod = 0;
	for(unsigned int i=0; i<tseq.size(); ++i)
		if (cs.find(tseq[i])!=cs.end()) 
			allprod += cs[tseq[i]];
	int check = 0;
	map<Transition*,int>::iterator rit;
	for(rit=cs.begin(); rit!=cs.end(); ++rit)
		if (getRemains().find(rit->first)!=getRemains().end() && getRemains()[rit->first]>0)
		{
			check += getRemains()[rit->first] * rit->second;
			incr -= getRemains()[rit->first]*rit->second;
			if (incr<0) incr=0; 
		}
//	cerr << allprod << " " << incr << " " << check << endl;
	if (c.getSubTransitions().empty() || incr<2) c.setRHS(allprod+incr+check);
	else c.setRHS(allprod+1+check);
//	cerr<< "BMC: "; c.showConstraint(cerr); cerr<<endl;
	// add a constraint, but only if it is non-empty; return success
	c.calcConstraint();
	c.setRecent(true);
	if (c.checkAnyTransition()) return setConstraint(c);
	else setFailureConstraint(c);
	return false;
}

/** Create constraints for places that hinder transitions by missing tokens. The new constraint
	forces an additional token on each such place (instead of one token over all such places like the
	normal constraint would do). This will increase the speed of the algorithm significantly.
	@param im The incidence matrix of the Petri net.
*/
void PartialSolution::buildSimpleConstraints(IMatrix& im) {
	// get the set of forbidden transitions and the undermarked places for these transitions
	set<Transition*> forbidden; // transitions that should have but couldn't fire
	set<Place*> undermarked; // if a place hinders some forbidden transition(s) from firing
	map<Place*,int> undermarking; // the minimal amount needed for activation of some transition(s)
	// now go through all unfirable transitions and their presets
	map<Transition*,int>::iterator tit;
	for(tit=remains.begin(); tit!=remains.end(); ++tit)
	if (tit->second>0) // should have but couldn't fire
	{
		forbidden.insert(tit->first); // all transitions that couldn't fire are forbidden
		map<Place*,int> pre(im.getPreset(*(tit->first))); // get the preset
		map<Place*,int>::iterator pit;
		for(pit=pre.begin(); pit!=pre.end(); ++pit) // go through the preset
		{
			int cons = pit->second;
			if (cons>(int)(m[*(pit->first)])) { // additional tokens are needed
				undermarked.insert(pit->first); // place hasn't got enough tokens, so tag it
				if (undermarking[pit->first]==0 || undermarking[pit->first]>cons-(int)(m[*(pit->first)]))
					undermarking[pit->first]=cons-m[*(pit->first)]; // minimum amount needed to activate some transition
			}
		}
	} 
	// then create a graph for Tarjan's algorithm
	Tarjan tj(forbidden,undermarked);
	// remember the connections between nodes also externally for easier reference later
	map<Place*,set<Transition*> > prepmap; // forbidden pre-transitions that (would) increase number of tokens
	map<Transition*,set<Place*> > pretmap; // undermarked pre-places of a forbidden transition
	// add an arc from each transition to its undermarked place(s)
	set<Transition*>::iterator fit;
	for(fit=forbidden.begin(); fit!=forbidden.end(); ++fit)
	{
		map<Place*,int> pre(im.getPreset(**fit));
		map<Place*,int>::iterator pit;
		for(pit=pre.begin(); pit!=pre.end(); ++pit)
			if (undermarked.find(pit->first)!=undermarked.end())
				{ tj.addArc(*fit,pit->first); pretmap[*fit].insert(pit->first); }
	}
	// add an arc from undermarked places to forbidden transitions that increase the number
	// of tokens on such a place
	for(fit=forbidden.begin(); fit!=forbidden.end(); ++fit)
	{
		map<Place*,int> col(im.getColumn(**fit)); // get effect of forbidden transition, not postset
		map<Place*,int>::iterator pit;
		for(pit=col.begin(); pit!=col.end(); ++pit)
			if (pit->second>0) // puts additional tokens on the place
				if (undermarked.find(pit->first)!=undermarked.end()) // place in undermarked set
					{ tj.addArc(pit->first,*fit); prepmap[pit->first].insert(*fit); }
	}
	// make loops on the places, so that our version of Tarjan's algorithm will not lose
	// components made of single places (components of single transitions may be lost, but we don't care)
	set<Place*>::iterator pit;
	for(pit=undermarked.begin(); pit!=undermarked.end(); ++pit)
		tj.addArc(*pit,*pit);
	// call Tarjan's algorithm
	vector<set<Transition*> > tcomp;
	vector<set<Place*> > pcomp;
	tj.getComponents(tcomp,pcomp);
	// go through all the places in a component
	for(unsigned int i=0; i<pcomp.size(); ++i)
		if (pcomp[i].size()>0) // only create a new constraint if there are places at all
		{
			bool b=true; // true as long as all pre-pre-places of places are in the same component,
			// i.e. the component is a sink in the component graph
			for(pit=pcomp[i].begin(); pit!=pcomp[i].end(); ++pit)
			{	// check the pre-transitions and THEIR pre-places
				set<Transition*> tset(prepmap[*pit]);
				set<Transition*>::iterator txit;
				for(txit=tset.begin(); txit!=tset.end(); ++txit)
				{
					set<Place*> ppset(pretmap[*txit]);
					set<Place*>::iterator pxit;
					for(pxit=ppset.begin(); pxit!=ppset.end(); ++pxit)
						if (pcomp[i].find(*pxit)==pcomp[i].end()) b=false; // a place outside the component!
				}
//cerr << (*pit)->getName() << ":" << undermarking[*pit] << ", ";
			}
//cerr << endl;
			if (b) // now we can create a constraint from this set of places, i.e. at least one token
			// from outside of this component is definitely needed to make its transitions firable.
			{
				map<Place*,int> ptmp; // temporary, weights for constraint-building
				int incr=0; // contains the increment of the RHS of the constraint
				for(pit=pcomp[i].begin(); pit!=pcomp[i].end(); ++pit)
					ptmp[*pit]=1; // this place should be in the constraint
				// now calculate the (increment for the) right hand side.
				// if there are transitions inside the component, activate at least one of them, and
				// it may then activate the others; if there are no transitions in the
				// component, the forbidden transitions cannot increase the token number
				// on any of the places in the component upon firing. Therefore, all the
				// necessary tokens must come from the outside.
				if (tcomp[i].size()>0) // there are transitions in the component
				{
					for(fit=tcomp[i].begin(); fit!=tcomp[i].end(); ++fit)
					{
						int addtimes(remains[*fit]); // additional required firings of a forbidden transition to activate others
						for(pit=pcomp[i].begin(); pit!=pcomp[i].end(); ++pit)
						{ // check if the transition produces too much tokens anywhere in the component
							int prod(im.getEntry(**fit,**pit));
							if (prod>0 && prod*addtimes>undermarking[*pit]) addtimes=undermarking[*pit]/prod;
						}

						// now go through all places and calculate the maximal token need for the transition
						int inctmp=0;
						map<Place*,int> pre(im.getPreset(**fit));
						for(pit=pcomp[i].begin(); pit!=pcomp[i].end(); ++pit)
						{ 	// calc need for the first instance (preset) and every later instance (change)
							// of a forbidden transition.
							if (pre[*pit]>inctmp+(int)(m[**pit])) inctmp=pre[*pit]-m[**pit]; // get enough for first firing
							int need = -addtimes*im.getEntry(**fit,**pit); // first + later firings
							if (addtimes>0) { // and for later firings
								if (need>inctmp) inctmp=need; 
							}
//cerr << "P:" << (*pit)->getName() << " need: " << need << " addtimes: " << addtimes << " m:" << m[**pit] << " inctmp: " << inctmp << endl;
						}
						// minimize over all forbidden transitions; zero means uninitialised value
						if (inctmp>0&&(inctmp<incr||incr==0)) incr=inctmp;
//cerr << (*fit)->getName() << ":" << addtimes << "?" << inctmp << ":" << incr << endl;
					}
				}
				else // there are no transitions in the component (which should consist of one place only) 
				{
					int postneed=0; // tokens that must remain in the component after the last forbidden transition has fired
					int cons=0; // effective consumption due to firing all forbidden transitions
					for(pit=pcomp[i].begin(); pit!=pcomp[i].end(); ++pit) // go through all places anyway
					{
						int posttmp=0,posttmpi=0; // minimal weight of the place in the transition vector's postset (cumulative)
						int constmp=-m[**pit]; // additional tokens needed on this place, at least 0
						map<int,set<Transition*> > post; // transitions ordered by the weight of the place pit in their postset
						for(fit=forbidden.begin(); fit!=forbidden.end(); ++fit)
						{
							pnapi::Arc* a(m.getPetriNet().findArc(**fit,**pit)); // get the weight of the place in the postset
							if (a) post[a->getWeight()].insert(*fit); else post[0].insert(*fit); // fit the place into the post map accordingly
							// calculate how many tokens get lost due to firing the forbidden transition as often as required, and add this number
							constmp -= im.getEntry(**fit,**pit)*remains[*fit];
						}
						// now go through the post map and approximate how many tokens must remain on the place
						map<int,set<Transition*> >::iterator poit;
						for(poit=post.begin(); poit!=post.end(); ++poit)
						{
							if (posttmpi<poit->first) { posttmp += poit->first-posttmpi; posttmpi=0; } // some tokens cannot be consumed
							else posttmpi-=poit->first; // the late transitions can still consume more
							// now check the next set of transitions for how many tokens they consume
							set<Transition*>::iterator xit;
							for(xit=poit->second.begin(); xit!=poit->second.end(); ++xit)
							{
								posttmpi += im.getLoops(**xit,**pit);
								if (im.getEntry(**xit,**pit)<0) posttmpi -= im.getEntry(**xit,**pit)*remains[*xit];
							}
							// iterate, the next set becoming the late set of transitions, getting a new next set
						}
						if (constmp<0) { posttmp+=constmp; constmp=0; } // tokens produced may be forwarded to posttmp
						if (posttmp<0) { posttmp=0; } // need of less than zero is need zero
						postneed += posttmp;
						cons += constmp;
//cerr << (*pit)->getName() << " cons: " << constmp << " post: " << posttmp << " cfm: " << cfm[*pit] << endl;
					}
					incr = cons+postneed; // the overall minimum number of tokens needed in this component for the forbidden transitions 
				}
				// we now know a number of additional tokens needed, so we can build the constraint 
				buildMultiConstraint(ptmp,incr,forbidden);
			}
		}
}

/*
int PartialSolution::checkUndermarking(Marking& m0, set<Transition*>& tset, set<Place*>& pset, set<Place*>& dead, IMatrix& im) {
	map<Transition*,map<Place*,int> > ipt,lpt;
	map<Transition*,int> cnt;
	set<Place*>::iterator pit;	
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		map<Place*,int> pmap,lmap;
		for(pit=pset.begin(); pit!=pset.end(); ++pit)
		{
			Arc* a(m0.getPetriNet().findArc(**pit,**tit));
			int aw = 0;
			if (a) aw = a->getWeight();
			pmap[*pit]=aw;
			a = m0.getPetriNet().findArc(**tit,**pit);
			if (a) {
				pmap[*pit]-=a->getWeight(); 
				if (pmap[*pit]>=0) lmap[*pit]=a->getWeight(); 
				else { pmap[*pit]=0; lmap[*pit]=aw; }
			}
		}
		ipt[*tit] = pmap;
		lpt[*tit] = lmap;
		cnt[*tit] = count(tseq.begin(),tseq.end(),*tit)+getRemains()[*tit];
	}

	Marking m1(m0);
	// calculate the minimal token need (-1 stating artificially that the variable is not initialized)
	int minneed = -1;
	// if the component won't fire, the necessary tokens have to be generated by other means, this is the maximal need
	if (dead.empty())
	{
		minneed = 0;
		map<Place*,int> fneed(underFinalMarking(im));
		set<Place*> pa(getPlacesForZSK(tset,im));
		set<Place*>::iterator pmit;
		for(pmit=pa.begin(); pmit!=pa.end(); ++pmit)
			minneed += fneed[*pmit];
	} 
	// if the component can be activated, only the tokens necessary for its activation are needed
	// we check for the minimal need at any point in the firing sequence so far and at any transition
	for(int i=0; i<=tseq.size(); ++i)
	{
		if (i>0) im.successor(m1,*(tseq[i-1]));
		if (i>0) --cnt[tseq[i-1]];
		// if a dead place (undermarked, not markable) is in the preset of a transition, this transition cannot be made firable
		// tgo is the set that POSSIBLY COULD be made firable
		set<Transition*> tgo;
		for(tit=tset.begin(); tit!=tset.end(); ++tit)
		{
			bool b = true;
			for(pit=dead.begin(); pit!=dead.end(); ++pit)
			{
				Arc* a(m0.getPetriNet().findArc(**pit,**tit));
				if (a && a->getWeight()>m1[**pit]) b = false;
			}
			if (b) tgo.insert(*tit);
		}
		// now go through the candidates and calc the minimal undermarking (minneed)
		for(tit=tgo.begin(); tit!=tgo.end(); ++tit)
		if (cnt[*tit]>0) // the transition appears in the preliminal solution
		{
			int internneed = 0;
			for(pit=pset.begin(); pit!=pset.end(); ++pit)
			{
				if (m1[**pit]<ipt[*tit][*pit]*cnt[*tit]+lpt[*tit][*pit]) 
					internneed+=ipt[*tit][*pit]*cnt[*tit]+lpt[*tit][*pit]-m1[**pit];
			}
			if (minneed<0 || minneed>internneed) minneed=internneed;
		}
		
	}
	return (minneed==0 ? 1 : minneed);
}
*/

/*
set<Place*> PartialSolution::getPlacesForZSK(const set<Transition*>& tset, IMatrix& im) {
	set<Place*> pre,post,all;
	set<Transition*>::iterator it;
	for(it=tset.begin(); it!=tset.end(); ++it)
	{
		map<Place*,int>& col(im.getColumn(**it));
		map<Place*,int>& loop(im.getLoopColumn(**it));
		map<Place*,int>::iterator mit;
		for(mit=col.begin(); mit!=col.end(); ++mit)
		{
			if (mit->second>0) post.insert(mit->first);
			if (mit->second<0) pre.insert(mit->first);
		}
		for(mit=loop.begin(); mit!=loop.end(); ++mit)
		{
			if (mit->second>0) all.insert(mit->first);
		}
	}
	set<Place*>::iterator pit;
	for(pit=post.begin(); pit!=post.end(); ++pit)
			all.insert(*pit);
	return all;
}
*/

/*
map<Place*,int> PartialSolution::findPlacesAfter(set<Transition*>& szk, IMatrix& im) {
	map<Place*,int> pmap,result;
	map<Place*,int>::iterator mit;
	set<Transition*>::iterator it;
	// calculate the total produce-consume on each incident place
	for(it=szk.begin(); it!=szk.end(); ++it)
	{
		map<Place*,int> padd(im.getColumn(**it));
		for(mit=padd.begin(); mit!=padd.end(); ++mit)
			if (pmap.find(mit->first)!=pmap.end()) pmap[mit->first]+=mit->second;
			else pmap[mit->first]=mit->second;
	}
	for(mit=pmap.begin(); mit!=pmap.end(); ++mit)
	{
		// eliminate zero entries
		if (mit->second>0) result[mit->first]=mit->second;
	}
	return result;
}
*/

/*
set<Place*> PartialSolution::findPlacesIn(set<Transition*>& szk, IMatrix& im) {
	set<Place*> result;
	map<Place*,int>::iterator mit;
	set<Transition*>::iterator it;
	// calculate the tokens produced (even if consumed at the same time) on each incident place
	for(it=szk.begin(); it!=szk.end(); ++it)
	{
		map<Place*,int>& padd(im.getColumn(**it));
		for(mit=padd.begin(); mit!=padd.end(); ++mit)
			if (mit->second>0) result.insert(mit->first);
		map<Place*,int>& ploop(im.getLoopColumn(**it));
		for(mit=ploop.begin(); mit!=ploop.end(); ++mit)
			if (mit->second>0) result.insert(mit->first);
	}
	return result;
}
*/

/*
set<Place*> PartialSolution::findPlacesDead(set<Transition*>& szk, IMatrix& im) {
	map<Place*,int> pch = im.getChange(getRemains());
	map<Place*,int>::iterator mit;
	set<Place*> pset;
	set<Transition*>::iterator it;
	for(it=szk.begin(); it!=szk.end(); ++it)
	{
		map<Place*,int> post = im.getColumn(**it);
		for(mit=post.begin(); mit!=post.end(); ++mit)
			if (mit->second>0 && pch[mit->first]>0) 
			{
				bool b = true;
				set<Arc*> arcs(mit->first->getPresetArcs());
				set<Arc*>::iterator ait;
				for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
					if (szk.find(&((*ait)->getTransition()))==szk.end()) b=false;
				if (b) pset.insert(mit->first);
			}
	}
	return pset;
}
*/

/** Check whether this partial solution is a full solution.
	@return If this represents a full solution.
*/
bool PartialSolution::isSolved() { return fullSolution; }

/** Mark the partial solution as a full one, i.e. we have just solved the reachability problem.
*/
void PartialSolution::setSolved() { fullSolution = true; }

/** Mark all constraints as non-recent so they cannot be the reason for not finding solutions.
	@param jump Whether jumps or normal constraint are to be touched.
*/
void PartialSolution::touchConstraints(bool jump) {
	set<Constraint>::iterator cit;
	for(cit=constraints.begin(); cit!=constraints.end(); ++cit)
		if (cit->isJump()==jump) const_cast<Constraint*>(&(*cit))->setRecent(false);
	for(cit=failure.begin(); cit!=failure.end(); ++cit)
		if (cit->isJump()==jump) const_cast<Constraint*>(&(*cit))->setRecent(false);
}

/** Calculates which forbidden transition are disabled because of the given set of places
	in this partial solution.
	@param pset The set of places of interest (second part of the map is irrelevant).
	@param im Reference to the incidence matrix of the Petri net
	@return Those forbidden transitions that need more tokens from one of the places in pset
		than available in the marking last reached.
*/ 
set<Transition*> PartialSolution::disabledTransitions(map<Place*,int> pset, IMatrix& im) {
	set<Transition*> result;
	map<Place*,int>::iterator pit;
	map<Transition*,int>::iterator mtit;
	for(mtit=remains.begin(); mtit!=remains.end(); ++mtit)
		if (mtit->second>0)
		{
			map<Place*,int> pre(im.getPreset(*(mtit->first)));
			for(pit=pset.begin(); pit!=pset.end(); ++pit)
				if (pre[pit->first]>(int)(m[*(pit->first)])) 
					{ result.insert(mtit->first); break; }
		}
	return result;
}

/** Calculate the value of the LHS of the constraint under the marking reached
	by the firing sequence of this partial solution.
	@param c The constraint to evaluate.
	@return The value of the LHS.
*/
int PartialSolution::getActualRHS(const Constraint& c) {
	const map<Transition*,int>& cs(c.getLHS());
	int allprod = 0;
	for(unsigned int i=0; i<tseq.size(); ++i)
		if (cs.find(tseq[i])!=cs.end()) 
			allprod += cs.find(tseq[i])->second;
	return allprod;
}

/** Calculate the parikh image of the firing sequence and store it.
*/
map<Transition*,int> PartialSolution::calcParikh() {
	parikh.clear();
	map<Transition*,int> result;
	for(unsigned int i=0; i<tseq.size(); ++i)
		++result[tseq[i]];
	parikh.push_back(result);
	return result;
}

/** Add a parikh image of a firing sequence to a failure.
	@param The parikh image to add.
*/
void PartialSolution::addParikh(map<Transition*,int>& p) { parikh.push_back(p); }

/** Get all parikh images of firing sequences for this failure.
	@return A vector of parikh images.
*/
vector<map<Transition*,int> >& PartialSolution::getParikh() { return parikh; }

Transition* PartialSolution::getNextJC(int& val) { 
	if (jc.empty()) return NULL;
	val = jc.begin()->second;
	return transitionorder[jc.begin()->first];
}

void PartialSolution::setJC(map<Transition*,int>& diff) { 
	jc.clear();
	map<Transition*,int>::iterator mit;
	for(mit=diff.begin(); mit!=diff.end(); ++mit)
		if (mit->second>0)
			jc[revtorder[mit->first]] = mit->second;
}

void PartialSolution::popJC() { if (!jc.empty()) jc.erase(jc.begin()); }

bool PartialSolution::compareSequence(vector<Transition*> seq) {
	if (seq.size()!=tseq.size()) return false;
	map<Transition*,int> cnt;
	for(int i=0; i<tseq.size(); ++i) {
		++cnt[tseq[i]];
		--cnt[seq[i]];
	}
	map<Transition*,int>::iterator mit;
	for(mit=cnt.begin(); mit!=cnt.end(); ++mit)
		if (mit->second!=0) return false;
	return true;
}
