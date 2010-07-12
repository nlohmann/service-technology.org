// -*- C++ -*-

/*!
 * \file    extmarking.cc
 *
 * \brief   Class ExtMarking
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/05/14
 *
 * \date    $Date: 2010-05-14 14:47:01 +0200 (Fr, 16. Mai 2010) $
 *
 * \version $Revision: -1 $
 */

#include <map>
#include <iostream>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "extmarking.h"

using pnapi::PetriNet;
using pnapi::Marking;
using pnapi::Place;
using pnapi::Transition;
using std::map;
using std::cout;
using std::endl;
using std::ostream;

	/*************************************
	* Implementation of class ExtMarking *
	*************************************/

/** Standard constructor for the empty extended marking (all entries zero)
*/
ExtMarking::ExtMarking() {}

/** Standard constructor for a given marking
	@param m A marking to be converted to an extended marking.
*/
ExtMarking::ExtMarking(const Marking& m) {
	map<const Place*,unsigned int>::const_iterator mit;
	for(mit=m.begin(); mit!=m.end(); ++mit)
		if (mit->second>0) lb[const_cast<Place*>(mit->first)]=(int)(mit->second);
}

/** Destructor.
*/
ExtMarking::~ExtMarking() {}

/** Print method for extended markings.
	@param o Stream to print to.
	@param porder How the places should be ordered for printing.
*/
void ExtMarking::show(ostream& o, const vector<Place*>& porder) {
	map<Place*,bool>::iterator bit; // for enumeration of the type: single value, open interval or closed interval
	map<Place*,int>::iterator mit; // for enumeration of the interval bounds
	bool comma = false; // to surpress the first comma in the output
	for(unsigned int i=0; i<porder.size(); ++i) // go through all places in the given order
	{
		bit = type.find(porder[i]); // get the marking type for this place
		mit = lb.find(porder[i]); // and the lower bound of the interval / the single value
		if (bit==type.end() && mit==lb.end()) continue; // if none exists, don't print the zero tokens
		if (comma) o << ","; else comma=true; // if a comma should be printed
		o << porder[i]->getName(); // print the name of the place
		if (bit==type.end()) { // if it's a single value ...
			if (mit->second>1) o << ":" << mit->second; // ... greater than one, append the number of tokens
		} else { // if it's an interval, print the lower bound
			if (mit==lb.end()) o << ":0"; // which may be zero (not set)
			else o << ":" << mit->second; // or some value greater than zero
			if (bit->second) o << "+"; // if it's an open interval, append a plus (0+ is omega)
			else o << "-" << ub[porder[i]]; // otherwise print the upper bound
		}
	}
}

/** Set the number of tokens on a place to omega, unlimited and any number
	@param p The place.
	@return If this worked.
*/
bool ExtMarking::setOmega(Place* p) {
	if (!p) return false; // check if a place is given
	map<Place*,int>::iterator mit(lb.find(p)); // check if a lower bound is set
	if (mit!=lb.end()) lb.erase(mit); // if so, delete it
	return setUnlimited(p); // and make it an open interval
}

/** Removes the upper bound for the number of tokens on a place.
	@param p The place.
	@return If this worked.
*/
bool ExtMarking::setUnlimited(Place* p) {
	if (!p) return false; // check if a place is given
	map<Place*,int>::iterator mit(ub.find(p)); // check if an upper bound is set
	if (mit!=ub.end()) ub.erase(mit); // if so, delete it
	type[p]=true; // and set the type to "open interval"
	return true;
}

/** Sets the token number on a place to an interval.
	@param p The place.
	@param low The lower bound of the interval.
	@param high The upper bound of the interval.
	@return If this worked.
*/
bool ExtMarking::setInterval(Place* p, int low, int high) {
	if (!p || low<0 || high<low) return false; // check if the parameters are ok
	type[p]=false; // set the type to "closed interval"
	lb[p]=low; // set the lower bound
	ub[p]=high; // set the higher bound
	if (low==0) lb.erase(lb.find(p)); // if the lower bound is zero, delete it (it's the default)
	return true;
}

