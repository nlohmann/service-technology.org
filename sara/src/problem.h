// -*- C++ -*-

/*!
 * \file    problem.h
 *
 * \brief   Class Problem
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/09/16
 *
 * \date    $Date: 2012-06-13 12:00:00 +0200 (Mi, 13. Jun 2012) $
 *
 * \version $Revision: 1.10 $
 */

#ifndef PROBLEM_H
#define PROBLEM_H

#include <map>
#include <set>
#include <vector>
#include <string>
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "imatrix.h"

using std::map;
using std::string;
using std::set;
using std::vector;
using pnapi::Place;
using pnapi::Transition;
using pnapi::PetriNet;
using pnapi::Marking;

namespace sara {

/*! \brief Objects of this class contain problems (e.g. reachability) that are to be solved. */
class Problem {
public:
	/// Standard constructor.
	Problem();

	/// Destructor.
	~Problem();

	/// Deinitializer.
	void clear();

	/// Set the problem name
	void setName(string s);

	/// Set the problem goal
	void setGoal(int g);

	/// Set the Petri net filename
	void setFilename(string fn);

	/// Get the Petri net filename
	string getFilename() const;

	/// Set the Petri net type
	void setNetType(int nt);

	/// Set initial token number for a place name
	void setInit(string s, int i);

	/// Set final token number for a place name
//	void setFinal(string s, int f);

	/// Set cover mode for a place name
//	void setCover(string s, int mode);

	/// Set final token number and cover mode for a place name
	void setFinal(string s, int mode, int f);

	/// Set cover mode for all places not appearing in the final marking
	void setGeneralCover(bool cov);

	/// Get the initial marking
	Marking getInitialMarking();

	/// Get the final marking
	Marking getFinalMarking();

	/// Get the cover/bound/reach requirements for all places
	map<Place*,int> getCoverRequirement();

	/// Get the transition vector to realize
	map<Transition*,int> getVectorToRealize();

	/// Get the Petri net (will load the net from a file if necessary)
	PetriNet* getPetriNet();

#ifdef SARALIB
	/// Set the Petri net (for already loaded nets only)
	void setPetriNet(PetriNet& net);
#endif

	/// Get the incidence matrix of the Petri net (which must be loaded already)
	IMatrix* getIMatrix();

	/// Get the problem goal
	int getGoal() const;

	/// Get the problem name
	string getName() const;

	/// Check whether this problem needs the same Petri net and inherit it 
	void checkForNetReference(Problem& p);

	/// Calculate the global orderings for places and transitions
	bool calcPTOrder();

	/// Add an external constraint
	void addConstraint(map<string,int> lhs, int comp, int rhs);

	/// Get an external constraint
	void getConstraint(unsigned int pos, map<Transition*,int>& lhs, int& comp, int& rhs);

	/// Get the number of external constraints
	unsigned int getNumberOfConstraints();

	/// Print the vector of transitions to realize
	void showTVector();

	/// Print the initial marking
	void showInitial();

	/// Print the extended final marking
	void showFinal();

	/// Print the global constraints
	void showConstraints();

	/// Set whether the final result of a property check should be inverted
	void setNegateResult(bool neg);

	/// Set the name of the property to be checked by this problem instance
	void setResultText(string res);

	/// Whether the final result of a property check should be inverted
	bool isNegateResult() const;

	/// The name of the property to be checked by this problem instance
	string getResultText() const;

	/// Set whether the final result of a property check should be computed as an OR
	void setOrResult(bool res);

	/// Whether the final result of a property check should be ORed
	bool isOrResult() const;

	/// Goals
	enum Types {
		REACHABLE = 0,
		REALIZABLE = 1,
		DUMMY = 2
	};

	/// Types of Petri nets
	enum NetTypes {
		OWFN = 0,
		LOLA = 1,
		PNML = 2
	};

private:
	/// Name of the problem
	string name;

	/// Whether the Petri net must be unloaded after the problem is solved
	bool deinit;

	/// Whether token numbers on places not mentioned in the final marking are covered or reached
	bool generalcover;

	/// The problem type
	int type;

	/// The Petri net filename
	string filename;

	/// The initial marking (with place names instead of places)
	map<string,int> initial;

	/// The final marking or transition vector (with object names instead of objects)
	map<string,int> required;

	/// The cover requirement (with place names instead of places)
	map<string,int> cover;	

	/// The Petri net type
	int nettype;

	/// The Petri net
	PetriNet* pn;

	/// complex constraints, left hand
	vector<map<string, int> > clhs;

	/// complex constraints, comparator
	vector<int> ccomp;

	/// complex constraints, right hand
	vector<int> crhs;

	/// Whether the property checked here has to be negated
	bool negate;

	/// The name a property to be checked
	string result;

	/// Whether all or at least one problem has to check out for the property
	bool resor;

	/// An internal incidence matrix of the net
	IMatrix* im;
};

}

#endif

