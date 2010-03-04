// -*- C++ -*-

/*!
 * \file    constraint.cc
 *
 * \brief   Class Constraint
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/12
 *
 * \date    $Date: 2009-10-12 09:30:00 +0200 (Mo, 12. Okt 2009) $
 *
 * \version $Revision: -1 $
 */

#include <map>
#include <set>
#include <cstdio>
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif

#ifndef CONSTRAINT_H
#include "constraint.h"
#endif
#ifndef LPWRAPPER_H
#include "lpwrapper.h"
#endif

using pnapi::Place;
using pnapi::Transition;
using pnapi::Arc;
using std::set;
using std::map;
using std::cerr;
using std::endl;
using std::ostream;

	/********************************************************
	* Implementation of the methods of the class Constraint *
	********************************************************/

/** Standard constructor for class Constraint.
*/
Constraint::Constraint() : rhs(0),jump(false) { posplace.clear(); subtrans.clear(); cs.clear(); }

/** Jump constructor for class Constraint.
	@param jmp A solution of lp_solve which is to be discriminated.
*/
Constraint::Constraint(map<Transition*,int> jump) : rhs(-1)
{ 
	posplace.clear(); 
	subtrans.clear(); 
	this->jump = true;
	// calculate the right-hand-side minus one
	map<Transition*,int>::iterator it;
	for(it=jump.begin(); it!=jump.end(); ++it)
	{	rhs += it->second; cs[it->first] = 1; }
}

/** Destructor.
*/
Constraint::~Constraint() { posplace.clear(); subtrans.clear(); cs.clear(); }

/** Adds a place to the constraint. The place represents its own preset,
	i.e. the constraint will contain each transition t from the preset
	of the place p weighted with the weight of the arc t->p.
	@param p The place p.
	@param num A weight for place p, e.g. with a weight of num=2 the
		constraint will contain the preset of p with twice as large
		weights as if num=1 were chosen. Note: addPlace(p,1);
		addPlace(p,1) will set the weight to 1, only the last call
		counts.
*/ 
void Constraint::addPlace(Place& p, int num) {
	posplace[&p]=num;
}

/** Removes a place from the constraint. The difference to addPlace(p,0)
	is that the place will be erased from the constraint. These two
	methods lead to incomparable constraints, so removePlace(p) is the
	advised method.
	@param p The place to be removed from the constraint.
	@return If the removal was successfully done.
*/
bool Constraint::removePlace(Place& p) {
	map<Place*,int>::iterator it(posplace.find(&p));
	bool ret = (it!=posplace.end());
	if (ret) posplace.erase(it);
	return ret;
}

/** Gets information about all places in the constraint, i.e. the latest
	call to addPlace/removePlace for each place.
	@return A map from places to their weights in the constraint.
*/
const map<Place*,int>& Constraint::getPlaces() const {
	return posplace;
}

/** Forbids a transition to be in this constraint. The transition will not
	appear no matter how often places with this transition in their
	preset are added to the constraint via addPlace().
	@param t The transition to be forbidden.
*/
void Constraint::addSubTransition(Transition& t) {
	subtrans.insert(&t);
}

/** Reallows a forbidden transition. If the transition is in the presets of
	some places added via addPlace() to the constraint, the transition
	will automatically appear with the correct weight.
	@param t The transition to be allowed again.
	@return If the operation was successful.
*/
bool Constraint::removeSubTransition(Transition& t) {
	set<Transition*>::iterator it(subtrans.find(&t));
	bool ret = (it!=subtrans.end());
	if (ret) subtrans.erase(it);
	return ret;
}

/** Gets all forbidden transitions.
	@return The set of forbidden transitions.
*/
const set<Transition*>& Constraint::getSubTransitions() const {
	return subtrans;
}

/** Sets the right hand side of the constraint, i.e. the rhs in " ... >= rhs".
	@param rhs The right hand side of the constraint.
*/
void Constraint::setRHS(int rhs) { this->rhs = rhs; }

