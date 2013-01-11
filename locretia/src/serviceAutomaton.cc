/*****************************************************************************\
 Locretia -- generating logs...

 Copyright (c) 2012 Simon Heiden

 Locretia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Locretia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Locretia.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "serviceAutomaton.h"
#include "verbose.h"
#include "util.h"

using std::set;

/******************
 * STATIC MEMBERS *
 ******************/

pnapi::Automaton* serviceAutomaton::sa = new pnapi::Automaton();


/******************
 * STATIC METHODS *
 ******************/

/*!
 \brief Checks if a final state is reachable with the given maximal trace length. Iterative depth search is used.

 \param[in]		trace_max_length	the given maximal length of a trace

 \return minimal trace length to be able to reach a final state or '-1' if no final state is reachable
 */
int serviceAutomaton::isFinalStateReachable(const int trace_max_length) {
	status("checking if a final state is reachable in %i steps...", trace_max_length);

	int result = -1;

	for (int depth = 1; depth <= trace_max_length; ++depth) {
		int counter = 0;
		result = traverse(**(sa->getInitialStates().begin()), depth, counter);

		if (result != -1)
			break;
	}

	if (result == -1) {
		status("no final state is reachable!");
	} else {
		status("at least one final state is reachable in %i steps!", result);
	}

	return result;
}

/*!
 \brief Help funtion for the 'isFinalStateReachable'-function

 \param[in]		markingID		the ID of the current node
 \param[in]		depth			the current depth of the iterative depth search
 \param[in]		counter			the amount of steps taken from the initial node to the current node

 \return amount of steps to reach a final state or '-1' if no final state reachable
 */
int serviceAutomaton::traverse(const pnapi::State& state, int depth, int counter) {
	if (state.isFinal())
		return counter;
	if (--depth < 0)
		return -1;

	++counter;
	set<pnapi::Edge *>::iterator myIterator;
	for(myIterator = state.getPostsetEdges().begin();
			myIterator != state.getPostsetEdges().end(); ++myIterator) {
		int result = traverse((*myIterator)->getDestination(), depth, counter);
		if (result != -1)
			return result;
	}

	return -1;
}
