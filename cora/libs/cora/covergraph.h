// -*- C++ -*-

/*!
 * \file    covergraph.h
 *
 * \brief   Class CoverGraph
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/05/12
 *
 * \date    $Date: 2010-05-12 12:00:00 +0200 (Mi, 12. Mai 2010) $
 *
 * \version $Revision: -1 $
 */

#ifndef COVERGRAPH_H
#define COVERGRAPH_H

#include <deque>
#include "pnapi/pnapi.h"
#include "rnode.h"
#include "cnode.h"
#include "extmarking.h"
#include "imatrix.h"

using pnapi::Marking;
using pnapi::PetriNet;
using std::deque;
using std::vector;

/*! \brief A class representing coverability graphs. 

	Coverability graphs can be of the standard variation or they can
	have nodes where the token number for a place can be an interval,
	closed or without an upper bound. Such nodes are handled as subnodes
	of the standard nodes, see the CNode structure. Stubborn set
	methods can be used to restrict the size of the graph by disallowing
	some redundant edges and nodes on paths to a goal marking, if the
	coverability graph is used to determine reachability.
*/
class CoverGraph {
public:
	/// Standard constructor.
	CoverGraph(PetriNet& pn, IMatrix& im, Marking& m, StubbornSet* stubset);

	/// Destructor.
	~CoverGraph();

	/// Check and compute a (possibly new) root node using a transition t
	bool createSuccessor(CNode& cn, Transition& t);

	/// Get the initial oot node
	CNode* getInitial();

	/// Compute (and possibly create) all successor nodes for a given node
	bool createSuccessors(CNode& cn);

	/// Print the coverability graph (with or without subnodes)
	void printGraph(vector<Place*>& porder, bool rootonly);

	/// Complete the coverability graph with root nodes.
	void completeGraph();

	/// Add the root node successors of one node from the todo list to the coverability graph.
	bool completeOneNode();

	/// Create a path for a sequence of transitions if possible (root nodes only)
	bool createPath(vector<Transition*> tvec);

	/// Check if a path (of root nodes) exists in the coverability graph
	bool checkPath(vector<Transition*> tvec);

	/// Find a path to a subnode in the coverability graph
	deque<Transition*> findPath(CNode& cn);

	/// Find a path to an extmarking in the coverability graph with a non-empty intersection
	deque<Transition*> findPath(ExtMarking& em);

	/// Check if all transitions on a path are firable and split the path if not
	bool splitPath(deque<Transition*>& path);

	/// Get all nodes
	set<RNode>& getAllNodes();

	/// Get the first node in the todo list (of nodes with possibly missing successors)
	CNode* firstToDo();

	/// Remove the first entry in the todo list if there is one.
	bool popToDo();

	/// Remove a given node from the todo list if it is in there.
	bool popToDo(CNode* cn);

	/// Put a node into the todo list at the appropriate point
	void pushToDo(CNode* cn);

	/// Switch stubborn set method on/off
	void useStubbornSetMethod(StubbornSet* stubset);

	/// Pump up a covering path so it becomes firable
	deque<Transition*> pumpPath(Marking m0, deque<Transition*> path, Marking mf);

private:
	/// The Petri net of this coverability graph
	PetriNet& net;

	/// The incidence matrix of the Petri net of this coverability graph
	IMatrix& imat;
	
	/// Pointer to initial node
	CNode* init;

	/// Set of all nodes for checking for existing extended markings and for easy destruction
	set<RNode> nodes;

	/// Map of nodes to work on, ordered by distance (first parameter) to the final marking
	map<int,set<CNode*> > todo;

	/// Stubborn set computing methods (NULL=no partial order reduction used)
	StubbornSet* sb;
};

#endif