/** Gets the right hand side of the constraint.
	@return The right hand side.
*/
int Constraint::getRHS() const { return rhs; }

/** Whether the constraint should have a <= instead of >= (for jumping to incomparable solutions)
	@return true if the constraint is of the form lhs(linear comb.) <= rhs(int)
*/
bool Constraint::isJump() const { return jump; }

/** Computes the constraint in a form used by lp_solve.
	@param coeff An array of REAL with the weights of the transitions to
		be sent to lp_solve via add_constraint(). If the operation is
		successful, the array will be filled by this method. 
	@param tpos A map ordering the transitions of the Petri net from 1
		to |T|.
	@param cols The number of columns (transitions) of the linear system.
		Also the dimension of coeff (coeff[cols+1]).
	@return If the calculation was successful. If not,
		the constraint vector coeff[] is zero-filled 
		and should not be sent to lp_solve, as it may render the 
		linear system infeasible (depending on the right hand side).
*/
bool Constraint::calcConstraint(REAL coeff[], map<Transition*,int>& tpos, int cols) const { 
	// clear coefficients for lp_solve
	for(int i=0; i<=cols; i++) coeff[i]=0;
	// if the constraint has just been defined indirectly (via posplace and subtrans), calculate it
	if (cs.empty()) { cerr << "sara: warning: constraint not calculated" << endl; return false; } //calcConstraint();
	// sort the coefficients for lp_solve
	map<Transition*,int>::const_iterator cit;
	for(cit=cs.begin(); cit!=cs.end(); ++cit)
	{
		int pos = tpos[cit->first];
		if (pos<0 || pos>=cols) { cerr << "sara: Constraint error, wrong size of constraint" << endl; return false; }
		coeff[pos+1] = cit->second;
	}
	// check if there are transitions in the constraint at all
	for(int i=0; i<=cols; ++i) if (coeff[i]>0) return true;
	return false;
}

/** Computes the constraint from the posplace and subtrans entries
	@return The linear combination (left hand side) of the constraint
		as a map. The map may be empty.
*/
map<Transition*,int>& Constraint::calcConstraint() { 
	if (!cs.empty()) return cs;
	map<Transition*,int> result;
	// go through all places in the constraint
	map<Place*,int>::iterator pit;
	for(pit=posplace.begin(); pit!=posplace.end(); ++pit)
	{
		Place& p = *(pit->first);
		if (&p==NULL) { cerr << "sara: Constraint error, null place" << endl; return cs; }
		// find the t->p arcs
		set<Arc*> arcs(p.getPresetArcs());
		set<Arc*>::iterator ait;
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Transition& t = (*ait)->getTransition();
			// find the p->t arcs
			Arc* a = t.getPetriNet().findArc(p,t); 
			// calculate if t produces more on p than it consumes
			int tmp = (*ait)->getWeight(); 
			if (a!=NULL) { tmp -= a->getWeight(); if (tmp<0) tmp=0; }
			// add the lp_solve coefficient for non-forbidden transitions
			if (subtrans.find(&t)==subtrans.end()) 
			{
				result[&t] += tmp;
/*
				// does int have a default value? If not:
				if (result.find(&t)!=result.end()) result[&t] += tmp;
				else result[&t] = tmp;
*/
			}
		}
	}
	// transfer the result, eliminating zero entries
	map<Transition*,int>::iterator rit;
	for(rit=result.begin(); rit!=result.end(); ++rit)
		if (rit->second>0) cs[rit->first]=rit->second;
	return cs;
}

/** Gets the LHS of the constraint as a weighted sum of transitions, if that
	has previously been calculated by calcConstraint().
	@return The weighted sum of transitions.
*/
const map<Transition*,int>& Constraint::getLHS() const { return cs; }

