/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    GraphNode.h
 *
 * \brief   functions for handling of nodes of IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef GraphNode_H_
#define GraphNode_H_

#include "mynew.h"
#include "enums.h"
#include <set>

class State;
class GraphEdge;
class successorNodeList;
class literal;
class GraphFormula;
class GraphFormulaAssignment;
class GraphFormulaMultiaryOr;
class GraphFormulaCNF;

typedef std::set<State*> StateSet;


class GraphNode {
private:
    /**
     * Number of this GraphNode in the graph.
     */
    unsigned int number;

    /**
     * Color of this GraphNode.
     */
    GraphNodeColor color;

    /**
     * Annotation of this node (a CNF) as a formula.
     */
    GraphFormulaCNF* annotation;

    /**
     * List of all the nodes succeeding this one including the edge between
     * them
     */
    successorNodeList* successorNodes;

public:
    bool hasFinalStateInStateSet;

    GraphNode(int);
    ~GraphNode();

    int * eventsUsed;
    int eventsToBeSeen;

    unsigned int getNumber() const;
    void setNumber(unsigned int);

    // states in GraphNode
    bool addState(State *);

    // this set contains only a reduced number of states in case the state
    // reduced graph is to be build.
    StateSet reachGraphStateSet;

    // traversing successors/predecessors
    void addSuccessorNode(GraphEdge *);

    GraphEdge * getNextSuccEdge();

    void resetIteratingSuccNodes();

    // annotation
    GraphFormulaCNF* getCNFformula();
    std::string getCNFString();

    // temporary test assignment that sets yet to be visited successors to true
    // and therefore allows for early discovery of a formula that cannot get true 
    GraphFormulaAssignment* testAssignment;
    GraphFormulaAssignment* getAssignment();

    void addClause(GraphFormulaMultiaryOr*);

    GraphNodeColor analyseNodeByFormula();

    GraphNodeColor getColor() const;
    void setColor(GraphNodeColor c);

    bool isToShow(const GraphNode* rootOfGraph) const;

    void removeLiteralFromFormula(unsigned int, GraphEdgeType);

    friend bool operator < (GraphNode const&, GraphNode const& );

#undef new
    /**
     * Provides user defined operator new. Needed to trace all new operations
     * on this class.
     */
    NEW_OPERATOR(GraphNode)
#define new NEW_NEW
};

#endif /*GraphNode_H_*/
