// -*- C++ -*-

/*!
 * \file    subnet.cc
 *
 * \brief   Class SubNet
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/07/03
 *
 * \date    $Date: 2012-11-20 12:00:00 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */

#include "subnet.h"
#include <assert.h>
#include "verbose.h"

using std::cout;
using std::endl;

	/****************************************************
	* Implementation of the methods of the class SubNet *
	****************************************************/

/** Constructor for the full net as its own subnet.
	@param imatrix The incidence matrix of the net.
*/
SubNet::SubNet(const IMatrix& imatrix) 
	: im(imatrix) {
	TransitionID tsize(im.numTransitions());
	PlaceID psize(im.numPlaces());

	tset.resize(tsize);
	tsetb.assign(tsize,true);
	for(TransitionID id=0; id<tsize; ++id) tset[id]=id;

	pinner.resize(psize);
	pinnerb.assign(psize,true);	
	for(PlaceID id=0; id<psize; ++id) pinner[id]=id;

	pinb.assign(psize,false);
	poutb.assign(psize,false);
	port.assign(psize,0);
}

/** Constructor for a subnet with a given set of transitions.
	@param imatrix The incidence matrix of the full net.
	@param tvec A list of transition to be contained in the subnet, including their pre- and postsets.
*/
SubNet::SubNet(const IMatrix& imatrix, const vector<TransitionID>& tvec) : im(imatrix),tset(tvec) {
	TransitionID tsize(im.numTransitions());
	PlaceID psize(im.numPlaces());
	unsigned int newport(1);

	tsetb.assign(tsize,false);
	pinnerb.assign(psize,false);
	pinb.assign(psize,false);
	poutb.assign(psize,false);
	port.assign(psize,0);

	assert(tset.size()<=tsize);
	for(TransitionID i=0; i<tset.size(); ++i)
		tsetb[tset[i]]=true;

	for(TransitionID i=0; i<tset.size(); ++i) 
	{
		TArcs::const_iterator it;
		PArcs::const_iterator pit;
		const TArcs& pre(im.getTPreset(tset[i]));
		for(it=pre.begin(); it!=pre.end(); ++it) 
		{
				const PArcs& ppre(im.getPPreset(it->first));
				for(pit=ppre.begin(); pit!=ppre.end(); ++pit)
				{
					if (!tsetb.at(pit->first)) {
							pinb.at(it->first)=true;
							port[it->first]=newport;
						break;
					}
				}
				if (!pinb.at(it->first)) {
					pinnerb[it->first]=true;
				}
		}

		const TArcs& post(im.getTPostset(tset[i]));
		for(it=post.begin(); it!=post.end(); ++it) 
		{
				const PArcs& ppost(im.getPPostset(it->first));
				for(pit=ppost.begin(); pit!=ppost.end(); ++pit)
				{
					if (!tsetb.at(pit->first)) {
						poutb.at(it->first)=true;
						port[it->first]=newport;
						break;
					}
				}
				if (!poutb.at(it->first)) {
					pinnerb[it->first]=true;
				}
		}
	}

	for(PlaceID i=0; i<psize; ++i)
	{
		if (pinb[i]) pin.push_back(i);
		if (poutb[i]) pout.push_back(i);
		if (pinnerb[i]) pinner.push_back(i);
	}
}


/** Constructor for a subnet with a given set of transitions.
	@param net The full net.
	@param tv A list of transition to be contained in the subnet, including their pre- and postsets.
	@param newport The port number of new interface places. 
*/
SubNet::SubNet(const SubNet& net, const vector<TransitionID>& tv, unsigned int newport) 
	: im(net.im),tset(tv),port(net.port) {
	TransitionID tsize(im.numTransitions());
	PlaceID psize(im.numPlaces());

	tsetb.assign(tsize,false);
	pinnerb.assign(psize,false);
	pinb.assign(psize,false);
	poutb.assign(psize,false);

	assert(tset.size()<=tsize);
	for(TransitionID i=0; i<tset.size(); ++i)
		tsetb[tset[i]]=true;

	for(TransitionID i=0; i<tset.size(); ++i) 
	{
		PArcs::const_iterator pit;
		TArcs::const_iterator it;
		const TArcs& pre(im.getTPreset(tset[i]));
		for(it=pre.begin(); it!=pre.end(); ++it) 
		{
			if (net.isInput(it->first)) {
					pinb.at(it->first)=true;
			} else {
				const PArcs& ppre(im.getPPreset(it->first));
				for(pit=ppre.begin(); net.checkT(pit,ppre.end()); ++pit)
				{
					if (!tsetb.at(pit->first)) {
							pinb.at(it->first)=true;
							port[it->first]=newport;
						break;
					}
				}
				if (!pinb.at(it->first)) {
					pinnerb[it->first]=true;
				}
			}
		}

		const TArcs& post(im.getTPostset(tset[i]));
		for(it=post.begin(); it!=post.end(); ++it) 
		{
			if (net.isOutput(it->first)) {
				poutb.at(it->first)=true;
			} else {
				const PArcs& ppost(im.getPPostset(it->first));
				for(pit=ppost.begin(); net.checkT(pit,ppost.end()); ++pit)
				{
					if (!tsetb.at(pit->first)) {
						poutb.at(it->first)=true;
						port[it->first]=newport;
						break;
					}
				}
				if (!poutb.at(it->first)) {
					pinnerb[it->first]=true;
				}
			}
		}
	}

	for(PlaceID i=0; i<psize; ++i)
	{
		if (pinb[i]) pin.push_back(i);
		if (poutb[i]) pout.push_back(i);
		if (pinnerb[i]) pinner.push_back(i);
	}
}