/** Checks if forbidding a transition will have any effect. It is advised
	not to forbid transitions if it would not have any effect.
	@param t The transition to be checked for.
	@return If forbidding the transition would have an effect.
*/
bool Constraint::checkSubTransition(Transition& tr) { 
	// iterate through all places in the constraint
	map<Place*,int>::iterator pit;
	for(pit=posplace.begin(); pit!=posplace.end(); ++pit)
	{
		Place& p = *(pit->first);
		if (&p==NULL) { cerr << "sara: Constraint error, null place" << endl; return false; }
		// find all t->p arcs
		set<Arc*> arcs(p.getPresetArcs());
		set<Arc*>::iterator ait;
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Transition& t = (*ait)->getTransition();
			// find a matching p->t arc
			Arc* a = t.getPetriNet().findArc(p,t);
			// calculate if t effectively produces tokens 
			int tmp = (*ait)->getWeight(); 
			if (a!=NULL) { tmp -= a->getWeight(); if (tmp<0) tmp=0; }
			// if so and t is the transition to be checked, it would appear in the constraint
			if (tmp>0 && &tr==&t) return true;
		}
	}
	// t would not appear with positive weight in the constraint
	return false;
}

/** Checks whether the constraint contains any non-forbidden transitions at all.
	@return If the constraint is semantically non-empty (in the above sense).
*/
bool Constraint::checkAnyTransition() const { 
	// iterate through all places in the constraint
	map<Place*,int>::const_iterator pit;
	for(pit=posplace.begin(); pit!=posplace.end(); ++pit)
	{
		Place& p = *(pit->first);
		if (&p==NULL) { cerr << "sara: Constraint error, null place" << endl; return false; }
		// find all t->p arcs
		set<Arc*> arcs(p.getPresetArcs());
		set<Arc*>::iterator ait;
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Transition& t = (*ait)->getTransition();
			// find a p->t arc
			Arc* a = t.getPetriNet().findArc(p,t); 
			// calculate the effective produce of t on p
			int tmp = (*ait)->getWeight(); 
			if (a!=NULL) { tmp -= a->getWeight(); if (tmp<0) tmp=0; }
			// if t produces on p and is not in the forbidden list, the constraint is non-empty
			if (tmp>0 && subtrans.find(&t)==subtrans.end()) return true;
		}
	}
	// the constraint is empty (though ist may contain places and forbidden transitions)
	return false;
}

/** Compares two constraints.
	@param c The second constraint.
	@return 0 if constraints are incomparable (different places or 
		forbidden transitions), 1 if this is stronger than c, -1 otherwise.
*/
int Constraint::compare(const Constraint& c) const {
	if (cs.empty() || c.cs.empty()) { cerr << "sara: warning: empty constraint in comparison" << endl; return 0; }
	if (jump!=c.jump) return 0;
	if (cs!=c.cs || c.cs!=cs) return 0;
	if (rhs > c.rhs) return (jump?-1:1);
	return (jump?1:-1);
}

/** Prints the constraint on stderr (without any end-of-lines).
	@param s The stream to print on.
*/
void Constraint::showConstraint(ostream& s) const {
	// If a comma is needed for the comma separated list
	bool comma = false;
	s << "[";
	if (jump) { // direct (jump) constraint, not via places
		map<Transition*,int>::const_iterator cit;
		for(cit=cs.begin(); cit!=cs.end(); ++cit)
		{
			if (comma) s << ",";
			s << cit->first->getName() << ":" << cit->second;
			comma = true;
		}
		s << "]<=" << getRHS() << " "; //"-" << getRHSLimit() << " ";
		return;
	}
	// print all places in the constraint
	map<Place*,int>::const_iterator pxit;
	for(pxit=getPlaces().begin(); pxit!=getPlaces().end(); ++pxit)
	{
		if (comma) s << ",";
		s << pxit->first->getName() << ":" << pxit->second;
		comma = true;
	}
	// print all forbidden transitions
	set<Transition*>::const_iterator txit;
	for(txit=getSubTransitions().begin(); txit!=getSubTransitions().end(); ++txit)
	{
		if (comma) s << ",";
		s << (*txit)->getName();
		comma = true;
	}
	s << "]>=" << getRHS() << " "; //"-" << getRHSLimit() << " ";
}

