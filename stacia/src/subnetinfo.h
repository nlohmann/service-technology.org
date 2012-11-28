
/*!
 * \file    subnetinfo.h
 *
 * \brief   Class SubNetInfo
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/07/13
 *
 * \date    $Date: 2012-11-20 14:47:01 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */

#ifndef SUBNETINFO_H
#define SUBNETINFO_H

// include header files
#include <config.h>
#include <libgen.h>
#include <string>
#include <utility>

#include "types.h"
#include "subnet.h"
#include "siphontrap.h"
#include "imatrix.h"
#include "config-log.h"
#include "verbose.h"

using std::string;

class Matchings;

class SubNetInfo {

public:
	/// Constructor for the additional siphon/trap info for subnets 
	SubNetInfo(const SubNet& net);

	/// Compute the subnet info from the fully computed siphons and traps
	bool computeComponentInfo(const SiphonTrap& stobj);

	/// Compute the subnet info from incremental siphon and trap information
	bool computeComponentInfo(const Matchings& m, bool mt);

	/// Print the siphon IDs and interfaces
	void printIntmap() const;

	/// Print the interfaces of interface-elementary traps
	void printLQ() const;

	/// Print the interfaces of token-elementary traps
	void printLM() const;

	/// Print the wrapping sets of interfaces of interface-elementary traps
	void printwQ() const;

	/// Print the wrapping sets of interfaces of token-elementary traps
	void printwM() const;

	/// Print an interface
	void printInterface(const Interface& interface) const;

	/// Print a wrapping of an interface
	void printWrapping(const Wrapping& wr) const;

	/// Print the complete info store in the structure
	void printInfo() const;

	/// Test inclusion of ordered vectors (true if set2 is included in set1)
	bool includes(const SiphonList& set1, const SiphonList& set2) const;

//	bool includes(const vector<unsigned int>& set1, const set<unsigned int>& set2) const;

	/// Test inclusion of sets (true in interface2 is included in interface1)
	bool includes(const Interface& set1, const Interface& set2) const;

	/// Get the unique generator of an interface if it exists
	int getUniqueElement(const Interface& interface, bool siphon) const;

	/// The subnet for which this info object has been constructed
	const SubNet& net;

	/// The incidence matrix of the whole Petri net
	const IMatrix& im;

	/// Get the siphon IDs and interfaces
	const map<SiphonID,Interface>& getIntMap() const;

	/// Get the interface of a siphon
	const Interface& getIntMap(SiphonID SID) const;

	/// Get all interfaces of interface-elementary traps
	const vector<Interface>& getLQ() const;

	/// Get all interfaces of token-elementary traps
	const vector<Interface>& getLM() const;

	/// Get an interface of an interface-elementary trap by (the interface's) ID
	const Interface& getLQ(InterfaceID IID) const;

	/// Get an interface of a token-elementary trap by (the interface's) ID
	const Interface& getLM(InterfaceID IID) const;

	/// Get the set of all wrappings belonging to interfaces of interface-elementary traps
	const map<InterfaceID,set<Wrapping> >& getwQ() const;

	/// Get the set of all wrappings belonging to interfaces of token-elementary traps
	const map<InterfaceID,set<Wrapping> >& getwM() const;

	/// Get the set of all wrappings belonging to the interface (by ID) of an interface-elementary trap
	const set<Wrapping>& getwQ(InterfaceID IID) const;

	/// Get the set of all wrappings belonging to the interface (by ID) of a token-elementary trap
	const set<Wrapping>& getwM(InterfaceID IID) const;

	/// Compute wrapping sets from a matching object
	void computeWrappingSets(const Matchings& m, InterfaceID interfacenr, const Matching& matching, InterfaceID tti, int side);

private:
	/// Compute wrapping sets directly from traps and siphons
	void computeWrappingSets(map<InterfaceID,set<Wrapping> >& wQ, InterfaceID wpos, const Interface& Li, const TrapList& lif);

	/// Check if a siphon is redundant
	bool checkRedundancy(SiphonID siphon);

	/// Inner check for redundancy of siphons
	bool checkRedundancy(const SiphonList& siphons, vector<bool>& wQcover, vector<bool>& wMcover);

	/// Remove a redundant siphon from all wrapping sets
	void removeRedundant(SiphonID sid);

	/// All siphons and traps of the subnet, if they have been fully computed
	const SiphonTrap* st;

	/// Map storing siphon info (IDs and interfaces)
	map<SiphonID,Interface> intmap;

	/// Interfaces of interface-elementary siphons
	vector<Interface> LQ;

	/// Interfaces of token-elementary siphons
	vector<Interface> LM;

	/// Wrapping sets for interfaces of interface-elementary siphons
	map<InterfaceID,set<Wrapping> > wQ;

	/// Wrapping sets for interfaces of token-elementary siphons
	map<InterfaceID,set<Wrapping> > wM;

	// A dummy
	Interface idummy;

	// A dummy
	set<Wrapping> wdummy;
};

#endif
