// -*- C++ -*-

/*!
 * \file    facts.cc
 *
 * \brief   Class Facts
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2013/24/05
 *
 * \date    $Date: 2013-06-26 12:00:00 +0200 (Mi, 26. Jun 2013) $
 *
 * \version $Revision: 1.00 $
 */

#include "facts.h"
#include "imatrix.h"
#include "Runtime.h"
#include <pthread.h>
#include <string>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

using pnapi::formula::Negation;
using pnapi::formula::Conjunction;
using pnapi::formula::Disjunction;
using pnapi::formula::FormulaTrue;
using pnapi::formula::FormulaFalse;
using pnapi::formula::FormulaEqual;
using pnapi::formula::FormulaNotEqual;
using pnapi::formula::FormulaGreaterEqual;
using pnapi::formula::FormulaLessEqual;
using pnapi::formula::FormulaGreater;
using pnapi::formula::FormulaLess;
using pnapi::formula::Proposition;

	/***************************************************
	* Implementation of the methods of the class Facts *
	***************************************************/

/** Constructor
	@param mat The incidence matrix
*/
Facts::Facts(IMatrix& mat) : im(mat), condition(0) {
	orig_facts = NOFACTS;
	transfer_facts = ALLFACTS;
	locks = (pthread_rwlock_t*) calloc(MAXTYPES, sizeof(pthread_rwlock_t));
	for(unsigned int i=0; i<MAXTYPES; ++i)
		pthread_rwlock_init(locks + i, NULL);
}

/** Constructor
	@param mat The incidence matrix
	@param cond Prerequisite properties to uphold
*/
Facts::Facts(IMatrix& mat, uint64_t cond) : im(mat), condition(cond) {
	orig_facts = NOFACTS;
	transfer_facts = ALLFACTS;
	locks = (pthread_rwlock_t*) calloc(MAXTYPES, sizeof(pthread_rwlock_t));
	for(unsigned int i=0; i<MAXTYPES; ++i)
		pthread_rwlock_init(locks + i, NULL);
}

/** Destructor
*/
Facts::~Facts() {
	for(unsigned int i=0; i<MAXTYPES; ++i)
		pthread_rwlock_destroy(locks + i);
	free(locks);
}

/** Print general properties
	@param facts The properties to print
	@return A string to be printed
*/
string Facts::print(uint64_t facts) {
	string s;
	bool comma(false);
	for(unsigned int i=0; i<64; ++i)
		if (facts & (1LL << i))
		{
			if (comma) s += ", ";
			else comma = true;
			switch (i) {
				case  0: s += "non-live"; break;
				case  1: s += "live"; break;
				case  2: s += "CTL-X"; break;
				case  3: s += "LTL-X"; break;
				case  4: s += "CTL"; break;
				case  5: s += "LTL"; break;
				case  6: s += "unbounded"; break;
				case  7: s += "bounded"; break;
				case  8: s += "ALTL"; break;
				case  9: s += "ALTL-X"; break;
				case 10: s += "paths"; break;
				case 11: s += "complex paths"; break;
				case 12: s += "safe"; break;
				case 13: s += "unsafe"; break;
				case 14: s += "bisimulation"; break;
				case 15: s += "reversibility"; break;
			}
		}
	return s;
}

/** Remove properties that are strictly less expressive than others
	@param facts The properties that hold
	@param direction How the inheritance works (from original to reduced net or the other way)
*/
void Facts::reduceFacts(uint64_t& facts, bool direction) {
	if (direction) {
		if (facts & CTL) facts &= ~CTLX;
		if (facts & LTL) facts &= ~LTLX;
		if (facts & ALTL) facts &= ~ALTLX;
		if (facts & PATHS) facts &= ~COMPLEXPATHS;
	} else {
		if (facts & CTLX) facts &= ~CTL;
		if (facts & LTLX) facts &= ~LTL;
		if (facts & ALTLX) facts &= ~ALTL;
		if (facts & COMPLEXPATHS) facts &= ~PATHS;
		if (facts & UNBOUNDED) facts &= ~UNSAFE;
		if (facts & SAFETY) facts &= ~BOUNDEDNESS;
	}
}

bool Facts::hasStatus(unsigned int status, Types type)
{
	return (((status >> type) & 7L) != NONE);
}

