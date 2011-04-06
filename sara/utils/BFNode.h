// -*- C++ -*-

/*!
 * \file    BFNode.h
 *
 * \brief   Class BFNode
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2011/03/28
 *
 * \date    $Date: 2011-03-28 14:47:01 +0200 (Mo, 28. Mar 2011) $
 *
 * \version $Revision: 1.05 $
 */

#ifndef BFNODE_H
#define BFNODE_H

#include <vector>
#include <map>
#include <set>
#include <string>

using std::vector;
using std::map;
using std::set;
using std::string;

/*! \brief Nodes for Boolean Formula Tree
*/
class BFNode {
 public:
	/// Constructor
	BFNode();

	/// Destructor for nodes
	~BFNode();

	/// Collect all nodes in the acyclic graph
	void allNodes(set<BFNode*>& all);

	/// Remove all sons without deleting them
	void clear();

	/// Make a node into an inner node.
	bool setInner(int bop);

	/// Make a node into a leaf.
	bool setLeaf(string place, int comp, int val);

	/// Add a son node to an inner node
	bool addSon(BFNode* son);

	/// Set all sons of an inner node at once
	bool setSons(set<BFNode*> sons);

	/// Get all sons of an inner node at once
	set<BFNode*> getSons();

	/// Check if the node is a leaf
	int isLeaf();

	/// Get the boolean operator for a leaf
	int getOp();

	/// Get the place name for a leaf
	string getPlace();

	/// Convert formula to DNF
	void toDNF(bool neg);

	/// Successor nodes (sons) in the tree
	set<BFNode*> succ;

	/// Atomic formula: name of the place
	string p;

	/// Atomic formula: comparator (0=subformula)
	int cp;

	/// Atomic formula: number of tokens, Subformula: boolean operator
	int op;

	/// Comparator types
	enum CType { BFINVALID=-1, BFFORM=0, BFLE=1, BFEQ=2, BFGE=3, BFNE=4 };

	/// Boolean operator types
	enum BFOp { BFNOT=0, BFAND=1, BFOR=2 };
};

#endif

