// -*- C++ -*-

/*!
 * \file    matchings.cc
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


// include header files
#include "matchings.h"

using std::cout;
using std::endl;

extern bool texit;

/** Constructor. Computes all three types of matchings from the children information
	about siphons and traps.
	@param i1 The info for the first child.
	@param i2 The info for the second child.
	@param mt True if closed interface-trap matchings can be omitted.
*/
Matchings::Matchings(const SubNetInfo& i1, const SubNetInfo& i2, bool mt) : inf1(i1),inf2(i2) {
	computeMatchings(SIPHON,true);
	computeMatchings(ITRAP,mt);
	computeMatchings(TTRAP,true);
	computeMatchingLists();
}

/** Compute the matchings of a certain type.
	@param type Possible types are SIPHON, ITRAP, and TTRAP.
	@param mt True if closed interface-trap matchings can be omitted (ITRAP only).
*/
void Matchings::computeMatchings(STType type, bool mt) {
	// the common interface set of the two subnets
	Interface common(getCommonInterface());

	// uchoices contains all possible selections of interfaces for a matching,
	// one list for each recursion step. The list for a step n depends on the
	// choice for a previous step (found in uselect[]). In step 1 all interfaces
	// originating from places not in the common interface are selectable, as
	// at least one external interface place must be contained in a matching
	// (exception: token trap interfaces, where a place with tokens must be contained).
	// In consecutive steps interfaces originating from a common interface place
	// are selectable if that interface place has not been covered so far.
	vector<vector<ExtInterfaceID> > uchoices; // -j=jth interface of first net, +j=j-1st interface of second net
	vector<int> uselect; // -1 = none selected, position in uchoices[] otherwise

	// the unique element of an interface. This is either the interface place from which
	// an elementary trap/siphon originates or -1. The latter means that the interface
	// consists of only one place (input for siphons, output for interface traps)
	// or that the originating place of a token trap is an inner place.
	vector<ExtGenerator> ufirst;

	// the unique element from which an elementary trap/siphon
	// should originate for this recursion step
	vector<ExtGenerator> unique;

	// to mark places from the common interface as covered by the (partially
	// constructed) matching
	vector<bool> ucommon(inf1.net.getMatrix().numPlaces(),false);

	// init
	unique.push_back(0); // first element has no meaning
	vector<ExtInterfaceID> uinit; // initial step for uchoices[]
	map<SiphonID,Interface>::const_iterator iit;
	switch (type) {
		case SIPHON:
						for(iit=inf1.getIntMap().begin(); iit!=inf1.getIntMap().end(); ++iit)
						{
							int ue(inf1.getUniqueElement(iit->second,true));
							if ((ue>=0 && common.find(ue)==common.end())
								|| (ue<0 && common.find(*(iit->second.begin()))==common.end()))
							{
								uinit.push_back(-(iit->first)-1);
								ufirst.push_back(ue);
							}
						}
						for(iit=inf2.getIntMap().begin(); iit!=inf2.getIntMap().end(); ++iit)
						{
							int ue(inf2.getUniqueElement(iit->second,true));
							if ((ue>=0 && common.find(ue)==common.end())
								|| (ue<0 && common.find(*(iit->second.begin()))==common.end()))
							{
								uinit.push_back(iit->first);
								ufirst.push_back(ue);
							}
						}
						break;
		case ITRAP:		for(unsigned int i=0; i<inf1.getLQ().size(); ++i)
						{
							int ue(inf1.getUniqueElement(inf1.getLQ(i),false));
							if (!mt || (ue>=0 && common.find(ue)==common.end())
								|| (ue<0 && common.find(*(inf1.getLQ(i).begin()))==common.end()))
							{
								uinit.push_back(-i-1);
								ufirst.push_back(ue);
							}
						}
						for(unsigned int i=0; i<inf2.getLQ().size(); ++i)
						{
							int ue(inf2.getUniqueElement(inf2.getLQ(i),false));
							if (!mt || (ue>=0 && common.find(ue)==common.end())
								|| (ue<0 && common.find(*(inf2.getLQ(i).begin()))==common.end()))
							{
								uinit.push_back(i);
								ufirst.push_back(ue);
							}
						}
						break;
		case TTRAP:		if (mt) for(unsigned int i=0; i<inf1.getLM().size(); ++i)
						{
							int ue(inf1.getUniqueElement(inf1.getLM(i),false));
							uinit.push_back(-i-1);
							ufirst.push_back(ue);
						}
						if (mt) for(unsigned int i=0; i<inf2.getLM().size(); ++i)
						{
							int ue(inf2.getUniqueElement(inf2.getLM(i),false));
							uinit.push_back(i);
							ufirst.push_back(ue);
						}
	}
	uchoices.push_back(uinit);
	uselect.push_back(-1);

	// document the position in uchoices where the selections for the uncovered 
	// common interface places originating from this step in uchoices start.
	vector<unsigned int> ustartpos;
	// ufree denotes the first unused position in uchoices (size() may be larger temporarily)
	unsigned int ufree(1);
	ustartpos.push_back(ufree);
	// active position in the uchoices vector; this is the recursion step we are in
	int pos(0);
	// unique element and selected interface in the active recursion step
	ExtGenerator ue;
	ExtInterfaceID elt;

	// main loop
	while (pos>=0) {
		if (texit) return; // in case another thread has a solution
		// deselect old element at pos
		ue = unique.at(pos);
		if (uselect.at(pos)>-1) {
			if (ufree>ustartpos.at(pos)) {
				for(unsigned int i=ustartpos[pos]; i<ufree; ++i)
					ucommon[unique[i]]=false;
				ufree = ustartpos.at(pos);
				uchoices.resize(ufree);
				uselect.resize(ufree);
				unique.resize(ufree);
				ustartpos.resize(ufree);
			}
		}

		// select next element at pos or backtrack
		++uselect.at(pos);
		if (uselect[pos]>=uchoices[pos].size()) { uselect[pos]=-1; --pos; continue; }

		// construct new choices
		ustartpos.at(pos) = ufree;
		if (pos==0) ue = ufirst[uselect[pos]];
		elt = uchoices[pos][uselect[pos]];
		bool side(elt<0);
		const SubNetInfo& inf(elt<0?inf1:inf2);
		const SubNetInfo& infalt(elt<0?inf2:inf1);
		if (elt<0) elt=-elt-1;
		const Interface& places(type==SIPHON?inf.getIntMap(elt):(type==ITRAP||pos>0?inf.getLQ(elt):inf.getLM(elt)));
		Interface::const_iterator sit;
		for(sit=places.begin(); sit!=places.end(); ++sit)
		{
			if (ucommon[*sit]) continue; // do nothing if we already created a selection list for this place
			if (common.find(*sit)==common.end()) continue; // do nothing outside the common interface
			ucommon[*sit]=true;
			unique.push_back(*sit);
			uselect.push_back(-1);
			vector<ExtInterfaceID> utmp;
			if (type==SIPHON) {
				if (*sit==ue) continue; // do nothing for the unique place
				map<SiphonID,Interface>::const_iterator iit;
				for(iit=infalt.getIntMap().begin(); iit!=infalt.getIntMap().end(); ++iit)
				{
					int ui(infalt.getUniqueElement(iit->second,true));
					if (ui==*sit) utmp.push_back(side?iit->first:-(iit->first)-1);
				}
			} else {
				for(unsigned int i=0; i<infalt.getLQ().size(); ++i)
				{
					int ui(infalt.getUniqueElement(infalt.getLQ(i),false));
					if (ui==*sit) utmp.push_back(side?i:-i-1);
					else if (ui<0 && ue==*sit) {
						set<PlaceID>::const_iterator outit(infalt.getLQ(i).begin());
						if (outit!=infalt.getLQ(i).end())
							if (*outit==*sit) utmp.push_back(side?i:-i-1);
					}
				}
			}
			uchoices.push_back(utmp);
			ustartpos.push_back(ufree);
			++ufree;
		}

		// do we have a complete matching?
		if (pos+1<uselect.size()) { ++pos; continue; }

		// matching is complete, get it and its interface and store it
		set<InterfaceID> ltmp,rtmp;
		vector<InterfaceID> lmatch,rmatch;
		Interface interface;
		for(unsigned int i=0; i<uselect.size(); ++i)
		{
			elt = uchoices[i][uselect[i]];
			bool xside(elt<0);
			const SubNetInfo& xinf(elt<0?inf1:inf2);
			if (elt<0) elt=-elt-1;
			const Interface& xplaces(type==SIPHON?xinf.getIntMap(elt):(type==ITRAP||i>0?xinf.getLQ(elt):xinf.getLM(elt)));
			for(sit=xplaces.begin(); sit!=xplaces.end(); ++sit)
				if (common.find(*sit)==common.end()) 
					interface.insert(*sit);
			if (i==0 && type==TTRAP) continue;
			if (xside) ltmp.insert(elt);
			else rtmp.insert(elt);
		}
		lmatch.assign(ltmp.begin(),ltmp.end());
		rmatch.assign(rtmp.begin(),rtmp.end());

		switch (type) {
			case SIPHON:	swmatch.push_back(make_pair(lmatch,rmatch));
							swmatchif.push_back(interface);
							break;
			case ITRAP:		itmatch.push_back(make_pair(lmatch,rmatch));
							itmatchif.push_back(interface);
							break;
			case TTRAP:		ttmatch.push_back(make_pair(lmatch,rmatch));
							ttmatchif.push_back(interface);
							elt = uchoices[0][uselect[0]];
							ttmatchtt.push_back(elt<0?-elt-1:elt);
							ttmatchside.push_back(elt>=0);
							break;
		}
		
	}

}