/** Print properties to screen
*/
void Facts::printFacts() {
	reduceFacts(orig_facts, false);
	if (orig_facts)
		cout << endl << "Deduced properties: " << print(orig_facts) << endl;
	reduceFacts(transfer_facts, true);
	if (transfer_facts)
		cout << "Checkable properties: " << print(transfer_facts) << endl;

	set<Vertex>::iterator sit;
	map<Vertex,unsigned int>::iterator xit;
	map<set<Vertex>,set<Vertex> >::iterator blit;

	cout << endl << "Equivalences (original net --> reduced net):" << endl;
	for(blit=mark2.begin(); blit!=mark2.end(); ++blit)
	{
		bool flag(false), comma(true);
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			xit = state.find(*sit);
			if (blit->first.size()<2 && xit!=state.end() && hasStatus(xit->second,MARKING))
				flag = true;
			if (!im.isInvisible(*sit)) comma = false;
		}
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
		{
			xit = state.find(*sit);
			if (!im.exists(*sit) && xit!=state.end() && hasStatus(xit->second,MARKING))
				flag = true;
		}
		if (flag || comma) continue;

		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			if (comma) cout << "+";
			cout << "m(" << im.getName(*sit) << ")";
			comma = true;
		}
		cout << " --> ";
		comma = false;
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
		{
			if (comma) cout << "+";
			cout << "m(" << im.getName(*sit) << ")";
			comma = true;
		}
		int add(markadd[blit->first]);
		if (add>0 && comma) cout << "+";
		if (add || !comma) cout << add;
		cout << endl;
	}
	for(blit=bounded.begin(); blit!=bounded.end(); ++blit)
	{
		bool comma(true);
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			if (!im.exists(*sit)) comma = false;
		if (!comma) continue;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			xit = state.find(*sit);
			if (blit->first.size()<2 && xit!=state.end() && hasStatus(xit->second,BOUNDED))
				comma = false;
			if (!im.isInvisible(*sit)) comma = false;
		}
		if (comma) continue;

		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			if (comma) cout << "&";
			cout << "bounded(" << im.getName(*sit) << ")";
			comma = true;
		}
		cout << " --> ";
		comma = false;
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
		{
			if (comma) cout << "|";
			cout << "bounded(" << im.getName(*sit) << ")";
			comma = true;
		}
		if (!comma) cout << "false";
		cout << endl;
	}
	for(blit=safe.begin(); blit!=safe.end(); ++blit)
	{
		bool comma(true);
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			if (!im.exists(*sit)) comma = false;
		if (!comma) continue;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			xit = state.find(*sit);
			if (blit->first.size()<2 && xit!=state.end() && hasStatus(xit->second,SAFE))
				comma = false;
			if (!im.isInvisible(*sit)) comma = false;
		}
		if (comma) continue;

		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			if (comma) cout << "&";
			cout << "safe(" << im.getName(*sit) << ")";
			comma = true;
		}
		cout << " --> ";
		comma = false;
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
		{
			if (comma) cout << "|";
			cout << "safe(" << im.getName(*sit) << ")";
			comma = true;
		}
		if (!comma) cout << "false";
		cout << endl;
	}
	for(blit=live.begin(); blit!=live.end(); ++blit)
	{
		bool comma(true);
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			if (!im.exists(*sit)) comma = false;
		if (!comma) continue;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			xit = state.find(*sit);
			if (blit->first.size()<2 && xit!=state.end() && hasStatus(xit->second,LIVE))
				comma = false;
			if (!im.isInvisible(*sit)) comma = false;
		}
		if (comma) continue;

		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			if (comma) cout << "|";
			cout << "live(" << im.getName(*sit) << ")";
			comma = true;
		}
		cout << " --> ";
		comma = false;
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
		{
			if (comma) cout << "|";
			cout << "live(" << im.getName(*sit) << ")";
			comma = true;
		}
		if (!comma) cout << "false";
		cout << endl;
	}

	bool condition(checkCondition(CTL | LTL | CTLX | LTLX));
	bool conditio2(checkCondition(ALTL | ALTLX));
	for(xit=state.begin(); xit!=state.end(); ++xit)
	{
		unsigned status;
		if (im.isPlace(xit->first))
		{
			if ((status = (xit->second >> MARKING) & 7L) > 0)
				if (condition && status == UNKNOWN && !im.isInvisible(xit->first))
					cout << "m(" << im.getName(xit->first) << ") --> N/A" << endl;

			unsigned int bstate((xit->second >> BOUNDED) & 7L);
			unsigned int sstate((xit->second >> SAFE) & 7L);
			if (!im.isInvisible(xit->first))
			{
				if (sstate == ISTRUE) cout << "safe(" << im.getName(xit->first) << ") --> true" << endl;
				else if (bstate == ISFALSE) cout << "bounded(" << im.getName(xit->first) << ") --> false" << endl;
				else {
					if (sstate == ISFALSE)
						cout << "safe(" << im.getName(xit->first) << ") --> false" << endl;
					if (bstate == ISTRUE)
						cout << "bounded(" << im.getName(xit->first) << ") --> true" << endl;
					if (condition && sstate == UNKNOWN)
						cout << "safe(" << im.getName(xit->first) << ") --> N/A" << endl;
					if (condition && bstate == UNKNOWN)
						cout << "bounded(" << im.getName(xit->first) << ") --> N/A" << endl;
				}
			}
		} else {
			if ((status = (xit->second >> LIVE) & 7L) != NONE && !im.isInvisible(xit->first) && im.isOriginal(xit->first))
			{
				switch (status) {
					case ISFALSE:	cout << "live(" << im.getName(xit->first) << ") --> false" << endl; 
									break;
					case ISTRUE:	cout << "live(" << im.getName(xit->first) << ") --> true" << endl; 
									break;
					case UNKNOWN:	if (conditio2)
										cout << "live(" << im.getName(xit->first) << ") --> N/A" << endl; 
									break;
				}
			}
		}
	}

	if (checkCondition(PATHS | COMPLEXPATHS)) // || (transfer_facts & (PATHS|COMPLEXPATHS)))
	{
		if (!path.empty()) 
			cout << endl << "Path Translations (reduced net --> original net):" << endl;
		map<Vertex, Path>::iterator pit;
		for(pit=path.begin(); pit!=path.end(); ++pit)
			if (((state[pit->first] >> PATH) & 7L) == NONE && !im.isPlace(pit->first))
			{
				cout << (pit->first==NO_NODE ? "*init*" : im.getName(pit->first)) << " -->";
				for(unsigned int i=0; i<pit->second.size(); ++i) {
					cout << " ";
					printPathElement(pit,i);
				}
				cout << endl;
			} 
	}
}

