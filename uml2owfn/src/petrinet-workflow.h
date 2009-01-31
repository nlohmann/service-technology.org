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
#include "UML-public.h"
#include "petrinet-formula.h"

typedef enum terminationSemantics_e {
  TERM_UML_STANDARD,
  TERM_WORKFLOW,
  TERM_IGNORE_DATA,
  TERM_ORJOIN
} terminationSemantics_t;

/*!
 * \brief   A Petri net with a workflow interpretation
 *
 *          Class to represent Petri net workflows, extending #PetriNet
 *
 * \ingroup petrinet
 */

class ExtendedWorkflowNet: public PetriNet {
	friend class FlowContentElement;
	friend class PetriNet;

public:

  // Petri net nodes that implement specific parts of the process

  /// places that represent pins of tasks and control-flow nodes
  set<Place*>     pinPlaces;

  /// transitions that represent input pinsets of a process
  set<Transition *> process_inputPinSets;

  /// transitions that represent output pinsets of a process
  set<Transition *> process_outputPinSets;

  /// places that represent start nodes of a process
  set<Place*>     process_startNodes;

  /// places that represent end nodes of a process
  set<Place*>     process_endNodes;

  /// places that represent stop nodes of a process
  set<Place*>     process_stopNodes;

  /// places that represent are internal to a process
  set<Place*>     process_internalPlaces;

  /// places denoting that an incriterion has fired
  set<Place*>     process_inputCriterion_used;


  /// merges two places
  Place* mergePlaces(Place * & p1, Place * & p2);

  /// merges two places given two roles
  Place* mergePlaces(string role1, string role2);

  /// removes a place from the net
  void removePlace(Place *p);

  /// merges two transitions
  Transition*  mergeTransitions(Transition * & t1, Transition * & t2);

  /// removes a transition from the net
  void removeTransition(Transition *t);

  /// turn beginning of open workflow net into a workflow net
  void soundness_initialPlaces();

  /// turn end of open workflow net into a workflow net
  void soundness_terminalPlaces(bool liveLocks, bool stopNodes, terminationSemantics_t termination);

	set< Node* > isFreeChoice() const; ///< check whether this net is a free-choice net
	Node* isPathCovered() const; ///< check whether every node of the net is on a path from alpha to omega
	bool isConnected() const;    ///< check whether the net is connected
	bool isStructurallyCorrect() const; ///< check whether net still represents a valid UML2-AD

	int inputSize() const;     ///< size of the input interface
	int outputSize() const;    ///< size of the output interface

	unsigned int reduce(unsigned int reduction_level);

	/// remove places representing unconnected pins in the given net
  void removeUnconnectedPins ();

  /// re-enumerate the roles of all nodes of the net
	void anonymizeNodes();


	/// create the formula describing the final state
  FormulaState* createFinalStatePredicate () const;

  /// create the formula describing the final state
  FormulaState* createOmegaPredicate (bool stopNodes) const;

  /// create the formula describing the safeness of the net
  FormulaState* createSafeStatePredicate (bool mustBeNonEmpty) const;

private:
  void reduce_series_places();
  void reduce_series_transitions();
};


#endif /*PETRINETWORKFLOW_H_*/