/** Set the number of tokens on a place to a natural number.
	@param p The place.
	@param tokens The number of tokens.
	@return If this worked.
*/
bool ExtMarking::setNumber(Place* p, int tokens) {
	if (!p || tokens<0) return false; // check if the parameters are ok
	map<Place*,int>::iterator mit(ub.find(p)); // check if an upper bound is set
	if (mit!=ub.end()) ub.erase(mit); // if so, delete it
	if (tokens>0) lb[p]=tokens; // set the value unless it is zero
	else { mit = lb.find(p); if (mit!=lb.end()) lb.erase(mit); } // in which case it's deleted (default)
	map<Place*,bool>::iterator bit(type.find(p)); // check if the type is set
	if (bit!=type.end()) type.erase(bit); // if so, delete it (no type = single value)
	return true;
}

/** Check whether an extended marking is allowed in a regular coverability graph, i.e.
	if it contains no intervals and all unlimited places have a lower bound of zero.
	@return If this is a regular extended marking.
*/
bool ExtMarking::isRegular() {
	if (!ub.empty()) return false; // if there are upper bounds anywhere, it's not regular
	map<Place*,int>::iterator hit;
	map<Place*,bool>::iterator bit;
	for(bit=type.begin(); bit!=type.end(); ++bit) { // check the type of all places
		if (!bit->second) return false; // a closed interval is not allowed (this should not happen!)
		else if (bit->first) if (lb.find(bit->first)!=lb.end()) return false;
		// for open intervals a lower bound greater than zero is not allowed
	}
	return true; // all places checked
}

/** Check whether this extended marking covers em, but only if both are regular.
	@param em The extended marking to check this one against.
	@return If this marking covers em.
*/
bool ExtMarking::isCovering(ExtMarking& em) {
	if (!em.isRegular()) return false; // stop the check ...
	if (!isRegular()) return false; // ... if one of the markings is not regular
	map<Place*,bool>::iterator bit; // go through all omegas of the second marking
	for(bit=em.type.begin(); bit!=em.type.end(); ++bit)
		if (type.find(bit->first)==type.end()) return false; // if this marking has a single value, it doesn't cover
	map<Place*,int>::iterator mit; // now go through all single values (greater than zero) of the second marking
	for(mit=em.lb.begin(); mit!=em.lb.end(); ++mit) // check for each place:
		if (type.find(mit->first)!=type.end()) continue; // if this marking has an omega, we might cover
		else if (lb.find(mit->first)==lb.end()) return false; // if this marking is zero, we don't cover
		else if (mit->second>lb[mit->first]) return false; // if this marking is lower, we don't cover
	return true; // we came through for all places, so this marking covers the second marking
}

/** Introduce omegas for all places where the token number is increased compared to the
	parameter em. No check is done whether em is covered.
	@param em The regular extended marking to check this one against.
	@return The set of places where omegas have newly been introduced.
*/
set<Place*> ExtMarking::setOmega(ExtMarking& em) {
	set<Place*> result; // the set of places where omegas are newly introduced.
	if (!em.isRegular() || !isRegular()) return result; // both markings must be regular
	map<Place*,int>::iterator mit;
	for(mit=lb.begin(); mit!=lb.end(); ++mit) // check all lower bounds (greater than zero) in this marking 
	{
		if (em.type.find(mit->first)!=em.type.end()) continue; // em has an omega here, so no increment
		if (em.lb.find(mit->first)==em.lb.end()) result.insert(mit->first); // if em is zero here, we are greater
		else if (em.lb[mit->first]<mit->second) result.insert(mit->first); // we also may be explicitly greater
	}
	set<Place*>::iterator pit; // now go through all the places where we cover em
	for(pit=result.begin(); pit!=result.end(); ++pit)
		setOmega(*pit); // and set them to omega
	return result;
}

