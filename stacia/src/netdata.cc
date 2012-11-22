// -*- C++ -*-

/*!
 * \file    netdata.cc
 *
 * \brief   Class NetData
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/09/14
 *
 * \date    $Date: 2012-11-20 12:00:00 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */

#include "netdata.h"

using std::cout;
using std::endl;

	/*****************************************************
	* Implementation of the methods of the class NetData *
	*****************************************************/

/** Constructor for a new component and all its relevant data.
	@param im The incidence matrix of the Petri net.
	@param tvec A vector containing all transitions of this components.
*/
NetData::NetData(const IMatrix& im, const vector<TransitionID>& tvec) 
	: sni(NULL),child1(NULL),child2(NULL),threaddone(false),st_computed(false),used(false),st(NULL),m(NULL) {
	sn = new SubNet(im,tvec);
}

/** Constructor that unifies two components. The two components are marked as used (modification!).
	@param nd1 The first component.
	@param nd2 The second component.
	@param tdone The state of the new component (still in computation/done).
*/
NetData::NetData(NetData* nd1, NetData* nd2, bool tdone)
	: sni(NULL),child1(nd1),child2(nd2),threaddone(tdone),st_computed(nd1->st_computed),used(false),st(NULL),m(NULL) {
	sn = new SubNet(*(nd1->sn),*(nd2->sn));
	nd1->used = true;
	nd2->used = true;
}

/** Destructor. */
NetData::~NetData() {
	delete sn;
	delete sni;
	delete st;
	delete m;
}

/** Get the component encapsuled in this data structure.
	@return The subnet contained in this structure.
*/
SubNet& NetData::getSubNet() const { return *sn; }

/** Get the component encapsuled in this data structure.
	@return A pointer to the subnet contained in this structure.
*/
SubNet* NetData::getSubNetPtr() const { return sn; }

/** Get information about siphons and traps for the encapsuled component.
	@return A pointer to the corresponding SubNetInfo structure.
*/
SubNetInfo* NetData::getSubNetInfo() { 
	if (!sni) sni = new SubNetInfo(*sn);
	return sni; 
}

/** Check if this component has already been taken into account by the current plan.
	Components must not be used twice.
	@return True if so.
*/
bool NetData::isUsed() const { return used; }

/** Check if all siphon&trap data about this component has been collected. The
	computation must be complete before computing information about supercomponents
	containing this component can start,
	@return True if all data is complete.
*/
bool NetData::isDone() const { return threaddone; }

/** Check if the computations necessary for this component have been done.
	@return True if so.
*/
bool NetData::isComputed() const { return st_computed; }

/** Check if the computations for this component have been scheduled.
	@return True if so.
*/
bool NetData::isScheduled() const { return st_computed; }

/** Mark this component's additional info as completely collected. */
void NetData::setDone() { threaddone = true; }

/** Mark the computations for this component as complete. */
void NetData::setComputed() { st_computed = true; }

/** Mark the computations for this component as scheduled. */
void NetData::setScheduled() { st_computed =  true; }

/** Get one of the child components necessary for this component's computations.
	@param right False gets the left child, true the right one.
	@return Pointer to the child (NULL if no child exists).
*/
NetData* NetData::getChild(bool right) { return (right ? child2 : child1); }

/** Set an identifier for this component. Exclusiveness must be ensured by the user.
	@param nr The identifier.
*/
void NetData::setIndex(unsigned int nr) { index = nr; minpartner = NULL; }

/** Get the previosly set identifier for this component.
	@return This component's identifier.
*/
unsigned int NetData::getIndex() const { return index; }

/** Introduce another component as a potential partner for this one.
	@param nd The other component.
*/
void NetData::addPartner(NetData* nd) {
	if (isUsed() || nd->isUsed()) return;
	unsigned int ifsize(sn->combinedInterfaceSize(nd->getSubNet()));
	if (ifsize<mininterface || minpartner==NULL) {
		mininterface = ifsize;
		minpartner = nd;
	}
	partners[ifsize].insert(nd);
}

/** Update the partner list for this component. Necessary whenever a potential partner
	added previously is planned (by setUsed()). Must be called before potential partners
	are checked the next time.
*/
void NetData::checkPartner() {
	while (true)
	{
		if (partners.empty()) { minpartner=NULL; return; }
		if (partners.begin()->second.empty()) partners.erase(partners.begin());
		else if ((*(partners.begin()->second.begin()))->isUsed()) 
			partners.begin()->second.erase(partners.begin()->second.begin());
		else break;
	}
	mininterface = partners.begin()->first;
	minpartner = *(partners.begin()->second.begin());
}