/** Print complex path elements to screen
	@param it Iterator pointing to the path element to print
	@param pos Pointer to the exact path element to print, will be incremented
*/
void Facts::printPathElement(const map<Vertex,Path>::iterator& it, unsigned int& pos) {
	Vertex p(it->second[pos]);
	if (!im.isPlace(p)) {
		cout << im.getName(p);
		return;
	}
	cout << "(";
	while (it->second[++pos] != p) printPathElement(it,pos);
	cout << " | ";
	while (it->second[++pos] != p) printPathElement(it,pos);
	cout << ")";
}

/** Print general properties to JSON object
	@param log The JSON object
	@param name Name of the top member of log (for deduced/inherited)
	@param facts The properties
*/
void Facts::print(JSON& log, string name, uint64_t facts) {
	for(unsigned int i=0; i<64; ++i)
		if (facts & (1LL << i))
		{
			switch (i) {
				case  0: log[name] += "non-live"; break;
				case  1: log[name] += "live"; break;
				case  2: log[name] += "CTL-X"; break;
				case  3: log[name] += "LTL-X"; break;
				case  4: log[name] += "CTL"; break;
				case  5: log[name] += "LTL"; break;
				case  6: log[name] += "unbounded"; break;
				case  7: log[name] += "bounded"; break;
				case  8: log[name] += "ALTL"; break;
				case  9: log[name] += "ALTL-X"; break;
				case 10: log[name] += "paths"; break;
				case 11: log[name] += "complex paths"; break;
				case 12: log[name] += "safe"; break;
				case 13: log[name] += "unsafe"; break;
				case 14: log[name] += "bisimulation"; break;
				case 15: log[name] += "reversibility"; break;
			}
		}
}

