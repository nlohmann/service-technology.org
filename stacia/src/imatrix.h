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
 * \date    $Date: 2012-11-20 12:00:00 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.12 $
 */

#ifndef IMATRIX_H
#define IMATRIX_H

#include "pnapi/pnapi.h"
#include "types.h"

using pnapi::PetriNet;
using pnapi::Transition;
using pnapi::Place;
using std::string;

/*! \brief For computing and storing incidence and loop matrices of a Petri net
	and handling the firing of transitions
	
	Class for the incidence matrix of a Petri net. The incidence matrix consists of 
	one map (from places to int) per transition. Additionally, loops are accounted for,
	so IMatrix can contain the full structure information of the Petri net.
*/
class IMatrix {

public:
	/// Constructor. Builds an empty incidence matrix.
	IMatrix(PetriNet& pn);

	/// Copy-like Constructor for Place Splitting
	IMatrix(const IMatrix& im, vector<PlaceID>& split);

	/// Compute all necessary tables and values.
	void precompute();

	/// Get the underlying Petri net
	const PetriNet& getNet() const;

	/// Get the ID of a transition
	TransitionID tID(const Transition* t) const;

	/// Get the ID of a place
	PlaceID pID(const Place* p) const;

	/// Get the transition belonging to an ID
	Transition* tPTR(TransitionID t) const;

	/// Get the place belonging to an ID
	Place* pPTR(PlaceID p) const;

	/// Get the transition name belonging to an ID
	string tName(TransitionID t) const;

	/// Get the place name belonging to an ID
	string pName(PlaceID p) const;

	/// Get the column of the incidence matrix regarding a transition t.
	const TArcs& getColumn(TransitionID t) const;

	/// Get the postset (with weights) of a transition t
	const TArcs& getTPostset(TransitionID t) const;

	/// Get the postset (with weights) of a place p
	const PArcs& getPPostset(PlaceID p) const;

	/// Get the preset (with weights) of a transition t
	const TArcs& getTPreset(TransitionID t) const;

	/// Get the preset (with weights) of a place p
	const PArcs& getPPreset(PlaceID p) const;

	/// Get one entry in the incidence matrix.
	int getEntry(TransitionID t, PlaceID p) const;

	/// Get the number of loops between a place and a transition.
	int getLoops(TransitionID t, PlaceID p) const;

	/// Get the number of arcs from t to p.
	int getTPWeight(TransitionID t, PlaceID p) const;

	/// Get the number of arcs from p to t.
	int getPTWeight(PlaceID p, TransitionID t) const;

	/// Get the number of loops between a transition and all its neighboring places.
	const TArcs& getLoopColumn(TransitionID t) const;

	/// Calculate the predecessor marking (before t has been fired).
	void predecessor(Marking& m, TransitionID t) const;

	/// Calculate the successor marking (after firing t).
	void successor(Marking& m, TransitionID t) const;

	/// Compare the net result of two transitions and deliver all places where the first transition yields less tokens than the second.
	vector<PlaceID> compareOutput(TransitionID t1, TransitionID t2) const;

	/// Check if a transition vector can fire under a marking regarding a certain set of places only.
	bool checkRestrictedActivation(const Marking& m, const FiringSequence& tv, const vector<PlaceID>& restriction) const;

	/// Calculates the token change induced by firing a transition vector
	Marking getChange(const FiringVector& fv) const;

	/// Get the size of the largest preset of a place.
	unsigned int pMaxPreset() const;

	/// Get the size of the largest postset of a place.
	unsigned int pMaxPostset() const;

	/// Get the size of the largest preset of a transition.
	unsigned int tMaxPreset() const;

	/// Get the size of the largest postset of a transition.
	unsigned int tMaxPostset() const;

	/// Get an overapproximated number of conflicting transitions for any transition.
	unsigned int maxConflict() const;

	/// Get the number of places in the net.
	unsigned int numPlaces() const;

	/// Get the number of transitions in the net.
	unsigned int numTransitions() const;

	/// Obtain a logic formula by which containment of p in a siphon implies other places in the siphon
	const vector<set<PlaceID> >& getSiphonImplication(PlaceID p) const;

	/// Obtain a logic formula by which containment of p in a trap implies other places in the trap
	const vector<set<PlaceID> >& getTrapImplication(PlaceID p) const;

	/// Compute the necessary places to split such that the net decomposes into as many components as possible
	vector<PlaceID> placesToSplit(unsigned int size) const;

	/// Get all transitions in the same component as the given one.
	vector<TransitionID> componentTransitions(TransitionID t) const;

	/// Get the number of tokens on a place
	unsigned int marking(PlaceID p) const;

	/// Get all places marked initially
	vector<PlaceID> getMarked() const;

	/// Print a siphon (set of places)
	void printSiphon(set<PlaceID> s) const;

	/// Verbosity level.
	int verbose;

private:

	/// initialisation
	void precompute2();

	/// The Petri net for this incidence matrix
	PetriNet& petrinet;

	/// Converter from transitions to unsigned int (ID)
	map<const Transition*,TransitionID> tid;

	/// Converter from places to unsigned int (ID)
	map<const Place*,PlaceID> pid;

	/// Converter from ID to transition
	vector<Transition*> idt; // [TransitionID]

	/// Converter from ID to place
	vector<Place*> idp; // [PlaceID]

	/// Converter from ID to transition name
	vector<string> tname; // [TransitionID]

	/// Converter from ID to place name
	vector<string> pname; // [PlaceID]

	/// The incidence matrix
	vector<TArcs> mat; // [TransitionID]

	/// The matrix containing the loops (all entries non-negative)
	vector<TArcs> loop; // [TransitionID]

	/// Presets of transitions
	vector<TArcs> pre; // [TransitionID]

	/// Postsets of transitions
	vector<TArcs> post; // [TransitionID]

	/// Presets of places
	vector<PArcs> ppre; // [PlaceID]

	/// Postsets of places
	vector<PArcs> ppost; // [PlaceID]

	/// Tokens in the net
	vector<unsigned int> tokens; // [PlaceID]

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

	/// siphon implications per place
	vector<vector<set<PlaceID> > > simpl; // [PlaceID][unsigned int]

	/// trap implications per place
	vector<vector<set<PlaceID> > > timpl; // [PlaceID][unsigned int]
};

#endif

