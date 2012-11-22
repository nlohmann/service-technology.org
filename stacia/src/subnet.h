// -*- C++ -*-

/*!
 * \file    subnet.h
 *
 * \brief   Class SubNet
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/07/02
 *
 * \date    $Date: 2012-11-20 14:47:01 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */

#ifndef SUBNET_H
#define SUBNET_H

// include header files
#include "types.h"
#include "imatrix.h"
#include "config-log.h"

class SubNet {

public:
	/// Constructor for the full net as its own subnet
	SubNet(const IMatrix& im);

	/// Constructor for a subnet containing the given transitions only
	SubNet(const IMatrix& im, const vector<unsigned int>& tvec);

	/// Constructor for a subnet containing the given transitions only
	SubNet(const SubNet& net, const vector<unsigned int>& tv, unsigned int newport);

	/// Constructor for the union of two subnets
	SubNet(const SubNet& sn1, const SubNet& sn2);

	/// Destructor
	~SubNet();

	/// Get all non-interface place of the subnet
	const vector<PlaceID>& getInnerPlaces() const;

	/// Get all transitions of the subnet
	const vector<TransitionID>& getTransitions() const;

	/// Get all input places of the subnet
	const vector<PlaceID>& getInput() const;

	/// Get all output places of the subnet
	const vector<PlaceID>& getOutput() const;

	/// Get all marked places of the subnet
	vector<PlaceID> getMarked() const;

	/// Get all interface places of the subnet
	vector<PlaceID> getInterface() const;

	/// for-loop condition&increment for pre/postsets of places
	bool checkT(PArcs::const_iterator& it, const PArcs::const_iterator& endit) const;
	/// for-loop condition&increment for pre/postsets of transitions
	bool checkP(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const;
	/// for-loop condition&increment for inner places
	bool checkInner(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const;
	/// for-loop condition&increment for interface places
	bool checkInOut(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const;
	/// for-loop condition&increment for input places
	bool checkIn(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const;
	/// for-loop condition&increment for output places
	bool checkOut(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const;
	/// for-loop condition&increment for marked places
	bool checkMarked(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const;

	/// for-loop condition&increment for pre/postsets of places
	bool checkT(PArcs::iterator& it, const PArcs::iterator& endit) const;
	/// for-loop condition&increment for pre/postsets of transitions
	bool checkP(TArcs::iterator& it, const TArcs::iterator& endit) const;
	/// for-loop condition&increment for inner places
	bool checkInner(TArcs::iterator& it, const TArcs::iterator& endit) const;
	/// for-loop condition&increment for interface places
	bool checkInOut(TArcs::iterator& it, const TArcs::iterator& endit) const;
	/// for-loop condition&increment for input places
	bool checkIn(TArcs::iterator& it, const TArcs::iterator& endit) const;
	/// for-loop condition&increment for output places
	bool checkOut(TArcs::iterator& it, const TArcs::iterator& endit) const;
	/// for-loop condition&increment for marked places
	bool checkMarked(TArcs::iterator& it, const TArcs::iterator& endit) const;

	/// Check if a place is an inner place
	bool internal(PlaceID p) const;

	/// Check if a place is an interface place
	bool interface(PlaceID p) const;

	/// Check if a place is an input place
	bool isInput(PlaceID p) const;

	/// Check if a place is an output place
	bool isOutput(PlaceID p) const;

	/// Check if a place is marked
	bool isMarked(PlaceID p) const;

	/// Check if a place exists in this subnet
	bool isPlace(PlaceID p) const;

	/// Check if a transition exists in this subnet
	bool isTransition(TransitionID t) const;

	/// Get the whole net as PNAPI object
	const PetriNet& underlyingNet() const;

	/// Get the whole net
	const IMatrix& getMatrix() const;

	/// Get an unused port number
	unsigned int getNewPortNumber() const;

	/// Print the subnet
	void printNet() const;

	/// Print the given interface 
	void printInterface(const Interface& interface) const;

	/// Get the siphon implications of a place
	const vector<set<PlaceID> >& getSiphonImplication(PlaceID p) const;

	/// Get the trap implications of a place
	const vector<set<PlaceID> >& getTrapImplication(PlaceID p) const;

	/// Compute the interface size of the union of two subnets
	unsigned int combinedInterfaceSize(const SubNet& sn) const;

	/// Compute the size of the common interface of two subnets
	unsigned int innerInterfaceSize(const SubNet& sn) const;

	/// Get the combined interface of two subnets
	Interface combinedInterface(const SubNet& sn) const;

	/// Compute the number of places in only one of the interface - the given one or the subnet's
	int diffInterface(const Interface& i) const;

private:
	/// Incidence matrix of the whole net
	const IMatrix& im;

	/// List of transition IDs in this subnet 
	vector<TransitionID> tset; // [unsigned int]

	/// Flags for transitions contained in this subnet (by ID)
	vector<char> tsetb; // [TransitionID]

	/// List of inner places in this subnet
	vector<PlaceID> pinner; // [unsigned int]

	/// Flags for inner places (by iD)
	vector<char> pinnerb; // [PlaceID]

	/// List of input places
	vector<PlaceID> pin; // [unsigned int]

	/// List for output places (by ID)
	vector<PlaceID> pout; // [unsigned int]

	/// Flags for input places (by ID)
	vector<char> pinb; // [PlaceID]

	/// Flags for output places (by ID)
	vector<char> poutb; // [PlaceID]

	/// Map from place IDs to port numbers
	vector<unsigned int> port; // [PlaceID]

	/// Next unused port number
	static unsigned int nextportnr;

	// dummy
	vector<set<PlaceID> > zeroimpl;
};

#endif