/** Print all properties to a JSON object
	@param log The JSON object
*/
void Facts::printFacts(JSON& log) {
	reduceFacts(orig_facts, false);
	if (orig_facts) print(log, "deduced properties", orig_facts);
	reduceFacts(transfer_facts, true);
	if (transfer_facts) print(log, "checkable properties", transfer_facts);

	set<Vertex>::iterator sit;
	map<Vertex,unsigned int>::iterator xit;
	map<set<Vertex>,set<Vertex> >::iterator blit;

	for(blit=mark2.begin(); blit!=mark2.end(); ++blit)
	{
		bool flag(false), comma(true);
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			xit = state.find(*sit);
			if (blit->first.size()<2 && xit!=state.end() && hasStatus(xit->second,MARKING))
				flag = true;
			if (!im.isInvisible(*sit))
				comma = false;
		}
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
		{
			xit = state.find(*sit);
			if (!im.exists(*sit) && xit!=state.end() && hasStatus(xit->second,MARKING))
				flag = true;
		}
		if (flag || comma) continue;

		string tmp;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			if (comma) tmp+="+"; else comma=true;
			tmp += im.getName(*sit);
		}
		if (blit->second.size()>1 || (markadd[blit->first]!=0 && blit->second.size()>0))
			log["marking"][tmp] += "+";
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			log["marking"][tmp] += im.getName(*sit);
		if (blit->second.empty())
			log["marking"][tmp] = markadd[blit->first];
		else if (markadd[blit->first]!=0)
			log["marking"][tmp] += markadd[blit->first];
	}
	for(blit=bounded.begin(); blit!=bounded.end(); ++blit)
	{
		bool comma(true);
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			if (!im.exists(*sit)) comma = false;
		if (!comma) continue;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			xit = state.find(*sit);
			if (blit->first.size()<2 && xit!=state.end() && hasStatus(xit->second,BOUNDED))
				comma = false;
			if (!im.isInvisible(*sit)) comma = false;
		}
		if (comma) continue;

		string tmp;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			if (comma) tmp+="&"; else comma=true;
			tmp += im.getName(*sit);
		}
		if (blit->second.size()>1) 
			log["bounded"][tmp] += "|";
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			log["bounded"][tmp] += im.getName(*sit);
	}
	for(blit=safe.begin(); blit!=safe.end(); ++blit)
	{
		bool comma(true);
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			if (!im.exists(*sit)) comma = false;
		if (!comma) continue;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			xit = state.find(*sit);
			if (blit->first.size()<2 && xit!=state.end() && hasStatus(xit->second,SAFE))
				comma = false;
			if (!im.isInvisible(*sit)) comma = false;
		}
		if (comma) continue;

		string tmp;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			if (comma) tmp+="&"; else comma=true;
			tmp += im.getName(*sit);
		}
		if (blit->second.size()>1) 
			log["safe"][tmp] += "|";
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			log["safe"][tmp] += im.getName(*sit);
	}
	for(blit=live.begin(); blit!=live.end(); ++blit)
	{
		bool comma(true);
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			if (!im.exists(*sit)) comma = false;
		if (!comma) continue;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			xit = state.find(*sit);
			if (blit->first.size()<2 && xit!=state.end() && hasStatus(xit->second,LIVE))
				comma = false;
			if (!im.isInvisible(*sit)) comma=false;
		}
		if (comma) continue;

		string tmp;
		for(sit=blit->first.begin(); sit!=blit->first.end(); ++sit)
		{
			if (comma) tmp+="|"; else comma=true;
			tmp += im.getName(*sit);
		}
		if (blit->second.size()>1) 
			log["live"][tmp] += "|";
		for(sit=blit->second.begin(); sit!=blit->second.end(); ++sit)
			log["live"][tmp] += im.getName(*sit);
	}

	bool condition(checkCondition(CTL | LTL | CTLX | LTLX));
	bool conditio2(checkCondition(ALTL | ALTLX));
	for(xit=state.begin(); xit!=state.end(); ++xit)
	{
		unsigned status;
		if (im.isPlace(xit->first))
		{
			if ((status = (xit->second >> MARKING) & 7L) != NONE)
				if (condition && status == UNKNOWN && !im.isInvisible(xit->first))
					log["marking"][im.getName(xit->first)] = "N/A";

			if ((status = (xit->second >> BOUNDED) & 7L) != NONE && !im.isInvisible(xit->first))
			{
				switch (status) {
					case ISFALSE:	log["bounded"][im.getName(xit->first)] = false; 
									break;
					case ISTRUE:	log["bounded"][im.getName(xit->first)] = true;
									break;
					case UNKNOWN:	if (condition) 
										log["bounded"][im.getName(xit->first)] = "N/A"; 
									break;
				}
			}
			if ((status = (xit->second >> SAFE) & 7L) != NONE && !im.isInvisible(xit->first))
			{
				switch (status) {
					case ISFALSE:	log["safe"][im.getName(xit->first)] = false; 
									break;
					case ISTRUE:	log["safe"][im.getName(xit->first)] = true;
									break;
					case UNKNOWN:	if (condition) 
										log["safe"][im.getName(xit->first)] = "N/A"; 
									break;
				}
			}
		} else {
			if ((status = (xit->second >> LIVE) & 7L) != NONE && !im.isInvisible(xit->first) && im.isOriginal(xit->first))
			{
				switch (status) {
					case ISFALSE:	log["live"][im.getName(xit->first)] = false; 
									break;
					case ISTRUE:	log["live"][im.getName(xit->first)] = true; 
									break;
					case UNKNOWN:	if (conditio2) 
										log["live"][im.getName(xit->first)] = "N/A"; 
									break;
				}
			}
		}
	}

	if (checkCondition(PATHS | COMPLEXPATHS)) // || (transfer_facts & (PATHS|COMPLEXPATHS)))
		for(map<Vertex, Path>::iterator pit=path.begin(); pit!=path.end(); ++pit)
			if (((state[pit->first] >> PATH) & 7L) == NONE)
				for(unsigned int i=0; i<pit->second.size(); ++i)
					if (!im.isPlace(pit->first) && im.isPlace(pit->second[i]))
						printPathElement(log["path"][pit->first==NO_NODE ? "*init*" : im.getName(pit->first)], pit, i);
					else 
						log["path"][pit->first==NO_NODE ? "*init*" : im.getName(pit->first)] += im.getName(pit->second[i]);
}

