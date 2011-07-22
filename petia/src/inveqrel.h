// -*- C++ -*-

/*!
 * \file    inveqrel.h
 *
 * \brief   Class InvEqRel
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2011/07/13
 *
 * \date    $Date: 2011-07-13 14:47:01 +0200 (Mi, 13. Jul 2011) $
 *
 * \version $Revision: 1.0 $
 */

#ifndef INVEQREL_H
#define INVEQREL_H

#include <vector>
#include <map>
#include <set>

#include "pnapi/pnapi.h"

using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using pnapi::Node;
using std::vector;
using std::map;
using std::set;

/*! \brief This class maintains equivalence classes of the places of a net
		regarding membership in semipositive place invariants.
*/
class InvEqRel {

public:
	/// Standard constructor
	InvEqRel(PetriNet& pn, bool ntype, bool pricheck);

	/// Destructor.
	~InvEqRel();

	/// Two places/transitions to be in the same equivalence class
	bool join(Node* n1, Node* n2);

	/// Two places/transitions to be in the same equivalence class, successful joins are counted
	bool pjoin(Node* n1, Node* n2);

	/// Coarsen the relation according to an invariant distinguishing two sets of non-equivalent nodes
	void split(map<Node*,int> inv);

	/// Get the classes as they are now, approx=true: relation may be coarser, otherwise finer than equivalence
	vector<set<Node*> > getClasses(bool approx);

	/// Check if an equivalence relation has been reached, if not, return two places for which the relation is still unknown
	bool getUndecided(Node*& n1, Node*& n2);

	/// Precompute some equivalent nodes from the structure of the net
	void simpleEquivalences();

	/// Find the representative of the class containing the node in ''below''
	Node* findClass(Node* n);

	/// Find the ID of the class containing the node in ''below''
	int findClassNum(Node* n);

	/// Get the number of joins in the preparation phase
	unsigned int preJoinsDone() const;

	/// Initialize priority checks. Called by constructor if pricheck=true
	void initPriorityChecks();

	/// Get a priority check if there are any. Called by getUndecided()
	bool findPriorityCheck(Node*& n1, Node*& n2);

private:
	/// to find a node in the coarser relation ''above''
	map<Node*,unsigned int> toclass;

	/// a relation coarser than the equivalence
	vector<set<Node*> > above;

	/// a relation finer than the equivalence
	map<Node*,set<Node*> > below;

	/// first active entry in above
	unsigned int start;

	/// The Petri net
	PetriNet& net;

	/// data structure for union-find in `below'
	vector<int> unionfind;

	/// mapper from int to places, for union-find
	vector<Node*> nvec;

	/// mapper from places to int, for union-find
	map<Node*,int> nmap;

	/// number of joins done in the preparation phase
	unsigned int prejoin;

	/// vector of pre/postsets where the other is a singleton
	vector<set<Node*> > ppcheck;

	/// vector of singleton pre/postsets
	vector<Node*> ppnode;

	bool nodetype;
	set<Node*> places;
	set<Node*> transitions;
};

#endif

