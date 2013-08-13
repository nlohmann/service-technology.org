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

namespace sara {

class Constraint;

}

#include "lpwrapper.h"
#include "partialsolution.h"

using pnapi::Place;
using pnapi::Transition;
using std::set;
using std::map;
using std::ostream;

namespace sara {

/*! \brief for computing and storing additional constraints to the marking equation

	The class Constraint contains a single, additional constraint for the
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
	/// Standard constructor for increment constraints (place/token based)
	Constraint();

	/// Standard constructor for jump constraints or single-transition constraints
	Constraint(map<Transition*,int> jump, bool isjump);

	/// Destructor
	~Constraint();

	/// Adds the preset of a place p with factor weight to an increment constraint	
	void addPlace(Place& p, int weight);

	/// Returns the map from places to their weights.
	const map<Place*,int>& getPlaces() const;

	/// Forbids a transition to appear in an increment constraint
	void addSubTransition(Transition &t);

	/// Tests if a transition can be forbidden, i.e. if it appears with positive weight
	bool checkSubTransition(Transition &t);

	/// Tests if the increment constraint has a transition with positive weight at all
	bool checkAnyTransition() const;

	/// Returns the set of forbidden transitions in an increment constraint
	const set<Transition*>& getSubTransitions() const;

	/// Sets the right hand side of an increment constraint
	void setRHS(int rhs);

	/// Returns the right hand side of the constraint
	int getRHS() const;

	/// Calculates the weighted sum of transitions in the constraint for lp_solve
	bool calcConstraint(REAL coeff[], map<Transition*,int>& tpos, int cols) const;

	/// Calculates the weighted sum of transitions in an increment constraint
	map<Transition*,int>& calcConstraint();

	/// Gets the weighted sum of transitions if it has been calculated
	const map<Transition*,int>& getLHS() const;

	/// Delete the weighted sum (for recalculation)
	void clearLHS();

	/// Compares two constraints, 0:incomparable (different LHS), 1:RHS>c.RHS, -1:RHS<=c.RHS
	int compare(const Constraint& c) const;

	/// Prints a constraint
	void showConstraint(ostream& s) const;

	/// Whether the constraint is used for jumping to incomparable solutions
	bool isJump() const;

	/// Set the recent flag making a constraint recent (able to be reason for failures) or old
	void setRecent(bool r);

	/// Check the recent flag
	bool isRecent() const;

	/// Comparison method for set::less
	bool operator<(const Constraint& rhs) const;

	/// Equivalence of constraints
	bool operator==(const Constraint& rhs) const;

	/// Clean a set of failure constraints regarding this constraint
	bool cleanConstraintSet(set<Constraint>& sc) const;

	/// Whether a constraint is a non-jump with only one transition with weight one, i.e. t>=n
	Transition* isSingle() const;

private:
	/// Weighted places, their presets will form the increment constraint
	map<Place*,int> posplace;

	/// Forbidden transitions, will not appear even if in the preset of a place from posplace
	set<Transition*> subtrans;

	/// Right hand side of the constraint
	int rhs;

	/// Semantics of the constraint, calculated on demand (or by initialization in case of a jump)
	map<Transition*,int> cs;

	/// Whether the constraints has a <= (instead of >=), i.e. is a jump constraint
	bool jump;

	/// Whether the constraint is new and may be the reason for a failure (not finding solutions)
	bool recent;
};

}

#endif