/** Print a path element to a JSON object
	@param json The JSON object
	@param it Iterator pointing to the path to be printed
	@param pos The position inside the path to print next, may be incremented
*/
void Facts::printPathElement(JSON& json, const map<Vertex,Path>::iterator& it, unsigned int& pos) {
	Vertex p(it->second[pos]);
	JSON tmp;
	map<Vertex,Path>::iterator pth(path.find(p));
	if (pth==path.end())
		tmp["*if*"] += im.getName(p);
	else
		for(unsigned int i=0; i<pth->second.size(); ++i)
			tmp["*if*"] += im.getName(pth->second[i]);
	while (it->second[++pos] != p)
		if (im.isPlace(it->second[pos])) printPathElement(tmp["then"],it,pos);
		else tmp["then"] += im.getName(it->second[pos]);
	while (it->second[++pos] != p)
		if (im.isPlace(it->second[pos])) printPathElement(tmp["else"],it,pos);
		else tmp["else"] += im.getName(it->second[pos]);
	json += tmp;
}

/** Declare a fact/property as deduced
	@param fact The fact
	@param remove If the property should also be removed from the transfer list (default: false)
*/
void Facts::addFact(uint64_t fact, bool remove) {
	pthread_rwlock_wrlock(locks + FACTS);
	orig_facts |= fact;
	if (remove) transfer_facts &= ~fact;
	pthread_rwlock_unlock(locks + FACTS);
}

/** Declare a fact/property as non-inheritable
	@param fact The fact
*/
void Facts::removeFact(uint64_t fact) {
	pthread_rwlock_wrlock(locks + FACTS);
	transfer_facts &= ~fact;
	pthread_rwlock_unlock(locks + FACTS);
}

/** Check if a fact/property is a prerequisite
	@param fact The fact
	@return If that is the case
*/
bool Facts::checkCondition(uint64_t fact) {
	return (condition & fact);
}

