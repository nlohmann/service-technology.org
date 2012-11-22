// -*- C++ -*-

/*!
 * \file    netdata.h
 *
 * \brief   Class NetData
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/09/14
 *
 * \date    $Date: 2012-11-20 14:47:01 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */

#ifndef NETDATA_H
#define NETDATA_H

// include header files
#include "types.h"
#include "subnet.h"
#include "subnetinfo.h"
#include "imatrix.h"
#include "matchings.h"
#include "siphontrap.h"
#include "config-log.h"


/*! \brief This class encapules components for the conquer phase of the algorithm.
	In addition to the components themselves also meta-information about them is stored.
	Components can be childless (for minimal components), then siphons and traps are
	stored explicitly as well as information on the completion of their computation.
	Components with children have additional information on how to collect the siphons
	and traps from their children as well as who their children are. 
*/
class NetData {

public:

	/// Constructor, storing incidence matrix and the transitions spanning this component
	NetData(const IMatrix& im, const vector<TransitionID>& tvec);

	/// Union-Constructor
	NetData(NetData* nd1, NetData* nd2, bool tdone);

	/// Destructor
	~NetData();

	/// Get a pointer to the contained subnet
	SubNet* getSubNetPtr() const;

	/// Get the contained subnet
	SubNet& getSubNet() const;

	/// Get a pointer to the info structure about siphon and traps of the contained subnet
	SubNetInfo* getSubNetInfo();

	/// Flag indicating whether this component has already been taken into account in the current plan
	bool isUsed() const;

	/// Flag indicating whether this component can be used in further computations
	bool isDone() const;

	/// Flag indicating whether the computations for this component are done
	bool isComputed() const;

	/// Flag indicating whether the computations for this component have been scheduled
	bool isScheduled() const;

	/// Get a child component of this component
	NetData* getChild(bool right);

	/// Mark info on this component as collected 
	void setDone();

	/// Mark this component as completely computed
	void setComputed();

	/// Mark the computation of the info for this component as scheduled
	void setScheduled();

	/// Set an identifier for this component
	void setIndex(unsigned int nr);

	/// Get the identifier for this component
	unsigned int getIndex() const;

	/// Add a potential partner to a list for this component
	void addPartner(NetData* nd);

	/// Refresh the list of potential partners
	void checkPartner();

	/// Get the best potential partner
	NetData* getMinPartner() const;

	/// Get the interface size of the union with the best potential partner
	unsigned int getMinInterfaceSize() const;

	/// Initialise a walk through all potential partners, from best to worst
	void traversePartners();

	/// Get the next potential partner in the walk
	NetData* nextPartner(unsigned int& cisize);

	/// Set info on siphons and trap directly
	void setSiphonTrap(SiphonTrap* stobj);

	/// Set info on how to build siphons and traps from children's info
	void setMatchings(Matchings* mobj);

	/// Get a siphon by its ID
	set<PlaceID> getSiphon(SiphonID ID) const;

	/// Print all open siphons represented by the meta info in this component
	void printSiphonMatchings() const;

private:
	/// The actual component
	SubNet* sn;

	/// Additional info for the component
	SubNetInfo* sni;

	/// The left child of the component
	NetData* child1;

	/// The right child of the component
	NetData* child2;

	/// If the component has been scheduled
	bool threaddone;

	/// If the component has been computed completely
	bool st_computed;

	/// If the component is contained in the plan
	bool used;

	/// Identifier for this component
	unsigned int index;

	/// The best potential partner component
	NetData* minpartner;

	/// The size of the unions interface (with the best partner)
	unsigned int mininterface;

	/// Memory for the potential partners, sorted by union's interface size
	map<unsigned int,set<NetData*> > partners;

	/// Iterator for a walk through the potential partners
	map<unsigned int,set<NetData*> >::iterator pit;

	/// Iterator for a walk through the potential partners
	set<NetData*>::iterator nit;

	/// Siphons and traps for childless components
	SiphonTrap* st;

	/// Siphon and trap meta info deducted from child components
	Matchings* m;
};

#endif

