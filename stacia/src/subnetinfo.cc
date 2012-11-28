// -*- C++ -*-

/*!
 * \file    subnetinfo.cc
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


// include header files
#include "subnetinfo.h"
#include "matchings.h"
#include "sthread.h"
#include "config-log.h"
#include "verbose.h"

using std::cout;
using std::endl;
using std::string;

extern bool texit;
extern bool multithreaded;
extern pthread_mutex_t main_mutex;
extern pthread_cond_t main_cond;
extern set<unsigned int> tindex;
extern set<unsigned int> idleID;
extern unsigned int maxthreads;
extern unsigned int siphonthreshold;

/** Constructor.
	@param subnet The subnet for which the info is constructed.
*/
SubNetInfo::SubNetInfo(const SubNet& subnet) : net(subnet),im(subnet.getMatrix()) {}

/** Inclusion test for siphon lists (ordered vectors).
	@param set1 A vector of monotonely increasing numbers.
	@param set2 A vector of monotonely increasing numbers.
	@return True if set2 is a subset of set1 or equal to it.
*/
bool SubNetInfo::includes(const SiphonList& set1, const SiphonList& set2) const {
	unsigned int j(0);
	for(unsigned int i=0; i<set2.size(); ++i)
	{
		while (j<set1.size() && set1.at(j)<set2.at(i)) ++j;
		if (j==set1.size() || set1.at(j)>set2.at(i)) return false;
	}
	return true;
}

/*
bool SubNetInfo::includes(const vector<unsigned int>& set1, const set<unsigned int>& set2) const {
	unsigned int j(0);
	set<unsigned int>::iterator sit;
	for(sit=set2.begin(); sit!=set2.end(); ++sit)
	{
		while (j<set1.size() && set1.at(j)<*sit) ++j;
		if (j==set1.size() || set1.at(j)>*sit) return false;
	}
	return true;
}
*/

/** Inclusion test for interfaces.
	@param set1 An interface.
	@param set2 An interface.
	@return True if set2 is a subset of set1 or equal to it.
*/
bool SubNetInfo::includes(const Interface& set1, const Interface& set2) const {
	if (set2.empty()) return true;
	if (set1.size()<set2.size()) return false;
	Interface::const_iterator sit1,sit2;
	for(sit1=set1.begin(),sit2=set2.begin(); sit1!=set1.end(); ++sit1)
	{
		if (*sit2<*sit1) break;
		else if (*sit1==*sit2) ++sit2;
		if (sit2==set2.end()) return true;
	}
	return false;
}

/** Compute the subnet info from a SiphonTrap object.
	@param stobj An object containing all elementary siphons and traps of the subnet.
	@return True if the computation was stopped prematurely by another thread.
*/
bool SubNetInfo::computeComponentInfo(const SiphonTrap& stobj) {
	st = &stobj;

	// define map from siphons to their interfaces
	map<Interface,SiphonList>::const_iterator oit;
	for(oit=st->sitoid.begin(); oit!=st->sitoid.end(); ++oit)
		for(unsigned int i=0; i<oit->second.size(); ++i)
			intmap[oit->second[i]] = oit->first;
	if (intmap.size()>=siphonthreshold) { texit=true; return true; }

	// define interfaces of interface-elementary traps
	map<Interface,TrapList>::const_iterator oit2;
	for(oit2=st->titoid.begin(); oit2!=st->titoid.end(); ++oit2)
		LQ.push_back(oit2->first);

	// define interfaces of token-elementary traps
	for(oit2=st->mtitoid.begin(); oit2!=st->mtitoid.end(); ++oit2)
		LM.push_back(oit2->first);

	// define wrapping sets for trap interfaces
	for(InterfaceID i=0; i<LQ.size(); ++i)
		computeWrappingSets(wQ,i,LQ[i],st->titoid.find(LQ[i])->second);

	// define wrapping sets for token trap interfaces
	for(InterfaceID i=0; i<LM.size(); ++i)
		computeWrappingSets(wM,i,LM[i],st->mtitoid.find(LM[i])->second);

	// check for and eliminiate redundant siphons
	bool redundant(true);
	while (redundant && !texit)
	{
		redundant=false;
		map<SiphonID,Interface>::iterator iit;
		for(iit=intmap.begin(); iit!=intmap.end(); ++iit)
			if (checkRedundancy(iit->first)) break;
		if (iit!=intmap.end()) redundant=true;
		if (redundant) removeRedundant(iit->first);
	}
	return false;
}