/** Get the common part of the interfaces of the children.
	@return The common part of the interfaces.
*/
Interface Matchings::getCommonInterface() const {
	Interface common;
	const vector<PlaceID>& input1(inf1.net.getInput());
	for(unsigned int i=0; i<input1.size(); ++i)
		if (inf2.net.isOutput(input1[i])) common.insert(input1[i]);
	const vector<PlaceID>& input2(inf2.net.getInput());
	for(unsigned int i=0; i<input2.size(); ++i)
		if (inf1.net.isOutput(input2[i])) common.insert(input2[i]);
	return common;
}

/** Print all matching of a certain type, including the interfaces.
	@param type The type of matchings (SIPHON, ITRAP, TTRAP).
*/
void Matchings::printMatchings(STType type) const {
	const vector<Matching>& tmp((type==SIPHON?swmatch:(type==ITRAP?itmatch:ttmatch)));
	bool comma1(false);
	for(unsigned int i=0; i<tmp.size(); ++i)
	{
		if (comma1) cout << ", "; else comma1=true;
		printMatching(i,type);
		printMatchingInterface(i,type);
	}
}

/** Print a given matching.
	@param ID The ID of the matching to be printed.
	@param type The type of the matching to be printed.
*/
void Matchings::printMatching(MatchingID ID, STType type) const {
	const vector<Matching>& tmp((type==SIPHON?swmatch:(type==ITRAP?itmatch:ttmatch)));
	cout << "[{";
	bool comma2(false);
	if (type>1 && !ttmatchside[ID]) { cout << ttmatchtt[ID] << "!"; comma2=true; }
	for(unsigned int j=0; j<tmp[ID].first.size(); ++j)
	{
		if (comma2) cout << ","; else comma2=true;
		if (type>1 && j==0 && tmp[ID].first.empty()) { cout << "!"; ++j; }
		cout << tmp[ID].first[j];
	}
	cout << "},{";
	comma2=false;
	if (type>1 && ttmatchside[ID]) { cout << ttmatchtt[ID] << "!"; comma2=true; }
	for(unsigned int j=0; j<tmp[ID].second.size(); ++j)
	{
		if (comma2) cout << ","; else comma2=true;
		if (type>1 && j==0 && tmp[ID].second.empty()) { cout << "!"; ++j; }
		cout << tmp[ID].second[j];
	}
	cout << "}]";
}

