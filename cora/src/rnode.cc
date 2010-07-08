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

