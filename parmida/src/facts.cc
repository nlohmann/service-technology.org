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
#include <assert.h>

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
	@param cond Prerequisite properties to uphold
*/
Facts::Facts(IMatrix& mat, uint64_t cond) : im(mat), condition(cond) {
	orig_facts = NOFACTS;
	transfer_facts = ALLFACTS;
	Vertex maxv(im.getMaxVertex());
	factlock = (pthread_rwlock_t*) calloc(1, sizeof(pthread_rwlock_t));
	pthread_rwlock_init(factlock, NULL);
	pathlock = (pthread_rwlock_t*) calloc(maxv+1, sizeof(pthread_rwlock_t));
	for(Vertex v=0; v<=maxv; ++v)
		pthread_rwlock_init(pathlock + v, NULL);
	dellock = (pthread_rwlock_t*) calloc(maxv, sizeof(pthread_rwlock_t));
	for(Vertex v=0; v<maxv; ++v)
		pthread_rwlock_init(dellock + v, NULL);
	path = new Path[maxv+1];
	mark_add = (int*) calloc(maxv, sizeof(int));
//	mark_lhs = new set<Vertex>[maxv];
	mark_rhs = new set<Vertex>[maxv];
	mark_ih = new set<Vertex>[maxv];
	mark_lhs = new map<Vertex,int>[maxv];
	mark_lhs_factor = (int*) calloc(maxv, sizeof(int));
	mark_lhs_fix = (int*) calloc(maxv, sizeof(int));
	bounded_lhs = new set<Vertex>[maxv];
	bounded_rhs = new set<Vertex>[maxv];
	bounded_ih = new set<Vertex>[maxv];
	live_lhs = new set<Vertex>[maxv];
	live_rhs = new set<Vertex>[maxv];
	live_ih = new set<Vertex>[maxv];
	safe_lhs = new set<Vertex>[maxv];
	safe_rhs = new set<Vertex>[maxv];
	safe_ih = new set<Vertex>[maxv];
	state = (unsigned int*) calloc(maxv, sizeof(unsigned int));
}

/** Destructor
*/
Facts::~Facts() {
	Vertex maxv(im.getMaxVertex());
	for(Vertex v=0; v<maxv; ++v)
        pthread_rwlock_destroy(dellock+v);
	free(dellock);
    pthread_rwlock_destroy(factlock);
	free(factlock);
	for(Vertex v=0; v<=maxv; ++v)
	    pthread_rwlock_destroy(pathlock+v);
	free(pathlock);
	free(mark_add);
	delete[] mark_lhs;
	delete[] mark_rhs;
	delete[] mark_ih;
	free(mark_lhs_factor);
	free(mark_lhs_fix);
	delete[] bounded_lhs;
	delete[] bounded_rhs;
	delete[] bounded_ih;
	delete[] live_lhs;
	delete[] live_rhs;
	delete[] live_ih;
	delete[] safe_lhs;
	delete[] safe_rhs;
	delete[] safe_ih;
	free(state);
}

