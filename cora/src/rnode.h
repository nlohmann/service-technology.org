// -*- C++ -*-

/*!
 * \file    rnode.h
 *
 * \brief   Class RNode
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/05/12
 *
 * \date    $Date: 2010-05-12 12:00:00 +0200 (Mi, 12. Mai 2010) $
 *
 * \version $Revision: -1 $
 */

#ifndef RNODE_H
#define RNODE_H

#include "cnode.h"

/*! \brief Wrapped Nodes of the coverability graph that allow a total ordering by the markings. */
class RNode {
public:
	/// Standard constructor.
	RNode(CNode& cnode);

	/// Destructor.
	~RNode();

	/// Pointer to CNode
	CNode* cnode;

	/// Comparison operator for set ordering
	bool operator<(const RNode& right) const;

	/// Equality of nodes
	bool operator==(const RNode& right) const;
};

#endif

