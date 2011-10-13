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

#ifndef DFS_GRAPH_H
#define DFS_GRAPH_H

#define G_STATE_FINAL 1
#define G_STATE_RELEVANT 2
#define G_STATE_VISITED 4

#include <list>
#include <pnapi/pnapi.h>

void DFS_graph();
void printCurrentRun();

typedef struct {
    pnapi::Transition *const transition;
    const unsigned int successor;
} innerTransition;

typedef struct {
   std::list<innerTransition> transitions;
   std::list<innerTransition>::iterator curTransition;
} innerState;

#endif
