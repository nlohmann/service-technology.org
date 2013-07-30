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
		INVARIANT = 0x10000LL,

		ALLFACTS = 0x1FFFFLL,
		NOFACTS = 0x0LL
	};

	/// Print general facts to screen
	string print(uint64_t facts);

	/// Print all properties to screen
	void printFacts();

	/// Recursive path builder with screen output
	void printPathElement(const Path& rhs, unsigned int& pos);

	/// Print general facts to JSON object
	void print(JSON& log, string name, uint64_t facts);

	/// Print all properties to JSON object
	void printFacts(JSON& log);

	/// Recursive path builder with JSON object construction
	void printPathElement(JSON& json, const Path& rhs, unsigned int& pos);

	/// add a general fact that can be deduced directly from the reduction process
	void addFact(uint64_t facts, bool remove = true);

	/// remove a fact for which inheriting becomes invalid
	void removeFact(uint64_t facts);

	/// remove facts that strictly less expressive than others
	void reduceFacts(uint64_t& facts, bool direction);

	/// check if a general fact has been deduced
	bool checkFact(uint64_t facts);

	/// check if a general fact is a prerequisite
	bool checkCondition(uint64_t facts);

	/// add to the right side of a property map
	void setMarking(Vertex id, Vertex out, int add=0);
	void setBounded(Vertex id, Vertex out);
	void setSafe(Vertex id, Vertex out);
	void setLive(Vertex id, Vertex out);

	/// add to the right side of a property map
	void setMarking(Vertex id, set<Vertex>& out, int add=0);
	void setBounded(Vertex id, set<Vertex>& out);
	void setSafe(Vertex id, set<Vertex>& out);
	void setLive(Vertex id, set<Vertex>& out);

	/// add to the right side of a property map
	void setMarking(map<Vertex,int>& id, Vertex out, int factor);
//	void setMarking(set<Vertex>& id, Vertex out);
	void setBounded(set<Vertex>& id, Vertex out);
	void setSafe(set<Vertex>& id, Vertex out);
	void setLive(set<Vertex>& id, Vertex out);

	/// add a path to a property after unwinding
	void addPath(Vertex id, Path out, bool before = true, bool empty = false);

	/// add a path to a property directly
	void addFixedPath(Vertex id, Path out, bool before = true);

	/// unwind a path
	Path getPath(Path& in);

	/// set a path to a property directly
//	void setPath(Vertex id, const Path& out);

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

	bool hasStatus(unsigned int status, Types type, Status val = NONE);

	/// set the status of a property
	void setStatus(Types type, Vertex id, Status status);


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
	unsigned int* state;

	/// marking info: fixed tokens to add to a property
	int* mark_add;

	/// marking info
//	set<Vertex>* mark_lhs;
	set<Vertex>* mark_rhs;
	set<Vertex>* mark_ih;
	map<Vertex,int>* mark_lhs;
	int* mark_lhs_factor;
	int* mark_lhs_fix;

	/// boundedness info
	set<Vertex>* bounded_lhs;
	set<Vertex>* bounded_rhs;
	set<Vertex>* bounded_ih;

	/// safety info
	set<Vertex>* safe_lhs;
	set<Vertex>* safe_rhs;
	set<Vertex>* safe_ih;

	/// liveness info
	set<Vertex>* live_lhs;
	set<Vertex>* live_rhs;
	set<Vertex>* live_ih;

	/// path info
	Path* path;

	pthread_rwlock_t* factlock;
	pthread_rwlock_t* pathlock;
	pthread_rwlock_t* dellock;
};

#endif