/** Compute the result of firing a transition. No check is done whether the transition is enabled,
	but negative token numbers will be replaced by zero if they occur.
	@param t The transition to be fired.
	@param reg If the extended marking should remain regular, i.e. 0+ (omega) is the only range allowed.
	@param im The incidence matrix of the Petri net.
*/
void ExtMarking::successor(Transition& t, bool reg, IMatrix& im) {
	map<Place*,int>& change(im.getColumn(t)); // the token effect of firing t
	map<Place*,int>::iterator mit;
	for(mit=change.begin(); mit!=change.end(); ++mit) // check all places with non-zero effect
	{
		if (reg && type.find(mit->first)!=type.end()) // for regular extmarkings
			if (type[mit->first]) continue; // omega remains omega whatever t does
		lb[mit->first] += mit->second; // add the effect to the lower bound
		if (lb[mit->first]<=0) lb.erase(lb.find(mit->first)); // and take care if it becomes zero (or less)
		if (ub.find(mit->first)!=ub.end()) // if the upper bound exists
		{
			ub[mit->first] += mit->second; // add the effect
			if (ub[mit->first]<=0) setNumber(mit->first,0); // and if the bound becomes zero or less, the interval becomes a single value
		}
	}
}

/** Compute the result of reverse firing a transition. No check is done,
	but negative token numbers will be replaced by zero if they occur.
	@param t The transition to be reverse fired.
	@param reg If the extended marking should remain regular, i.e. 0+ (omega) is the only range allowed.
	@param im The incidence matrix of the Petri net.
*/
void ExtMarking::predecessor(Transition& t, bool reg, IMatrix& im) {
	map<Place*,int>& change(im.getColumn(t)); // get the effect of firing t
	map<Place*,int>::iterator mit;
	for(mit=change.begin(); mit!=change.end(); ++mit) // go through all places with non-zero effect
	{
		if (reg && type.find(mit->first)!=type.end()) // for regular extmarkings
			if (type[mit->first]) continue; // omega must remain omega always
		lb[mit->first] -= mit->second; // otherwise add the effect of t
		if (lb[mit->first]<=0) lb.erase(lb.find(mit->first)); // and set the lower bound to zero if it becomes zero or less
		if (ub.find(mit->first)!=ub.end()) // if an upper bound exists
		{
			ub[mit->first] -= mit->second; // add the effect as well
			if (ub[mit->first]<=0) setNumber(mit->first,0); // and make it a single value zero if it reaches zero or goes below
		}
	}
}

/** Check whether a transition is enabled under all markings contained in an extended marking.
	@param t The transition.
	@param im The incidence matrix of the Petri net.
	@return If t is enabled under all contained markings.
*/
bool ExtMarking::isEnabled(Transition& t, IMatrix& im) {
	map<Place*,int> pre(im.getPreset(t)); // obtain the weighted preset of t
	map<Place*,int>::iterator mit;
	for(mit=pre.begin(); mit!=pre.end(); ++mit) // go through all places in the preset of t
	{
		if (lb.find(mit->first)==lb.end()) return false; // if there could be no tokens, t can't fire
		if (lb[mit->first]<mit->second) return false; // if there could be too few, t can't fire
	}
	return true; // all places checked, t can fire
}

/** Check whether a transition is disabled under all markings contained in an extended marking.
	@param t The transition.
	@param im The incidence matrix of the Petri net.
	@return If t is disabled under all contained markings.
*/
bool ExtMarking::isDisabled(Transition& t, IMatrix& im) {
	map<Place*,int> pre(im.getPreset(t)); // get the weighted preset of t
	map<Place*,int>::iterator mit;
	for(mit=pre.begin(); mit!=pre.end(); ++mit) // check all places of the preset
	{
		if (type.find(mit->first)==type.end()) // if the marking has a single value here
		{
			if (lb.find(mit->first)==lb.end()) return true; // and it is zero, t is disabled always
			if (lb[mit->first]<mit->second) return true; // same holds if there are too few tokens
			continue; // otherwise this place has sufficient tokens, so no decision is made so far
		}
		if (type[mit->first]) continue; // if we have an open interval, there are enough tokens on this place, no decision made
		if (ub[mit->first]<mit->second) return true; // if there is an upper bound with too few tokens, t is diabled always
	}
	return false; // all checks passed, t is not always disabled
}

