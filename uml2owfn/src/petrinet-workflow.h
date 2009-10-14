/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2007, 2008, 2009  Dirk Fahland and Martin Znamirowski

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
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


// include PN Api headers
#include "pnapi/pnapi.h"

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
 *          Class to represent Petri net workflows, extending PetriNet
 *
 * \ingroup petrinet
 */

namespace pnapi
{

class ExtendedWorkflowNet: public PetriNet {
	friend class FlowContentElement;
	friend class PetriNet;

public:

  // Petri net nodes that implement specific parts of the process

  /// places that represent pins of tasks and control-flow nodes
  set<pnapi::Place*>     pinPlaces;

  /// transitions that represent input pinsets of a process
  set<pnapi::Transition *> process_inputPinSets;

  /// transitions that represent output pinsets of a process
  set<pnapi::Transition *> process_outputPinSets;

  /// places that represent start nodes of a process
  set<pnapi::Place*>     process_startNodes;

  /// places that represent end nodes of a process
  set<pnapi::Place*>     process_endNodes;

  /// places that represent stop nodes of a process
  set<pnapi::Place*>     process_stopNodes;

  /// places that represent are internal to a process
  set<pnapi::Place*>     process_internalPlaces;

  /// places denoting that an incriterion has fired
  set<pnapi::Place*>     process_inputCriterion_used;

  /// places that may be marked in the final state
  set<pnapi::Place*>     omegaPlaces;

  /// merges two places
  pnapi::Place* mergePlaces(pnapi::Place * & p1, pnapi::Place * & p2);

  /// merges two places given two roles
  pnapi::Place* mergePlaces(string role1, string role2);

  /// removes a place from the net
  void deletePlace(pnapi::Place & p);

  /// merges two transitions
  pnapi::Transition*  mergeTransitions(pnapi::Transition * & t1, pnapi::Transition * & t2);

  /// removes a transition from the net
  void deleteTransition(Transition & t);

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

  /// remove pinset transitions without pre- and post-places
  void deleteIsolatedPinsetTransitions();

  /// re-enumerate the roles of all nodes of the net
	void anonymizeNodes();

	/// rename all identifier to LoLA compatible format
	void toLoLAIdent();


	/// create the formula describing the final state
  FormulaState* createFinalStatePredicate () const;

  /// create the formula describing the final state
  FormulaState* createOmegaPredicate (bool stopNodes) const;

  /// create the formula describing the safeness of the net
  FormulaState* createSafeStatePredicate (bool mustBeNonEmpty) const;

  /// extend a multi-terminal workflow net to a single-terminal workflow net
  bool complete_to_WFnet_DPE();

  /*
  /// removes places, transitions and adjusts the interface according to the role set and mode
  void ExtendedWorkflowNet::cutByRoles();
   */

private:
  int reduce_series_places();
  int reduce_series_transitions();
  int reduce_identical_places();

  bool sameweights(Node *n) const;

  /// set of all nodes that are forward reachable from the given node
  void forwardReachableNodes(const pnapi::Node *n, set<pnapi::Node*>* reachable) const;

  /// set of all nodes that are backward reachable from the given node
  void backwardReachableNodes(const pnapi::Node *n, set<pnapi::Node*>* reachable) const;
};


// <Dirk.F start> UML2oWFN, needs identifier conversion
/// convert a given string into a LoLA/oWFN-compatible string
string toLoLAident(string name);
// <Dirk.F end>

} // namespace pnapi

#endif /*PETRINETWORKFLOW_H_*/