/// computes the ggt of two unsigned integers
inline int ggT(int a, int b)
{
    while (true) {
        a %= b;
        if (!a) return b;
        b %= a;
        if (!b) return a;
    }
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
				case 16: s += "markings"; break;
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

bool Facts::hasStatus(unsigned int status, Types type, Status val)
{
	if (val == NONE) return (((status >> type) & 7L) != NONE);
	return (((status >> type) & 7L) == val);
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
	cout << endl << "Equivalences (original net --> reduced net):" << endl;
	for(Vertex v=0; v<im.getMaxVertex(); ++v)
	{
/*
		if (!mark_rhs[v].empty() || mark_add[v] || hasStatus(state[v],MARKING,ISTRUE)) {
			cout << "m(" << im.getName(v) << ") --> ";
			bool comma(false);
			for(sit=mark_rhs[v].begin(); sit!=mark_rhs[v].end(); ++sit)
			{
				if (comma) cout << "+ "; else comma=true;
				cout << "m(" << im.getName(*sit) << ") ";
			}
			if (mark_rhs[v].empty()) cout << mark_add[v];
			else if (mark_add[v]) cout << "+ " << mark_add[v];
			cout << endl;
		}
*/
		if (im.isInvisible(v)) continue;
		if (!safe_rhs[v].empty() || hasStatus(state[v],SAFE,ISFALSE) || hasStatus(state[v],SAFE,ISTRUE)) {
			cout << "safe(" << im.getName(v) << ") --> ";
			bool comma(false);
			for(sit=safe_rhs[v].begin(); sit!=safe_rhs[v].end(); ++sit)
			{
				if (comma) cout << "| "; else comma=true;
				cout << "safe(" << im.getName(*sit) << ") ";
			}
			if (hasStatus(state[v],SAFE,ISFALSE)) cout << "false";
			else if (hasStatus(state[v],SAFE,ISTRUE)) cout << "true";
			cout << endl;
		}
		if (!bounded_rhs[v].empty() || hasStatus(state[v],BOUNDED,ISFALSE) || hasStatus(state[v],BOUNDED,ISTRUE)) {
			cout << "bounded(" << im.getName(v) << ") --> ";
			bool comma(false);
			for(sit=bounded_rhs[v].begin(); sit!=bounded_rhs[v].end(); ++sit)
			{
				if (comma) cout << "| "; else comma=true;
				cout << "bounded(" << im.getName(*sit) << ") ";
			}
			if (hasStatus(state[v],BOUNDED,ISFALSE)) cout << "false";
			else if (hasStatus(state[v],BOUNDED,ISTRUE)) cout << "true";
			cout << endl;
		}
		if (!live_rhs[v].empty() || hasStatus(state[v],LIVE,ISFALSE) || hasStatus(state[v],LIVE,ISTRUE)) {
			cout << "live(" << im.getName(v) << ") --> ";
			if (!live_rhs[v].empty()) {
				bool comma(false);
				cout << "live(";
				for(sit=live_rhs[v].begin(); sit!=live_rhs[v].end(); ++sit)
				{
					if (comma) cout << "+"; else comma=true;
					cout << im.getName(*sit);
				}
			} else if (hasStatus(state[v],LIVE,ISFALSE)) cout << "false";
			else if (hasStatus(state[v],LIVE,ISTRUE)) cout << "true";
			cout << endl;
		}
	}
	for(Vertex v=0; v<im.getMaxVertex(); ++v)
	{
/*
		if (!mark_lhs[v].empty()) {
			for(sit=mark_lhs[v].begin(); sit!=mark_lhs[v].end(); ++sit)
				if (!im.isInvisible(*sit)) break;
			if (sit!=mark_lhs[v].end()) {
				bool comma(false);
				for(sit=mark_lhs[v].begin(); sit!=mark_lhs[v].end(); ++sit)
				{
					if (comma) cout << "+ "; else comma=true;
					cout << "m(" << im.getName(*sit) << ") ";
				}
				cout << "--> m(" << im.getName(v) << ")" << endl;
			}
		}
*/
		if (mark_lhs_factor[v] && (im.exists(v) || mark_lhs_fix[v]>=-1)) {
			map<Vertex,int>::iterator mit;
			bool comma(false);
			for(mit=mark_lhs[v].begin(); mit!=mark_lhs[v].end(); ++mit)
			{
				if (mit->second) {
					if (comma) { if (mit->second<0) cout << "- "; else cout << "+ "; } 
					else comma=true;
					if (mit->second!=1) cout << (mit->second<0?-1:1)*mit->second;
					if (mit->first != NO_NODE)
						cout << "m(" << im.getName(mit->first) << ")";
					cout << " ";
				}
			}
			cout << "--> ";
			if (mark_lhs_fix[v]>=-1 && mark_lhs_factor[v]) {
				if (mark_lhs_fix[v]>=0) 
					cout << mark_lhs_fix[v]*mark_lhs_factor[v] << endl;
				else cout << "any" << endl; 
			} else {
				if (mark_lhs_factor[v]!=1) cout << mark_lhs_factor[v];
				cout << "m(" << im.getName(v) << ")" << endl;
			}
		}
		if (!im.exists(v)) continue;
		if (!safe_lhs[v].empty()) {
			for(sit=safe_lhs[v].begin(); sit!=safe_lhs[v].end(); ++sit)
				if (!im.isInvisible(*sit)) break;
			if (sit!=safe_lhs[v].end()) {
				bool comma(false);
				for(sit=safe_lhs[v].begin(); sit!=safe_lhs[v].end(); ++sit)
				{
					if (comma) cout << "& "; else comma=true;
					cout << "safe(" << im.getName(*sit) << ") ";
				}
				cout << "--> safe(" << im.getName(v) << ")" << endl;
			}
		}
		if (!bounded_lhs[v].empty()) {
			for(sit=bounded_lhs[v].begin(); sit!=bounded_lhs[v].end(); ++sit)
				if (!im.isInvisible(*sit)) break;
			if (sit!=bounded_lhs[v].end()) {
				bool comma(false);
				for(sit=bounded_lhs[v].begin(); sit!=bounded_lhs[v].end(); ++sit)
				{
					if (comma) cout << "& "; else comma=true;
					cout << "bounded(" << im.getName(*sit) << ") ";
				}
				cout << "--> bounded(" << im.getName(v) << ")" << endl;
			}
		}
		if (!live_lhs[v].empty()) {
			for(sit=live_lhs[v].begin(); sit!=live_lhs[v].end(); ++sit)
				if (!im.isInvisible(*sit)) break;
			if (sit!=live_lhs[v].end()) {
				bool comma(false);
				for(sit=live_lhs[v].begin(); sit!=live_lhs[v].end(); ++sit)
				{
					if (comma) cout << "| "; else comma=true;
					cout << "live(" << im.getName(*sit) << ") ";
				}
				cout << "--> live(" << im.getName(v) << ")" << endl;
			}
		}
	}

	if (checkCondition(PATHS | COMPLEXPATHS))
	{
		cout << endl << "Path Translations (reduced net --> original net):" << endl;
		Vertex maxv(im.getMaxVertex());
		if (!path[maxv].empty()) {
			cout << "*init* -->";
			for(unsigned int i=0; i<path[maxv].size(); ++i) {
				cout << " ";
				printPathElement(path[maxv],i);
			}
			cout << endl;
		}
		for(Vertex v=0; v<maxv; ++v)
			if (((state[v] >> PATH) & 7L) == NONE && !im.isPlace(v))
			{
				cout << im.getName(v) << " -->";
				for(unsigned int i=0; i<path[v].size(); ++i) {
					cout << " ";
					printPathElement(path[v],i);
				}
				cout << endl;
			} 
	}
}

/** Print complex path elements to screen
	@param it Iterator pointing to the path element to print
	@param pos Pointer to the exact path element to print, will be incremented
*/
void Facts::printPathElement(const Path& rhs, unsigned int& pos) {
	Vertex p(rhs[pos]);
	if (!im.isPlace(p)) {
		cout << im.getName(p);
		return;
	}
	cout << "(";
	while (rhs[++pos] != p) printPathElement(rhs,pos);
	cout << " | ";
	while (rhs[++pos] != p) printPathElement(rhs,pos);
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
				case 16: log[name] += "markings"; break;
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
	for(Vertex v=0; v<im.getMaxVertex(); ++v)
	{
/*
		if (!mark_rhs[v].empty() || mark_add[v] || hasStatus(state[v],MARKING,ISTRUE)) {
			if (mark_rhs[v].size()+(mark_add[v]?1:0)>1)
				log["marking"][im.getName(v)] += "+";
			for(sit=mark_rhs[v].begin(); sit!=mark_rhs[v].end(); ++sit)
				log["marking"][im.getName(v)] += im.getName(*sit);
			if (mark_rhs[v].empty()) 
				log["marking"][im.getName(v)] = mark_add[v];
			else if (mark_add[v]) 
				log["marking"][im.getName(v)] += mark_add[v];
		}
*/
		if (im.isInvisible(v)) continue;
		if (!safe_rhs[v].empty() || hasStatus(state[v],SAFE,ISFALSE) || hasStatus(state[v],SAFE,ISTRUE)) {
			if (safe_rhs[v].size()>1) log["safe"][im.getName(v)] += "|";
			for(sit=safe_rhs[v].begin(); sit!=safe_rhs[v].end(); ++sit)
				log["safe"][im.getName(v)] += im.getName(*sit);
			if (hasStatus(state[v],SAFE,ISFALSE)) log["safe"][im.getName(v)] = false;
			else if (hasStatus(state[v],SAFE,ISTRUE)) log["safe"][im.getName(v)] = true;
		}
		if (!bounded_rhs[v].empty() || hasStatus(state[v],BOUNDED,ISFALSE) || hasStatus(state[v],BOUNDED,ISTRUE)) {
			if (bounded_rhs[v].size()>1) log["bounded"][im.getName(v)] += "|";
			for(sit=bounded_rhs[v].begin(); sit!=bounded_rhs[v].end(); ++sit)
				log["bounded"][im.getName(v)] += im.getName(*sit);
			if (hasStatus(state[v],BOUNDED,ISFALSE)) log["bounded"][im.getName(v)] = false;
			else if (hasStatus(state[v],BOUNDED,ISTRUE)) log["bounded"][im.getName(v)] = true;
		}
		if (!live_rhs[v].empty() || hasStatus(state[v],LIVE,ISFALSE) || hasStatus(state[v],LIVE,ISTRUE)) {
			if (live_rhs[v].size()>1) log["live"][im.getName(v)] += "+";
			for(sit=live_rhs[v].begin(); sit!=live_rhs[v].end(); ++sit)
				log["live"][im.getName(v)] += im.getName(*sit);
			if (hasStatus(state[v],LIVE,ISFALSE)) log["live"][im.getName(v)] = false;
			else if (hasStatus(state[v],LIVE,ISTRUE)) log["live"][im.getName(v)] = true;
		}
	}
	for(Vertex v=0; v<im.getMaxVertex(); ++v)
	{
/*
		if (!mark_lhs[v].empty()) {
			for(sit=mark_lhs[v].begin(); sit!=mark_lhs[v].end(); ++sit)
				if (!im.isInvisible(*sit)) break;
			if (sit!=mark_lhs[v].end()) {
				bool comma(false);
				string tmp;
				for(sit=mark_lhs[v].begin(); sit!=mark_lhs[v].end(); ++sit)
				{
					if (comma) tmp += "+"; else comma=true;
					tmp += im.getName(*sit);
				}
				log["marking"][tmp] = im.getName(v);
			}
		}
*/
		if (mark_lhs_factor[v] && (im.exists(v) || mark_lhs_fix[v]>=-1)) {
			if (mark_lhs_fix[v]>=0)
				log["marking"][im.getName(v)]["*fixed*"] = mark_lhs_fix[v]*mark_lhs_factor[v];
			else if (mark_lhs_fix[v]==-1)
				log["marking"][im.getName(v)]["*fixed*"] = "any";
			log["marking"][im.getName(v)]["*factor*"] = mark_lhs_factor[v];
			map<Vertex,int>::iterator mit;
			for(mit=mark_lhs[v].begin(); mit!=mark_lhs[v].end(); ++mit)
			{
				if (mit->first == NO_NODE)
					log["marking"][im.getName(v)]["*init*"] = mit->second;
				else 
					log["marking"][im.getName(v)][im.getName(mit->first)] = mit->second;
			}
		}
		if (!im.exists(v)) continue;
		if (!safe_lhs[v].empty()) {
			for(sit=safe_lhs[v].begin(); sit!=safe_lhs[v].end(); ++sit)
				if (!im.isInvisible(*sit)) break;
			if (sit!=safe_lhs[v].end()) {
				bool comma(false);
				string tmp;
				for(sit=safe_lhs[v].begin(); sit!=safe_lhs[v].end(); ++sit)
				{
					if (comma) tmp += "&"; else comma=true;
					tmp += im.getName(*sit);
				}
				log["safe"][tmp] = im.getName(v);
			}
		}
		if (!bounded_lhs[v].empty()) {
			for(sit=bounded_lhs[v].begin(); sit!=bounded_lhs[v].end(); ++sit)
				if (!im.isInvisible(*sit)) break;
			if (sit!=bounded_lhs[v].end()) {
				bool comma(false);
				string tmp;
				for(sit=bounded_lhs[v].begin(); sit!=bounded_lhs[v].end(); ++sit)
				{
					if (comma) tmp += "&"; else comma=true;
					tmp += im.getName(*sit);
				}
				log["bounded"][tmp] = im.getName(v);
			}
		}
		if (!live_lhs[v].empty()) {
			for(sit=live_lhs[v].begin(); sit!=live_lhs[v].end(); ++sit)
				if (!im.isInvisible(*sit)) break;
			if (sit!=live_lhs[v].end()) {
				bool comma(false);
				string tmp;
				for(sit=live_lhs[v].begin(); sit!=live_lhs[v].end(); ++sit)
				{
					if (comma) tmp += "|"; else comma=true;
					tmp += im.getName(*sit);
				}
				log["live"][tmp] = im.getName(v);
			}
		}
	}

	if (checkCondition(PATHS | COMPLEXPATHS)) // || (transfer_facts & (PATHS|COMPLEXPATHS)))
	{
		Vertex maxv(im.getMaxVertex());
		for(unsigned int i=0; i<path[maxv].size(); ++i)
			if (im.isPlace(path[maxv][i]))
				printPathElement(log["path"]["*init*"], path[maxv], i);
			else 
				log["path"]["*init*"] += im.getName(path[maxv][i]);
		for(Vertex v=0; v<maxv; ++v)
			if (((state[v] >> PATH) & 7L) == NONE)
				for(unsigned int i=0; i<path[v].size(); ++i)
					if (!im.isPlace(v) && im.isPlace(path[v][i]))
						printPathElement(log["path"][im.getName(v)], path[v], i);
					else 
						log["path"][im.getName(v)] += im.getName(path[v][i]);
	}
}

/** Print a path element to a JSON object
	@param json The JSON object
	@param rhs The path to be printed
	@param pos The position inside the path to print next, may be incremented
*/
void Facts::printPathElement(JSON& json, const Path& rhs, unsigned int& pos) {
	Vertex p(rhs[pos]);
	JSON tmp;
	if (path[p].empty())
		tmp["*if*"] += im.getName(p);
	else
		for(unsigned int i=0; i<path[p].size(); ++i)
			tmp["*if*"] += im.getName(path[p][i]);
	while (rhs[++pos] != p)
		if (im.isPlace(rhs[pos])) printPathElement(tmp["then"],rhs,pos);
		else tmp["then"] += im.getName(rhs[pos]);
	while (rhs[++pos] != p)
		if (im.isPlace(rhs[pos])) printPathElement(tmp["else"],rhs,pos);
		else tmp["else"] += im.getName(rhs[pos]);
	json += tmp;
}

/** Declare a fact/property as deduced
	@param fact The fact
	@param remove If the property should also be removed from the transfer list (default: false)
*/
void Facts::addFact(uint64_t fact, bool remove) {
	pthread_rwlock_wrlock(factlock);
	orig_facts |= fact;
	if (remove) transfer_facts &= ~fact;
	pthread_rwlock_unlock(factlock);
}

/** Declare a fact/property as non-inheritable
	@param fact The fact
*/
void Facts::removeFact(uint64_t fact) {
	pthread_rwlock_wrlock(factlock);
	transfer_facts &= ~fact;
	pthread_rwlock_unlock(factlock);
}

/** Check if a fact/property is a prerequisite
	@param fact The fact
	@return If that is the case
*/
bool Facts::checkFact(uint64_t fact) {
	return (orig_facts & fact);
}

/** Check if a fact/property is a prerequisite
	@param fact The fact
	@return If that is the case
*/
bool Facts::checkCondition(uint64_t fact) {
	return (condition & fact);
}

/** Add info to a detail property.
	@param id The ID of the node for which the property holds
	@param out The ID of the node to be set for the property (i.e. translation)
	@param add A fixed value to be added to the property (MARKING only)
*/
void Facts::setMarking(Vertex id, Vertex out, int add) {
	if (Runtime::args_info.netonly_given) return;
	if (hasStatus(state[id],MARKING)) return;
	mark_rhs[id].insert(out);
	mark_add[id] = add;
	mark_ih[out].insert(id);
	set<Vertex>::iterator sit;
	for(sit=mark_ih[id].begin(); sit!=mark_ih[id].end(); ++sit)
	{
	    pthread_rwlock_wrlock(dellock + *sit);
		if (mark_rhs[*sit].erase(id)) {
			mark_rhs[*sit].insert(out);
			mark_add[*sit] += add;
			mark_ih[out].insert(*sit);
		}
	    pthread_rwlock_unlock(dellock + *sit);
	}
	mark_ih[id].clear();
}

void Facts::setBounded(Vertex id, Vertex out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::BOUNDEDNESS | Facts::UNBOUNDED)) return;
	if (hasStatus(state[id],BOUNDED)) return;
	bounded_rhs[id].insert(out);
	bounded_ih[out].insert(id);
	set<Vertex>::iterator sit;
	for(sit=bounded_ih[id].begin(); sit!=bounded_ih[id].end(); ++sit)
	{
	    pthread_rwlock_wrlock(dellock + *sit);
		if (bounded_rhs[*sit].erase(id)) {
			bounded_rhs[*sit].insert(out);
			bounded_ih[out].insert(*sit);
		}
	    pthread_rwlock_unlock(dellock + *sit);
	}
	bounded_ih[id].clear();
}

