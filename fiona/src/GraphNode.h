/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider, Niels Lohmann, Leonard Kern                  *
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
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef GraphNode_H_
#define GraphNode_H_

#include "state.h"
#include "SinglyLinkedList.h"
#include "GraphFormula.h"
#include "GraphEdge.h"
#include "mynew.h"
#include "debug.h"
#include "options.h"
#include <set>
#include <cassert>


typedef std::set<State*> StateSet;

using namespace std;


/****************
 * enumerations *
 ****************/

enum GraphNodeColor_enum {
    RED,            ///< bad GraphNode
    BLUE            ///< good GraphNode
};

enum GraphNodeDiagnosisColor_enum {
    DIAG_UNSET,     ///< color of this node is not yet set
    DIAG_RED,       ///< bad node: you will reach a deadlock
    DIAG_BLUE,      ///< good node: you will reach a final state
    DIAG_ORANGE,    ///< dangerous node: be careful!
    DIAG_VIOLET     ///< bad node: in some state you will reach a deadlock
};


/************************
 * class GraphNodeColor *
 ************************/

class GraphNodeColor {
    private:
        /// color to build IG or OG
        GraphNodeColor_enum color_;

    public:
        /// constructor
        GraphNodeColor(GraphNodeColor_enum color = RED);

        /// return node color as string
        std::string toString() const;

        /// typecast operator
        operator GraphNodeColor_enum() const;
};


/*********************************
 * class GraphNodeDiagnosisColor *
 *********************************/

class GraphNodeDiagnosisColor {
    private:
        /// color for diagnosis
        GraphNodeDiagnosisColor_enum diagnosis_color_;

    public:
        /// constructor
        GraphNodeDiagnosisColor(GraphNodeDiagnosisColor_enum color = DIAG_RED);

        /// return node diagnosis color as string
        std::string toString() const;

        /// typecast operator
        operator GraphNodeDiagnosisColor_enum() const;
};



/*******************
* class GraphNode *
*******************/

class GraphNode {
    public:

        /// Type of the container that holds all leaving edges of this GraphNode.
        typedef SList<GraphEdge*> LeavingEdges;

    protected:

        /// Number of this GraphNode in the graph.
        unsigned int number;

        /// Name of this GraphNode in the graph.
        std::string name;

        /// Color of this GraphNode.
        GraphNodeColor color;

        //! Diagnosis color of this GraphNode.
        GraphNodeDiagnosisColor diagnosis_color;

        /// Contains all leaving edges.
        LeavingEdges leavingEdges;

    public:

        /// These three are needed for OG/IG construction.
        bool hasFinalStateInStateSet;
        int* eventsUsedInput;
        int* eventsUsedOutput;

        /// This set contains only a reduced number of states in case the state
        /// reduced graph is to be build.
        StateSet reachGraphStateSet;

        /// constructor (no parameters)
        GraphNode();

        /// constructor (three parameters)
        GraphNode(const std::string& _name,
                        GraphNodeColor _color = BLUE,
                        unsigned int number = 12345678);

        /// Destroys this GraphNode.
        virtual ~GraphNode();

        /// get the node number
        unsigned int getNumber() const;

        /// set the node number
        void setNumber(unsigned int);

        /// get the node name
        std::string getName() const;

        /// set the node name
        void setName(std::string);

        /// get the node color
        GraphNodeColor getColor() const;

        /// set the node color
        void setColor(GraphNodeColor c);

        /// Is the node color blue?
        bool isBlue() const;

        /// Is the node color red?
        bool isRed() const;

        /// get the node diagnosis color
        GraphNodeDiagnosisColor getDiagnosisColor() const;

        /// set the diagnosis color
        GraphNodeDiagnosisColor setDiagnosisColor(GraphNodeDiagnosisColor c);
        
        /// Adds a leaving edge to this node.
        void addLeavingEdge(GraphEdge* edge);

        /// Returns an iterator for this node's leaving edges.
        LeavingEdges::Iterator getLeavingEdgesIterator();

        /// Returns a constant iterator for this node's leaving edges.
        LeavingEdges::ConstIterator getLeavingEdgesConstIterator() const;

        /// Returns the number of leaving edges.
        unsigned int getLeavingEdgesCount() const;

        /// adds a state to the states of a GraphNode
        bool addState(State*);

        /// returns true iff a colored successor of v can be avoided
        bool coloredSuccessorsAvoidable(GraphNodeDiagnosisColor_enum color) const;
        
        /// returns true iff edge e is possible in every state
        bool edgeEnforcable(GraphEdge* e) const;
        
        /// returns true iff e changes the color of the common successors
        bool changes_color(GraphEdge* e) const;

        /// returns true iff node should be shown according to the "show" parameter
        bool isToShow(const GraphNode* rootOfGraph) const;

        /// determines whether this node has a edge with the given label
        bool hasEdgeWithLabel(const std::string&) const;

        /// determines whether this node has a blue edge with a given label
        bool hasBlueEdgeWithLabel(const std::string&) const;

        /// returns a edge of this node with the given label
        GraphEdge* getEdgeWithLabel(const std::string&) const;

        /// returns the destination node of the edge with the given label
        GraphNode* followEdgeWithLabel(const std::string&);

        /// determines whether an assinment satisfies this node's annotation
        bool assignmentSatisfiesAnnotation(const GraphFormulaAssignment&) const;

        /// returns the assignment that is imposed by present or absent arcs
        /// leaving the node
        GraphFormulaAssignment* getAssignment() const;

        /// removes all edges from this node to the given one
        void removeEdgesToNode(const GraphNode*);

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphNode)
#define new NEW_NEW
};


#endif /*GraphNode_H_*/
