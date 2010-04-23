// -*- C++ -*-

/*!
 * \file    imatrix.h
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

#ifndef IMATRIX_H
#define IMATRIX_H

#include <set>
#include <map>
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif

using pnapi::PetriNet;
using pnapi::Transition;
using pnapi::Place;
using pnapi::Marking;
using std::set;
using std::map;
using std::vector;

/** Class for the incidence matrix of a Petri net. The incidence matrix is partially built only 
	on demand, one map (from places to int) per transition. Additionally, loops are accounted for,
	so IMatrix can contain the full structure information of the Petri net.
*/
class IMatrix {

public:
	/// Constructor. Builds an empty incidence matrix.
	IMatrix(PetriNet& pn);

	/// Get the column of the incidence matrix regarding a transition t.
	map<Place*,int>& getColumn(Transition& t);

	/// Get the postset (with weights) of a transition t
	map<Place*,int> getPostset(Transition& t);

	/// Get the preset (with weights) of a transition t
	map<Place*,int> getPreset(Transition& t);

	/// Get one entry in the incidence matrix.
	int getEntry(Transition& t, Place& p);

	/// Get the number of loops between a place and a transition.
	int getLoops(Transition& t, Place& p);

	/// Get the number of loops between a transition and all its neighboring places.
	map<Place*,int>& getLoopColumn(Transition& t);

	/// Calculate the predecessor marking (before firing t).
	void predecessor(Marking& m, Transition& t);

	/// Calculate the successor marking (after firing t).
	void successor(Marking& m, Transition& t);

	/// Compare the net result of two transition and deliver all places where the first transition yields less tokens than the second.
	set<Place*> compareOutput(Transition& t1, Transition& t2);

	/// Check if a transition vector can fire under a marking regarding a certain set of places only.
	bool checkRestrictedActivation(Marking& m, vector<Transition*>& tv, set<Place*>& restriction);

	/// Calculates the token change induced by firing a transition vector fv
	map<Place*,int> getChange(map<Transition*,int>& fv);

	/// Verbosity level.
	int verbose;

private:
	/// The Petri net for this incidence matrix
	const PetriNet& petrinet;

	/// The matrices for incidence and loops
	map<Transition*,map<Place*,int> > mat,loop;
};

#endif