void Facts::setSafe(Vertex id, Vertex out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::SAFETY | Facts::UNSAFE)) return;
	if (hasStatus(state[id],SAFE)) return;
	safe_rhs[id].insert(out);
	safe_ih[out].insert(id);
	set<Vertex>::iterator sit;
	for(sit=safe_ih[id].begin(); sit!=safe_ih[id].end(); ++sit)
	{
	    pthread_rwlock_wrlock(dellock + *sit);
		if (safe_rhs[*sit].erase(id)) {
			safe_rhs[*sit].insert(out);
			safe_ih[out].insert(*sit);
		}
	    pthread_rwlock_unlock(dellock + *sit);
	}
	safe_ih[id].clear();
}

void Facts::setLive(Vertex id, Vertex out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::LIVENESS | Facts::NONLIVE)) return;
	if (hasStatus(state[id],LIVE)) return;
	live_rhs[id].insert(out);
	live_ih[out].insert(id);
	set<Vertex>::iterator sit;
	for(sit=live_ih[id].begin(); sit!=live_ih[id].end(); ++sit)
	{
	    pthread_rwlock_wrlock(dellock + *sit);
		if (live_rhs[*sit].erase(id)) {
			live_rhs[*sit].insert(out);
			live_ih[out].insert(*sit);
		}
	    pthread_rwlock_unlock(dellock + *sit);
	}
	live_ih[id].clear();
}

