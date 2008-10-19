/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2008  Dirk Fahland <dirk.fahland@service-technolog.org>

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

#ifndef BOMPROCESS_H_
#define BOMPROCESS_H_

#include "pnapi.h"
#include "block.h"


#include <utility>		// for pair
#include <list>
#include <map>
#include <set>


using std::map;
using std::list;
using std::pair;
using std::set;

class BomProcess
{
public:
	//set<Block*>			bom_nodes;
	//set<Node*>			pn_nodes;

	//map<Node*, Block*>	pn_to_bom;

  /// Petri net nodes that implement specific parts of the process
	set<Place*>			pinPlaces;

	set<Transition *>	process_inputPinSets;
	set<Transition *>	process_outputPinSets;

	set<Place*>			process_endNodes;
	set<Place*>			process_stopNodes;
	set<Place*>			process_internalPlaces;

	// places denoting that an incriterion has fired
	set<Place*>     process_inputCriterion_used;

	/// create the formula describing the final state
	FormulaState* createFinalStatePredicate (PetriNet* PN) const;
	FormulaState* createOmegaPredicate (PetriNet* PN, bool stopNodes) const;
	FormulaState* createSafeStatePredicate (PetriNet* PN, bool mustBeNonEmpty) const;

	/// remove places representing unconnected pins in the given net
	void removeUnconnectedPins (PetriNet *PN);

	/// remove places and transitions representing unused output pinsets
	void removeEmptyOutputPinSets (PetriNet *PN);

	/// make input places internal and introduce a unique initial place to the net
	void soundness_initialPlaces (PetriNet *PN);

	/// make output places internal and add live-locks at all terminal places
	void soundness_terminalPlaces (PetriNet *PN, bool liveLocks, bool stopNodes, bool keepPinsets, bool wfNet, bool orJoin);

	/// constructor
	BomProcess();
	/// destructor
	virtual ~BomProcess();
};

#endif /*BOMPROCESS_H_*/
