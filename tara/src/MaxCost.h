/*****************************************************************************\
 Tara-- <<-- Tara -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef MAX_COST_H
#define MAX_COST_H

#include <list>
#include <pnapi/pnapi.h>

// compute the maxCost of the inner Graph
unsigned int maxCost(pnapi::PetriNet* net);

void printCurrentRun();

typedef struct {
    pnapi::Transition *const transition;
    const unsigned int successor;
    unsigned int costs;
} innerTransition;

typedef struct {
   std::deque<innerTransition> transitions;
   std::deque<innerTransition>::iterator curTransition;
   bool inStack;
   bool final;
   int curCost;
   unsigned int maxCosts;
} innerState;

#endif