/** Add info to a detail property.
	@param type Whether the info is about MARKING, BOUNDED, SAFE, or LIVE
	@param id The ID of the node for which the property holds
	@param out The IDs of the nodes to be added to the property (i.e. translation)
	@param add A fixed value to be added to the property (MARKING only)
*/
void Facts::setMarking(Vertex id, set<Vertex>& out, int add) {
	if (Runtime::args_info.netonly_given) return;
	if (hasStatus(state[id],MARKING)) return;
	set<Vertex>::iterator sit;
	for(sit=mark_ih[id].begin(); sit!=mark_ih[id].end(); )
	{
		Vertex tmp(*sit);
	    pthread_rwlock_wrlock(dellock + tmp);
		if (mark_rhs[*sit].erase(id)) {
			mark_rhs[*sit].insert(out.begin(),out.end());
			mark_add[*sit] += add;
			++sit;
		} else mark_ih[id].erase(sit++);
	    pthread_rwlock_unlock(dellock + tmp);
	}
	mark_ih[id].insert(id);
	for(sit=out.begin(); sit!=out.end(); ++sit)
		mark_ih[*sit].insert(mark_ih[id].begin(),mark_ih[id].end());
	mark_ih[id].clear();
	mark_rhs[id].swap(out);
	mark_add[id] = add;
	if (out.empty() && add==0)
		state[id] |= ISTRUE << MARKING;
}

