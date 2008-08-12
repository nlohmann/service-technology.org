/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
 Rachel is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Rachel (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

#ifndef __SCHEDULE_H
#define __SCHEDULE_H

#include <map>
#include <set>
#include <algorithm>

#include "Graph.h"


class Scheduler {
    private:
        std::set<std::pair<Node, Node> > nodes;
        std::map<std::pair<Node, Node>, std::set<std::pair<Node, Node> > > edges;

        void addNode(Node q1, Node q2);
        void addEdge(Node q11, Node q12, Node q21, Node q22);

    public:
        void initialize(Graph &g1, Graph &g2, Node q1, Node q2);
        void schedule();

        void toDot();
};

#endif
