// -*- C++ -*-

/*!
 * \file    siphontrap.cc
 *
 * \brief   Class SiphonTrap
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/07/02
 *
 * \date    $Date: 2012-11-20 14:47:01 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */


// include header files
#include "siphontrap.h"
#include "config-log.h"
#include "verbose.h"

using std::cout;
using std::endl;
using std::string;

extern bool texit;

/** Constructor. Computes all elementary siphons and traps.
	@param subnet The SubNet for which to compute elementary siphons and traps.
*/
SiphonTrap::SiphonTrap(const SubNet& subnet) : net(subnet),im(subnet.getMatrix()) {}

/** Function to compute all elementary siphons ans traps.
*/
void SiphonTrap::computeBruteForce() {
	elementaryTraps();
	if (texit) return;
	elementarySiphons();
}

/** Function to compute all p-minimal elementary siphons.
	@param p A place to be contained in all the elementary siphons.
*/
void SiphonTrap::elementarySiphons(PlaceID p) {
	initData(p,true);
	minst.clear();
	elemRec(p,true);
	pfirst.clear(); // for later use as dummy
}

/** Function to compute all p-minimal elementary traps.
	@param p A place to be contained in all the elementary traps.
*/
void SiphonTrap::elementaryTraps(PlaceID p) {
	initData(p,false);
	minst.clear();
	elemRec(p,false);
	pfirst.clear(); // for later use as dummy
}

/** Function to compute elementary siphons or traps from an
	initialised data structure.
	@param fixp A fixed place that must be contained in the result.
	@param siphon True for computation of siphons, false for traps.
*/
void SiphonTrap::elemRec(PlaceID fixp, bool siphon) {
	while (tindex>=0) {
		if (texit) return;
		if (tindex>=tlast) {
			if (checkMin(fixp,siphon)) save(fixp);
			--tindex;
			continue;
		}
		TransitionID t(tline[tindex]);
		PlaceID p;
		if (!titinit[t]) {
			const TArcs& tpre(siphon ? im.getTPreset(t) : im.getTPostset(t));
			if (tpre.empty()) { --tindex; continue; }
			tselect[t]=tpre.begin();
			titinit[t]=true;
			TArcs::const_iterator it;
			for(it=tpre.begin(); net.checkP(it,tpre.end()); ++it) {
				if (pvec[it->first]) {
					tselect[t]=tpre.end();
					break;
				}
			}
			if (tselect[t]!=tpre.end()) {
				p=tselect[t]->first;
				if (!pvec[p]) {
					pvec[p]=true;
					pgenby[p]=t;
					pfirst[p]=tlast;
					const PArcs& pre(siphon ? im.getPPreset(p) : im.getPPostset(p));
					PArcs::const_iterator it2;
					for(it2=pre.begin(); net.checkT(it2,pre.end()); ++it2) {
						if (!tvec[it2->first]) {
							tvec[it2->first]=true;
							titinit[it2->first]=false;
							tline[tlast++]=it2->first;
						}
					}
				}
			}
			++tindex;
		} else {
			const TArcs::const_iterator& stend(siphon ? im.getTPreset(t).end() : im.getTPostset(t).end());
			if (tselect[t]!=stend) {
				p=tselect[t]->first;
				if (pgenby[p]==t) {
					pvec[p]=false;
					for(unsigned int i=pfirst[p]; i<tlast; ++i)
						tvec[tline[i]]=false;
					tlast=pfirst[p];
				}
				++(tselect[t]);
				net.checkP(tselect[t],stend);
			}
			if (tselect[t]==stend) {
				titinit[t]=false;
				--tindex;
			} else {
				p=tselect[t]->first;
				if (!pvec[p]) {
					pvec[p]=true;
					pgenby[p]=t;
					pfirst[p]=tlast;
					const PArcs& pre(siphon ? im.getPPreset(p) : im.getPPostset(p));
					PArcs::const_iterator it;
					for(it=pre.begin(); net.checkT(it,pre.end()); ++it) {
						if (!tvec[it->first]) {
							tvec[it->first]=true;
							titinit[it->first]=false;
							tline[tlast++]=it->first;
						}
					}
				}
				++tindex;
			}
		}
	}
}