void Facts::setBounded(Vertex id, set<Vertex>& out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::BOUNDEDNESS | Facts::UNBOUNDED)) return;
	if (hasStatus(state[id],BOUNDED)) return;
	set<Vertex>::iterator sit;
	for(sit=bounded_ih[id].begin(); sit!=bounded_ih[id].end(); )
	{
		Vertex tmp(*sit);
	    pthread_rwlock_wrlock(dellock + tmp);
		if (bounded_rhs[*sit].erase(id))
			bounded_rhs[*(sit++)].insert(out.begin(),out.end());
		else bounded_ih[id].erase(sit++);
	    pthread_rwlock_unlock(dellock + tmp);
	}
	bounded_ih[id].insert(id);
	for(sit=out.begin(); sit!=out.end(); ++sit)
		bounded_ih[*sit].insert(bounded_ih[id].begin(),bounded_ih[id].end());
	bounded_ih[id].clear();
	bounded_rhs[id].swap(out);
}

void Facts::setSafe(Vertex id, set<Vertex>& out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::SAFETY | Facts::UNSAFE)) return;
	if (hasStatus(state[id],SAFE)) return;
	set<Vertex>::iterator sit;
	for(sit=safe_ih[id].begin(); sit!=safe_ih[id].end(); )
	{
		Vertex tmp(*sit);
	    pthread_rwlock_wrlock(dellock + tmp);
		if (safe_rhs[*sit].erase(id))
			safe_rhs[*(sit++)].insert(out.begin(),out.end());
		else safe_ih[id].erase(sit++);
	    pthread_rwlock_unlock(dellock + tmp);
	}
	safe_ih[id].insert(id);
	for(sit=out.begin(); sit!=out.end(); ++sit)
		safe_ih[*sit].insert(safe_ih[id].begin(),safe_ih[id].end());
	safe_ih[id].clear();
	safe_rhs[id].swap(out);
}

