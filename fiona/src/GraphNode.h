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
#include "GraphEdge.h"
#include "owfn.h"
#include <set>
#include "SinglyLinkedList.h"

class State;
class literal;
class GraphFormula;
class GraphFormulaAssignment;
class GraphFormulaMultiaryOr;
class GraphFormulaCNF;

typedef std::set<State*> StateSet;


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
    DIAG_GREEN,     ///< innocent node: you did not make anything wrong yet
    DIAG_ORANGE     ///< dangerous node: be careful!
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
        GraphNodeDiagnosisColor(GraphNodeDiagnosisColor_enum color = RED);
        
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

    //! Type of the container that holds all leaving edges of this GraphNode.
    typedef SList<GraphEdge<>*> LeavingEdges;

private:

    //! Number of this GraphNode in the graph.
    unsigned int number;

    //! Name of this GraphNode in the graph.
    std::string name;

    //! Color of this GraphNode.
    GraphNodeColor color;
    
    //! Diagnosis color of this GraphNode.
    GraphNodeDiagnosisColor diagnosis_color;    

    //! Annotation of this node (a CNF) as a formula.
    GraphFormulaCNF* annotation;

    //! Contains all leaving edges.
    LeavingEdges leavingEdges;

public:

    bool hasFinalStateInStateSet;

    GraphNode(int);
    ~GraphNode();

    int * eventsUsed;

    unsigned int getNumber() const;
    void setNumber(unsigned int);

    std::string getName() const;
    void setName(std::string);

    // states in GraphNode
    bool addState(State *);

    // this set contains only a reduced number of states in case the state
    // reduced graph is to be build.
    StateSet reachGraphStateSet;

    //! Adds a leaving edge to this node.
    void addLeavingEdge(GraphEdge<>* edge);

    //! Returns an iterator that can be used to traverse all leaving edges of
    //! this GraphNode. Consult SList<T>::getIterator() for instructions how to
    //! use this iterator.
    LeavingEdges::Iterator getLeavingEdgesIterator();

    //! annotation
    GraphFormulaCNF* getAnnotation() const;

    //! temporary test assignment that sets yet to be visited successors to true
    //! and therefore allows for early discovery of a formula that cannot get true 
    GraphFormulaAssignment* testAssignment;
    GraphFormulaAssignment* getAssignment();

    void addClause(GraphFormulaMultiaryOr*);

    GraphNodeColor analyseNodeByFormula();

    /// get the node color
    GraphNodeColor getColor() const;
    
    /// get the node diagnosis color
    GraphNodeDiagnosisColor getDiagnosisColor() const;    
    
    /// set the node color
    void setColor(GraphNodeColor c);
    
    /// set the diagnosis color
    void setDiagnosisColor(GraphNodeDiagnosisColor c);
    
    bool isToShow(const GraphNode* rootOfGraph) const;

    void removeLiteralFromFormula(oWFN::Places_t::size_type, GraphEdgeType);

    //! Removes unneeded literals from the node's annotation. Labels of edges to
    //! red nodes are unneeded.
    void removeUnneededLiteralsFromAnnotation();

    friend bool operator < (GraphNode const&, GraphNode const& );

#undef new
    //! Provides user defined operator new. Needed to trace all new operations
    //! on this class.
    NEW_OPERATOR(GraphNode)
#define new NEW_NEW
};

#endif /*GraphNode_H_*/