/** Constructor for the union of two subnets.
	@param sn1 The first subnet,
	@param sn2 The second subnet.
*/
SubNet::SubNet(const SubNet& sn1, const SubNet& sn2) : im(sn1.im) {
	if (&(sn1.im)!=&(sn2.im)) abort(41,"union of subnets stemming from different nets");
	TransitionID tsize(im.numTransitions());
	PlaceID psize(im.numPlaces());

	tsetb.assign(tsize,false);
	pinnerb.assign(psize,false);
	pinb.assign(psize,false);
	poutb.assign(psize,false);
	port.assign(psize,0);

	for(PlaceID p=0; p<psize; ++p)
	{
		if (sn1.port[p] ^ sn2.port[p]) port[p] = sn1.port[p] + sn2.port[p];
		if (sn1.pinnerb[p] || sn2.pinnerb[p]) { pinnerb[p]=true; pinner.push_back(p); }
		if ((sn1.pinb[p] && sn2.poutb[p]) || (sn1.poutb[p] && sn2.pinb[p])) {
			pinnerb[p]=true; pinner.push_back(p);
		} else {
			if (sn1.pinb[p] || sn2.pinb[p]) { pinb[p]=true; pin.push_back(p);}
			if (sn1.poutb[p] || sn2.poutb[p]) { poutb[p]=true; pout.push_back(p); }
		}
	}	

	for(TransitionID t=0; t<tsize; ++t)
	{
		if (sn1.tsetb[t] || sn2.tsetb[t]) { tsetb[t]=true; tset.push_back(t); }
	}
}

/** Destructor. */
SubNet::~SubNet() {}

/** Get all inner places of this subnet.
	@return A list of the inner places by ID.
*/
const vector<PlaceID>& SubNet::getInnerPlaces() const { return pinner; }

/** Get all transitions in this subnet.
	@return A list of all transitions by ID.
*/
const vector<TransitionID>& SubNet::getTransitions() const { return tset; }

/** Get all input places of this subnet,
	@return A list of all input places by ID.
*/
const vector<PlaceID>& SubNet::getInput() const { return pin; }

/** Get all output places of this subnet.
	@return A list of all output places by ID.
*/
const vector<PlaceID>& SubNet::getOutput() const { return pout; }

/** Get all marked places in this subnet.
	@return A list of all marked places by ID.
*/
vector<PlaceID> SubNet::getMarked() const { 
	vector<PlaceID> res(im.getMarked());
	vector<PlaceID> res2;
	for(unsigned int p=0; p<res.size(); ++p)
		if (pinnerb[res[p]] || pinb[res[p]] || poutb[res[p]]) 
			res2.push_back(res[p]);
	return res2; 
}

/** Get all interface places of this subnet.
	@return A list of all interface places by ID.
*/
vector<PlaceID> SubNet::getInterface() const {
	vector<PlaceID> res;
	unsigned int i,j;
	for(i=0,j=0; i<pin.size() || j<pout.size(); )
	{
		if (i==pin.size()) { res.push_back(pout[j]); ++j; }
		else if (j==pout.size() || pin[i]<pout[j]) { res.push_back(pin[i]); ++i; }
		else { res.push_back(pout[j]); ++j; }
	}

	return res;
}

