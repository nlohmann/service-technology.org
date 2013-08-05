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
 * \date    $Date: 2010-08-13 12:00:00 +0200 (Fr, 13. Aug 2010) $
 *
 * \version $Revision: 1.01 $
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

namespace sara {
extern vector<Transition*> transitionorder;
extern map<Transition*,int> revtorder;
}

	/*************************************************************
	* Implementation of the methods of the class PartialSolution *
	*************************************************************/

namespace sara {

/** Constructor for the first partial solution (no output from lp_solve yet).
	@param m1 The initial marking.
*/ 
PartialSolution::PartialSolution(Marking& m1) : m(m1),markingEquation(false),jumpsdone(0) {
	fullSolution = false;
} 

/** Constructor for later partial solutions.
	@param ts The partial firing sequence.
	@param m1 The marking reached after the partial firing sequence.
	@param rvec The remaining (nonfirable) transitions. 
*/
PartialSolution::PartialSolution(vector<Transition*>& ts,Marking& m1,map<Transition*,int>& rvec) : tseq(ts),m(m1),remains(rvec),markingEquation(false),jumpsdone(0) {
	fullSolution = false;
}

/** Destructor.
*/
PartialSolution::~PartialSolution() {} 

/** Get method for the marking.
	@return The marking reached after the partial firing sequence.
*/
Marking& PartialSolution::getMarking() { return m; }

/* Set the marking reached after firing the partial firing sequence.
	@param m1 The Marking.
*/
//void PartialSolution::setMarking(Marking& m1) { m=m1; }

/** Get method for the partial firing sequence.
	@return The partial firing sequence.
*/
vector<Transition*>& PartialSolution::getSequence() { return tseq; }

/** Set method for the partial firing sequence.
	@param seq The partial firing sequence.
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

/** Set a single constraint.
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
	else {
		constraints.insert(c);
		if (c.isJump()) ++jumpsdone;
		else if (c.isSingle()) --jumpsdone;
	}
	return true;
}

/** Set a single failure constraint.
	@param c A constraint.
	@return True always.
*/
bool PartialSolution::setFailureConstraint(const Constraint& c) {
	failure.insert(c);
	return true;
}

/** Set all constraints of the partial solution at once, existing constraints are deleted.
	@param cv The new constraints.
*/
void PartialSolution::setConstraints(set<Constraint> cv) { constraints = cv; }

/** Get all constraints in the partial solution.
	@return The constraints.
*/
set<Constraint>& PartialSolution::getConstraints() { return constraints; }

/** Get all failed constraints in the partial solution.
	@return The constraints.
*/
set<Constraint>& PartialSolution::getFailureConstraints() { return failure; }

/** Remove all constraints from the partial solution.
*/
void PartialSolution::clearConstraints() { constraints.clear(); }

/** Sets the flag for feasibility of the marking equation. The flag is not set automatically!
	@param mef True, if the marking equation is feasible. 
*/
void PartialSolution::setFeasibility(bool mef) { markingEquation=mef; }

/** Check if the flag for feasibility of the marking equation is set.
	@return The flag.
*/
bool PartialSolution::isFeasible() const { return markingEquation; }

/** Prints the partial solution on cerr for debugging purposes.
*/
void PartialSolution::show() {
	cerr << "SEQ:  ";
	for(unsigned int j=0; j<getSequence().size(); j++)
		cerr << getSequence().at(j)->getName() << " ";
	cerr << endl;
	vector<map<Transition*,int> >& pa(getParikh());
	if (!pa.empty()) cerr << "Parikh Images:" << endl;
	for(unsigned int i=0; i<pa.size(); ++i)
	{
		cerr << "- ";
		map<Transition*,int>::iterator mit;
		for(mit=pa[i].begin(); mit!=pa[i].end(); ++mit)
			cerr << mit->first->getName() << ":" << mit->second << " ";
		cerr << endl;
	}
	map<const Place*,unsigned int>::const_iterator mit;
	cerr << "Marking: ";
	for(mit=getMarking().begin(); mit!=getMarking().end(); ++mit)
	{
		if (mit->second>0) cerr << mit->first->getName() << ":" << mit->second << " "; 
	}
	cerr << endl;
	cerr << "Remainder: ";
	map<Transition*,int>::iterator vit;
	for(vit=getRemains().begin(); vit!=getRemains().end(); ++vit)
		if (vit->second>0)
			cerr << vit->first->getName() << ":" << vit->second << " ";
	cerr << endl;
	cerr << "Constraints: ";
	set<Constraint>::iterator cit;
	for(cit=constraints.begin(); cit!=constraints.end(); ++cit)
		cit->showConstraint(cerr);
	cerr << endl;
}

/** Constructs a new increment constraint from a given weighted set of places and forbidden transitions and
	adds it to the partial solution.
	@param pmap The places (weighted) that need additional tokens.
	@param incr The number of additional tokens needed.
	@param forbidden A set of transitions that is not allowed to deliver these tokens.
	@return Whether the constraint is non-trivial and has thus been added.
*/
bool PartialSolution::buildMultiConstraint(map<Place*,int>& pmap, int incr, set<Transition*>& forbidden) {
/*
	Constraint c,c2;
	map<Place*,int>::iterator pit;
	for(pit=pmap.begin(); pit!=pmap.end(); ++pit)
		c2.addPlace(*(pit->first),pit->second);
	set<Transition*>::iterator tit;
	for(tit=forbidden.begin(); tit!=forbidden.end(); ++tit)
		if (c2.checkSubTransition(**tit)) c2.addSubTransition(**tit);
	map<Transition*,int>& cs2(c2.calcConstraint());
	map<Transition*,int>::iterator rit;
	for(pit=pmap.begin(); pit!=pmap.end(); ++pit)
	{
		bool produces(false);
		if (cs2.empty()) produces=true;
		else for(rit=cs2.begin(); rit!=cs2.end(); ++rit)
			if (rit->second>0 && im.getEntry(*(rit->first),*(pit->first))>0) produces=true;
		if (produces) c.addPlace(*(pit->first),pit->second);
	}
	for(tit=forbidden.begin(); tit!=forbidden.end(); ++tit)
		if (c.checkSubTransition(**tit)) c.addSubTransition(**tit);
*/
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
	forces an additional token on each such place.
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
					// first check if there were more tokens earlier
/**/
					map<Place*,int> red;
					reduceUndermarking(im,pcomp[i],red);
					for(map<Place*,int>::iterator mpit=red.begin(); mpit!=red.end(); ++mpit) {
						undermarking[mpit->first] -= mpit->second;
						if (undermarking[mpit->first] < 1) undermarking[mpit->first] = 1;
					}

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
/**/
					// reduce increment if there were more tokens earlier
					map<Place*,int> red;
					reduceUndermarking(im,pcomp[i],red);
					if (incr-red.begin()->second<1) incr=1; else incr -= red.begin()->second;
				}
				// we now know a number of additional tokens needed, so we can build the constraint 
				buildMultiConstraint(ptmp,(incr>1 ? incr:1),forbidden);
			}
		}
}

