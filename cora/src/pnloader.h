// -*- C++ -*-

/*!
 * \file    pnloader.h
 *
 * \brief   Class PNLoader
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/21
 *
 * \date    $Date: 2009-10-21 12:00:00 +0200 (Mi, 21. Okt 2009) $
 *
 * \version $Revision: -1 $
 */

#ifndef PNLOADER_H
#define PNLOADER_H

#include <map>
#include <set>
#include <vector>
#include <string>
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif

using std::map;
using std::string;
using std::set;
using std::vector;
using pnapi::Place;
using pnapi::Transition;
using pnapi::PetriNet;
using pnapi::Marking;

/*! \brief Objects of this class may contain problems to be solved. They usually allow to load a Petri net. */
class PNLoader {
public:
	/// Standard constructor.
	PNLoader();

	/// Destructor.
	~PNLoader();

	/// Deinitializer.
	void clear();

	/// Set the problem name
	void setName(string s);

	/// Set the problem goal
	void setGoal(int g);

	/// Set the Petri net filename
	void setFilename(string fn);

	/// Get the Petri net filename
	string getFilename();

	/// Set the Petri net type
	void setNetType(int nt);

	/// Set initial token number for a place name
	void setInit(string s, int i);

	/// Set final token number for a place name
	void setFinal(string s, int f);

	/// Set cover mode for a place name
	void setCover(string s, int mode);

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

	/// Get the Petri net
	PetriNet* getPetriNet();

	/// Get the problem goal
	int getGoal();

	/// Get the problem name
	string getName();

	/// Check whether this problem needs the same Petri net and inherit it 
	void checkForNetReference(PNLoader& p);

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

	/// Obtain the transition ordering
	const vector<Transition*>& getTOrder();

	/// Obtain the place ordering
	const vector<Place*>& getPOrder();

	/// Obtain the inverse transition ordering
	const map<Transition*,int>& getInvTOrder();

	/// Obtain the inverse place ordering
	const map<Place*,int>& getInvPOrder();

	/// Goals
	enum Types {
		NOTYPE = 0
	};

	/// Types of Petri nets
	enum NetTypes {
		OWFN = 0,
		LOLA = 1,
		PNML = 2
	};

	/// Comparisons
	enum Comparisons {
		EQ = 0,
		GE = 1,
		LE = -1
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

	/// transition ordering
	vector<Transition*> transitionorder;

	/// place ordering
	vector<Place*> placeorder;

	/// inverse transition ordering
	map<Transition*,int> revtorder;

	/// inverse place ordering
	map<Place*,int> revporder;
};

#endif