/** Loop check & increment for the pre- or postset of places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkT(PArcs::const_iterator& it, const PArcs::const_iterator& endit) const {
	while (it!=endit)
	{
		if (tsetb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for the pre- or postset of transitions.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkP(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const {
	while (it!=endit)
	{
		if (pinnerb.at(it->first)) return true;
		if (pinb.at(it->first)) return true;
		if (poutb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at inner places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkInner(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const {
	while (it!=endit)
	{
		if (pinnerb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at interface places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkInOut(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const {
	while (it!=endit)
	{
		if (pinb.at(it->first)) return true;
		if (poutb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at input places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkIn(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const {
	while (it!=endit)
	{
		if (pinb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at output places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkOut(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const {
	while (it!=endit)
	{
		if (poutb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at marked places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkMarked(TArcs::const_iterator& it, const TArcs::const_iterator& endit) const {
	while (it!=endit)
	{
		if (im.marking(it->first)>0) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkT(PArcs::iterator& it, const PArcs::iterator& endit) const {
	while (it!=endit)
	{
		if (tsetb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkP(TArcs::iterator& it, const TArcs::iterator& endit) const {
	while (it!=endit)
	{
		if (pinnerb.at(it->first)) return true;
		if (pinb.at(it->first)) return true;
		if (poutb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at inner places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkInner(TArcs::iterator& it, const TArcs::iterator& endit) const {
	while (it!=endit)
	{
		if (pinnerb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at interface places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkInOut(TArcs::iterator& it, const TArcs::iterator& endit) const {
	while (it!=endit)
	{
		if (pinb.at(it->first)) return true;
		if (poutb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at input places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkIn(TArcs::iterator& it, const TArcs::iterator& endit) const {
	while (it!=endit)
	{
		if (pinb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at output places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkOut(TArcs::iterator& it, const TArcs::iterator& endit) const {
	while (it!=endit)
	{
		if (poutb.at(it->first)) return true;
		++it;
	}
	return false;
}

/** Loop check & increment for a pre- or postset of transitions.
	Only halts at marked places.
	@param it Iterator to an arc map.
	@param endit end()-iterator to the same map.
	@return True until the end of the map is reached.
*/
bool SubNet::checkMarked(TArcs::iterator& it, const TArcs::iterator& endit) const {
	while (it!=endit)
	{
		if (im.marking(it->first)>0) return true;
		++it;
	}
	return false;
}

/** Check if a place is an inner place.
	@param p ID of the place.
	@return True if it is an inner place.
*/
bool SubNet::internal(PlaceID p) const { return pinnerb.at(p); }

/** Check if a place is an interface place.
	@param p ID of the place.
	@return True if it is an interface place.
*/
bool SubNet::interface(PlaceID p) const { return (pinb.at(p) || poutb.at(p)); }

/** Check if a place is an input place.
	@param p ID of the place.
	@return True if it is an input place.
*/
bool SubNet::isInput(PlaceID p) const { return pinb.at(p); }

/** Check if a place is an output place.
	@param p ID of the place.
	@return True if it is an output place.
*/
bool SubNet::isOutput(PlaceID p) const { return poutb.at(p); }

/** Check if a place is a marked place.
	@param p ID of the place.
	@return True if it is a marked place.
*/
bool SubNet::isMarked(PlaceID p) const { return (im.marking(p)>0); } //return (marking.find(p)!=marking.end()); }

/** Check if a place exists in this subnet.
	@param p ID of the place.
	@return True if the place exists.
*/
bool SubNet::isPlace(PlaceID p) const { return (pinnerb.at(p) || pinb.at(p) || poutb.at(p)); }

/** Check if a transition exists in this subnet.
	@param t ID of the transition.
	@return True if the transition exists.
*/
bool SubNet::isTransition(TransitionID t) const { return tsetb.at(t); }

/** Get the underlying net from the PNAPI.
	@return The Petri net.
*/
const PetriNet& SubNet::underlyingNet() const { return im.getNet(); }

/** Get the incidence matrix.
	@return The incidence matrix.
*/
const IMatrix& SubNet::getMatrix() const { return im; }

/** Get a new port number.
	@return A unique port number.
*/
unsigned int SubNet::getNewPortNumber() const {
	++nextportnr;
	return nextportnr;
}

/** The next free port number. */
unsigned int SubNet::nextportnr = 0;

