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
        GraphNodeDiagnosisColor(GraphNodeDiagnosisColor_enum color = DIAG_RED);

        /// return node diagnosis color as string
        std::string toString() const;

        /// typecast operator
        operator GraphNodeDiagnosisColor_enum() const;
};


/**
 * Common base class for GraphNode and OGFromFileNode. This class shall
 * alleviate merging those two classes. Merging both classes should be done as
 * follows:
 *  1. Merge all functionality of GraphNode and OGFromFileNode by moving
 *     members and methods to GraphNodeCommon. Merged members that were private
 *     in GraphNode and OGFromFileNode should be declared protected in
 *     GraphNodeCommon. Public members stay public.
 *  2. Eventually GraphNode and OGFromFileNode are empty. Then OGFromFileNode
 *     should be deleted, all protected members of GraphNodeCommon should be
 *     declared private, GraphNodeCommon should be turned from a template class
 *     into a normal class and be renamed to GraphNode.
 *  3. GraphEdge should be turned from a template class to a normal class with
 *     GraphNodeType = GraphNode.
 *
 * NOTE: This class needs to be a template class until GraphNode and
 * OGFromFileNode are empty, because they have GraphEdges of their
 * corresponding type. The template type is called GraphNodeType. It can be
 * GraphNode (the default) or OGFromFileNode.
 */
template<typename GraphNodeType = GraphNode>
class GraphNodeCommon {
public:

    /**
     * Type of the container that holds all leaving edges of this GraphNode.
     */
    typedef SList<GraphEdge<GraphNodeType>*> LeavingEdges;

protected:

    /**
     * Contains all leaving edges.
     */
    LeavingEdges leavingEdges;

public:

    /**
     * Adds a leaving edge to this node.
     */
    void addLeavingEdge(GraphEdge<GraphNodeType>* edge);

    /**
     * Returns an iterator that can be used to traverse all leaving edges of
     * this GraphNode from begin to end. This iterator can also be used to
     * modify the list of leaving edges, e.g., to remove an edge.
     * Consult SList<T>::getIterator() for instructions how to use this
     * iterator.
     */
    typename LeavingEdges::Iterator getLeavingEdgesIterator();

    /**
     * Returns a const iterator that can be used to traverse all leaving edges
     * of this GraphNode. You can not modify the list of leaving edges with
     * this const iterator. Consult SList<T>::getConstIterator() for
     * instructions how to use this iterator.
     */
    typename LeavingEdges::ConstIterator getLeavingEdgesConstIterator() const;

    /**
     * Returns the number of leaving edges.
     */
    unsigned int getLeavingEdgesCount() const;

};


/*******************
 * class GraphNode *
 *******************/

class GraphNode : public GraphNodeCommon<> {
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


template<typename GraphNodeType>
void GraphNodeCommon<GraphNodeType>::addLeavingEdge(
    GraphEdge<GraphNodeType>* edge) {

    leavingEdges.add(edge);
}


template<typename GraphNodeType>
typename GraphNodeCommon<GraphNodeType>::LeavingEdges::Iterator
GraphNodeCommon<GraphNodeType>::getLeavingEdgesIterator() {
    return leavingEdges.getIterator();
}


template<typename GraphNodeType>
typename GraphNodeCommon<GraphNodeType>::LeavingEdges::ConstIterator
GraphNodeCommon<GraphNodeType>::getLeavingEdgesConstIterator() const {
    return leavingEdges.getConstIterator();
}


template<typename GraphNodeType>
unsigned int GraphNodeCommon<GraphNodeType>::getLeavingEdgesCount() const {
    return leavingEdges.size();
}

#endif /*GraphNode_H_*/