/** Print the interface of a given matching.
	@param ID The ID of the matching.
	@param type The type of the matching.
*/
void Matchings::printMatchingInterface(unsigned int ID, STType type) const {
	cout << " -> {";
	Interface interface(getMatchingInterface(ID,type));
	Interface::iterator iit;
	bool comma(false);
	for(iit=interface.begin(); iit!=interface.end(); ++iit)
	{
		if (comma) cout << ","; else comma=true;
		cout << inf1.net.getMatrix().pName(*iit);
	}
	cout << "}";
}

/** Get a matching by ID.
	@param ID The ID of the matching.
	@param type The type of the matching.
	@return The matching itself.
*/
const Matching& Matchings::getMatching(MatchingID ID, STType type) const {
	switch (type) {
		case 0: return swmatch.at(ID);
		case 1: return itmatch.at(ID);
		case 2: return ttmatch.at(ID);
	}
}

/** Get the unique token trap in a matching of the type TTRAP.
	@param ID The ID of the selected matching.
	@return The position of the token trap.
*/
unsigned int Matchings::getTokenTrap(MatchingID ID) const {
	return ttmatchtt.at(ID);
}

/** Find out which child contains the unique token trap in a matching with type TTRAP.
	@param ID The selected matching.
	@return True if the token trap is contained in the second child.
*/
bool Matchings::getTokenTrapSide(MatchingID ID) const {
	return ttmatchside.at(ID);
}

