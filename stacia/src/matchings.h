
/*!
 * \file    matchings.h
 *
 * \brief   Class Matchings
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/07/13
 *
 * \date    $Date: 2012-11-20 14:47:01 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */

#ifndef MATCHINGS_H
#define MATCHINGS_H

// include header files
#include <config.h>
#include <libgen.h>
#include <string>
#include <utility>

#include "types.h"
#include "subnet.h"
#include "subnetinfo.h"
#include "imatrix.h"
#include "config-log.h"
#include "verbose.h"


class Matchings {

public:
	/// Constructor
	Matchings(const SubNetInfo& info1, const SubNetInfo& info2, bool mt);

	/// Types of matchings
	enum STType {
		SIPHON = 0,
		ITRAP = 1,
		TTRAP = 2
	};

	/// Print matchings of one type
	void printMatchings(STType type) const;

	/// Print a matching
	void printMatching(MatchingID ID, STType type) const;

	/// Print the interface of a matching
	void printMatchingInterface(MatchingID ID, STType type) const;

	/// Get a matching
	const Matching& getMatching(MatchingID ID, STType type) const;

	/// Get the unique token trap of a TTRAP matching
	unsigned int getTokenTrap(MatchingID ID) const;

	/// Detect which child contains the unique token trap of a TTRAP matching
	bool getTokenTrapSide(MatchingID ID) const;

	/// Get the interface of a matching
	const Interface& getMatchingInterface(MatchingID ID, STType type) const;

	/// Get the total number of matchings of one type
	unsigned int numMatchings(STType type) const;

	/// Get a list of matchings containing a selected siphon
	const MatchingList& getMatchingList(SiphonID siphon, bool side) const;

	/// Get the siphon/trap info of a child component
	const SubNetInfo& getSNI(bool side) const;

	/// Get the common part of the children's interfaces
	Interface getCommonInterface() const;

private:
	/// Compute all matchings of one type
	void computeMatchings(STType type, bool mt);

	/// Compute a mapping from siphons to containing matchings
	void computeMatchingLists();

	/// Info structure of the left/first child
	const SubNetInfo& inf1;

	/// Info structure of the right/second child
	const SubNetInfo& inf2;

	/// Matchings of type ITRAP
	vector<Matching> itmatch;

	/// Interfaces of matchings of type ITRAP
	vector<Interface> itmatchif;

	/// Matchings of type TTRAP
	vector<Matching> ttmatch;

	/// Interfaces of matchings of type ITRAP
	vector<Interface> ttmatchif;

	/// In which child the token traps of the TTRAP matchings reside
	vector<bool> ttmatchside;

	/// The interfaces of the unique token traps in the TTRAP matchings
	vector<InterfaceID> ttmatchtt;

	/// Matchings of type SIPHON
	vector<Matching> swmatch;

	/// Interfaces of matchings of type SIPHON
	vector<Interface> swmatchif;

	/// Maps from siphons of the left/right child to containing matchings
	map<SiphonID,MatchingList> lsmlist,rsmlist;

	/// Dummy for the empty matching list
	MatchingList mdummy;
};

#endif