/** Add info to a detail property.
	@param type Whether the info is about MARKING, BOUNDED, or LIVE
	@param id The ID of the node for which the property holds
	@param out The ID of the node to be added to the property (i.e. translation)
	@param add A fixed value to be added to the property (MARKING only)
*/
void Facts::addChange(unsigned int type, Vertex id, Vertex out, int add) {
	if (Runtime::args_info.netonly_given) return;
	set<Vertex> tmp;
	tmp.insert(out);
	addChange(type,id,tmp,add);
}

/** Add info to a detail property.
	@param type Whether the info is about MARKING, BOUNDED, SAFE, or LIVE
	@param id The ID of the node for which the property holds
	@param out The IDs of the nodes to be added to the property (i.e. translation)
	@param add A fixed value to be added to the property (MARKING only)
*/
void Facts::addChange(unsigned int type, Vertex id, set<Vertex>& out, int add) {
	if (Runtime::args_info.netonly_given) return;
	pthread_rwlock_wrlock(locks + type);
	map<set<Vertex>,set<Vertex> >& tmp2(type==MARKING ? mark2 : (type==BOUNDED ? bounded : (type==LIVE ? live : safe)));
	map<set<Vertex>,set<Vertex> >::iterator mit;
	for(mit=tmp2.begin(); mit!=tmp2.end(); ++mit)
		if (mit->second.find(id)!=mit->second.end()) 
		{
			mit->second.erase(id);
			set<Vertex>::iterator sit;
			for(sit=out.begin(); sit!=out.end(); ++sit)
				mit->second.insert(*sit);
			if (type == MARKING && add)
				markadd[mit->first] += add;
		}

	set<Vertex> tmp;
	tmp.insert(id);
	switch (type) {
		case MARKING:	mark2[tmp] = out; markadd[tmp] = add; break;
		case BOUNDED:	bounded[tmp] = out; break;
		case LIVE:		live[tmp] = out; break;
		case SAFE:		safe[tmp] = out; break;
	}
	pthread_rwlock_unlock(locks + type);
}

/** Add info to a detail property.
	@param type Whether the info is about MARKING, BOUNDED, SAFE, or LIVE
	@param id The IDs of the nodes for which the property holds (i.e. a complex object)
	@param out The ID of the node to be added to the property (i.e. translation)
	@param add A fixed value to be added to the property (MARKING only)
*/
void Facts::addChange(unsigned int type, set<Vertex>& id, Vertex out, int add) {
	if (Runtime::args_info.netonly_given) return;
	pthread_rwlock_wrlock(locks + type);
	if (id.find(out)!=id.end())
	{
		// this may fail for future extensions
		map<set<Vertex>,set<Vertex> >& tmp(type==MARKING ? mark2 : (type==BOUNDED ? bounded : (type==LIVE ? live : safe)));
		map<set<Vertex>,set<Vertex> >::iterator mit;
		for(mit=tmp.begin(); mit!=tmp.end(); ++mit)
			if (mit->second.size()==1 && *(mit->second.begin())==out) 
			{
				id.erase(out);
				set<Vertex>::iterator sit;
				for(sit=mit->first.begin(); sit!=mit->first.end(); ++sit)
					id.insert(*sit);
				if (type == MARKING) {
					add += markadd[mit->first];
					markadd.erase(mit->first);
				}
				tmp.erase(mit);
				break;
			}
	}

	set<Vertex> tmp;
	tmp.insert(out);
	switch (type) {
		case MARKING:	mark2[id] = tmp; markadd[id] = add; break;
		case BOUNDED:	bounded[id] = tmp; break;
		case LIVE:		live[id] = tmp; break;
		case SAFE:		safe[id] = tmp; break;
	}
	pthread_rwlock_unlock(locks + type);
}