/** Check whether this partial solution is a full solution.
	@return If this represents a full solution.
*/
bool PartialSolution::isSolved() const { return fullSolution; }

/** Mark the partial solution as a full one, i.e. we have just solved the reachability problem.
*/
void PartialSolution::setSolved() { fullSolution = true; }

/** Mark all constraints as non-recent so they cannot be the reason for not finding solutions.
	@param jump Whether jumps or increment constraint are to be touched.
*/
void PartialSolution::touchConstraints(bool jump) {
	set<Constraint>::iterator cit;
	for(cit=constraints.begin(); cit!=constraints.end(); ++cit)
		if (cit->isJump()==jump) const_cast<Constraint*>(&(*cit))->setRecent(false);
	for(cit=failure.begin(); cit!=failure.end(); ++cit)
		if (cit->isJump()==jump) const_cast<Constraint*>(&(*cit))->setRecent(false);
}

/** Calculate which forbidden transition are disabled because of the given set of places
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
	@param c The constraint to be evaluated.
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
	@return The parikh image.
*/
map<Transition*,int> PartialSolution::calcParikh() {
	parikh.clear();
	map<Transition*,int> result;
	for(unsigned int i=0; i<tseq.size(); ++i)
		++result[tseq[i]];
	parikh.push_back(result);
	return result;
}

/** Set a parikh image of a firing sequence in a failure.
	@param p The parikh image to SET.
*/
void PartialSolution::addParikh(map<Transition*,int>& p) { parikh.clear(); parikh.push_back(p); }


/** Get all parikh images of firing sequences for this failure.
	@return A vector of parikh images.
*/
vector<map<Transition*,int> >& PartialSolution::getParikh() { return parikh; }

