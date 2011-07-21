// -*- C++ -*-

/*!
 * \file    inveqrel.h
 *
 * \brief   Class InvEqRel
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2011/07/13
 *
 * \date    $Date: 2011-07-13 14:47:01 +0200 (Mi, 13. Jul 2011) $
 *
 * \version $Revision: 1.0 $
 */

#ifndef INVEQREL_H
#define INVEQREL_H

#include <vector>
#include <map>
#include <set>

#include "pnapi/pnapi.h"

using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using std::vector;
using std::map;
using std::set;

/*! \brief This class maintains equivalence classes of the places of a net
		regarding membership in semipositive place invariants.
*/
class InvEqRel {

public:
	/// Standard constructor
	InvEqRel(PetriNet& pn);

	/// Destructor.
	~InvEqRel();

	/// Two places to be in the same equivalence class
	bool join(Place* p1, Place* p2);

	/// Two places to be in the same equivalence class, successful joins are counted
	bool pjoin(Place* p1, Place* p2);

	/// Coarsen the relation according to an invariant distinguishing two sets of non-equivalent places
	void split(map<Place*,int> inv);

	/// Get the classes as they are now, approx=true: relation may be coarser, otherwise finer than equivalence
	vector<set<Place*> > getClasses(bool approx);

	/// Check if an equivalence relation has been reached, if not, return two places for which the relation is still unknown
	bool getUndecided(Place*& p1, Place*& p2);

	/// Precompute some equivalent places from the structure of the net
	void simpleEquivalences();

	/// Find the representative of the class containing the place in ``below''
	Place* findClass(Place* p);

	/// Find the ID of the class containing the place in ``below''
	int findClassNum(Place* p);

	/// Get the number of joins in the preparation phase
	unsigned int preJoinsDone();

private:
	/// to find a place in the coarser relation ``above''
	map<Place*,unsigned int> toclass;

	/// a relation coarser than the equivalence
	vector<set<Place*> > above;

	/// a relation finer than the equivalence
	map<Place*,set<Place*> > below;

	/// first active entry in above
	unsigned int start;

	/// The Petri net
	PetriNet& net;

	/// data structure for union-find in `below'
	vector<int> unionfind;

	/// mapper from int to places, for union-find
	vector<Place*> pvec;

	/// mapper from places to int, for union-find
	map<Place*,int> pmap;

	/// number of joins done in the preparation phase
	unsigned int prejoin;
};

#endif