/** Splits a portion from this extended marking where the number of tokens on a place p
	is guaranteed to be lower than a given number num. This extended marking is 
	changed (not containing the split away portion anymore) unless the procedure fails. 
	@param p A place that may or may not contain at least num tokens.
		Both cases must be possible in this extended marking, i.e. the number of tokens
		must lie between the lower and the upper bound (the latter being omega for the open interval).
	@param num The minimal number of tokens on place p for a marking to remain in this ExtMarking. 
	@return The split away portion of this extended marking in case of success. Otherwise,
		an invalid (possibly zero or this) extended marking is returned.
*/
ExtMarking ExtMarking::split(Place& p, int num) {
	int low; // the lower bound for p
	if (lb.find(&p)==lb.end()) low=0; // the default case
	else low = lb[&p]; // the explicit case
	if (num<=low) { ExtMarking em; return em; } // all markings remain here
	ExtMarking em(*this); // make a copy that can be changed
	if (type.find(&p)==type.end()) return em; // single value for p, so no split is possible
	if (type[&p]) // if we have an open interval for p
	{
		lb[&p] = num; // it now begins at num
		if (num==low+1) em.setNumber(&p,low); // while the copy gets the lower numbers, possibly just a single value
		else em.setInterval(&p,low,num-1);	// or a closed interval
	} else { // if we have a closed interval
		if (num>ub[&p]) return em; // all markings belong to the split away part
		lb[&p] = num; // if we can split, this marking gets the interval from num upwards
		if (num==ub[&p]) setNumber(&p,num); // which is possibly a single value
		em.ub[&p] = num-1; // and the split part gets the lower numbers
		if (num==low+1) em.setNumber(&p,low); // which also could be a single value
	}
	return em; // the split was done, return the two extended markings (this and the return value)
}

/** Find a place that may or may not enable a transition t depending on the number of tokens
	in this extended marking. If the transition t is either always enabled or always disabled,
	no such place will be found.
	@param t A transition.
	@param im The incidence matrix of the Petri net.
	@return A place that can but need not have enough tokens to enable t in this extended marking,
		if such a place exists. Otherwise returns NULL.
*/
Place* ExtMarking::findSplitPlace(Transition& t, IMatrix& im) {
	map<Place*,int> pre(im.getPreset(t)); // obtain the weighted preset of t
	map<Place*,int>::iterator mit;
	for(mit=pre.begin(); mit!=pre.end(); ++mit) // and check all its places p (with weight >0)
	{
		if (type.find(mit->first)==type.end()) continue; // p can only have a single number of tokens, so no split possible at p
		int low; // the lower bound for p
		if (lb.find(mit->first)==lb.end()) low=0; // in the default case
		else low = lb[mit->first]; // and in the explicit case
		if (mit->second<=low) continue; // if there are always enough tokens, we cannot split at p
		if (type[mit->first]) return mit->first; // in case of an open interval, we can split at p
		if (ub[mit->first]>=mit->second) return mit->first; // if p lies within the closed interval, we can also split
	}
	return NULL; // no place for splitting found
}

/** Compute all transitions firable under some marking contained in this extended marking.
	@param pn The Petri net.
	@param im The incidence matrix of the Petri net.
	@return The set of all firable transitions (under at least one possible marking).
*/
set<Transition*> ExtMarking::firableTransitions(PetriNet& pn, IMatrix& im) {
	set<Transition*> tset(pn.getTransitions()); // get all transitions of the net
	set<Transition*>::iterator tit(tset.begin()); // and walk through them
	while (tit!=tset.end()) // till the end
	{
		if (isDisabled(**tit,im)) tset.erase(tit++); // if a transition is always disabled, delete it
		else ++tit;
	}
	return tset; // so only those transition remain that can be enabled
}