/** Set a flag determining whether a constraint is recent. Recent constraints
	can become part of a counterexample.
	@param r If the constraint is recent.
*/
void Constraint::setRecent(bool r) { recent=r; }

/** Determinine whether a constraint is recent. Recent constraints
	can become part of a counterexample.
	@return If the constraint is recent.
*/
bool Constraint::isRecent() const { return recent; }

/** Comparison operator for constraints for use as set::less.
	@param right Right hand side of the expression.
	@return If this is less than right.
*/
bool Constraint::operator<(const Constraint& right) const {
	if (cs.size()<right.cs.size()) return true;
	if (cs.size()>right.cs.size()) return false;
	if (cs<right.cs) return true;
	if (cs>right.cs) return false;
	if (jump&&!right.jump) return false;
	if (!jump&&right.jump) return true;
	if (rhs<right.rhs) return true;
//	if (rhs>right.rhs) return false;
//	if (limit<right.limit) return true;
	return false;
}

/** Comparison operator for constraints in set comparisons.
	@param right Right hand side of the expression.
	@return If both constraints are equal.
*/
bool Constraint::operator==(const Constraint& right) const {
	if (cs.size()!=right.cs.size()) return false;
	if (jump!=right.jump) return false;
	if (rhs!=right.rhs) return false;
	return (cs==right.cs);	
} 

/** Remove all constraints with more places and transitions from the set.
	@param The set of constraints to clean up.
	@return Whether this constraint should be inserted into the set.
*/
bool Constraint::cleanConstraintSet(set<Constraint>& sc) const {
	bool result(true);
	set<Constraint>::iterator sit(sc.begin());
	set<Constraint>::iterator stmp;
	while (sit!=sc.end()) {
		bool pinc = false; // whether the place set comparison yielded true
		const map<Place*,int>& scp(sit->getPlaces());
		const map<Place*,int>& thp(getPlaces());
		map<Place*,int>::const_iterator scpit;
		map<Place*,int>::const_iterator thpit;
		// check first if (places of) thp is a subset of (places of) scp
		for(scpit=scp.begin(),thpit=thp.begin(); scpit!=scp.end(); ++scpit)
		{
			if (scpit->first!=thpit->first) continue;
			++thpit;
			if (thpit==thp.end()) break;
		}
		// on a positive result, mark queue entry as possibly obsolete
		if (thpit==thp.end()) pinc = true;

		const set<Transition*>& sct(sit->getSubTransitions());
		const set<Transition*>& tht(getSubTransitions());
		set<Transition*>::const_iterator sctit;
		set<Transition*>::const_iterator thtit;
		// check now if tht is a subset of sct
		if (!tht.empty()) {
			for(sctit=sct.begin(),thtit=tht.begin(); sctit!=sct.end(); ++sctit)
			{
				if ((*sctit)!=(*thtit)) continue;
				++thtit;
				if (thtit==tht.end()) break;
			}
			// on a positive result, mark queue entry as obsolete (delete it)
			if (thtit==tht.end() && pinc) { stmp=sit; ++sit; sc.erase(stmp); continue; }
		} else if (pinc) { stmp=sit; ++sit; sc.erase(stmp); continue; }

		if (result) {
			pinc = false;
			// check if scp is a subset of thp
			for(thpit=thp.begin(),scpit=scp.begin(); thpit!=thp.end(); ++thpit)
			{
				if (scpit->first!=thpit->first) continue;
				++scpit;
				if (scpit==scp.end()) break;
			}
			// on a positive result, mark this as possibly obsolete
			if (scpit==scp.end()) pinc = true;
			if (!sct.empty()) {
				for(thtit=tht.begin(),sctit=sct.begin(); thtit!=tht.end(); ++thtit)
				{
					if ((*sctit)!=(*thtit)) continue;
					++sctit;
					if (sctit==sct.end()) break;
				}
				// on a positive result, mark this as obsolete
				if (sctit==sct.end() && pinc) result = false;
			} else if (pinc) result = false;
		}
		++sit;
	}
	return result;
}

