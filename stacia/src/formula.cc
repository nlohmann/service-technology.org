// -*- C++ -*-

/*!
 * \file    formula.cc
 *
 * \brief   Class Formula
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/07/24
 *
 * \date    $Date: 2012-11-20 14:47:01 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */


// include header files
#include <string>
#include <iostream>
#include <sstream>
#include "formula.h"

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

/// MiniSat entry function
extern bool minisat(vector< vector <int> > &, vector<bool>&, vector<int>&);
extern pthread_mutex_t sat_mutex;
extern bool texit;

/** Constructor for incremental checks of the siphon-trap property. Only siphons that become
	closed in the current component are checked.
	@param m The matchings for the current component.
	@param mt If a check is made for siphons without marked traps (true) or siphons containing no traps at all (false).
*/
Formula::Formula(const Matchings& m, bool mt) : maxvarnum(0) {
	createSiphonFormula(m);
	createMatchingFormula(m,mt);
}

/** Constructor for full checks of the siphon-trap property.
	@param sn The (sub)net to check for siphons without a marked trap.
	@param mt If a check is made for siphons without marked traps (true) or siphons containing no traps at all (false).
*/ 
Formula::Formula(const SubNet& sn, bool mt) : maxvarnum(0) {
	createSiphonFormula(sn);
	createNonEmptyFormula();
	createMaxTrapFormula(sn,mt);
}

/** Create a subformula that is satisfied by siphons.
	@param sn The (sub)net to check for siphons without a marked trap.
*/ 
void Formula::createSiphonFormula(const SubNet& sn) {
	const vector<PlaceID>& places(sn.getInnerPlaces());
	for(unsigned int i=0; i<places.size(); ++i)
	{
		stringstream sstr;
		sstr << "S" << sn.getMatrix().pName(places[i]);
		string name;
		sstr >> name;
		svarnum[places[i]] = ++maxvarnum;
		names[maxvarnum] = name;
	}
	for(unsigned int i=0; i<places.size(); ++i)
	{
		const vector<set<PlaceID> >& simpl(sn.getSiphonImplication(places[i]));
		for(unsigned int k=0; k<simpl.size(); ++k)
		{
			vector<Literal> clause;
			clause.push_back(-svarnum[places[i]]);
			set<unsigned int>::iterator sit;
			for(sit=simpl.at(k).begin(); sit!=simpl.at(k).end(); ++sit)
				if (sn.internal(*sit)) clause.push_back(svarnum[*sit]);
			clause.push_back(0);
			f.push_back(clause);
		}
	}
}

/** Create a subformula that is satisfied by non-empty sets of places. */ 
void Formula::createNonEmptyFormula() {
	vector<Literal> clause;
	map<int,Literal>::iterator mit;
	for(mit=svarnum.begin(); mit!=svarnum.end(); ++mit)
		clause.push_back(mit->second);
	if (!clause.empty()) {
		clause.push_back(0);
		f.push_back(clause);
	}
}