/** Checks if a set just computed by elemRec() is minimal.
	@param fixp A fixed place that must be contained in the result.
	@param siphon True for computation of siphons, false for traps.
*/
bool SiphonTrap::checkMin(PlaceID fixp, bool siphon) const {
	vector<bool> check(pvec);
	for(unsigned int i=0; i<tlast; ++i)
	{
		unsigned int cnt(0);
		PlaceID p;
		TArcs::const_iterator it;
		const TArcs& pre(siphon ? im.getTPreset(tline[i]) : im.getTPostset(tline[i]));
		for(it=pre.begin(); net.checkP(it,pre.end()); ++it)
			if (pvec[it->first]) {
				++cnt;
				p = it->first;
			}
		if (cnt<2) check[p]=false;
	}
	for(PlaceID i=0; i<check.size(); ++i)
		if (check[i] && i!=fixp) return false;
	return true;
}

/** Saves the just computed siphon or trap to the list of p-minimal elementary sets.
	@param fixp A fixed place that must be contained in the result.
*/
void SiphonTrap::save(PlaceID fixp) {
	vector<PlaceID> vec;
	for(PlaceID i=0; i<pvec.size(); ++i)
		if (pvec[i]) vec.push_back(i);
	minst[vec]=fixp;
}

/** Initialise data structures for computing p-minimal siphons or traps.
	@param p The place p that must be contained in the siphons or traps.
	@param siphons True for computation of siphons, false for traps.
*/
void SiphonTrap::initData(PlaceID p, bool siphon) {
	pvec.assign(im.numPlaces(),false);
	tvec.assign(im.numTransitions(),false);
	titinit.assign(im.numTransitions(),false);
	tselect.resize(im.numTransitions());
	tline.resize(im.numTransitions());
	pgenby.resize(im.numPlaces());
	pfirst.resize(im.numPlaces());
	pvec[p]=true;
	pgenby[p]=-1;
	pfirst[p]=0;
	tlast=0;
	PArcs::const_iterator it;
	const PArcs& pre(siphon ? im.getPPreset(p) : im.getPPostset(p));
	for(it=pre.begin(); net.checkT(it,pre.end()); ++it) {
		tvec[it->first]=true;
		titinit[it->first]=false;
		tline[tlast++]=it->first;
	}
	tindex=0;
}

/** Print a just computed siphon or trap.
*/
void SiphonTrap::print() const {
	for(PlaceID i=0; i<pvec.size(); ++i)
		if (pvec[i]) cout << im.pName(i) << " ";
	cout << endl;
}

/** Print a given siphon or trap. Input places are marked with a preceding
	star, output places with a trailing star.
	@param set1 The siphon or trap to be printed.
*/
void SiphonTrap::printSiphonTrap(const vector<PlaceID>& set1) const {
	bool comma(false);
	for(unsigned int i=0; i<set1.size(); ++i)
	{
		if (comma) cout << " "; else comma=true;
		if (net.isInput(set1.at(i))) cout << "*";
		cout << im.pName(set1.at(i));
		if (net.isOutput(set1.at(i))) cout << "*";
	}
}

/** Print a siphon with a given ID.
	@param ID The ID of the siphon.
*/
void SiphonTrap::printSiphon(SiphonID ID) const {
	printSiphonTrap(esptr[ID]->first);
}

/** Print a trap with a given ID.
	@param ID The ID of the trap.
*/
void SiphonTrap::printTrap(TrapID ID) const {
	printSiphonTrap(etptr[ID]->first);
}