void Facts::setLive(Vertex id, set<Vertex>& out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::LIVENESS | Facts::NONLIVE)) return;
	if (hasStatus(state[id],LIVE)) return;
	set<Vertex>::iterator sit;
	for(sit=live_ih[id].begin(); sit!=live_ih[id].end(); )
	{
		Vertex tmp(*sit);
	    pthread_rwlock_wrlock(dellock + tmp);
		if (live_rhs[*sit].erase(id))
			live_rhs[*(sit++)].insert(out.begin(),out.end());
		else live_ih[id].erase(sit++);
	    pthread_rwlock_unlock(dellock + tmp);
	}
	live_ih[id].insert(id);
	for(sit=out.begin(); sit!=out.end(); ++sit)
		live_ih[*sit].insert(live_ih[id].begin(),live_ih[id].end());
	live_ih[id].clear();
	live_rhs[id].swap(out);
}

/** Add info to a detail property.
	@param type Whether the info is about MARKING, BOUNDED, SAFE, or LIVE
	@param id The IDs of the nodes for which the property holds (i.e. a complex object)
	@param out The ID of the node to be added to the property (i.e. translation)
	@param add A fixed value to be added to the property (MARKING only)
*/
/*
void Facts::setMarking(set<Vertex>& id, Vertex out) {
	if (Runtime::args_info.netonly_given) return;
	set<Vertex> tmp;
	set<Vertex>::iterator sit;
	for(sit=id.begin(); sit!=id.end(); ++sit)
		if (mark_lhs[*sit].empty()) tmp.insert(*sit);
		else {
			tmp.insert(mark_lhs[*sit].begin(),mark_lhs[*sit].end());
			mark_lhs[*sit].clear();
		}
	mark_lhs[out].swap(tmp);
}
*/
void Facts::setMarking(map<Vertex,int>& id, Vertex out, int factor) {
	if (!checkCondition(Facts::CTL | Facts::CTLX | Facts::LTL | Facts::LTLX | Facts::INVARIANT)) return;
/*
for(map<Vertex,int>::iterator mit=id.begin(); mit!=id.end(); ++mit)
	if (mit->first == NO_NODE) cout << mit->second << " ";
	else cout << mit->second << im.getName(mit->first) << " ";
cout << "--> " << factor << im.getName(out) << endl;
*/
	if (id.empty()) {
		mark_lhs_fix[out] = factor;
		if (mark_lhs_factor[out]==0) {
			mark_lhs_factor[out] = 1;
			mark_lhs[out][out] = 1;
		}
		return;
	}
	int mult(1);
	map<Vertex,int> tmp;
	map<Vertex,int>::iterator mit;
	for(mit=id.begin(); mit!=id.end(); ++mit)
		if (mit->first==NO_NODE || mark_lhs[mit->first].empty()) { 
			tmp[mit->first] += mit->second * mult;
			if (mit->first!=NO_NODE) mark_rhs[mit->first].insert(out);
		} else {
			map<Vertex,int>::iterator mit2;
			int h(mark_lhs_factor[mit->first]);
			int factor1(h/ggT(h,mit->second));
			int factor2(mit->second/ggT(h,mit->second));
			if (factor1!=1) {
				for(mit2=tmp.begin(); mit2!=tmp.end(); ++mit2)
					mit2->second *= factor1;
				mult *= factor1;
			}
			for(mit2=mark_lhs[mit->first].begin(); mit2!=mark_lhs[mit->first].end(); ++mit2)
				tmp[mit2->first] += mit2->second * mult * factor2;
			mark_lhs[mit->first].clear();
			mark_lhs_factor[mit->first] = 0;
			mark_rhs[mit->first].insert(out);
		}
	mark_lhs[out].swap(tmp);
	mark_lhs_factor[out] = factor * mult;
	mark_lhs_fix[out] = -2;
}


