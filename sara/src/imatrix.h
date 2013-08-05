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
 * \date    $Date: 2012-06-13 12:00:00 +0200 (Mi, 13. Jun 2012) $
 *
 * \version $Revision: 1.10 $
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

namespace sara {

struct SThread;

/*! \brief For dynamically computing and storing incidence and loop matrices of a Petri net
	and handling the firing of transitions
	
	Class for the incidence matrix of a Petri net. The incidence matrix is partially built only 
	on demand, one map (from places to int) per transition. Additionally, loops are accounted for,
	so IMatrix can contain the full structure information of the Petri net.
*/
class IMatrix {

public:
	/// Constructor. Builds an empty incidence matrix.
	IMatrix(PetriNet& pn);

	// Compute all necessary tables and values.
	void precompute();

	/// Get the column of the incidence matrix regarding a transition t.
	map<Place*,int>& getColumn(Transition& t);

	/// Get the postset (with weights) of a transition t
	map<Place*,int>& getPostset(Transition& t);

	/// Get the postset (with weights) of a place p
	map<Transition*,int>& getPostset(Place& p);

	/// Get the preset (with weights) of a transition t
	map<Place*,int>& getPreset(Transition& t);

	/// Get the preset (with weights) of a place p
	map<Transition*,int>& getPreset(Place& p);

	/// Get one entry in the incidence matrix.
	int getEntry(Transition& t, Place& p);

	/// Get the number of loops between a place and a transition.
	int getLoops(Transition& t, Place& p);

	/// Get the number of arcs from t to p.
	int getWeight(Transition& t, Place& p);

	/// Get the number of arcs from p to t.
	int getWeight(Place& p, Transition& t);

	/// Get the number of loops between a transition and all its neighboring places.
//	map<Place*,int>& getLoopColumn(Transition& t);

	/// Calculate the predecessor marking (before t has been fired).
	void predecessor(Marking& m, Transition& t);

	/// Calculate the successor marking (after firing t).
	void successor(Marking& m, Transition& t);

	/// Compare the net result of two transitions and deliver all places where the first transition yields less tokens than the second.
	vector<Place*>& compareOutput(SThread* thr, Transition& t1, Transition& t2);

	/// Check if a transition vector can fire under a marking regarding a certain set of places only.
	bool checkRestrictedActivation(Marking& m, vector<Transition*>& tv, vector<Place*>& restriction);

	/// Calculates the token change induced by firing a transition vector
	map<Place*,int> getChange(map<Transition*,int>& fv);

	/// Get the size of the largest preset of a place.
	unsigned int pMaxPreset();

	/// Get the size of the largest postset of a place.
//	unsigned int pMaxPostset();

	/// Get the size of the largest preset of a transition.
//	unsigned int tMaxPreset();

	/// Get the size of the largest postset of a transition.
//	unsigned int tMaxPostset();

	/// Get an overapproximated number of conflicting transitions for any transition.
//	unsigned int maxConflict();

	/// Get the number of places in the net.
	unsigned int numPlaces();

	/// Get the number of transitions in the net.
	unsigned int numTransitions();

	/// Verbosity level.
	int verbose;

private:
	/// The Petri net for this incidence matrix
	const PetriNet& petrinet;

	/// The incidence matrix
	map<Transition*,map<Place*,int> > mat;

	/// The matrix containing the loops (all entries non-negative)
	map<Transition*,map<Place*,int> > loop;

	/// Presets of transitions
	map<Transition*,map<Place*,int> > pre;

	/// Postsets of transitions
	map<Transition*,map<Place*,int> > post;

	/// Presets of places
	map<Place*,map<Transition*,int> > ppre;

	/// Postsets of places
	map<Place*,map<Transition*,int> > ppost;

	/// Maximal preset size for places
	unsigned int pmaxpreset;

	/// Maximal postset size for places
	unsigned int pmaxpostset;

	/// Maximal preset size for transitions
	unsigned int tmaxpreset;

	/// Maximal postset size for transitions
	unsigned int tmaxpostset;

	/// Maximal conflict set for transitions (overapproximated)
	unsigned int maxconflict;

	/// Number of places in the net
	unsigned int numplaces;

	/// Number of transitions in the net
	unsigned int numtransitions;
};

}

#endif

