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
 * \file    petrinet-workflow.h
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

private:
  void reduce_series_places();
  void reduce_series_transitions();
};


#endif /*PETRINETWORKFLOW_H_*/
