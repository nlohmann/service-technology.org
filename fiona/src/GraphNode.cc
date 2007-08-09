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
 * \file    GraphNode.cc
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

#include "mynew.h"
#include "GraphNode.h"
#include "GraphFormula.h"
#include "state.h"
#include "debug.h"
#include "options.h"    
#include <cassert>


using namespace std;


/************************
 * class GraphNodeColor *
 ************************/

GraphNodeColor::GraphNodeColor(GraphNodeColor_enum color) :
    color_(color) {

}

std::string GraphNodeColor::toString() const {
    switch (color_) {
        case BLUE:  return "blue";
        case RED:   return "red";
        
        default:
            assert(false);
            return "undefined color";            
    }
}

GraphNodeColor::operator GraphNodeColor_enum() const {
    return color_;
}


/*********************************
 * class GraphNodeDiagnosisColor *
 *********************************/

GraphNodeDiagnosisColor::GraphNodeDiagnosisColor(GraphNodeDiagnosisColor_enum color) :
    diagnosis_color_(color) {
    
}

std::string GraphNodeDiagnosisColor::toString() const {
    switch (diagnosis_color_) {
        case DIAG_UNSET:    return "unset";
        case DIAG_RED:      return "red";
        case DIAG_BLUE:     return "blue";
        case DIAG_GREEN:    return "green";
        case DIAG_ORANGE:   return "orange";
            
        default:
            assert(false);
            return "undefined color";
    }
}

GraphNodeDiagnosisColor::operator GraphNodeDiagnosisColor_enum() const {
    return diagnosis_color_;
}


/*******************
 * class GraphNode *
 *******************/

//! \param numberEvents the number of events that have to be processed from this node
//! \brief constructor
GraphNode::GraphNode(int numberEvents) :

    GraphNodeCommon<>(),                // initialize father

    diagnosis_color(DIAG_UNSET),
    hasFinalStateInStateSet(false),
    testAssignment(NULL) {

    eventsUsed = new int [numberEvents];

    for (int i = 0; i < numberEvents; i++) {
        eventsUsed[i] = 0;
    }
}


//! \brief destructor
GraphNode::~GraphNode() {
    trace(TRACE_5, "GraphNode::~GraphNode() : start\n");

    if (eventsUsed != NULL) {
        delete[] eventsUsed;
    }

    delete annotation;

    numberDeletedVertices++;
    trace(TRACE_5, "GraphNode::~GraphNode() : end\n");
}


//! \param s pointer to the state that is to be added to this node
//! \brief adds the state s to the list of states
bool GraphNode::addState(State * s) {
    assert(s != NULL);
    pair<StateSet::iterator, bool> result = reachGraphStateSet.insert(s);
    return result.second;       // returns whether the element got inserted (true) or not (false)
}


//! \param myclause the clause to be added to the annotation of the current node
//! \brief adds a new clause to the CNF formula of the node
void GraphNode::addClause(GraphFormulaMultiaryOr* myclause) {
    annotation->addClause(myclause);
}


void GraphNode::removeLiteralFromFormula(oWFN::Places_t::size_type i, GraphEdgeType type) {
    trace(TRACE_5, "GraphNode::removeLiteralFromFormula(oWFN::Places_t::size_type i, GraphEdgeType type) : start\n");
    
    if (type == SENDING) {
        //cout << "remove literal " << PN->getInputPlace(i)->getLabelForCommGraph() << " from annotation " << annotation->asString() << " of node number " << getName() << endl;
        annotation->removeLiteral(PN->getInputPlace(i)->getLabelForCommGraph());
    } else {
        //cout << "remove literal " << PN->getOutputPlace(i)->getLabelForCommGraph() << " from annotation " << annotation->asString() << " of node number " << getName() << endl;
        annotation->removeLiteral(PN->getOutputPlace(i)->getLabelForCommGraph());
    }
    
    trace(TRACE_5, "GraphNode::removeLiteralFromFormula(oWFN::Places_t::size_type i, GraphEdgeType type) : end\n");
}

void GraphNode::removeUnneededLiteralsFromAnnotation() {
    GraphNode::LeavingEdges::ConstIterator edgeIter =
        getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        GraphEdge<>* edge = edgeIter->getNext();
        if (edge->getDstNode()->getColor() == RED) {
            annotation->removeLiteral(edge->getLabel());
        }
    }
    delete edgeIter;
}


// return the assignment that is imposed by present or absent arcs leaving node v
GraphFormulaAssignment* GraphNode::getAssignment() {
    
    trace(TRACE_5, "computing assignment of node " + getName() + "\n");
    
    GraphFormulaAssignment* myassignment = new GraphFormulaAssignment();
    
    // traverse outgoing edges and set the corresponding literals
    // to true if the respective node is BLUE
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge<>* edge = edgeIter->getNext();
        if (edge->getDstNode()->getColor() == BLUE) {
            myassignment->setToTrue(edge->getLabel());
        }
    }
    delete edgeIter;
    
    // only if node has final state, set assignment of literal final to true
    if (this->hasFinalStateInStateSet == true) {
        myassignment->setToTrue(GraphFormulaLiteral::FINAL);
    }
    
    //cout << "ende" << endl;
    return myassignment;
    
}


//! \param c color of GraphNode
//! \brief sets the color of the GraphNode to the given color
void GraphNode::setDiagnosisColor(GraphNodeDiagnosisColor c) {
    diagnosis_color = c;
}


//! \brief returns the diagnosis color of the GraphNode
GraphNodeDiagnosisColor GraphNode::getDiagnosisColor() const {
    return diagnosis_color;
}


bool GraphNode::isToShow(const GraphNode* rootOfGraph) const {
    
    if (parameters[P_SHOW_ALL_NODES] ||
        (parameters[P_SHOW_NO_RED_NODES] && (getColor() != RED)) ||
        (!parameters[P_SHOW_NO_RED_NODES] && (getColor() == RED)) ||
        (getColor() == BLUE) ||
        (this == rootOfGraph)) {
        
        return (parameters[P_SHOW_EMPTY_NODE] || reachGraphStateSet.size() != 0);
    } else {
        return false;
    }
}


//! \result the color of the node
//! \brief analyses the node, sets its color, and returns the new color
GraphNodeColor GraphNode::analyseNodeByFormula() {
    
    trace(TRACE_5, "GraphNode::analyseNodeByFormula() : start\n");
    
    // computing the assignment given by outgoing edges (to blue nodes)
    GraphFormulaAssignment* myassignment = this->getAssignment();
    bool result = this->getAnnotation()->value(*myassignment);
    delete myassignment;
    
    trace(TRACE_5, "GraphNode::analyseNodeByFormula() : end\n");
    
    if (result) {
        trace(TRACE_3, "\t\t\t node analysed blue, formula " + this->getAnnotation()->asString() + "\n");
        return BLUE;
    } else {
        trace(TRACE_3, "\t\t\t node analysed red, formula " + this->getAnnotation()->asString() + "\n");
        return RED;
    }
}


//! \param left left hand GraphNode
//! \param right right hand GraphNode
//! \brief implements the operator < by comparing the states of the two vertices
bool operator < (GraphNode const& left, GraphNode const& right) {
    return (left.reachGraphStateSet < right.reachGraphStateSet);	
}