/** Create a subformula that is satisfied by an unmarked maximal trap inside a siphon.
	@param sn The (sub)net to check for siphons without a marked trap.
	@param mt If a contained maximal trap is allowed at all (true=unmarked allowed, false=should not exist).
*/ 
void Formula::createMaxTrapFormula(const SubNet& sn, bool mt) {
//	if (texit) return;
	const vector<PlaceID>& places(sn.getInnerPlaces());
	if (places.empty()) return;
	map<unsigned int,vector<Literal> > ptvar;
	for(unsigned int i=0; i<places.size(); ++i)
	{
		vector<Literal> pvar;
		for(unsigned int j=0; j<=places.size(); ++j)
		{
			stringstream sstr;
			if (j==0) sstr << "S" << sn.getMatrix().pName(places[i]);
			else if (j==places.size()) sstr << "T" << sn.getMatrix().pName(places[i]);
			else sstr << "T" << sn.getMatrix().pName(places[i]) << "." << j;
			string name;
			sstr >> name;
			if (j==0) pvar.push_back(i+1);
			else { pvar.push_back(++maxvarnum); names[maxvarnum]=name; }
		}
		ptvar[places[i]] = pvar;
	}

	// Unmarked Max Trap:
	vector<Literal> clause;
	for(unsigned int i=0; i<places.size(); ++i)
		if (!mt || sn.isMarked(places[i])) {
			clause.clear();
			clause.push_back(-ptvar[places[i]].back());
			clause.push_back(0);
			f.push_back(clause);
		}

	// Create the MaxTrap Formula, implication left to right
	for(unsigned int i=0; i<places.size(); ++i)
	{
		for(unsigned int j=1; j<=places.size(); ++j)
		{
			clause.clear();
			clause.push_back(-ptvar[places[i]][j]);
			clause.push_back(ptvar[places[i]][j-1]);
			clause.push_back(0);
			f.push_back(clause);

			const vector<set<PlaceID> >& timpl(sn.getTrapImplication(places[i]));
			for(unsigned int k=0; k<timpl.size(); ++k)
			{
				clause.clear();
				clause.push_back(-ptvar[places[i]][j]);
				set<unsigned int>::iterator tit;
				for(tit=timpl[k].begin(); tit!=timpl[k].end(); ++tit)
					if (sn.internal(*tit)) clause.push_back(ptvar[*tit][j-1]);
				clause.push_back(0);
				f.push_back(clause);
			}
		}
	}

	// implication right to left
	// helper variables zjt
	set<string> zjt;
	map<string,Literal> zjtvar;
	for(unsigned int i=0; i<places.size(); ++i)
	{
		for(unsigned int j=1; j<=places.size(); ++j)
		{
			clause.clear();
			clause.push_back(ptvar[places[i]][j]);
			clause.push_back(-ptvar[places[i]][j-1]);

			const PArcs& ppost(sn.getMatrix().getPPostset(places[i]));
			PArcs::const_iterator mit;
			for(mit=ppost.begin(); sn.checkT(mit,ppost.end()); ++mit)
			{
				stringstream sstr;
				sstr << "ZJT" << sn.getMatrix().tName(mit->first) << "." << j;
				string name;
				sstr >> name;

				if (zjt.find(name)==zjt.end()) {
					zjt.insert(name);
					zjtvar[name] = ++maxvarnum;
					names[maxvarnum] = name;
					vector<Literal> cs2;
					cs2.push_back(maxvarnum);
					const TArcs& tpost(sn.getMatrix().getTPostset(mit->first));
					TArcs::const_iterator mit2;
					for(mit2=tpost.begin(); sn.checkInner(mit2,tpost.end()); ++mit2)
					{
						vector<Literal> cs3;
						cs3.push_back(-maxvarnum);
						cs3.push_back(-ptvar[mit2->first][j-1]);
						cs3.push_back(0);
						f.push_back(cs3);

						cs2.push_back(ptvar[mit2->first][j-1]);
					}
					cs2.push_back(0);
					f.push_back(cs2);
				}
				clause.push_back(zjtvar[name]);
			}
			clause.push_back(0);
			f.push_back(clause);
		}
	}
}

/** Create a formula that is satisfied by closed siphons (not touching the interface).
	At least one child siphon must be selected and siphon dependencies must be fulfilled.
	@param m The matchings (containing the open siphons of the children).
*/
void Formula::createSiphonFormula(const Matchings& m) {
	Interface ci(m.getCommonInterface());
	SiphonList innersiphon1,innersiphon2;
	map<SiphonID,Interface>::const_iterator iit;

	// find out which siphons are inner siphons, not touching the outer interface
	const SubNetInfo& sni1(m.getSNI(false));
	for(iit=sni1.getIntMap().begin(); iit!=sni1.getIntMap().end(); ++iit)
		if (sni1.includes(ci,iit->second)) innersiphon1.push_back(iit->first);
	const SubNetInfo& sni2(m.getSNI(true));
	for(iit=sni2.getIntMap().begin(); iit!=sni2.getIntMap().end(); ++iit)
		if (sni2.includes(ci,iit->second)) innersiphon2.push_back(iit->first);

	// create names for the siphons
	map<string,ExtSiphonID> siphons;
	map<ExtSiphonID,Literal>::iterator sit;
	for(unsigned int i=0; i<innersiphon1.size(); ++i)
	{
		stringstream sstr;
		sstr << "sa" << innersiphon1[i];
		string name;
		sstr >> name;
		siphons[name] = -innersiphon1[i]-1;
		siphonname[-innersiphon1[i]-1] = name;
		svarnum[-innersiphon1[i]-1] = ++maxvarnum;
		names[maxvarnum] = name;
	}
	for(unsigned int i=0; i<innersiphon2.size(); ++i)
	{
		stringstream sstr;
		sstr << "sb" << innersiphon2[i];
		string name;
		sstr >> name;
		siphons[name] = innersiphon2[i];
		siphonname[innersiphon2[i]] = name;
		svarnum[innersiphon2[i]] = ++maxvarnum;
		names[maxvarnum] = name;
	}

	// create phi1
	vector<Literal> cs1;
	for(sit=svarnum.begin(); sit!=svarnum.end(); ++sit)
		cs1.push_back(sit->second);
	cs1.push_back(0);
	f.push_back(cs1);

	// create phi2
	for(sit=svarnum.begin(); sit!=svarnum.end(); ++sit)
	{
		const SubNet& sn1(sit->first<0 ? sni1.net : sni2.net);
		const Interface& if1(sit->first<0 ? sni1.getIntMap(-sit->first-1) : sni2.getIntMap(sit->first));
		for(Interface::const_iterator xit=if1.begin(); xit!=if1.end(); ++xit)
		{
			if (sn1.isInput(*xit)) {
				cs1.clear();
				cs1.push_back(-sit->second);
				map<ExtSiphonID,Literal>::iterator sit2;
				for(sit2=svarnum.begin(); sit2!=svarnum.end(); ++sit2)
				{
					const SubNet& sn2(sit2->first<0 ? sni1.net : sni2.net);
					if (sn2.isOutput(*xit))
					{
						const Interface& if2(sit2->first<0 ? sni1.getIntMap(-sit2->first-1) : sni2.getIntMap(sit2->first));
						if (if2.find(*xit)!=if2.end()) cs1.push_back(sit2->second);
					}
				}
				cs1.push_back(0);
				f.push_back(cs1);
			}
		}
	}
}