/** Comparison operator for regular extended markings, does not realize covering.
	@param right Right hand side of the expression.
	@return If this is less than right.
*/
bool ExtMarking::operator<(const ExtMarking& right) const {
	map<Place*,int>::const_iterator lit1; // iterators to the lower bound maps
	map<Place*,int>::const_iterator lit2;
	map<Place*,bool>::const_iterator bit1; // and the type maps of the two extmarkings
	map<Place*,bool>::const_iterator bit2;
	bit1 = type.begin();
	lit1 = lb.begin();
	bit2 = right.type.begin();
	lit2 = right.lb.begin();
	Place* p; // the next place to check
	
	while (true) {
		p = NULL; // the following lines find the earliest (remaining) place appearing in any of the four maps
		if (bit1!=type.end()) p=bit1->first;
		if (lit1!=lb.end()) if (!p || p>lit1->first) p=lit1->first;
		if (bit2!=right.type.end()) if (!p || p>bit2->first) p=bit2->first;
		if (lit2!=right.lb.end()) if (!p || p>lit2->first) p=lit2->first;
		if (!p) break; // if there is no such place, we are done

		if (right.type.find(p)==right.type.end()) { // if p of right has a single value
			if (type.find(p)!=type.end()) return false; // and p of left is omega, left is not <
			int l1,l2; // get the lower bounds for p in left and right
			if (lb.find(p)==lb.end()) l1=0; else l1=lb.find(p)->second;
			if (right.lb.find(p)==right.lb.end()) l2=0; else l2=right.lb.find(p)->second;
			if (l2<l1) return false; // if they are different, we can order the extmarkings now
			if (l1<l2) return true; // (no matter what other places might say!)
		} else if (type.find(p)==type.end()) return true; // if right is omega, left not, left is <

		if (p==bit1->first) ++bit1; // for those iterators that pointed to p, find the next place
		if (p==lit1->first) ++lit1;
		if (p==bit2->first) ++bit2;
		if (p==lit2->first) ++lit2;
	}
	return false; // both extmarkings were identical
}

/** Comparison operator for regular extended markings in set comparisons.
	@param right Right hand side of the expression.
	@return If both extended markings are equal.
*/
bool ExtMarking::operator==(const ExtMarking& right) const {
	if (type!=right.type) return false; // just check if every parameter is the same ...
	if (lb!=right.lb) return false;
	if (ub!=right.ub) return false;
	return true;
}

/** Check if there are markings in this extended marking where a place p can have less
	than a given number of tokens as well as at least as many.
	@param p The place.
	@param token The number of tokens.
	@return If the extended marking could be split into two non-empty subsets by the condition.
*/
bool ExtMarking::splitExists(Place& p, int token) {
	if (token==0) return false; // if the number is zero, there are no markings with less tokens on p
	if (type.find(&p)==type.end()) return false; // if the marking dictates a single value, no split is possible
	if (lb.find(&p)!=lb.end()) // if the lower bound is not zero
		if (lb[&p]>=token) return false; // check if it is greater than the number: no split possible
	if (type[&p]) return true; // for open intervals a split is now possible
	return (ub[&p]>=token); // for closed intervals the number must be inside the interval
}

/** Check if there are markings in this extended marking where a place p can have less
	than a given number of tokens.
	@param p The place.
	@param token The number of tokens.
	@return If the extended marking can have less than token tokens on place p.
*/
bool ExtMarking::splitLowerExists(Place& p, int token) {
	if (token==0) return false; // less than zero is not possible
	if (lb.find(&p)!=lb.end()) // if the lower bound exists
		if (lb[&p]>=token) return false; // it must be less than the number
	return true; // we can have fewer than the number of tokens on p
}

