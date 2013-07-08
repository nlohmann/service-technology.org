// -*- C++ -*-

/*!
 * \file    facts.h
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

#ifndef FACTS_H
#define FACTS_H

#include <map>
#include <string>
#include <deque>
#include "imatrix.h"
#include "JSON.h"

using std::map;
using std::string;
using std::deque;
using pnapi::Place;
using pnapi::formula::Formula;

typedef deque<Vertex> Path;

/*! \brief For storing information about properties and property transfers from the original net
	as well as changes that need to be made to formulas
*/
class Facts {

public:
	/// Constructor
	Facts(IMatrix& mat);

	/// Constructor with condition
	Facts(IMatrix& mat, uint64_t cond);

	/// Destructor
	~Facts();

	/// Properties that can be inherited
	enum props {
		NONLIVE = 0x1LL,
		LIVENESS = 0x2LL,
		CTLX = 0x4LL,
		LTLX = 0x8LL,
		CTL = 0x10LL,
		LTL = 0x20LL,
		UNBOUNDED = 0x40LL,
		BOUNDEDNESS = 0x80LL,
		ALTL = 0x100LL,
		ALTLX = 0x200LL,
		PATHS = 0x400LL,
		COMPLEXPATHS = 0x800LL,
		SAFETY = 0x1000LL,
		UNSAFE = 0x2000LL,
		BISIM = 0x4000LL,
		REVERSE = 0x8000LL,

		ALLFACTS = 0xFFFFLL,
		NOFACTS = 0x0LL
	};

	/// Print general facts to screen
	string print(uint64_t facts);

	/// Print all properties to screen
	void printFacts();

	/// Recursive path builder with screen output
	void printPathElement(const map<Vertex,Path>::iterator& it, unsigned int& pos);

	/// Print general facts to JSON object
	void print(JSON& log, string name, uint64_t facts);

	/// Print all properties to JSON object
	void printFacts(JSON& log);

	/// Recursive path builder with JSON object construction
	void printPathElement(JSON& json, const map<Vertex,Path>::iterator& it, unsigned int& pos);

	/// add a general fact that can be deduced directly from the reduction process
	void addFact(uint64_t facts, bool remove = true);

	/// remove a fact for which inheriting becomes invalid
	void removeFact(uint64_t facts);

	/// remove facts that strictly less expressive than others
	void reduceFacts(uint64_t& facts, bool direction);

	/// check if a general fact is a prerequisite
	bool checkCondition(uint64_t facts);

	/// add to the right side of a property map
	void addChange(unsigned int type, Vertex id, Vertex out, int add=0);

	/// add to the right side of a property map
	void addChange(unsigned int type, Vertex id, set<Vertex>& out, int add=0);

	/// add to the right side of a property map
	void addChange(unsigned int type, set<Vertex>& id, Vertex out, int add=0);

	/// set the status of a property
	void setStatus(unsigned int type, Vertex id, unsigned int status);

	/// add a path to a property after unwinding
	void addPath(Vertex id, Path out, bool before = true);

	/// add a path to a property directly
	void addFixedPath(Vertex id, Path out, bool before = true);

	/// unwind a path
	Path getPath(Path& in);

	/// set a path to a property directly
	void setPath(Vertex id, const Path& out);

	/// Adapt the final condition to the reduced net
	Formula* transferFormula(const Formula& oldf, map<const Place*,const Place*>& translate);

	enum Types { // most values are used for bit-shift operations, three bits per type
		MARKING = 0,
		FACTS = 1,
		STATUS = 2,
		BOUNDED = 3,
		LIVE = 6,
		PATH = 9,
		SAFE = 12,

		MAXTYPES = 13
	};

	enum Status {
		NONE = 0,
		ISFALSE = 4,
		ISTRUE = 5,
		UNKNOWN = 6,
		UNUSABLE = 7
	};

private:

	/// incidence matrix stuff
	IMatrix& im;

	/// prerequisite facts
	uint64_t condition;

	/// deduced facts
	uint64_t orig_facts;

	/// inherited facts
	uint64_t transfer_facts;

	/// general state of specific property
	map<Vertex, unsigned int> state;

	/// marking info: fixed tokens to add to a property
	map<set<Vertex>, int> markadd;

	/// marking info
	map<set<Vertex>, set<Vertex> > mark2;

	/// boundedness info
	map<set<Vertex>, set<Vertex> > bounded;

	/// safety info
	map<set<Vertex>, set<Vertex> > safe;

	/// liveness info
	map<set<Vertex>, set<Vertex> > live;

	/// path info
	map<Vertex, Path> path;

//	map<Vertex, map<Vertex, int> > pathcond;

	pthread_rwlock_t* locks;
	// TYPES: unchanged
	// 1: facts
	// 2: state
	// 5: markadd
};

#endif
