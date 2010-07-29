// -*- C++ -*-

/*!
 * \file    rnode.cc
 *
 * \brief   Class RNode
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/05/14
 *
 * \date    $Date: 2010-05-14 14:47:01 +0200 (Fr, 16. Mai 2010) $
 *
 * \version $Revision: -1 $
 */

#include "rnode.h"
#include "cnode.h"

	/********************************
	* Implementation of class RNode *
	********************************/

/** Standard constructor. */ 
RNode::RNode(CNode& cn) : cnode(&cn) {}

/** Destructor. */
RNode::~RNode() {}

/** Comparison operator for nodes, does not realize covering.
	@param right Right hand side of the expression.
	@return If this is less than right.
*/
bool RNode::operator<(const RNode& right) const {
	return (cnode->getMarking()<right.cnode->getMarking());
}

/** Comparison operator for nodes in set comparisons.
	@param right Right hand side of the expression.
	@return If the markings of both nodes are equal.
*/
bool RNode::operator==(const RNode& right) const {
	return (cnode->getMarking()==right.cnode->getMarking());
}

/** Set up the info on how to pump up the newly introduced omega places in this node.
	@param pumppaths A vector of paths that can pump up certain sets of places.
	@param pumpsets A vector of sets of places that can be pumped up. Sets and paths
		belonging to each other have the same index in both parameters.
*/ 
void RNode::setPumpInfo(vector<deque<Transition*> >& pumppaths, vector<set<Place*> >& pumpsets) {
	paths = pumppaths;
	places = pumpsets;
}

/** Get the info on how to pump up the omega places introduced in this node.
	@param pumppaths Return value: A vector of paths that can pump up certain sets of places.
		Upon calling the vector may be empty.
	@param pumpsets Return value: A vector of sets of places that can be pumped up. Sets and paths
		belonging to each other have the same index in both parameters. Should be empty upon calling.
*/ 
void RNode::getPumpInfo(vector<deque<Transition*> >& pumppaths, vector<set<Place*> >& pumpsets) const {
	pumppaths.clear();
	pumppaths = paths;
	pumpsets.clear();
	pumpsets = places;
}