void Facts::setBounded(set<Vertex>& id, Vertex out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::BOUNDEDNESS | Facts::UNBOUNDED)) return;
	set<Vertex> tmp;
	set<Vertex>::iterator sit;
	for(sit=id.begin(); sit!=id.end(); ++sit)
		if (bounded_lhs[*sit].empty()) tmp.insert(*sit);
		else {
			tmp.insert(bounded_lhs[*sit].begin(),bounded_lhs[*sit].end());
			bounded_lhs[*sit].clear();
		}
	bounded_lhs[out].swap(tmp);
}

void Facts::setSafe(set<Vertex>& id, Vertex out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::SAFETY | Facts::UNSAFE)) return;
	set<Vertex> tmp;
	set<Vertex>::iterator sit;
	for(sit=id.begin(); sit!=id.end(); ++sit)
		if (safe_lhs[*sit].empty()) tmp.insert(*sit);
		else {
			tmp.insert(safe_lhs[*sit].begin(),safe_lhs[*sit].end());
			safe_lhs[*sit].clear();
		}
	safe_lhs[out].swap(tmp);
}

void Facts::setLive(set<Vertex>& id, Vertex out) {
	if (Runtime::args_info.netonly_given) return;
	if (!checkCondition(Facts::LIVENESS | Facts::NONLIVE)) return;
	set<Vertex> tmp;
	set<Vertex>::iterator sit;
	for(sit=id.begin(); sit!=id.end(); ++sit)
		if (live_lhs[*sit].empty()) tmp.insert(*sit);
		else {
			tmp.insert(live_lhs[*sit].begin(),live_lhs[*sit].end());
			live_lhs[*sit].clear();
		}
	live_lhs[out].swap(tmp);
}

/** Set the status of a detail property
	@param type Property type (MARKING, BOUNDED, SAFE, LIVE, PATH)
	@param id The ID of the node for which the property holds
	@param status The new status (NONE, ISFALSE, ISTRUE, UNKNOWN, UNUSABLE). Notes: UNUSABLE cannot be changed again.
		ISFALSE and ISTRUE are for BOUNDED, SAFE, and LIVE only.
*/
void Facts::setStatus(Types type, Vertex id, Status status) {
	if (Runtime::args_info.netonly_given && type!=PATH) return;
	if ((state[id] >> type) & 7L == UNUSABLE) return;
	state[id] &= ~(7L << type);
	state[id] |= (status&7L) << type;
	if (type == PATH) return;

	if (status==UNUSABLE || status==ISFALSE || status == ISTRUE)
	{
		set<Vertex>* ih;
		set<Vertex>* rhs;
		switch (type) {
			case MARKING: ih = mark_ih; rhs = mark_rhs; break;
			case BOUNDED: ih = bounded_ih; rhs = bounded_rhs; break;
			case SAFE:	  ih = safe_ih; rhs = safe_rhs; break;
			case LIVE:	  ih = live_ih; rhs = live_rhs; break;
		}
		rhs[id].clear();
		set<Vertex>::iterator sit,sit2;
		set<Vertex> sw1,sw2;
		sw1.insert(id);
		while (!sw1.empty()) {
			for(sit=sw1.begin(); sit!=sw1.end(); ++sit)
				for(sit2=ih[*sit].begin(); sit2!=ih[*sit].end(); ++sit2)
				{
				    pthread_rwlock_wrlock(dellock + *sit2);
					if (!hasStatus(state[*sit2], type))
					{
						if (status == ISFALSE) {
							rhs[*sit2].erase(*sit);
							if (rhs[*sit2].empty()) {
								sw2.insert(*sit2);
								state[*sit2] |= (status&7L) << type;
							}
						} else {
							sw2.insert(*sit2);
							rhs[*sit2].clear();
							state[*sit2] |= (status&7L) << type;
						}
					}
				    pthread_rwlock_unlock(dellock + *sit2);
				}
			sw1.swap(sw2);
			sw2.clear();
		}
	} 
}