/** Print this subnet, */
void SubNet::printNet() const {
	for(unsigned int i=0; i<tset.size(); ++i)
	{
		cout << "(";
		bool comma(false);
		TArcs::const_iterator ait;
		const TArcs& pre(im.getTPreset(tset[i]));
		for(ait=pre.begin(); checkP(ait,pre.end()); ++ait)
		{
			if (comma) cout << ","; else comma=true;
			if (pinb[ait->first]) cout << "[" << port[ait->first] << "]";
			cout << im.pName(ait->first);
			if (poutb[ait->first]) cout << "[" << port[ait->first] << "]";
		}
		cout << ")--";
		cout << im.tName(tset[i]);
		cout << "->(";
		comma = false;
		const TArcs& post(im.getTPostset(tset[i]));
		for(ait=post.begin(); checkP(ait,post.end()); ++ait)
		{
			if (comma) cout << ","; else comma=true;
			if (pinb[ait->first]) cout << "[" << port[ait->first] << "]";
			cout << im.pName(ait->first);
			if (poutb[ait->first]) cout << "[" << port[ait->first] << "]";
		}
		cout << ")" << endl;
	}
}

/** Get the siphon implications for a given place with respect to this subnet.
	@param p ID of the place.
	@return The siphon implications.
*/
const vector<set<PlaceID> >& SubNet::getSiphonImplication(PlaceID p) const {
	if (poutb[p] || pinnerb[p]) return im.getSiphonImplication(p);
	return zeroimpl;
}

/** Get the trap implications for a given place with respect to this subnet.
	@param p ID of the place.
	@return The trap implications.
*/
const vector<set<PlaceID> >& SubNet::getTrapImplication(PlaceID p) const {
	if (pinb[p] || pinnerb[p]) return im.getTrapImplication(p);
	return zeroimpl;
}

/** Get the size of the interface of the union of two subnets.
	@param sn The second subnet.
	@return The size.
*/
unsigned int SubNet::combinedInterfaceSize(const SubNet& sn) const {
	unsigned int i,res(0);
	for(i=0; i<pin.size(); ++i)
		if (!sn.isOutput(pin[i])) ++res;
	for(i=0; i<pout.size(); ++i)
		if (!sn.isInput(pout[i])) ++res;
	const vector<unsigned int>& snin(sn.getInput());
	for(i=0; i<snin.size(); ++i)
		if (!poutb[snin[i]]) ++res;
	const vector<unsigned int>& snout(sn.getOutput());
	for(i=0; i<snout.size(); ++i)
		if (!pinb[snout[i]]) ++res;
	return res;
}

/** Get the number of common places of two subnets,
	@param sn The second subnet.
	@return The number of places in the common interface.
*/
unsigned int SubNet::innerInterfaceSize(const SubNet& sn) const {
	unsigned int i,res(0);
	for(i=0; i<pin.size(); ++i)
		if (sn.isOutput(pin[i])) ++res;
	for(i=0; i<pout.size(); ++i)
		if (sn.isInput(pout[i])) ++res;
	return res;
}

/** Get the interface of the union of two subnets.
	@param sn The second subnet.
	@return The interface.
*/
Interface SubNet::combinedInterface(const SubNet& sn) const {
	Interface res;
	unsigned int i;
	for(i=0; i<pin.size(); ++i)
		if (!sn.isOutput(pin[i])) res.insert(pin[i]);
	for(i=0; i<pout.size(); ++i)
		if (!sn.isInput(pout[i])) res.insert(pout[i]);
	const vector<PlaceID>& snin(sn.getInput());
	for(i=0; i<snin.size(); ++i)
		if (!poutb[snin[i]]) res.insert(snin[i]);
	const vector<unsigned int>& snout(sn.getOutput());
	for(i=0; i<snout.size(); ++i)
		if (!pinb[snout[i]]) res.insert(snout[i]);
	return res;
}

/** As combinedInterfaceSize() but takes just an interface instead of a subnet as parameter.
	@param i The interface of the second subnet.
	@return The size of the interface.
*/
int SubNet::diffInterface(const Interface& i) const {
	int cnt(getInput().size()+getOutput().size());
	Interface::const_iterator it;
	for(it=i.begin(); it!=i.end(); ++it)
		if (interface(*it)) cnt -= 2;
	return cnt;
}

/** Print an interface.
	@param interface The interface.
*/
void SubNet::printInterface(const Interface& interface) const {
	Interface::const_iterator iit;
	cout << "{";
	bool comma(false);
	for(iit=interface.begin(); iit!=interface.end(); ++iit)
	{
		if (comma) cout << ","; else comma=true;
		cout << im.pName(*iit);
	}
	cout << "}";
}