/** Compute wrapping sets for info structure derived from a SiphonTrap object.
	@param w The structure where wrapping sets are stored.
	@param wpos The ID of the trap interface for which the wrapping sets should be computed.
	@param Li The trap interface for which the wrapping sets should be computed.
	@param lif The list of traps with the interface Li.
*/
void SubNetInfo::computeWrappingSets(map<InterfaceID,set<Wrapping> >& w, InterfaceID wpos, const Interface& Li, const TrapList& lif) {
	for(unsigned int i=0; i<lif.size(); ++i)
	{
		const Trap& trap(st->etptr[lif.at(i)]->first);
		vector<unsigned int> matched(trap.size(),0);
		SiphonList siphons;
		vector<vector<unsigned int> > wrap; // values are positions in trap
		unsigned int pos(1);
		while (pos>0) {
			if (texit) return;
			if (pos>siphons.size()) {
				SiphonID val(siphons.empty() ? 0 : siphons.back()+1);
				if (val>=st->esptr.size()) { --pos; continue; }
				siphons.push_back(val);
				wrap.push_back(vector<unsigned int> ());
			} else if (siphons.back()+1<st->esptr.size()) {
				++siphons.back();
				for(unsigned int j=0; j<wrap.back().size(); ++j)
					--matched[wrap.back()[j]];
				wrap.back().clear();
			} else {
				siphons.pop_back();
				for(unsigned int j=0; j<wrap.back().size(); ++j)
					--matched[wrap.back()[j]];
				wrap.pop_back();
				--pos;
				continue;
			}
			const Siphon& siphon(st->esptr[siphons.back()]->first);
			unsigned int j,k;
			bool selectok(false);
			for(j=0,k=0; j<trap.size() && k<siphon.size(); )
			{
				if (trap.at(j)==siphon.at(k)) {
					if (matched[j]==0) selectok=true;
					wrap.back().push_back(j);
					++matched[j];
					++j;
					++k;
				} else if (trap.at(j)<siphon.at(k)) ++j;
				else ++k;
			}
			if (!selectok) continue;
			for(j=0; j<trap.size(); ++j) if (!matched[j]) break;
			if (j==trap.size()) {
				for(k=0; k<wrap.size(); ++k)
				{
					selectok=false;
					for(unsigned int l=0; l<wrap[k].size(); ++l)
						if (matched[wrap[k][l]]==1) selectok=true;
					if (!selectok) break;
				}
				if (selectok) w[wpos].insert(siphons);
				continue;
			}
			++pos;
		}
	}
}

/** Print all siphon IDs with their interfaces. */
void SubNetInfo::printIntmap() const {
	map<SiphonID,Interface>::const_iterator mit;
	Interface::const_iterator sit;
	bool comma2(false);
	for(mit=intmap.begin(); mit!=intmap.end(); ++mit)
	{
		if (comma2) cout << ", "; else comma2=true;
		cout << mit->first << "->{";
		bool comma(false);
		for(sit=mit->second.begin(); sit!=mit->second.end(); ++sit)
		{
			if (comma) cout << ","; else comma=true;
			cout << im.pName(*sit);
		}
		cout << "}";
	}
}

/** Print all interfaces of interface-elementary traps. */
void SubNetInfo::printLQ() const {
	Interface::const_iterator iit;
	bool comma(false);
	for(InterfaceID i=0; i<LQ.size(); ++i)
	{
		if (comma) cout << ", "; else comma=true;
		cout << "{";
		bool comma2(false);
		for(iit=LQ[i].begin(); iit!=LQ[i].end(); ++iit)
		{
			if (comma2) cout << ","; else comma2=true;
			cout << im.pName(*iit);
		}
		cout << "}";
	}
}

/** Print all interfaces of token-elementary traps. */
void SubNetInfo::printLM() const {
	Interface::const_iterator iit;
	bool comma(false);
	for(InterfaceID i=0; i<LM.size(); ++i)
	{
		if (comma) cout << ", "; else comma=true;
		cout << "{";
		bool comma2(false);
		for(iit=LM[i].begin(); iit!=LM[i].end(); ++iit)
		{
			if (comma2) cout << ","; else comma2=true;
			cout << im.pName(*iit);
		}
		cout << "}";
	}
}

/** Print the wrapping sets of interface-elementary traps. */
void SubNetInfo::printwQ() const {
	map<InterfaceID,set<Wrapping> >::const_iterator wit;
	Interface::const_iterator iit;
	set<Wrapping>::const_iterator lit;
	for(wit=wQ.begin(); wit!=wQ.end(); ++wit)
	{
		cout << "{";
		bool comma(false);
		for(iit=LQ[wit->first].begin(); iit!=LQ[wit->first].end(); ++iit)
		{
			if (comma) cout << ","; else comma=true;
			cout << im.pName(*iit);
		}
		cout << "} -> {";
		comma = false;
		for(lit=wit->second.begin(); lit!=wit->second.end(); ++lit)
		{
			if (comma) cout << ", "; else comma=true;
			cout << "{";
			bool comma2(false);
			for(unsigned int i=0; i<lit->size(); ++i)
			{
				if (comma2) cout << ","; else comma2=true;
				cout << (*lit)[i];
			}
			cout << "}";
		}
		cout << "}" << endl;
	}
}