/** Check if two extended marking contain a common marking.
	@param m The extended marking to intersect this one with.
	@return Whether there is a marking in the intersection of the two extended markings.
*/
bool ExtMarking::hasIntersectionWith(const ExtMarking& m) const {
	map<Place*,int>::const_iterator lit1; // iterators for the lower bounds
	map<Place*,int>::const_iterator lit2;
	map<Place*,bool>::const_iterator bit1; // and the types of the two extmarkings
	map<Place*,bool>::const_iterator bit2;
	bit1 = type.begin();
	lit1 = lb.begin();
	bit2 = m.type.begin();
	lit2 = m.lb.begin();
	Place* p; // the next place to check
	
	while (true) {
		p = NULL; // find the earliest next place in the four iterators
		if (bit1!=type.end()) p=bit1->first;
		if (lit1!=lb.end()) if (!p || p>lit1->first) p=lit1->first;
		if (bit2!=m.type.end()) if (!p || p>bit2->first) p=bit2->first;
		if (lit2!=m.lb.end()) if (!p || p>lit2->first) p=lit2->first;
		if (!p) break; // if there is none, we are through all iterators

		int l1,l2; // the lower bounds for p in both extmarkings
		if (lb.find(p)==lb.end()) l1=0; else l1=lb.find(p)->second; // the lower bound for the left extmarking
		if (m.lb.find(p)==m.lb.end()) l2=0; else l2=m.lb.find(p)->second; // and for the right extmarking
		if (m.type.find(p)==m.type.end()) { // if the right is a single value
			if (type.find(p)==type.end()) { if (l1!=l2) return false; } // it must either different from left
			else if (type.find(p)->second) { if (l1>l2) return false; } // or below left's open interval
			else if (l1>l2 || l2>ub.find(p)->second) return false; // or outside left's closed interval to deny an intersection
		} else if (m.type.find(p)->second) { // if right has an open interval
			if (type.find(p)==type.end()) { if (l2>l1) return false; } // either left's single value must be below that
			else if (!type.find(p)->second) { if (ub.find(p)->second<l2) return false; } // or left's closed interval must be below that to deny an intersection
		} else { // now right has a closed interval, so to deny an intersection
			if (type.find(p)==type.end()) { if (l2>l1 || l1>m.ub.find(p)->second) return false; } // left could be a single value outside
			else if (type.find(p)->second) { if (m.ub.find(p)->second<l1) return false; } // or an open interval above
			else if (l1>m.ub.find(p)->second || l2>ub.find(p)->second) return false; // or a closed interval fully below or above right's
		}

		if (p==bit1->first) ++bit1; // increment those iterators that were pointing
		if (p==lit1->first) ++lit1; // at the place we just worked on
		if (p==bit2->first) ++bit2;
		if (p==lit2->first) ++lit2;
	}
	return true; // the extmarking did not fully exclude each other for any place, so they intersect
}

/** Get the lower bound for the token number on a place p.
	@param p The place.
	@return The lower bound.
*/
int ExtMarking::getLowerBound(Place& p) {
	if (lb.find(&p)==lb.end()) return 0; // the default value
	return lb[&p];
}

/** Get the upper bound (or the single value) for the token number on a place p.
	@param p The place.
	@return The upper bound, or -1 if there is no upper bound.
*/
int ExtMarking::getUpperBound(Place& p) {
	if (type.find(&p)==type.end()) return getLowerBound(p);
	if (type[&p]) return -1;
	return ub[&p];
}

/** Check if the given extended marking is omega for the place p.
	@param p The place to check for omega.
	@return If the place p is marked omega.
*/
bool ExtMarking::isOmega(Place& p) {
	if (lb.find(&p)!=lb.end()) return false;
	if (type.find(&p)==type.end()) return false;
	return type[&p];
}

/** Check if the given extended marking has an open interval for the place p.
	@param p The place to check for unlimited tokens.
	@return If the place p can have an unlimited number of tokens.
*/
bool ExtMarking::isOpen(Place& p) {
	if (type.find(&p)==type.end()) return false;
	return type[&p];
}