/** Add a path in front of or after an existing property. If the property does not exist, it will be set to the identity first.
	@param id The ID of the node for which the property holds
	@param out The path to add. The path will be unwound before adding.
	@param before If the path is to be insert in front of the property (default: true) or after it (false)
	@param empty If the path should not contain the ID id.
*/
void Facts::addPath(Vertex id, Path out, bool before, bool empty) {
	if (!checkCondition(PATHS | COMPLEXPATHS)) return;
	if (id == NO_NODE) id = im.getMaxVertex(); // redirection for *init* sequence
	pthread_rwlock_wrlock(pathlock + id);
	if (id < im.getMaxVertex() && path[id].empty() && !empty)
		path[id].push_back(id);
	Path tmp;
	for(unsigned int i=0; i<out.size(); ++i)
		if (!path[out[i]].empty())
			tmp.insert(tmp.end(), path[out[i]].begin(), path[out[i]].end());
		else tmp.push_back(out[i]);
	path[id].insert(before ? path[id].begin() : path[id].end(), tmp.begin(), tmp.end());
/*
	if (id != NO_NODE && path.find(id)==path.end()) 
		path[id].push_back(id);
	Path tmp;
	for(unsigned int i=0; i<out.size(); ++i)
		if (path.find(out[i])!=path.end())
			tmp.insert(tmp.end(), path[out[i]].begin(), path[out[i]].end());
		else tmp.push_back(out[i]);
	path[id].insert(before ? path[id].begin() : path[id].end(), tmp.begin(), tmp.end());
*/
	pthread_rwlock_unlock(pathlock + id);
}

/** Add a path in front of or after an existing property. If the property does not exist, it will be set to the identity first.
	@param id The ID of the node for which the property holds
	@param out The path to add. The path will NOT be unwound before adding.
	@param before If the path is to be insert in front of the property (default: true) or after it (false)
*/
void Facts::addFixedPath(Vertex id, Path out, bool before) {
	if (!checkCondition(PATHS | COMPLEXPATHS)) return;
	if (id == NO_NODE) id = im.getMaxVertex(); // redirection for *init* sequence
	pthread_rwlock_wrlock(pathlock + id);
	if (id < im.getMaxVertex() && path[id].empty())
		path[id].push_back(id);
	path[id].insert(before ? path[id].begin() : path[id].end(), out.begin(), out.end());
	pthread_rwlock_unlock(pathlock + id);
}

/** Unwind a path, i.e. project it from the actual reduction state to the original net.
	@param in The path from the actual reduction state
	@return The unwound path in the original net
*/
Path Facts::getPath(Path& in) {
	Path tmp;
	if (checkCondition(PATHS | COMPLEXPATHS)) {
		for(unsigned int i=0; i<in.size(); ++i)
		{
			pthread_rwlock_rdlock(pathlock + in[i]);
			if (!path[in[i]].empty())
				tmp.insert(tmp.end(),path[in[i]].begin(),path[in[i]].end());
			else tmp.push_back(in[i]);
			pthread_rwlock_unlock(pathlock + in[i]);
		}
	}
	return tmp;
}

/** Set a path. The path will not be unwound.
	@param id The ID of the node for which the path should be set
	@param out The path to be set.
*/
/*
void Facts::setPath(Vertex id, const Path& out) {
	if (!checkCondition(PATHS | COMPLEXPATHS)) return;
	pthread_rwlock_wrlock(pathlock + id);
	path[id] = out;
	pthread_rwlock_unlock(pathlock + id);
}
*/

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
			tokens = 0;
			if (mark_rhs[place].empty()) {
				if (mark_lhs_factor[place]==1 && mark_lhs_fix[place]>=0) {
					tokens = -mark_lhs_fix[place];
					place = NO_NODE;
				}
			} else {
				place = *(mark_rhs[place].begin());
				tokens = mark_lhs[place].rbegin()->second;
			}
/*
			tokens = -mark_add[place];
			if (mark_rhs[place].empty()) {
				if (tokens || hasStatus(state[place],MARKING,ISTRUE))
					place = NO_NODE;
			} else place = *(mark_rhs[place].begin());
*/
/*
			set<Vertex> tmp;
			tmp.insert(place);
			map<set<Vertex>,set<Vertex> >::iterator it1(mark2.find(tmp));
			map<set<Vertex>,int>::iterator it2(markadd.find(tmp));
			if (it2!=markadd.end()) tokens=-it2->second;
			if (it1!=mark2.end()) {
				if (it1->second.empty()) place = NO_NODE;
				else place = *(it1->second.begin());
			}
*/
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

