/*****************************************************************************\
  UML2oWFN - Petri Net API workflow extension.
  
  Copyright (C) 2008 Dirk Fahland
  
  UML2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  UML2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU UML2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

/*!
 * \file    petrinet-workflow.cc
 *
 * \brief   Petri Net API: workflow extension
 *
 * \author  Dirk Fahland <fahland@informatik.hu-berlin,de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2008-06-06
 *
 * \date    \$Date: 2007/06/28 07:38:17 $
 *
 * \note    This file is part of the tool UML2oWFN.
 *
 * \version \$Revision: 1.222 $
 *
 * \ingroup petrinet
 */

/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <iostream>
#include <utility>

#include <stack>

#include "petrinet-workflow.h"
#include "helpers.h"		// helper functions (setUnion, setDifference, max, toString)
#include "pnapi.h"

using std::stack;
using std::set;
using std::cerr;
using std::endl;
using std::string;


/******************************************************************************
 * Member methods on properties of extended workflow nets
 *****************************************************************************/

set< Node* > ExtendedWorkflowNet::isFreeChoice() const {
	
	set< Node* > resultSet;
	
	for (set<Transition *>::iterator it = T.begin(); it != T.end(); it++)
	{
		set<Transition *>::iterator it2 = it; it2++;	// get next transition
		for (; it2 != T.end(); it2++)
		{
			Transition* t = static_cast<Transition*>(*it);
			Transition* t2 = static_cast<Transition*>(*it2);
			
			bool empty = true;
			for (set<Node *>::iterator pre = t->preset.begin(); pre != t->preset.end(); pre++) {
				if (t2->preset.find(*pre) != t2->preset.end()) {
					empty = false;	// intersection is non-empty
				} else if (!empty) {
					//cerr << "transitions " << t->nodeFullName() << " and " << t2->nodeFullName() << " are not free-choice" << endl;
					// pre sets of t1 and t2 are not equal
					//return false;
					resultSet.insert(t);
					resultSet.insert(t2);
				}
			}
		}
	}
	return resultSet;
}

Node* ExtendedWorkflowNet::isPathCovered() const {
	
	stack< Node* > searchStack;
	
	set< Node* > fromAlpha;
	Place* p = findPlace("alpha");
	if (p == NULL) return false;
	else searchStack.push(p);
	
	while (!searchStack.empty()) {
		Node *n = searchStack.top(); searchStack.pop();
		fromAlpha.insert(n);

		for (set<Node *>::iterator n2 = n->postset.begin(); n2 != n->postset.end(); n2++) {
			// already seen in this search
			if (fromAlpha.find(*n2) != fromAlpha.end())
				continue;
			searchStack.push(*n2);
		}
	}
	
	set< Node* > fromAlphaToOmega;
	for (set<Place *>::iterator n = P.begin(); n != P.end(); n++) {
		string name = (*n)->nodeFullName();
		if (name.find("omega", 0) != string::npos) {
			searchStack.push(*n);
		}
	}
	
	while (!searchStack.empty()) {
		Node *n = searchStack.top(); searchStack.pop();
		fromAlphaToOmega.insert(n);

		for (set<Node *>::iterator n2 = n->preset.begin(); n2 != n->preset.end(); n2++) {
			// already seen in this search
			if (fromAlphaToOmega.find(*n2) != fromAlphaToOmega.end())
				continue;
			// node is not reachable from alpha, but from omega
			if (fromAlpha.find(*n2) == fromAlpha.end()) {
				//cerr << "wf-struct: node " << (*n2)->nodeFullName() << " is not on a path from alpha to omega" << endl;
				return *n2;
			}
			searchStack.push(*n2);
		}
	}
	
	for (set<Transition *>::iterator n = T.begin(); n != T.end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: transition " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}
	
	for (set<Place *>::iterator n = P.begin(); n != P.end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: place " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}
	
	for (set<Place *>::iterator n = P_in.begin(); n != P_in.end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: input place " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}
	
	for (set<Place *>::iterator n = P_out.begin(); n != P_out.end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: output place " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}
	return NULL;
}
