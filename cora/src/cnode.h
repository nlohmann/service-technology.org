// -*- C++ -*-

/*!
 * \file    cnode.h
 *
 * \brief   Class CNode
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/05/12
 *
 * \date    $Date: 2010-05-12 12:00:00 +0200 (Mi, 12. Mai 2010) $
 *
 * \version $Revision: -1 $
 */

#ifndef CNODE_H
#define CNODE_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include "pnapi/pnapi.h"
#include "extmarking.h"
#include "imatrix.h"
#include "stubbornset.h"

using std::map;
using std::string;
using std::set;
using std::vector;
using std::ostream;
using pnapi::Place;
using pnapi::Transition;
using pnapi::PetriNet;
using pnapi::Marking;

/*! \brief Nodes of the coverability graph. 

	This class represents the nodes of coverability graphs with the additional
	ability that nodes can be split at (p,n)-borders where p is a place and
	n is a number of tokens for this place. Any node can be split into two
	subnodes, one containing all markings where p has less than n tokens, the
	other all markings where p has at least n tokens.

	A node that is not the result of a split is called a root node or a global node.
	Each root node contains a tree of its subnodes, where each inner node
	in the tree represents a (p,n)-decision (as above). Inner nodes do not
	have their own CNode structure and can be accessed only via their root node. 
	A leaf node of the tree has its own CNode structure and therefore knows e.g. 
	which markings are contained in it and where its absolute position in the 
	tree is. Root nodes have the CNode structure regardless of whether they are 
	inner or leaf nodes, but they can represent only extended markings where 
	places have either a single finite value or omega as token numbers. These 
	markings are called regular here.

	While root nodes are used to form a standard coverability graph (which is
	deterministic), subnodes can have edges to other (especially inner) subnodes
	thus leading to nondeterminism, the edge targetting all leafs below that subnode
	in its tree. New edges to new nodes can only be created for root nodes; if
	a subnode needs a new successor, first its root's successor must be built,
	then that successor can be split (or the root's successor can just be used 
	for both targets).
*/
class CNode {
public:
	/// Standard constructor for root nodes.
	CNode();

	/// Constructor with marking for root nodes.
	CNode(ExtMarking em);

	/// Constructor for subnodes.
	CNode(ExtMarking& extm, CNode& r, int father, bool higher);

	/// Destructor.
	~CNode();

	/// set the root node and make this node a subnode (if cnode is not NULL)
	void setRoot(CNode* cnode);

	/// get the root node of this node (can be the node itself)
	CNode* getRoot();

	/// check if the node is a root node
	bool isRoot();

	/// set an extended marking as label
	bool setMarking(ExtMarking& extm);

	/// get the extended marking belonging to this node
	ExtMarking& getMarking();

	/// compute the firable transitions (mainly for root nodes)
	void computeFirables(PetriNet& pn, IMatrix& im);

	/// compute the firable transitions of this subnode's root node
	bool computeFirables();

	/// add a list of transitions for which edges need to be created
	void addToFirables(vector<Transition*> tvec);

	/// set a successor to this node (cnode==NULL: declare the transition disabled for this subnode)
	void setSuccessor(Transition& t, CNode* cnode);

	/// check if the marking covers an earlier root node and introduce omegas accordingly
	bool addOmega(ExtMarking& extm);

	/// check if a transition still has to be worked on (possibly obtaining new edges emerging from here)
	bool needsWork(Transition& t);

	/// get a transition for which an edge still has to be created
	Transition* getWork();

	/// remove a transition from the todo list
	void workDone(Transition& t);

	/// check if a transition t already leads to a node cnode
	bool hasSuccessor(Transition& t, CNode& cnode);

	/// get the subnode successors of this node with edges labelled with transition t
	set<CNode*> getSuccessors(Transition& t);

	/// get the successor map of this (sub)node
	map<Transition*,CNode*> getSuccessors();

	/// get the subnode successor map of this (sub)node
	map<Transition*,int> getSubSuccessors();

	/// get the root successor of a (root or sub-) node
	CNode* getSuccessor(Transition& t);

	/// get the subnode index of the node an edge points to
	int getSubSuccessor(Transition& t);

	/// obtain all subnodes (result may contain NULL vectors among others)
	vector<CNode*> getSubNodes();

	/// obtain all subnodes in a subtree rooted at index
	set<CNode*> getSubNodes(int index);

	/// get all edge labels of edges originating at this subnode
	set<Transition*> getEdgeLabels();

	/// split some subnode, successors are inherited
	bool simpleSplit(Place& p, int token, IMatrix& im, StubbornSet* sb);

	/// split a root node that is a predecessor of a just split node cn (via transition t)
	bool preSplit(Transition& t, CNode& cn, IMatrix& im, StubbornSet* sb);
	
	/// print a root node including subtree and leaves
	void show(ostream& o, vector<Place*>& pord);

	/// get the default subnode successor (right son in case of a choice)
	CNode* getDefaultSubSuccessor(Transition& t);

	/// get the index (position) of this subnode in its root node tree
	int getPos();

	/// go to one of the sons of a subnode of this root node in the subtree, depending on a marking
	int goToSon(int subnode, ExtMarking& m);

	/// check if a subnode of this root node in a subtree is a leaf
	bool isLeaf(int subnode);

	/// get a sub-CNode of this root node by its index (result NULL for inner nodes)
	CNode* getSubNode(int subnode);

	/// set the stubborn set for this node
	void setStubbornSet(vector<Transition*> stubset);

	/// get/compute the stubborn set for this node
	vector<Transition*> getStubbornSet(StubbornSet& stubset, bool compute);

private:
	/// Successor root nodes, for subnodes an additional integer is needed
	map<Transition*,CNode*> succ;

	/// Successor subnode index
	map<Transition*,int> subsucc;

	/// Predecessor nodes (not differentiated by transitions, root nodes only)
	set<CNode*> pred;

	/// The stubborn set for this node, if available
	vector<Transition*> stubbornset;

	/// Transitions for which no arcs have been created yet (root nodes only)
	set<Transition*> todo;

	/// Pointer from subnode to root node, NULL if this is a root node
	CNode* root;

	/// node label (extended marking)
	ExtMarking em;

	/// Subnode father (-1 for root's non-existent father)
	vector<int> father;

	/// Subnode lower (left) son (-1 if non-existent)
	vector<int> lson;

	/// Subnode higher (right) son (-1 if non-existent)
	vector<int> rson;

	/// Subnode: place of the condition (NULL if non-existent)
	vector<Place*> pcond;

	/// Subnode: token number of the condition
	vector<int> tcond;

	/// Subnode: the node itself (leaves only)
	vector<CNode*> leaf;

	/// Subnode tree: first free position (for root nodes), for subnodes: its index in the tree
	int pos;
};

#endif