/** Compare the lp_solve solution stored with the given vector.
	@param vec The vector to compare with.
	@return If the stored vector is less or equal to the given one.
*/
bool PartialSolution::compareVector(map<Transition*,int>& vec) { 
	map<Transition*,int>::iterator pit,pit2;
	for(pit=fulltv.begin(); pit!=fulltv.end(); ++pit)
	{ // check if this solution is greater in some component
		pit2 = vec.find(pit->first);
		if (pit2==vec.end()) return false;
		if (pit->second>pit2->second) return false;
	}
	// fulltv <= vec
	for (pit2=vec.begin(); pit2!=vec.end(); ++pit2)
	{ // check if vec is greater
		if (pit2->second==0) continue;
		pit = fulltv.find(pit2->first);
		if (pit==fulltv.end()) return true;
		if (pit->second<pit2->second) return true;
	}
	// fulltv == vec
	return false; 
}

/** Get the next jump in the list of precalculated jumps.
	@param val On return: the right hand side n of the jump constraint.
	@return The transition t on the left hand side of the jump constraint t<=n.
*/
Transition* PartialSolution::getNextJC(int& val) { 
	if (jc.empty()) return NULL;
	val = jc.begin()->second;
	return transitionorder[jc.begin()->first];
}

/** Create the list of precalculated jump constraints.
	@param diff A map of jump constraints to be made.
*/
void PartialSolution::setJC(map<Transition*,int>& diff) { 
	jc.clear();
	map<Transition*,int>::iterator mit;
	for(mit=diff.begin(); mit!=diff.end(); ++mit)
		if (mit->second>0)
			jc[revtorder[mit->first]] = mit->second;
}

/** Remove the first jump in the list of precalculated jump constraints.
*/
void PartialSolution::popJC() { if (!jc.empty()) jc.erase(jc.begin()); }

/** Compare the firing sequence in this partial solution with seq.
	@param seq A firing sequence.
	@param rem A remainder of non-firable transitions.
	@return If the two sequences are permutations of each other and no new
		transitions appear in the remainder compared to the full transition
		vector of this partial solution.
*/
bool PartialSolution::compareSequence(vector<Transition*> seq, map<Transition*,int> rem) {
	if (seq.size()!=tseq.size()) return false;
	map<Transition*,int> cnt;
	for(unsigned int i=0; i<tseq.size(); ++i) {
		++cnt[tseq[i]];
		--cnt[seq[i]];
	}
	map<Transition*,int>::iterator mit;
	for(mit=cnt.begin(); mit!=cnt.end(); ++mit)
	{
		if (mit->second!=0) return false;
	}
	for(mit=rem.begin(); mit!=rem.end(); ++mit)
	{
		if (mit->second==0) continue;
		if (fulltv.find(mit->first)==fulltv.end()) return false;
		else if (fulltv.find(mit->first)->second==0) return false;
	}
	return true;
}

/** Transform jump constraints (with <=) in this partial solution into
	normal constraints (with >=).
	@param fullvector The solution vector generated by lp_solve to determine the normal constraints.
*/
void PartialSolution::transformJumps(map<Transition*,int>& fullvector) {
	bool hadjump(false);
	// Delete the jumps t<=m
	set<Constraint>::iterator cit;
	for(cit=constraints.begin(); cit!=constraints.end();)
		if (cit->isJump()) {
			constraints.erase(cit++);
			hadjump = true;
		} else ++cit;
	if (!hadjump) return;
	// Now create the constraints of the form t>=n so we always get at least this solution
	map<Transition*,int>::iterator mit;
	for(mit=fullvector.begin(); mit!=fullvector.end(); ++mit)
		if (mit->second>0) {
			map<Transition*,int> tmp;
			tmp[mit->first]=mit->second;
			Constraint c(tmp,false);
			setConstraint(c);
		}	
}


/** Reduce the number of missing tokens if there were more tokens earlier in the firing sequence.
	@param im The incidence matrix of the Petri net.
	@param pcomp The set of places that miss tokens.
	@param red The reduction values, i.e. how many more tokens have been encountered earlier.
*/
void PartialSolution::reduceUndermarking(IMatrix& im, const set<Place*>& pcomp, map<Place*,int>& red)
{
	map<Place*,int> tmp;
	set<Place*>::iterator pit;
	for(int i=tseq.size()-1; i>=0; --i) {
		map<Place*,int>& pmap(im.getColumn(*tseq[i])); // get the change for firing a transition
		for(pit=pcomp.begin(); pit!=pcomp.end(); ++pit) {
			map<Place*,int>::iterator ppit(pmap.find(*pit));
			if (ppit!=pmap.end()) {
				int x(tmp[*pit] -= ppit->second); // and subtract it from the marking (reverse-fire)
				if (x > red[*pit]) red[*pit] = x; // save the maximum token overhead in the sequence
			}
		}
	}
}


} // end namespace sara
