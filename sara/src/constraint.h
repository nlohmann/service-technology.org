// -*- C++ -*-

/*!
 * \file    constraint.h
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

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <set>
#include <map>
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif

class Constraint;

#include "lpwrapper.h"
#include "partialsolution.h"

using pnapi::Place;
using pnapi::Transition;
using std::set;
using std::map;
using std::ostream;

/** The class Constraint contains a single, additional constraint for the
	marking equation. A constraint consists of weighted transitions
	(the weight representing production of tokens minus their consumption
	on a set of places). Transitions can only be added to a constraint
	as a member of a preset of a place that needs additional tokens.
	If a transition is known not to be able to fire in the context of
	the constraint, it can explicitly be forbidden, to not take part in the
	constraint. Places (resp. their preset) can be added with a weight 
	of more than one to reflect a weighted sum of places, i.e. a token
	on one place has more weight than a token on another place.
	The constraint demands that the weighted sum of its transition
	firings should be greater or equal to some right hand side (RHS),
	which can be set independently.

	Alternatively, a constraint can also be defined directly as a weighted
	sum of transitions that must be less or equal to a certain value.
*/  
class Constraint {

public:
	/// standard constructor for the normal case
	Constraint();

	/// standard constructor for the less-or-equal variant
	Constraint(map<Transition*,int> jump);

	/// destructor
	~Constraint();

	/// adds the preset of a place p with factor weight to the constraint	
	void addPlace(Place& p, int weight);
/*
	/// sets the weight of place p to zero (the preset of p may still be influenced by other places)
	bool removePlace(Place &p);
*/
	/// returns the map from places to their weights.
	const map<Place*,int>& getPlaces() const;

	/// forbids a transition to appear in the constraint
	void addSubTransition(Transition &t);

	/// tests if a transition can be forbidden, i.e. if it appears with positive weight
	bool checkSubTransition(Transition &t);

	/// tests if the constraint has a transition with positive weight at all
	bool checkAnyTransition() const;
/*
	/// deletes a Transition from the forbidden set
	bool removeSubTransition(Transition& t);
*/
	/// return the set of forbidden transitions
	const set<Transition*>& getSubTransitions() const;

	/// sets the right hand side of the constraint
	void setRHS(int rhs);

	/// returns the right hand side of the constraint
	int getRHS() const;

	/// calculates the weighted sum of transitions in the constraint for lp_solve
	bool calcConstraint(REAL coeff[], map<Transition*,int>& tpos, int cols) const;

	/// calculates the weighted sum of transitions in a constraint
	map<Transition*,int>& calcConstraint();

	/// gets the weighted sum of transitions if it has been calculated
	const map<Transition*,int>& getLHS() const;

	/// compares two constraints, 0:incomparable, 1:RHS>c.RHS, -1:RHS<=c.RHS
	int compare(const Constraint& c) const;

	/// prints a constraint
	void showConstraint(ostream& s) const;

	/// whether the constraint is used for jumping to incomparable solutions
	bool isJump() const;

	/// set the recent flag making a constraint recent (able to be reason for failures) or old
	void setRecent(bool r);

	/// check the recent flag
	bool isRecent() const;

	/// comparison for set::less
	bool operator<(const Constraint& rhs) const;

	/// equivalence for constraints
	bool operator==(const Constraint& rhs) const;

	/// clean a set of failure constraints regarding this constraint
	bool cleanConstraintSet(set<Constraint>& sc) const;

private:
	/// weighted places, their presets will form the constraint
	map<Place*,int> posplace;

	/// forbidden transitions, will not appear even if in the preset of a place from posplace
	set<Transition*> subtrans;

	/// right hand side of the constraint
	int rhs;

	/// semantics of the constraint, calculated on demand (or by initialization in case of a jump)
	map<Transition*,int> cs;

	/// whether the constraints has a <= (instead of >=), for jump constraints only
	bool jump;

	/// whether the constraint is new and may be the reason for a failure (not finding solutions)
	bool recent;
};

#endif
