// -*- C++ -*-

/*!
 * \file    extmarking.h
 *
 * \brief   Class ExtMarking
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/05/14
 *
 * \date    $Date: 2010-07-30 12:00:00 +0200 (Fr, 30. Jul 2010) $
 *
 * \version $Revision: 0.3 $
 */

#ifndef EXTMARKING_H
#define EXTMARKING_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include "pnapi/pnapi.h"
#include "imatrix.h"

using std::map;
using std::string;
using std::set;
using std::vector;
using std::ostream;
using pnapi::Place;
using pnapi::Transition;
using pnapi::PetriNet;
using pnapi::Marking;

/*! \brief Extended markings for a specialized coverability graph. 

	This class realizes extended markings where the value for a place p representing
	its token number can be of three different kinds:
	1) a single value, 2) an open interval with lower bound a, or 3) a closed interval [a,b].
	The three kinds are represented in a map<Place,bool>, where a non-existing entry shows
	a single value, true shows an open interval, and false a closed interval. Two additional
	maps contain the lower and upper bounds (the latter is set only for closed intervals).
	A non-existing entry for a lower bound shows the value zero (also for single values).

	Extended markings used in coverability graphs usually allow only single values and
	the open interval with lower bound zero, also denoted as omega. Extended markings
	restricted to these values are called regular here.
*/
class ExtMarking {
public:
	/// Standard constructor.
	ExtMarking();

	/// Constructor with marking.
	ExtMarking(const Marking& m);

	/// Destructor.
	~ExtMarking();

	/// Print extended marking to output stream with places in the given ordering
	void show(ostream& o, const vector<Place*>& porder);

	/// Set a token number to omega
	bool setOmega(Place* p);

	/// Remove the upper bound of a token number on a place
	bool setUnlimited(Place* p);

	/// Remove the upper bound of a token number on a set of places
	void setUnlimited(set<Place*> pset);

	/// Set the token number on a place to an interval.
	bool setInterval(Place* p, int low, int high);

	/// Set the token number on a place to a natural number.
	bool setNumber(Place* p, int tokens);

	/// Check if the extended marking is allowed in a regular coverability graph
	bool isRegular();

	/// Check if this marking covers the marking em (and both are regular)
	bool isCovering(ExtMarking& em);
	
	/// Set to omega those places (returned) where the token number is exceeded (compared to em)
	set<Place*> setOmega(ExtMarking& em);

	/// Calculate the successor extended marking (possibly regular) after firing a transition t
	void successor(Transition& t, bool reg, IMatrix& im);

	/// Calculate the predecessor extended marking (possibly regular) before firing a transition t
//	void predecessor(Transition& t, bool reg, IMatrix& im);

	/// Check whether a transition is enabled under all contained markings
//	bool isEnabled(Transition& t, IMatrix& im);

	/// Check whether a transition is disabled under all contained markings
	bool isDisabled(Transition& t, IMatrix& im);

	/// Split an extended marking at the token number a transition needs on a place
	ExtMarking split(Place& p, int num);

	/// Find a place that can enable or disable a transition t in this extended marking
	Place* findSplitPlace(Transition& t, IMatrix& im);

	/// Compute the firable transitions (under at least one contained marking)
	set<Transition*> firableTransitions(PetriNet& pn, IMatrix& im);

	/// Comparison operator for sets, this is not the intuitive "is smaller than" partial order
	bool operator<(const ExtMarking& right) const;

	/// Equality of extended markings 
//	bool operator==(const ExtMarking& right) const;

	/// Check if the extended marking can be split at place p with a given token number
	bool splitExists(Place& p, int token);

	/// Check if the extended marking can have less than token tokens on place p
	bool splitLowerExists(Place& p, int token);

	/// Check if two extended marking have a non-empty intersection (a common marking)
	bool hasIntersectionWith(const ExtMarking& m) const;

	/// Get the lower bound for the token number on a place
	int getLowerBound(Place& p);

	/// Get the upper bound for the token number on a place
	int getUpperBound(Place& p);

	/// Check if a place is set to omega
//	bool isOmega(Place& p);

	/// Check if the number of tokens on a place is set to an open interval
	bool isOpen(Place& p);

	/// Check if the number of tokens on a place is set to an open interval
	set<Place*> getOpenPlaces();

	/// find a place where the marking m does not conform with this extmarking
	Place* distinguish(Marking& m, bool cover);

	/// compare the token number on a place to a marking, return true if it's lower
	bool lessThanOn(Marking& m, Place& p);

	/// get the distance from this node's t-successor to the given marking
	int distanceTo(Transition& t, Marking& m, IMatrix& im);

	/// get the distance from this node to the given marking
	int distanceTo(Marking& m);

private:
	/// If a place is unlimited (true), has a closed interval (false), or a single value (not set)
	map<Place*,bool> type;

	/// The lower bound for the token numbers (zero if not set)
	map<Place*,int> lb;

	/// The upper bound for the token numbers (for closed intervals only)
	map<Place*,int> ub;
};

#endif

