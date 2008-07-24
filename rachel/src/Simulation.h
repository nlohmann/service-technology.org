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

#ifndef __SIMULATION_H
#define __SIMULATION_H

#include "Action.h"
#include "Graph.h"
#include "EditDistance.h"


/*!
 * \brief weighted weak quantitative simulation
 *
 * The class implementes the weighted weak quantitative simulation as it is
 * defined in the paper "Simulation-based graph matching" by Sokolsky et al.
 * Instead of returning a single value, the function returns an edit script
 * that describes how a graph has to be changed to acheive simulation to
 * another graph.
 *
 * All functions are declarated as static values: the class is used for scoping
 * and grouping functions only. No object will be created of this class.
 *
 * The entry point of the class is Simulation::simulation.
 */
class Simulation : EditDistance {
    private:
        /// helper function 1 for simulation: insertion
        static ActionScript w1(Graph &g1, Graph &g2,
                               Node q1, Node q2);
        
        /// helper function 2 for simulation: keep, modify, or delete
        static ActionScript w2(Graph &g1, Graph &g2,
                               Node q1, Node q2);
        
        /// weighted weak quantitative simulation (helper)
        static Value simulation_recursively(Graph &g1, Graph &g2,
                                            Node q1, Node q2);
    
    public:
        /// weighted weak quantitative simulation
        static Value simulation(Graph &g1, Graph &g2);
};

#endif