/** Check whether a given marking is contained in this extmarking. If not, find a place
	proving that.
	@param m The marking that could be contained.
	@return NULL if the marking is contained, a place where the marking can be distinguished
		from this extmarking otherwise.
*/
Place* ExtMarking::distinguish(Marking& m) {
	map<const Place*,unsigned int>::const_iterator mit;
	for(mit=m.begin(); mit!=m.end(); ++mit) // visit every place where the marking is not zero (perhaps even some zeros)
	{
		Place* p(const_cast<Place*>(mit->first)); // get such a place
		if (type.find(p)==type.end()) // if this has a single value entry
		{
			if (lb.find(p)==lb.end()) { if (mit->second==0) continue; } // check if both markings are zero here, if so, look for another place
			else if (mit->second == (unsigned int)(lb[p])) continue; // otherwise check if they have the same value, then look for another place
		} 
		else if (type[p]) // in case this has an open interval
		{
			if (lb.find(p)==lb.end()) continue; // if it begins at zero, m might be contained
			else if (mit->second >= (unsigned int)(lb[p])) continue; // if m has more tokens than the lower bound, it may be contained
		}
		else // finally check for closed intervals of this, if m(p) lies within the interval
		{
			if (lb.find(p)==lb.end()) { if (mit->second<=(unsigned int)(ub[p])) continue; } // it's in [0,ub[p]]
			else if (mit->second>=(unsigned int)(lb[p]) && mit->second<=(unsigned int)(ub[p])) continue; // it's in [lb[p],ub[p]]
		}
		return p; // m could be distinguished from this at place p, so return it
	}
	map<Place*,int>::iterator lbit; // we still need to check those places p that did not appear in
	for(lbit=lb.begin(); lbit!=lb.end(); ++lbit) // m (m[p]=0), but where the lower bound of this is >0
	{
		if (m.getMap().find(lbit->first)==m.getMap().end()) return lbit->first; // m[p] does not exist, so we haven't checked is before
	}
	return NULL; // m is contained in this
}

/** Check if this extmarking has certainly less tokens on a place p than a marking m.
	@param m The marking to check against.
	@param p The place to check.
	@return If this extmarking is certainly smaller on p than m
*/
bool ExtMarking::lessThanOn(Marking& m, Place& p) {
	if (m.getMap().find(&p)==m.getMap().end()) return false; // m[p]=0
	if (type.find(&p)==type.end()) // if this has a single value for p
	{
		if (lb.find(&p)==lb.end()) { if (m[p]>0) return true; } // and it is zero, just check m[p]>0
		else if (m[p]>(unsigned int)(lb[&p])) return true; // otherwise check if the value is lower than m[p]
	} else if (!type[&p] && m[p]>(unsigned int)(ub[&p])) return true; // for closed intervals, check the upper bound
	return false; // this isn't smaller (happen especially if this has an open interval for p)
}

/** Compute the distance from this extmarking's t-successor to a given marking.
	Only single value entries in this extmarking are used to compute the distance,
	omegas and intervals are neglected.
	@param t The transition to fire from this extmarking.
	@param m The marking to compare to.
	@param im The incidence matrix of the Petri net.
	@return The distance (sum of differences).
*/
int ExtMarking::distanceTo(Transition& t, Marking& m, IMatrix& im) {
	ExtMarking copy(*this); // make a copy of this that can be changed
	copy.successor(t,true,im); // fire t
	return copy.distanceTo(m); // and compute the distance
}

/** Compute the distance from this extmarking to a given marking.
	Only single value entries in this extmarking are used to compute the distance,
	omegas and intervals are neglected.
	@param m The marking to compare to.
	@return The distance (sum of differences).
*/
int ExtMarking::distanceTo(Marking& m) {
	int result(0); // default (minimal) distance is zero
	map<const Place*,unsigned int>::const_iterator mit;
	for(mit=m.begin(); mit!=m.end(); ++mit) // go through all places explicitly declared in the marking m
	{
		Place* p(const_cast<Place*>(mit->first)); // get such a place
		if (type.find(p)!=type.end()) continue; // if this has an interval here, don't do anything
		if (lb.find(p)==lb.end()) result += mit->second; // if this is zero here, add m[p] to the distance
		else if (mit->second>(unsigned int)(lb[p])) result += ((int)(mit->second)-lb[p]); // otherwise add the difference between the markings
		else result -= ((int)(mit->second)-lb[p]); // resp. subtract it if it's negative
	}
	map<Place*,int>::iterator emit;
	for(emit=lb.begin(); emit!=lb.end(); ++emit) // now check if we missed some default-zeroes of m
	{
		if (m.getMap().find(emit->first)!=m.getMap().end()) continue; // if m[p] is explicitly given, we have handled it above
		if (type.find(emit->first)!=type.end()) continue; // we don't care about intervals
		result += emit->second; // our lower bound is the difference as m[p]=0, so add it to the distance
	}
	return result;
}

