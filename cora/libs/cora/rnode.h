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
 * \date    $Date: 2010-07-30 12:00:00 +0200 (Fr, 30. Jul 2010) $
 *
 * \version $Revision: 0.3 $
 */

#ifndef RNODE_H
#define RNODE_H

#include <set>
#include <vector>
#include <deque>
#include "cnode.h"

using std::set;
using std::vector;
using std::deque;

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
//	bool operator==(const RNode& right) const;

	/// Set info on how to pump omegas
	void setPumpInfo(vector<deque<Transition*> >& pumppaths, vector<set<Place*> >& pumpsets);

	/// Get info on how to pump omegas
	void getPumpInfo(vector<deque<Transition*> >& pumppaths, vector<set<Place*> >& pumpsets) const;

private:
	/// Paths needed for pumping unlimited places
	vector<deque<Transition*> > paths;

	/// Sets of places that can be pumped at this node
	vector<set<Place*> > places;
};

#endif