/** Print all elementary siphons with generator p.
	@param p The place from which the elementary siphons originate.
*/
void SiphonTrap::printElementarySiphons(PlaceID p) const {
		if (!net.interface(p)) return;
		cout << "Elementary Siphons for ";
		if (net.isInput(p)) cout << "Input"; else cout << "Output";
		cout << " Place " << im.pName(p) << ":" << endl;
		const SiphonList& IDs(getSiphonIDs(p));
		for(unsigned int i=0; i<IDs.size(); ++i)
		{
			const Siphon& siphon(getSiphon(IDs[i]));
			cout << "[" << IDs[i] << "]: ";
			for(unsigned int j=0; j<siphon.size(); ++j)
			{
				if (net.isInput(siphon.at(j))) cout << "*";
				cout << im.pName(siphon.at(j));
				if (net.isOutput(siphon.at(j))) cout << "*";
				cout << " ";
			}
			cout << endl;
		}
}

/** Print all elementary traps with generator p.
	@param p The place from which the elementary traps originate.
*/
void SiphonTrap::printElementaryTraps(PlaceID p) const {
		if (!net.isMarked(p) && !net.interface(p)) return;
		if (net.isMarked(p)) { 
			cout << "Token-";
			if (net.interface(p)) cout << "Interface-";
		} else cout << "Interface-";
		cout << "Elementary Traps for ";
		if (net.isInput(p)) cout << "Input";
		else if (net.isOutput(p)) cout << "Output";
		else cout << "initially marked";
		cout << " Place " << im.pName(p) << ":" << endl;
		const TrapList& IDs(getTrapIDs(p));
		for(unsigned int i=0; i<IDs.size(); ++i)
		{
			const Trap& trap(getTrap(IDs[i]));
			for(unsigned int j=0; j<trap.size(); ++j)
			{
				if (net.isInput(trap.at(j))) cout << "*";
				cout << im.pName(trap.at(j));
				if (net.isOutput(trap.at(j))) cout << "*";
				cout << " ";
			}
			cout << endl;
		}
}

/** Compute all elementary siphons. There may be redundant siphons in the result.
*/
void SiphonTrap::elementarySiphons() {
	vector<PlaceID> places(net.getInput());
	places.insert(places.end(), net.getOutput().begin(), net.getOutput().end());
	for(unsigned int i=0; i<places.size(); ++i)
	{
		elementarySiphons(places[i]);
		map<vector<PlaceID>,PlaceID>::iterator mit;
		for(mit=minst.begin(); mit!=minst.end(); ++mit)
			elemsiphons[mit->first].insert(mit->second);
	}
	pes.resize(im.numPlaces());
	map<Siphon,Generators>::iterator eit;
	for(eit=elemsiphons.begin(); eit!=elemsiphons.end(); ++eit)
	{
		SiphonID id(esptr.size());
		esptr.push_back(eit);
		Interface interface;
		for(unsigned int pid=0; pid<eit->first.size(); ++pid)
			if (net.interface(eit->first[pid])) interface.insert(eit->first[pid]);
		sitoid[interface].push_back(id);
		Generators::iterator xit;
		for(xit=eit->second.begin(); xit!=eit->second.end(); ++xit)
			pes[*xit].push_back(id);
	}
}

