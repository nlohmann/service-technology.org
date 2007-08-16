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
#include <cassert>
#include "SinglyLinkedList.h"
#include "GraphFormula.h"

class State;
class literal;
//class GraphFormula;
//class GraphFormulaAssignment;
//class GraphFormulaMultiaryOr;
//class GraphFormulaCNF;

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

    //! Number of this GraphNode in the graph.
    unsigned int number;

    //! Name of this GraphNode in the graph.
    std::string name;

    //! Color of this GraphNode.
    GraphNodeColor color;

    //! Annotation of this node (a CNF) as a formula.
    GraphFormulaCNF* annotation;

    //! Contains all leaving edges.
    LeavingEdges leavingEdges;

public:

    //! constructor
    GraphNodeCommon();

    //! constructor
    //! \param _name
    //! \param _annotation
    //! \param _color
    GraphNodeCommon(const std::string& _name, GraphFormula* _annotation, GraphNodeColor _color);

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

    /// is the node color blue?
    bool isBlue() const;

    /// is the node color red?
    bool isRed() const;

    //! get the annotation
    GraphFormulaCNF* getAnnotation() const;

    //! get the annotation as a string
    std::string getAnnotationAsString() const;

//----------

    /**
     * Destroys this GraphNodeCommon.
     */
    ~GraphNodeCommon();

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

    //! Diagnosis color of this GraphNode.
    GraphNodeDiagnosisColor diagnosis_color;    

public:

    bool hasFinalStateInStateSet;
    int * eventsUsed;

    // this set contains only a reduced number of states in case the state
    // reduced graph is to be build.
    StateSet reachGraphStateSet;

    GraphNode();
    ~GraphNode();

    /// adds a state to the states of a GraphNode
    bool addState(State *);

    void addClause(GraphFormulaMultiaryOr*);

    /// analyses the node and sets its color
    void analyseNode();

    /// get the node diagnosis color
    GraphNodeDiagnosisColor getDiagnosisColor() const;    

    /// set the diagnosis color
    void setDiagnosisColor(GraphNodeDiagnosisColor c);

    bool isToShow(const GraphNode* rootOfGraph) const;

    void removeLiteralFromAnnotation(const std::string& literal);

    /// Removes unneeded literals from the node's annotation. Labels of edges to
    /// red nodes are unneeded.
    void removeUnneededLiteralsFromAnnotation();

    friend bool operator < (GraphNode const&, GraphNode const& );

#undef new
    /// Provides user defined operator new. Needed to trace all new operations
    /// on this class.
    NEW_OPERATOR(GraphNode)
#define new NEW_NEW
};


/*************************
 * class GraphNodeCommon *
 *************************/


//! \brief constructor
template<typename GraphNodeType>
GraphNodeCommon<GraphNodeType>::GraphNodeCommon() :
        number(12345678),
        name("12345678"),
        color(BLUE) {
    
    annotation = new GraphFormulaCNF();

}


//! \brief constructor
template<typename GraphNodeType>
GraphNodeCommon<GraphNodeType>::
    GraphNodeCommon(const std::string& _name, GraphFormula* _annotation, GraphNodeColor _color) :
        number(12345678),
        name(_name),
        color(_color) {

    annotation = _annotation->getCNF();
    delete _annotation; // because getCNF() returns a newly create formula
}


//! returns the number of this node
//! \return number of this node
template<typename GraphNodeType>
unsigned int GraphNodeCommon<GraphNodeType>::getNumber() const {
    return number;
}


//! sets the number of this node
//! \param newNumber number of this node in the graph
template<typename GraphNodeType>
void GraphNodeCommon<GraphNodeType>::setNumber(unsigned int newNumber) {
    number = newNumber;
}


//! returns the name of this node
//! \return name of this node
template<typename GraphNodeType>
std::string GraphNodeCommon<GraphNodeType>::getName() const {
    return name;
}


//! sets the name of this node
//! \param newName new name of this node in the graph
template<typename GraphNodeType>
void GraphNodeCommon<GraphNodeType>::setName(std::string newName) {
    name = newName;
}


//! returns the color of the GraphNode
//! \return color of this node
template<typename GraphNodeType>
GraphNodeColor GraphNodeCommon<GraphNodeType>::getColor() const {
    return color;
}


//! sets the color of the GraphNode to the given color
//! \param c color of GraphNode
template<typename GraphNodeType>
void GraphNodeCommon<GraphNodeType>::setColor(GraphNodeColor c) {
    color = c;
}


template<typename GraphNodeType>
bool GraphNodeCommon<GraphNodeType>::isBlue() const {
    return getColor() == BLUE;
}


template<typename GraphNodeType>
bool GraphNodeCommon<GraphNodeType>::isRed() const {
    return getColor() == RED;
}


// returns the CNF formula that is the annotation of a node as a Boolean formula
template<typename GraphNodeType>
GraphFormulaCNF* GraphNodeCommon<GraphNodeType>::getAnnotation() const {
    return annotation;
}


template<typename GraphNodeType>
std::string GraphNodeCommon<GraphNodeType>::getAnnotationAsString() const {
    assert(annotation != NULL);
    return annotation->asString();
}


template<typename GraphNodeType>
GraphNodeCommon<GraphNodeType>::~GraphNodeCommon<GraphNodeType>() {
    typename LeavingEdges::ConstIterator iEdge = getLeavingEdgesConstIterator();
    while (iEdge->hasNext()) {
        GraphEdge<GraphNodeType>* edge = iEdge->getNext();
        delete edge;
    }
    delete iEdge;

    delete annotation;
}


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