/** Set the status of a detail property
	@param type Property type (MARKING, BOUNDED, SAFE, LIVE, PATH)
	@param id The ID of the node for which the property holds
	@param status The new status (NONE, ISFALSE, ISTRUE, UNKNOWN, UNUSABLE). Notes: UNUSABLE cannot be changed again.
		ISFALSE and ISTRUE are for BOUNDED, SAFE, and LIVE only.
*/
void Facts::setStatus(unsigned int type, Vertex id, unsigned int status) {
	if (Runtime::args_info.netonly_given && type!=PATH) return;
	pthread_rwlock_wrlock(locks + STATUS);
	if ((state[id] >> type) & 7L == UNUSABLE) {
		pthread_rwlock_unlock(locks + STATUS);
		return;
	}
	state[id] &= ~(7L << type);
	state[id] |= (status&7L) << type;
	pthread_rwlock_unlock(locks + STATUS);
	if (type == PATH) return;

	pthread_rwlock_wrlock(locks + type);
	map<set<Vertex>,set<Vertex> >& tmp2(type==MARKING ? mark2 : (type==BOUNDED ? bounded : (type==LIVE ? live : safe)));
	map<set<Vertex>,set<Vertex> >::iterator mit;
	for(mit=tmp2.begin(); mit!=tmp2.end(); )
	{
		if (mit->second.find(id)!=mit->second.end()) 
		{
			if (status == ISFALSE) { mit++->second.erase(id); continue; }
			if (mit->first.size()>1 && mit->second.size()==1) { ++mit; continue; }
			if (status == ISTRUE || status == UNUSABLE) {
				tmp2.erase(mit);
				pthread_rwlock_unlock(locks + type);
				setStatus(type,*(mit->first.begin()),status);
				pthread_rwlock_wrlock(locks + type);
				mit = tmp2.begin();
				continue;
			}
		}
		++mit;
	}
	pthread_rwlock_unlock(locks + type);
}

/** Add a path in front of or after an existing property. If the property does not exist, it will be set to the identity first.
	@param id The ID of the node for which the property holds
	@param out The path to add. The path will be unwound before adding.
	@param before If the path is to be insert in front of the property (default: true) or after it (false)
*/
void Facts::addPath(Vertex id, Path out, bool before) {
	if (!checkCondition(PATHS | COMPLEXPATHS)) return;
	pthread_rwlock_wrlock(locks + PATH);
	if (id != NO_NODE && path.find(id)==path.end()) 
		path[id].push_back(id);
	Path tmp;
	for(unsigned int i=0; i<out.size(); ++i)
		if (path.find(out[i])!=path.end())
			tmp.insert(tmp.end(), path[out[i]].begin(), path[out[i]].end());
		else tmp.push_back(out[i]);
	path[id].insert(before ? path[id].begin() : path[id].end(), tmp.begin(), tmp.end());
	pthread_rwlock_unlock(locks + PATH);
}

/** Add a path in front of or after an existing property. If the property does not exist, it will be set to the identity first.
	@param id The ID of the node for which the property holds
	@param out The path to add. The path will NOT be unwound before adding.
	@param before If the path is to be insert in front of the property (default: true) or after it (false)
*/
void Facts::addFixedPath(Vertex id, Path out, bool before) {
	if (!checkCondition(PATHS | COMPLEXPATHS)) return;
	pthread_rwlock_wrlock(locks + PATH);
	if (id != NO_NODE && path.find(id)==path.end()) 
		path[id].push_back(id);
	path[id].insert(before ? path[id].begin() : path[id].end(), out.begin(), out.end());
	pthread_rwlock_unlock(locks + PATH);
}

/** Unwind a path, i.e. project it from the actual reduction state to the original net.
	@param in The path from the actual reduction state
	@return The unwound path in the original net
*/
Path Facts::getPath(Path& in) {
	Path tmp;
	if (checkCondition(PATHS | COMPLEXPATHS)) {
		pthread_rwlock_rdlock(locks + PATH);
		for(unsigned int i=0; i<in.size(); ++i)
			if (path.find(in[i])!=path.end())
				tmp.insert(tmp.end(),path[in[i]].begin(),path[in[i]].end());
			else tmp.push_back(in[i]);
		pthread_rwlock_unlock(locks + PATH);
	}
	return tmp;
}

/** Set a path. The path will not be unwound.
	@param id The ID of the node for which the path should be set
	@param out The path to be set.
*/
void Facts::setPath(Vertex id, const Path& out) {
	if (!checkCondition(PATHS | COMPLEXPATHS)) return;
	pthread_rwlock_wrlock(locks + PATH);
	path[id] = out;
	pthread_rwlock_unlock(locks + PATH);
}

