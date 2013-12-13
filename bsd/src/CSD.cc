/*****************************************************************************\
 BSD -- generating BSD automata

 Copyright (c) 2013 Simon Heiden

 BSD is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 BSD is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with BSD.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <config.h>
#include <string>
#include <sstream>
#include "CSD.h"
#include "openNet.h"
#include "Label.h"

#include "verbose.h"

using std::set;
using std::vector;


/******************
 * STATIC MEMBERS *
 ******************/

std::map<BSDNode*, bool>* CSD::visited = NULL;

/******************
 * STATIC METHODS *
 ******************/

/*========================================================
 *-------------------- CSD computation -------------------
 *========================================================*/

/*!
 \brief Creates the CSD automaton based on the BSD automaton.

 */
void CSD::computeCSD(BSDgraph & graph) {
	visited = new std::map<BSDNode*, bool>;
	recursiveCSD(graph, graph.U);
	delete visited;
}

void CSD::recursiveCSD(BSDgraph & graph, BSDNode * node) {
	// only proceed if the node hasn't already been touched
	if (!(*visited)[node]) {
		// set the node to "visited"
		(*visited)[node] = true;
		// iterate through all predecessors of the node
		for (std::list<std::pair<Label_ID, struct _BSDNode *> >::const_iterator it = node->predecessors->begin();
				it != node->predecessors->end(); ++it) {
			if (SENDING(it->first) && it->second->lambda == 1) {
				// count the number of sending events (only releveant for nodes with lamda value 1)
				// if all sending events of a node with lamda value 1 lead to the U node
				// then the node becomes the U node
				if (++it->second->Ucount == graph.send_events) {
					it->second->isU = true;
					recursiveCSD(graph, it->second);
				}
			} else if (RECEIVING(it->first)) {
				// if a receiving event leads to the U node then the predecessor is also the U node
				it->second->isU = true;
				recursiveCSD(graph, it->second);
			}
			// if an event is a sending event and the lambda value of the preceeding node isn't equal to 1
			// then we don't have to call the function again
		}
	}
}