/** Compute all interface-elementary and token-elementary traps.
*/
void SiphonTrap::elementaryTraps() {
	vector<PlaceID> places(net.getMarked());
	places.insert(places.end(), net.getInput().begin(), net.getInput().end());
	places.insert(places.end(), net.getOutput().begin(), net.getOutput().end());
	vector<bool> done(im.numPlaces(),false);
	for(unsigned int i=0; i<places.size(); ++i)
	{
		if (done[i]) continue; else done[i]=true;
		elementaryTraps(places[i]);
		map<vector<PlaceID>,PlaceID>::iterator mit;
		for(mit=minst.begin(); mit!=minst.end(); ++mit)
			elemtraps[mit->first].insert(mit->second);
	}
	pet.resize(im.numPlaces());
	map<Trap,Generators>::iterator eit;
	for(eit=elemtraps.begin(); eit!=elemtraps.end(); ++eit)
	{
		TrapID id(etptr.size());
		etptr.push_back(eit);
		etmarked.push_back(false);
		Interface interface;
		for(unsigned int pid=0; pid<eit->first.size(); ++pid)
		{
			if (net.interface(eit->first[pid])) interface.insert(eit->first[pid]);
			if (net.isMarked(eit->first[pid])) etmarked[id]=true;
		}
		bool ie(false), me(false);
		Generators::iterator xit;
		for(xit=eit->second.begin(); xit!=eit->second.end(); ++xit)
		{
			pet[*xit].push_back(id);
			if (net.isMarked(*xit)) me=true;
			if (net.interface(*xit)) ie=true;
		}
		if (ie) titoid[interface].push_back(id);
		if (me) mtitoid[interface].push_back(id);
	}
}

/** Get a siphon by ID.
	@param ID The ID of the siphon.
	@return The siphon.
*/
const Siphon& SiphonTrap::getSiphon(SiphonID ID) const { return esptr.at(ID)->first; }

/** Get a trap by ID.
	@param ID The ID of the trap.
	@return The trap.
*/
const Trap& SiphonTrap::getTrap(TrapID ID) const { return etptr.at(ID)->first; }

/** Get the IDs of all p-generated elementary siphons.
	@param p The generator place for the elementary siphons.
	@return A list of siphon IDs.
*/
const SiphonList& SiphonTrap::getSiphonIDs(PlaceID p) const { return pes.at(p); }

/** Get the IDs of all p-generated elementary traps.
	@param p The generator place for the elementary traps, usually only works
			for interface or marked places.
	@return A list of trap IDs.
*/
const TrapList& SiphonTrap::getTrapIDs(PlaceID p) const { return pet.at(p); }

/** Get the IDs of all elementary siphons with a given interface.
	@param interface The exact interface for all the siphons to be extracted.
	@return A list of all elementary siphons with the given interface.
*/
const SiphonList& SiphonTrap::getSiphonIDs(Interface& interface) const { 
	map<Interface,SiphonList>::const_iterator it(sitoid.find(interface));
	if (it==sitoid.end()) {
//		pfirst.clear();
		return pfirst; // empty (dummy)
	}
	return it->second;
}

/** Get the IDs of all interface-elementary traps with a given interface.
	@param interface The exact interface for all the traps to be extracted.
	@return A list of all interface-elementary traps with the given interface.
*/
const TrapList& SiphonTrap::getIETrapIDs(Interface& interface) const {
	map<Interface,TrapList>::const_iterator it(titoid.find(interface));
	if (it==titoid.end()) {
//		pfirst.clear();
		return pfirst; // dummy
	}
	return it->second;
}

/** Get the IDs of all token-elementary traps with a given interface.
	@param interface The exact interface for all the traps to be extracted.
	@return A list of all token-elementary traps with the given interface.
*/
const TrapList& SiphonTrap::getTETrapIDs(Interface& interface) const {
	map<Interface,TrapList>::const_iterator it(mtitoid.find(interface));
	if (it==mtitoid.end()) {
//		pfirst.clear();
		return pfirst; // dummy
	}
	return it->second;
}

/** Get the number of all stored elementary siphons. Siphon IDs start with
	zero and go up to this number minus one.
	@return The number of elementary siphons.
*/
unsigned int SiphonTrap::numSiphons() const { return esptr.size(); }

/** Get the number of all stored elementary traps. Trap IDs start with
	zero and go up to this number minus one.
	@return The number of elementary traps.
*/
unsigned int SiphonTrap::numTraps() const { return etptr.size(); }

/** Check if a given trap is marked.
	@param trapID The ID of the trap to be checked.
	@return If the trap is marked.
*/
bool SiphonTrap::isMarked(TrapID trapID) const { return etmarked[trapID]; }