/** Print the wrapping sets of token-elementary traps. */
void SubNetInfo::printwM() const {
	map<InterfaceID,set<Wrapping> >::const_iterator wit;
	Interface::const_iterator iit;
	set<Wrapping>::const_iterator lit;
	for(wit=wM.begin(); wit!=wM.end(); ++wit)
	{
		printInterface(LM[wit->first]);
		bool comma(false);
		cout << " -> {";
		for(lit=wit->second.begin(); lit!=wit->second.end(); ++lit)
		{
			if (comma) cout << ", "; else comma=true;
			printWrapping(*lit);
		}
		cout << "}" << endl;
	}
}

/** Print an interface.
	@param interface The interface.
*/
void SubNetInfo::printInterface(const Interface& interface) const {
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

/** Print a wrapping-
	@param wr The wrapping.
*/
void SubNetInfo::printWrapping(const Wrapping& wr) const {
	cout << "{";
	bool comma(false);
	for(unsigned int i=0; i<wr.size(); ++i)
	{
		if (comma) cout << ","; else comma=true;
		cout << wr[i];
	}
	cout << "}";
}

/** Print the full info contained in SubNetInfo. */
void SubNetInfo::printInfo() const {
	cout << "IntMap: ";
	printIntmap();
	cout << endl;
	cout << "LQ: ";
	printLQ();
	cout << endl;
	cout << "LM: ";
	printLM();
	cout << endl;
	cout << "wQ: " << endl;
	printwQ();
	cout << "wM: " << endl;
	printwM();
	cout << endl;
}

/** Check if a siphon is redundant.
	@param siphon The siphon's ID.
	@return True if redundant.
*/
bool SubNetInfo::checkRedundancy(SiphonID siphon) {
		vector<PlaceID> interface(net.getInterface());
		vector<unsigned int> matched(interface.size(),0);
		SiphonList siphons(1,siphon);
		vector<vector<unsigned int> > wrap;
		vector<vector<bool> > wQcover,wMcover; // first: pos, second: trap interfaces
		SiphonID maxsiphon(intmap.rbegin()->first+1); //st->esptr.size());
		unsigned int pos(2);
		bool first(true);
		while (pos>1) {
			if (texit) return false;
			if (first) {
				first=false;
				wrap.push_back(vector<unsigned int> ());
				wQcover.push_back(vector<bool> (LQ.size(),false));
				wMcover.push_back(vector<bool> (LM.size(),false));
				--pos;
			} else if (pos>siphons.size()) {
				SiphonID val(siphons.size()<2 ? 0 : siphons.back()+1);
				while (intmap.find(val)==intmap.end() && val<maxsiphon) ++val;
				if (val==siphons[0]) ++val;
				while (intmap.find(val)==intmap.end() && val<maxsiphon) ++val;
				if (val==maxsiphon) { --pos; continue; }
				siphons.push_back(val);
				wrap.push_back(vector<unsigned int> ());
				wQcover.push_back(wQcover.back());
				wMcover.push_back(wMcover.back());
			} else if (siphons.back()<maxsiphon) {
				++siphons.back();
				while (intmap.find(siphons.back())==intmap.end() && siphons.back()<maxsiphon) ++siphons.back();
				if (siphons.back()==siphons[0]) ++siphons.back();
				while (intmap.find(siphons.back())==intmap.end() && siphons.back()<maxsiphon) ++siphons.back();
				if (siphons.back()<maxsiphon) {
					for(unsigned int j=0; j<wrap.back().size(); ++j)
						--matched[wrap.back()[j]];
					wrap.back().clear();
					wQcover.pop_back();
					wQcover.push_back(wQcover.back());
					wMcover.pop_back();
					wMcover.push_back(wMcover.back());
				}
			} 
			if (siphons.back()==maxsiphon) {
				siphons.pop_back();
				for(unsigned int j=0; j<wrap.back().size(); ++j)
					--matched[wrap.back()[j]];
				wrap.pop_back();
				wQcover.pop_back();
				wMcover.pop_back();
				--pos;
				continue;
			}
			const Interface& siphon1(intmap.at(siphons.back()));
			Interface::const_iterator sit;
			unsigned int k;
			bool selectok(false);
			for(sit=siphon1.begin(),k=0; sit!=siphon1.end() && k<interface.size(); )
			{
				if (*sit<interface[k]) { ++sit; continue; }
				if (*sit>interface[k]) { ++k; continue; }
				if (matched[k]==0) selectok=true;
				wrap.back().push_back(k);
				++matched[k];
				++sit;
				++k;
			}

			if (!selectok) continue;
			for(k=0; k<wrap.size(); ++k)
			{
				selectok=false;
				for(unsigned int l=0; l<wrap[k].size(); ++l)
					if (matched[wrap[k][l]]==1) selectok=true;
				if (!selectok) break;
			}
			if (selectok) 
				if (!checkRedundancy(siphons,wQcover.back(),wMcover.back()))
					return false;
			unsigned int j;
			for(j=0; j<matched.size(); ++j) if (!matched[j]) break;
			if (j<matched.size()) ++pos;
		}
	return true;
}

/** Inner loop of the redundancy check.
	@param M1 A siphon list.
	@param wQcover Wrapping set info for interface-elementary traps.
	@param wMcover Wrapping set info for token-elementary traps.
	@return False if redundancy is impossible.
*/
bool SubNetInfo::checkRedundancy(const SiphonList& M1, vector<bool>& wQcover, vector<bool>& wMcover) {
	Interface M1i; // the interface of M1
	M1i.insert(intmap[M1.at(0)].begin(),intmap[M1.at(0)].end());
	SiphonList M2; // a set of siphons worse than M1, to be constructed
	Interface M2i; // the interface of M2
	map<SiphonID,Interface>::iterator iit;
	unsigned int s(1);
	for(iit=intmap.begin(); iit!=intmap.end(); ++iit)
	{
		while (s<M1.size() && M1.at(s)<iit->first) ++s;
		if (s<M1.size() && M1.at(s)==iit->first) 
			M1i.insert(iit->second.begin(), iit->second.end());
	}
	for(iit=intmap.begin(); iit!=intmap.end(); ++iit)
	{
		if (iit->first == M1.at(0)) continue; // do not include the test siphon in M2
		if (includes(M1i,iit->second)) 
		{
			M2.push_back(iit->first);
			M2i.insert(iit->second.begin(), iit->second.end());
		}
	}
	if (!includes(M2i,M1i)) return false; // can't have the same interface without the test siphon
	// generate a version of M1 with monotonic ordering
	SiphonList M1a(M1);
	SiphonID first(M1a[0]);
	for(unsigned int i=0; i<M1a.size(); ++i)
		if (i+1==M1a.size()) M1a[i]=first;
		else if (M1a[i+1]<first) M1a[i]=M1a[i+1];
		else { M1a[i]=first; break; }

	vector<Wrapping> wrapvec;
	vector<int> wrapptr;
	for(InterfaceID i=0; i<LQ.size(); ++i)
	{
		if (wQcover[i]) continue; // father already covers a wrapping for this trap interface
		if (!includes(M1i,LQ[i])) continue; // we don't cover the trap interface
		set<Wrapping>::iterator wit;
		for(wit=wQ[i].begin(); wit!=wQ[i].end(); ++wit)
			if (includes(M1a,*wit)) break;
		if (wit!=wQ[i].end()) { wQcover[i]=true; continue; }
		for(wit=wQ[i].begin(); wit!=wQ[i].end(); ++wit)
			if (includes(M2,*wit)) { wrapvec.push_back(*wit); wrapptr.push_back(-1); }
	}

	for(InterfaceID i=0; i<LM.size(); ++i)
	{
		if (wMcover[i]) continue; // father already covers a wrapping for this trap interface
		if (!includes(M1i,LM[i])) continue; // we don't cover the trap interface
		set<Wrapping>::iterator wit;
		for(wit=wM[i].begin(); wit!=wM[i].end(); ++wit)
			if (includes(M1a,*wit)) break;
		if (wit!=wM[i].end()) { wMcover[i]=true; continue; }
		for(wit=wM[i].begin(); wit!=wM[i].end(); ++wit)
			if (includes(M2,*wit)) { wrapvec.push_back(*wit); wrapptr.push_back(-1); }
	}

	Interface::iterator xit;
	map<PlaceID,unsigned int> M2intcnt;
	for(unsigned int i=0; i<M2.size(); ++i)
	{
		for(xit=intmap[M2[i]].begin(); xit!=intmap[M2[i]].end(); ++xit)
			++M2intcnt[*xit];
	}

	vector<bool> wrapsel(intmap.rbegin()->first+1,false); // [SiphonID]
	SiphonID siphon;
	int pos(0);
	while (pos>=0 && pos<wrapvec.size())
	{
		if (texit) return false;
		if (wrapptr[pos]==-2) {
			wrapptr[pos]=-1;
			--pos;
		} else if (wrapptr[pos]==-1) {
			unsigned int i;
			for(i=0; i<wrapvec[pos].size(); ++i)
				if (wrapsel[wrapvec[pos][i]]) break;
			if (i==wrapvec[pos].size()) {
				wrapptr[pos]=0;
				siphon = wrapvec[pos][0];
				wrapsel[siphon]=true;
				for(xit=intmap[siphon].begin(); xit!=intmap[siphon].end(); ++xit)
					if (--M2intcnt[*xit]==0) wrapsel[siphon]=false;
				if (wrapsel[siphon]) ++pos;
			} else {
				wrapptr[pos]=-2;
				++pos;
			}
		} else {
			siphon = wrapvec[pos][wrapptr[pos]];
			wrapsel[siphon]=false;
			for(xit=intmap[siphon].begin(); xit!=intmap[siphon].end(); ++xit)
				++M2intcnt[*xit];
			++wrapptr[pos];
			if (wrapptr[pos]<wrapvec[pos].size()) {
				siphon = wrapvec[pos][wrapptr[pos]];
				wrapsel[siphon]=true;
				for(xit=intmap[siphon].begin(); xit!=intmap[siphon].end(); ++xit)
					if (--M2intcnt[*xit]==0) wrapsel[siphon]=false;
				if (wrapsel[siphon]) ++pos;
			} else {
				wrapptr[pos]=-1;
				--pos;
			}
		}
	}
	return(pos>=0);
}

/** Remove a redundant siphon from all wrapping sets.
	@param sid The siphon's ID.
*/
void SubNetInfo::removeRedundant(SiphonID sid) {
	intmap.erase(sid);
	map<InterfaceID,set<Wrapping> >::iterator wit;
	for(wit=wQ.begin(); wit!=wQ.end(); ++wit)
	{
		set<Wrapping>::iterator wrapit(wit->second.begin());
		while (wrapit!=wit->second.end())
		{
			unsigned int i;
			for(i=0; i<(*wrapit).size(); ++i)
				if ((*wrapit)[i]==sid) break;
			if (i==(*wrapit).size()) ++wrapit;
			else wit->second.erase(wrapit++);
		}
	}
	for(wit=wM.begin(); wit!=wM.end(); ++wit)
	{
		set<Wrapping>::iterator wrapit(wit->second.begin());
		while (wrapit!=wit->second.end())
		{
			unsigned int i;
			for(i=0; i<(*wrapit).size(); ++i)
				if ((*wrapit)[i]==sid) break;
			if (i==(*wrapit).size()) ++wrapit;
			else wit->second.erase(wrapit++);
		}
	}
}

/** Compute the subnet info from subnet children's info.
	@param m The matchings of this subnet.
	@return True on premature exit forced by another thread.
*/
bool SubNetInfo::computeComponentInfo(const Matchings& m, bool mt) {
	// get the siphon matchings and their interfaces
	intmap.clear();
	for(MatchingID ID=0; ID<m.numMatchings(Matchings::SIPHON); ++ID)
	{
		Interface interface(m.getMatchingInterface(ID,Matchings::SIPHON));
		if (!interface.empty()) intmap[ID]=interface;
	}
	if (intmap.size()>=siphonthreshold) { texit=true; return true; }

	// get the interface-trap matching interfaces
	set<Interface> LQtmp;
	LQ.clear();
	map<Interface,vector<MatchingID> > itom;
	for(MatchingID ID=0; ID<m.numMatchings(Matchings::ITRAP); ++ID)
	{
		Interface interface(m.getMatchingInterface(ID,Matchings::ITRAP));
		if (!mt || !interface.empty()) {
			LQtmp.insert(interface);
			itom[interface].push_back(ID);
		}
	}
	LQ.assign(LQtmp.begin(),LQtmp.end());

	// get the interface-trap matching interfaces
	set<Interface> LMtmp;
	LM.clear();
	map<Interface,vector<MatchingID> > itomm;
	if (mt) {
		for(MatchingID ID=0; ID<m.numMatchings(Matchings::TTRAP); ++ID)
		{
			Interface interface(m.getMatchingInterface(ID,Matchings::TTRAP));
			const Matching& ttmatching(m.getMatching(ID,Matchings::TTRAP));
			LMtmp.insert(interface);
			itomm[interface].push_back(ID);
		}
		LM.assign(LMtmp.begin(),LMtmp.end());
	}

	if (false) { //(multithreaded) {
		// construct the structure of wQ/wM, so it will not be changed later when the threads access it
		for(InterfaceID ID=0; ID<LQ.size(); ++ID) wQ[ID] = set<Wrapping> ();
		for(InterfaceID ID=0; ID<LM.size(); ++ID) wM[ID] = set<Wrapping> ();
		// compute wrappings for LQ
		pthread_mutex_lock(&main_mutex);
		for(InterfaceID ID=0; ID<LQ.size(); ++ID)
		{
			while (tindex.empty() && idleID.empty())
	 				pthread_cond_wait(&main_cond,&main_mutex);
			set<unsigned int>::iterator it;
			for(it=tindex.begin(); it!=tindex.end(); ++it) idleID.insert(*it);
			tindex.clear();
			unsigned int thrnr(*(idleID.begin()));
			idleID.erase(thrnr);
			Interface& interface(LQ.at(ID));
			initThread(thrnr,(unsigned int)0,*this,m,ID,itom[interface],Matchings::ITRAP);
		}

		// compute wrappings for LM
		for(InterfaceID ID=0; ID<LM.size(); ++ID)
		{
			while (tindex.empty() && idleID.empty())
	 				pthread_cond_wait(&main_cond,&main_mutex);
			set<unsigned int>::iterator it;
			for(it=tindex.begin(); it!=tindex.end(); ++it) idleID.insert(*it);
			tindex.clear();
			unsigned int thrnr(*(idleID.begin()));
			idleID.erase(thrnr);
			Interface& interface(LM.at(ID));
			initThread(thrnr,1,*this,m,ID,itomm[interface],Matchings::TTRAP);
		}
		while (tindex.size()+idleID.size()<maxthreads)
 				pthread_cond_wait(&main_cond,&main_mutex);
		set<unsigned int>::iterator it;
		for(it=tindex.begin(); it!=tindex.end(); ++it) idleID.insert(*it);
		tindex.clear();
		pthread_mutex_unlock(&main_mutex);
	} else {
		// compute wrappings for LQ
		for(InterfaceID ID=0; ID<LQ.size(); ++ID)
		{
			Interface& interface(LQ.at(ID));
			for(unsigned int mID=0; mID<itom[interface].size(); ++mID)
				computeWrappingSets(m,ID,m.getMatching(itom[interface][mID],Matchings::ITRAP),0,0);
		}

		// compute wrappings for LM
		for(InterfaceID ID=0; ID<LM.size(); ++ID)
		{
			Interface& interface(LM.at(ID));
			for(unsigned int mID=0; mID<itomm[interface].size(); ++mID)
			{
				int side(-1);
				if (m.getTokenTrapSide(itomm[interface][mID])) side=1;
				computeWrappingSets(m,ID,m.getMatching(itomm[interface][mID],Matchings::TTRAP),m.getTokenTrap(itomm[interface][mID]),side);

			}
		}
	}

	// Check for and eliminate redundant siphons
	bool redundant(true);
	while (redundant && !texit)
	{
		redundant=false;
		map<SiphonID,Interface>::iterator iit;
		for(iit=intmap.begin(); iit!=intmap.end(); ++iit)
			if (checkRedundancy(iit->first)) break;
		if (iit!=intmap.end()) redundant=true;
		if (redundant) removeRedundant(iit->first);
	}
	return false;
}

/** Get the unique place of an elementary siphon/trap.
	@param interface The interface of the siphon or trap.
	@param siphon True if it is a siphon.
	@return The ID of the unique place or -1 if none exists.
*/
int SubNetInfo::getUniqueElement(const Interface& interface, bool siphon) const {
	Interface::const_iterator sit;
	for(sit=interface.begin(); sit!=interface.end(); ++sit)
		if (siphon && net.isOutput(*sit)) return *sit; // PlaceID
		else if (!siphon && net.isInput(*sit)) return *sit;
	return -1; // no unique interface place exists
}

/** Compute the wrapping sets via matchings.
	@param m The matchings as a whole.
	@param iID The ID of the interface of an elementary trap for which wrapping sets are to be computed.
	@param matching A token-trap matching for which the wrapping sets are to be computed.
	@param tti The interface ID of the token trap in the matching.
	@param side If the token trap belongs to the right side of the matchings (and thus the right child).
*/
void SubNetInfo::computeWrappingSets(const Matchings& m, InterfaceID iID, const Matching& matching, InterfaceID tti, int side) {
	// map from siphons (in each subnet) to index of selected matching containing it in lsmlist/rsmlist
	if (m.getSNI(false).intmap.empty() || m.getSNI(true).intmap.empty()) return; // no siphons == no wrapping sets
	vector<int> select1(m.getSNI(false).intmap.rbegin()->first+1,-1),select2(m.getSNI(true).intmap.rbegin()->first+1,-1); // [SiphonID]
	vector<unsigned int> selcnt1(select1.size(),0), selcnt2(select2.size(),0); // [SiphonID]
	// how often each new siphon matching is selected
	vector<unsigned int> selmatch(m.numMatchings(Matchings::SIPHON),0); // [MatchingID]
	// iterators for walking through wrapping sets for trap interfaces in the subnet
	vector<set<Wrapping>::const_iterator> titowr,titowrbegin,titowrend;
	vector<bool> titowrinit;
	// initialise the iterators
	for(unsigned int i=0; i<matching.first.size(); ++i)
	{
		unsigned int ietinr(matching.first[i]);
		const set<Wrapping>& ietiw(m.getSNI(false).getwQ(ietinr));
		if (ietiw.empty()) return;
		titowrbegin.push_back(ietiw.begin());
		titowrend.push_back(ietiw.end());
	}
	unsigned int rightside(titowrbegin.size());
	if (side!=0) {
		const set<Wrapping>& tetiw(m.getSNI(side>0).getwM(tti));
		if (tetiw.empty()) return;
		titowrbegin.push_back(tetiw.begin());
		titowrend.push_back(tetiw.end());
		if (side<0) ++rightside;
	}
	for(unsigned int i=0; i<matching.second.size(); ++i)
	{
		unsigned int ietinr(matching.second[i]);
		const set<Wrapping>& ietiw(m.getSNI(true).getwQ(ietinr));
		if (ietiw.empty()) return;
		titowrbegin.push_back(ietiw.begin());
		titowrend.push_back(ietiw.end());
	}
	titowr.resize(titowrbegin.size());
	titowrinit.resize(titowrbegin.size(),false);
	int cnt(0);
	int pos(0),sindex(-2),smatch(-1);
	while (pos>=0) {
		if (texit) return; // exit condition in case of multithreading
		// check if we have a complete wrapping
		if (pos>=titowr.size()) {
			// we found a new wrapping!
			set<MatchingID> wrapping;
			for(SiphonID sind=0; sind<select1.size(); ++sind)
				if (selcnt1[sind]>0) wrapping.insert(m.getMatchingList(sind,false).at(select1[sind]));
			for(SiphonID sind=0; sind<select2.size(); ++sind)
				if (selcnt2[sind]>0) wrapping.insert(m.getMatchingList(sind,true).at(select2[sind]));
			Wrapping wrapping2;
			wrapping2.assign(wrapping.begin(),wrapping.end());
			// check for subsets & supersets is not done yet!
			set<Wrapping>::iterator wit(side==0 ? wQ[iID].begin() : wM[iID].begin());
			for(; wit!=(side==0 ? wQ[iID].end() : wM[iID].end()); )
			{
				if (includes(wrapping2,*wit)) break;
				else if (includes(*wit,wrapping2)) {
					if (side==0) wQ[iID].erase(wit++);
					else wM[iID].erase(wit++);
				}
				else ++wit;
			}
			// save the wrapping
			if (side==0 && wit==wQ[iID].end()) wQ[iID].insert(wrapping2);
			else if (side!=0 && wit==wM[iID].end()) wM[iID].insert(wrapping2);			
			// backtrack
			--pos;
			sindex = titowr[pos]->size()-1;
			smatch = (pos<rightside ? select1[titowr[pos]->at(sindex)] : select2[titowr[pos]->at(sindex)]);
		}

		// if no wrapping set is selected at this position so far, select the first one
		if (!titowrinit[pos]) {
			titowrinit[pos] = true;
			titowr[pos] = titowrbegin[pos];
			sindex = -2;
		}

		// if we have selected matchings for all siphons at the active position, advance a position
		if (sindex>=(int)(titowr[pos]->size())) {
			++pos;
			continue;
		}

		// if we have backtracked through all siphons at the active position, select the next wrapping set for this position
		if (sindex==-1) {
			++titowr[pos];
			// if there are no more wrappings at this position, backtrack
			if (titowr[pos]==titowrend[pos]) {
				titowrinit[pos]=false;
				--pos;
				if (pos>=0) {
					sindex = titowr[pos]->size()-1;
					smatch = (pos<rightside ? select1[titowr[pos]->at(sindex)] : select2[titowr[pos]->at(sindex)]);
				}
				continue;
			}
			sindex = -2;
		}

		// if we have selected a new wrapping set, go to the first siphon in it
		if (sindex==-2) {
			for(sindex=0; sindex<(int)(titowr[pos]->size()); ++sindex)
			{
				SiphonID siphon(titowr[pos]->at(sindex));
				if (m.getMatchingList(siphon,pos>=rightside).empty()) break;
			}
			smatch = -1;
			if (sindex<(int)(titowr[pos]->size())) { sindex = -1; continue; }
			else sindex = 0;
//			continue;
		}

		SiphonID siphon(titowr[pos]->at(sindex));
		const MatchingList& mlist(m.getMatchingList(siphon,pos>=rightside));

		// walk through the siphon matchings containing the siphon
		if (smatch==-1) {
			// select the first matching for a siphon
			unsigned int j;
			for(j=0; j<mlist.size(); ++j)
				if (selmatch[mlist[j]]>0) break;
			if (j<mlist.size()) smatch=j;
			else smatch=0;
		} else {
			// if we already had a matching, deselect it and advance to the next matching
			if (pos<rightside) {
				if (--selcnt1[siphon]==0) { select1[siphon]=-1; --selmatch[mlist[smatch]]; }
			} else {
				if (--selcnt2[siphon]==0) { select2[siphon]=-1; --selmatch[mlist[smatch]]; }
			}
			if (selmatch[mlist[smatch]]>0) { 
				if (--sindex>=0) smatch = (pos<rightside ? select1[titowr[pos]->at(sindex)] : select2[titowr[pos]->at(sindex)]);
				continue; 
			}
			++smatch;
		}

		// if we are through with all matchings for the active siphon, backtrack
		if (smatch>=(int)(mlist.size())) {
			if (--sindex>=0) smatch = (pos<rightside ? select1[titowr[pos]->at(sindex)] : select2[titowr[pos]->at(sindex)]);
			continue;
		}

		// select the matching
		if (pos<rightside) {
			if (++selcnt1[siphon]==1) { select1[siphon] = smatch; ++selmatch[mlist[smatch]]; }
		} else {
			if (++selcnt2[siphon]==1) { select2[siphon] = smatch; ++selmatch[mlist[smatch]]; }
		}

		++sindex;
		smatch = -1;
	}
}

/** Get the siphons in the info structure.
	@return A map from siphon IDs to the siphon interfaces.
*/
const map<SiphonID,Interface>& SubNetInfo::getIntMap() const { return intmap; }

/** Get the interfaces of the interface-elementary traps in the info structure.
	@return A vector of interfaces.
*/
const vector<Interface>& SubNetInfo::getLQ() const { return LQ; }

/** Get the interfaces of the token-elementary traps in the info structure.
	@return A vector of interfaces.
*/
const vector<Interface>& SubNetInfo::getLM() const { return LM; }

/** Get the wrapping sets for the interfaces of interface-elementary traps.
	@return Map from interface IDs to sets of wrappings.
*/
const map<InterfaceID,set<Wrapping> >& SubNetInfo::getwQ() const { return wQ; }

/** Get the wrapping sets for the interfaces of token-elementary traps.
	@return Map from interface IDs to sets of wrappings.
*/
const map<InterfaceID,set<Wrapping> >& SubNetInfo::getwM() const { return wM; }

/** Get the interface of a siphon.
	@param SID The siphon's ID.
	@return The interface.
*/
const Interface& SubNetInfo::getIntMap(SiphonID SID) const { 
	map<SiphonID,Interface>::const_iterator iit(intmap.find(SID));
	if (iit==intmap.end()) return idummy;
	return iit->second; 
}

/** Get the interface belonging to the interface ID of an interface-elementary trap.
	@param IID The ID of the interface.
	@return The interface itself.
*/
const Interface& SubNetInfo::getLQ(InterfaceID IID) const { return LQ[IID]; }

/** Get the interface belonging to the interface ID of a token-elementary trap.
	@param IID The ID of the interface.
	@return The interface itself.
*/
const Interface& SubNetInfo::getLM(InterfaceID IID) const { return LM[IID]; }

/** Get the wrapping set belonging to the interface of an interface-elementary trap.
	@param IID The ID of the interface.
	@return The wrapping set.
*/
const set<Wrapping>& SubNetInfo::getwQ(InterfaceID IID) const {
	map<InterfaceID,set<Wrapping> >::const_iterator wit(wQ.find(IID));
	if (wit==wQ.end()) return wdummy;
	return wit->second; 
}
	
/** Get the wrapping set belonging to the interface of a token-elementary trap.
	@param IID The ID of the interface.
	@return The wrapping set.
*/
const set<Wrapping>& SubNetInfo::getwM(InterfaceID IID) const {
	map<InterfaceID,set<Wrapping> >::const_iterator wit(wM.find(IID));
	if (wit==wM.end()) return wdummy;
	return wit->second; 
}


