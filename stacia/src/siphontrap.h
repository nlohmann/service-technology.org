
/*!
 * \file    siphontrap.h
 *
 * \brief   Class SiphonTrap
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/07/04
 *
 * \date    $Date: 2012-11-20 14:47:01 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */

#ifndef SIPHONTRAP_H
#define SIPHONTRAP_H

// include header files
#include <config.h>
#include <libgen.h>
#include <string>

#include "types.h"
#include "subnet.h"
#include "imatrix.h"
#include "config-log.h"
#include "verbose.h"

using std::string;

class SiphonTrap {

public:
	/// Constructor
	SiphonTrap(const SubNet& net);

	/// Compute siphons and traps by brute force
	void computeBruteForce(bool mt);

	/// Compute all elementary siphons starting at a given place
	void elementarySiphons(PlaceID p);

	/// Compute all elementary traps starting at a given place
	void elementaryTraps(PlaceID p);

	/// Get a siphon with given ID
	const Siphon& getSiphon(SiphonID ID) const;

	/// Get a trap with given ID
	const Trap& getTrap(TrapID ID) const;

	/// Get the list of all elementary siphons starting at a given place
	const SiphonList& getSiphonIDs(PlaceID p) const;

	/// Get the list of all elementary traps starting at a given place
	const TrapList& getTrapIDs(PlaceID p) const;

	/// Get all elementary siphons with a given interface
	const SiphonList& getSiphonIDs(Interface& interface) const;

	/// Get all interface-elementary traps with a given interface
	const TrapList& getIETrapIDs(Interface& interface) const;

	/// Get all token-elementary traps with a given interface
	const TrapList& getTETrapIDs(Interface& interface) const;

	/// Get the number of all elementary siphons
	unsigned int numSiphons() const;

	/// Get the number of all elementary traps
	unsigned int numTraps() const;

	/// Print all elementary siphons originating at a given place
	void printElementarySiphons(PlaceID p) const;

	/// Print all elementary traps originating at a given place
	void printElementaryTraps(PlaceID p) const;

	/// Print a siphon or trap given as a vector of place IDs
	void printSiphonTrap(const vector<PlaceID>& set1) const;

	/// Print a siphon given by its ID
	void printSiphon(SiphonID ID) const;

	/// Print a trap given by its ID
	void printTrap(TrapID ID) const;

	/// Check if a trap is marked
	bool isMarked(TrapID trapID) const;

private:
	/// Compute all elementary siphons
	void elementarySiphons();

	/// Compute all elementary traps
	void elementaryTraps(bool mt);

	/// Compute elementary siphons/traps recursively, starting at a given place
	void elemRec(PlaceID fixp, bool siphon);

	/// Init the computation for elementary siphons/traps starting at a given place
	void initData(PlaceID p, bool siphon);

	/// Check if computed siphon/trap is minimal
	bool checkMin(PlaceID fixp, bool siphon) const;

	/// Save a found elementary siphon/trap
	void save(PlaceID fixp);

	/// Print the found elementary siphon/trap
	void print() const;

	/// The (sub)net for which siphons/traps should be computed
	const SubNet& net;

	/// The whole net
	const IMatrix& im;

	vector<bool> tvec; // if transition is used, [TransitionID]
	vector<TArcs::const_iterator> tselect; // [TransitionID]
	vector<bool> titinit; // [TransitionID]
	vector<TransitionID> tline; // [unsigned int]
	unsigned int tlast;
	vector<unsigned int> pfirst; // first transition needed by this place, [PlaceID]
	vector<bool> pvec; // if place is used, [PlaceID]
	vector<int> pgenby; // -1 = original place, TransitionID otherwise, [PlaceID]
	int tindex;

	// The last found siphons or traps with their generating places
	map<vector<PlaceID>,PlaceID> minst; // first param is trap/siphon

public:

	/// Map from elementary siphons to their generator places
	map<Siphon,Generators> elemsiphons;

	/// Map from Siphon IDs to the corresponding siphon and generator
	vector<map<Siphon,Generators>::iterator> esptr; // [SiphonID]

	/// Map from place IDs to generated elementary siphons
	vector<SiphonList> pes; // [PlaceID]

	/// Map from interfaces of siphons to the siphons having that interface
	map<Interface,SiphonList> sitoid;

	/// Map from elementary traps to their generating places
	map<Trap,Generators> elemtraps;

	/// Map from trap IDs to the traps and their generators
	vector<map<Trap,Generators>::iterator> etptr; // [TrapID]

	/// Flag for marked traps (by ID)
	vector<bool> etmarked; // [TrapID]

	/// Map from place IDs to generated elementary traps
	vector<TrapList> pet; // [PlaceID]

	/// Map from interfaces of interface-elementary traps to the interface-elementary traps having that interface
	map<Interface,TrapList> titoid;

	/// Map from interfaces of token-elementary traps to the token-elementary traps having that interface
	map<Interface,TrapList> mtitoid;
};

#endif