/** Create a formula satisfied by a siphon not covering any trap matching.
	@param m The matchings of the current component.
	@param mt True=check for token trap matchings, false=check for interface trap matchings.
*/
void Formula::createMatchingFormula(const Matchings& m, bool mt) {
	if (texit) return;
	unsigned int num(m.numMatchings(mt ? Matchings::TTRAP : Matchings::ITRAP));
	nametoiif.clear();
	nametotif.clear();
	map<string,Literal> mlit;
	for(unsigned int i=0; i<num; ++i)
	{
		// obtain the matching
		const Matching& match(m.getMatching(i,mt ? Matchings::TTRAP : Matchings::ITRAP));

		// start creating the clause for this matching
		vector<Literal> cs1;

		if (mt) {
			unsigned int ttrap(m.getTokenTrap(i));
			bool ttside(m.getTokenTrapSide(i));

			// create a name for the token trap interface
			stringstream sstr1;
			sstr1 << "TT" << (ttside?"R":"L") << ttrap;
			string name1;
			sstr1 >> name1;
			nametotif[name1] = (ttside? ttrap : -ttrap-1);
			if (mlit.find(name1)==mlit.end()) { mlit[name1]= ++maxvarnum; names[maxvarnum]=name1; }
			cs1.push_back(mlit[name1]);
		}

		// walk the elementary interface traps and construct a name for each interface
		for(unsigned int j=0; j<match.first.size(); ++j)
		{
			stringstream sstr2;
			sstr2 << "ITL" << match.first[j];
			string name2;
			sstr2 >> name2;
			nametoiif[name2] = -match.first[j]-1;
			if (mlit.find(name2)==mlit.end()) { mlit[name2]= ++maxvarnum; names[maxvarnum]=name2; }
			cs1.push_back(mlit[name2]);
		}
		for(unsigned int j=0; j<match.second.size(); ++j)
		{
			stringstream sstr2;
			sstr2 << "ITR" << match.second[j];
			string name2;
			sstr2 >> name2;
			nametoiif[name2] = match.second[j];
			if (mlit.find(name2)==mlit.end()) { mlit[name2]= ++maxvarnum; names[maxvarnum]=name2; }
			cs1.push_back(mlit[name2]);
		}

		// add the clause for this matching
		cs1.push_back(0);
		f.push_back(cs1);
	}

	// for each token trap interface that appears in a matching ...
	map<string,ExtTrapID>::iterator mit;
	for(mit=nametotif.begin(); mit!=nametotif.end(); ++mit)
	{
		vector<Literal> cs1;

		// walk through the wrappings, for each wrapping construct a clause
		unsigned int id(mit->second<0 ? -mit->second-1 : mit->second);
		const SubNetInfo& sni(m.getSNI(mit->second>=0));
		set<Wrapping>::const_iterator wit;
		for(wit=sni.getwM(id).begin(); wit!=sni.getwM(id).end(); ++wit)
		{
			cs1.clear();
			cs1.push_back(-mlit[mit->first]);
			unsigned int k;
			for(k=0; k<wit->size(); ++k)
			{
				map<ExtSiphonID,Literal>::iterator name(svarnum.find((mit->second<0 ? -(*wit)[k]-1 : (*wit)[k])));
				if (name==svarnum.end()) break;
				cs1.push_back(-name->second);
			}
			if (k==wit->size()) {
				cs1.push_back(0);
				f.push_back(cs1);
			}
		}		
	}
	// for each interface trap interface that appears in a matching ...
	for(mit=nametoiif.begin(); mit!=nametoiif.end(); ++mit)
	{
		vector<Literal> cs1;

		// walk through the wrappings, for each wrapping construct a clause
		unsigned int id(mit->second<0 ? -mit->second-1 : mit->second);
		const SubNetInfo& sni(m.getSNI(mit->second>=0));
		set<Wrapping>::const_iterator wit;
		for(wit=sni.getwQ(id).begin(); wit!=sni.getwQ(id).end(); ++wit)
		{
			cs1.clear();
			cs1.push_back(-mlit[mit->first]);
			unsigned int k;
			for(k=0; k<wit->size(); ++k)
			{
				map<ExtSiphonID,Literal>::iterator name(svarnum.find((mit->second<0 ? -(*wit)[k]-1 : (*wit)[k])));
				if (name==svarnum.end()) break;
				cs1.push_back(-name->second);
			}
			if (k==wit->size()) {
				cs1.push_back(0);
				f.push_back(cs1);
			}
		}		
	}
}

