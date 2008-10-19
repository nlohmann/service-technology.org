/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2008  Dirk Fahland <dirk.fahland@service-technolog.org>,

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    petrinet-workflow.h
 *
 * \brief   Petri Net API: workflow extension
 *
 * \author  Dirk Fahland <fahland@informatik.hu-berlin,de>
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

#ifndef PETRINETWORKFLOW_H_
#define PETRINETWORKFLOW_H_


#include "petrinet.h"

/*!
 * \brief   A Petri net with a workflow interpretation
 *
 *          Class to represent Petri net workflows, extending #PetriNet
 *
 * \ingroup petrinet
 */

class ExtendedWorkflowNet: public PetriNet {
	friend class Block;

public:
	set< Node* > isFreeChoice() const; ///< check whether this net is a free-choice net
	Node* isPathCovered() const; ///< check whether every node of the net is on a path from alpha to omega

	unsigned int reduce(unsigned int reduction_level);

	void anonymizeNodes();   ///< re-enumerate the roles of all nodes of the net

private:
  void reduce_series_places();
  void reduce_series_transitions();
};


#endif /*PETRINETWORKFLOW_H_*/