/** Get the best potential partner, yielding a minimal interface.
	@return The best potential partner.
*/
NetData* NetData::getMinPartner() const { return minpartner; }

/** Get the size of the interface of the union of this component and
	its best potential partner.
	@return The interface size.
*/
unsigned int NetData::getMinInterfaceSize() const { return mininterface; }

/** Initialise a walk through all the potential partners. */
void NetData::traversePartners() {
	pit = partners.begin();
	nit = pit->second.begin();
}

/** Obtain the next potential partner. Partner are listed from best to worst.
	@param cisize Interface size of the union with the next partner.
	@return The next partner.
*/
NetData* NetData::nextPartner(unsigned int& cisize) {
	if (pit==partners.end()) return NULL;
	NetData* nd(*nit);
	cisize = pit->first;
	++nit;
	if (nit==pit->second.end()) {
		++pit;
		if (pit!=partners.end()) nit=pit->second.begin();
	}
	return nd;
}

/** Set the siphons and traps for this component. Use with childless components.
	@param stobj An object containing the siphons and traps.
*/
void NetData::setSiphonTrap(SiphonTrap* stobj) { st = stobj; }

/** Set info about siphons and traps for this component. To be used only for components with children.
	@param mobj Info on how to build the siphons and traps from the info contained in the children components.
*/
void NetData::setMatchings(Matchings* mobj) { m = mobj; }

/** Get a siphon by ID.
	@param ID The identifier of the siphon.
	@return The set of places contained in the siphon.
*/
set<PlaceID> NetData::getSiphon(SiphonID ID) const {
	set<PlaceID> result;
	if (m && m->numMatchings(Matchings::SIPHON)>=ID) {
		const Matching& matching(m->getMatching(ID,Matchings::SIPHON));
		for(unsigned int j=0; j<matching.first.size(); ++j)
		{
			set<PlaceID> tmp(child1->getSiphon(matching.first[j]));
			result.insert(tmp.begin(),tmp.end());
		}
		for(unsigned int j=0; j<matching.second.size(); ++j)
		{
			set<PlaceID> tmp(child2->getSiphon(matching.second[j]));
			result.insert(tmp.begin(),tmp.end());
		}
	} else if (st && st->numSiphons()>=ID) {
		const Siphon& siphon(st->getSiphon(ID));
		result.insert(siphon.begin(),siphon.end());
	}
	return result;
}

/*
set<PlaceID> NetData::getITrap(TrapID ID) const {
	set<PlaceID> result;
	if (m && m->numMatchings(Matchings::ITRAP)>=ID) {
		const Matching& matching(m->getMatching(ID,Matchings::ITRAP));
		for(unsigned int j=0; j<matching.first.size(); ++j)
		{
			set<PlaceID> tmp(child1->getITrap(matching.first[j]));
			result.insert(tmp.begin(),tmp.end());
		}
		for(unsigned int j=0; j<matching.second.size(); ++j)
		{
			set<PlaceID> tmp(child2->getITrap(matching.second[j]));
			result.insert(tmp.begin(),tmp.end());
		}
	} else if (st && st->numTraps()>=ID) {
		const Trap& trap(st->getTrap(ID));
		result.insert(trap.begin(),trap.end());
	}
	return result;
}
*/

/** Print all siphon matchings and evaluate which places are contained. */
void NetData::printSiphonMatchings() const {
	if (!m) return;
	for(unsigned int i=0; i<m->numMatchings(Matchings::SIPHON); ++i)
	{
		m->printMatching(i,Matchings::SIPHON);
		m->printMatchingInterface(i,Matchings::SIPHON);
		cout << ":(*";
		bool comma(false);
		set<PlaceID> tmp(getSiphon(i));
		for(set<PlaceID>::iterator it=tmp.begin(); it!=tmp.end(); ++it)
		{
			if (!comma) comma=true; else cout << ",";
			cout << sn->getMatrix().pName(*it);
		}
		cout << "*)" << endl;
	}
}

/*
void NetData::printITrapMatchings() const {
	if (!m) return;
	for(unsigned int i=0; i<m->numMatchings(Matchings::ITRAP); ++i)
	{
		m->printMatching(i,Matchings::ITRAP);
		m->printMatchingInterface(i,Matchings::ITRAP);
		cout << ":(*";
		bool comma(false);
		set<PlaceID> tmp(getITrap(i));
		for(set<PlaceID>::iterator it=tmp.begin(); it!=tmp.end(); ++it)
		{
			if (!comma) comma=true; else cout << ",";
			cout << sn->getMatrix().pName(*it);
		}
		cout << "*)" << endl;
	}
}
*/