/** Print the constructed formula. */
void Formula::print() const {
	for(unsigned int i=0; i<f.size(); ++i)
	{
		bool comma(false);
		cout << "(";
		for(unsigned int j=0; j<f[i].size(); ++j)
		{
			Literal l(f[i][j]);
			if (l==0 && j+1==f[i].size()) { cout << ")"; continue; }
			if (l<0) cout << "-";
			if (comma) {
				if (l>=0) cout << "+";
			} else comma = true;
			map<Literal,string>::const_iterator nit(names.find(l>=0 ? l:-l));
			cout << nit->second;
		}
		cout << endl;
	}

	cout << endl << "NumVars: " << maxvarnum << endl;
}

/** Evaluate the constructed formula.
	@param dummy A dummy.
	@return If the formula is satisfiable.
*/
bool Formula::check(bool dummy) {
	if (maxvarnum==0) return false; // no inner places -> no closed siphon with unmarked max trap
	vector<int> conflict;
	mssolution.clear();
	pthread_mutex_lock(&sat_mutex);
	satisfied = minisat(f,mssolution,conflict);
	pthread_mutex_unlock(&sat_mutex);
	return satisfied;
}

/** Print the result of the constructed formula (after it has been checked). */
void Formula::printResult() const {
	if (!satisfied) {
		cout << "Formula unsatisfiable" << endl;
		return;
	}
	cout << "Result: ";
	for(Literal j=1; j<=mssolution.size(); ++j)
		if ((mssolution)[j-1])
			cout << names.find(j)->second << " ";
	cout << endl;
	cout << "Result: ";
	for(Literal j=0; j<mssolution.size(); ++j)
		cout << mssolution[j] << " ";
	cout << endl;
}

/** Get the siphon belonging to a solution.
	@return The siphon (empty if non-existent).
*/
set<PlaceID> Formula::getSiphon() {
	set<PlaceID> result;
	if (!satisfied) return result;
	map<ExtSiphonID,Literal>::iterator sit;
	for(sit=svarnum.begin(); sit!=svarnum.end(); ++sit) 
		if (mssolution[sit->second-1])
			result.insert(sit->first);
	return result;
}

/** Get the matching belonging to a solution.
	@return The matching (representing a closed siphon).
*/
Matching Formula::getMatching() {
	Matching m;
	if (!satisfied) return m;
	map<ExtSiphonID,Literal>::const_iterator sit;
	for(sit=svarnum.begin(); sit!=svarnum.end(); ++sit)
	{
		ExtSiphonID eid(sit->first);
		Literal vid(sit->second);
		if (mssolution[vid-1]) {
			if (eid<0) m.first.push_back(-eid-1);
			else m.second.push_back(eid);
		}
	}
	return m;
}