/** Adapt the final condition to the reduced net
	@param oldf The formula of the original net
	@param translate How to translate the places
	@return The formula for the reduced net
*/
Formula* Facts::transferFormula(const Formula& oldf, map<const Place*,const Place*>& translate) {
	Formula* result;
	set<const Formula*>::iterator sit;
	set<const Formula*> newchildren;
	unsigned int place;
	unsigned int tokens(0);
	switch (oldf.getType()) {
		case Formula::F_NEGATION: {
			const set<const Formula*>& oldchildren(((const Negation&)oldf).getChildren());
			for(sit=oldchildren.begin(); sit!=oldchildren.end(); ++sit)
				newchildren.insert(transferFormula(**sit,translate));
			result = new Negation(newchildren);
			break; }
		case Formula::F_CONJUNCTION: {
			const set<const Formula*>& oldchildren(((const Conjunction&)oldf).getChildren());
			for(sit=oldchildren.begin(); sit!=oldchildren.end(); ++sit)
				newchildren.insert(transferFormula(**sit,translate));
			result = new Conjunction(newchildren);
			break; }
		case Formula::F_DISJUNCTION: {
			const set<const Formula*>& oldchildren(((const Disjunction&)oldf).getChildren());
			for(sit=oldchildren.begin(); sit!=oldchildren.end(); ++sit)
				newchildren.insert(transferFormula(**sit,translate));
			result = new Conjunction(newchildren);
			break; }
		case Formula::F_TRUE: {
			result = new FormulaTrue();
			break; }
		case Formula::F_FALSE: {
			result = new FormulaFalse();
			break; }
		default: {
			place = im.getID(((const Proposition&)oldf).getPlace());
			set<Vertex> tmp;
			tmp.insert(place);
			map<set<Vertex>,set<Vertex> >::iterator it1(mark2.find(tmp));
			map<set<Vertex>,int>::iterator it2(markadd.find(tmp));
			if (it2!=markadd.end()) tokens=-it2->second;
			if (it1!=mark2.end()) {
				if (it1->second.empty()) place = NO_NODE;
				else place = *(it1->second.begin());
			}
			tokens += ((const Proposition&)oldf).getTokens();
			break; }
	}
	if (place != NO_NODE) switch (oldf.getType()) {
		case Formula::F_EQUAL:
			if (tokens>=0) result = new FormulaEqual(*((Place*)im.getNode(place)),tokens);
			else result = new FormulaFalse();
			break;
		case Formula::F_NOT_EQUAL:
			if (tokens>=0) result = new FormulaNotEqual(*((Place*)im.getNode(place)),tokens);
			else result = new FormulaTrue();
			break;
		case Formula::F_GREATER_EQUAL:
			if (tokens>0) result = new FormulaGreaterEqual(*((Place*)im.getNode(place)),tokens);
			else result = new FormulaTrue();
			break;
		case Formula::F_GREATER:
			if (tokens>=0) result = new FormulaGreater(*((Place*)im.getNode(place)),tokens);
			else result = new FormulaTrue();
			break;
		case Formula::F_LESS_EQUAL:
			if (tokens>=0) result = new FormulaLessEqual(*((Place*)im.getNode(place)),tokens);
			else result = new FormulaFalse();
			break;
		case Formula::F_LESS:
			if (tokens>0) result = new FormulaLess(*((Place*)im.getNode(place)),tokens);
			else result = new FormulaFalse();
			break;
		default: break;
	} else switch (oldf.getType()) {
		case Formula::F_EQUAL:
			if (tokens==0) result = new FormulaTrue();
			else result = new FormulaFalse();
			break;
		case Formula::F_NOT_EQUAL:
			if (tokens!=0) result = new FormulaTrue();
			else result = new FormulaFalse();
			break;
		case Formula::F_GREATER_EQUAL:
			if (tokens<=0) result = new FormulaTrue();
			else result = new FormulaFalse();
			break;
		case Formula::F_GREATER:
			if (tokens<0) result = new FormulaTrue();
			else result = new FormulaFalse();
			break;
		case Formula::F_LESS_EQUAL:
			if (tokens>=0) result = new FormulaTrue();
			else result = new FormulaFalse();
			break;
		case Formula::F_LESS:
			if (tokens>0) result = new FormulaTrue();
			else result = new FormulaFalse();
			break;
		default: break;
	}
	for(sit=newchildren.begin(); sit!=newchildren.end(); ++sit)
		delete *sit;
	return result;
}