/** Get the interface of a matching.
	@param ID The ID of the selected matching.
	@param type The type of the selected matching.
	@return The interface of the matching.
*/
const Interface& Matchings::getMatchingInterface(MatchingID ID, STType type) const {
	switch (type) {
		case SIPHON:	return swmatchif.at(ID);
		case ITRAP:		return itmatchif.at(ID);
		case TTRAP:		return ttmatchif.at(ID);
	}
}

/** Get the number of matching of a type. Matching IDs range from 0 to that number minus one.
	@param type The type of matchings (SIPHON, ITRAP, TTRAP).
	@return The number of matchings in existence.
*/
unsigned int Matchings::numMatchings(STType type) const {
	switch (type) {
		case SIPHON: return swmatch.size();
		case ITRAP: return itmatch.size();
		case TTRAP: return ttmatch.size();
	}
}

/** Compute a mapping from the children's siphons to the matchings (type SIPHON) they are contained in. */
void Matchings::computeMatchingLists() {
	for(unsigned int i=0; i<swmatch.size(); ++i)
	{
		for(unsigned int j=0; j<swmatch[i].first.size(); ++j)
			lsmlist[swmatch[i].first[j]].push_back(i);
		for(unsigned int j=0; j<swmatch[i].second.size(); ++j)
			rsmlist[swmatch[i].second[j]].push_back(i);
	}
}

/** Get the list of matchings a siphon is contained in.
	@param siphon The ID of the siphon.
	@param side The child in which the siphon resides (true = right, second child).
	@return The list of matchings containing the siphon.
*/
const MatchingList& Matchings::getMatchingList(SiphonID siphon, bool side) const {
	map<SiphonID,MatchingList>::const_iterator mlist(side ? rsmlist.find(siphon) : lsmlist.find(siphon));
	map<SiphonID,MatchingList>::const_iterator mlend(side ? rsmlist.end() : lsmlist.end());
	if (mlist==mlend) return mdummy; // in case the siphon does not appear in any matching
	return mlist->second;
}

/** Get the child's additional info about siphons and traps.
	@param side True means the right, second child.
	@return The info structure.
*/
const SubNetInfo& Matchings::getSNI(bool side) const {
	return (side ? inf2 : inf1);
}

