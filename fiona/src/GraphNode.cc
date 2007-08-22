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
        case BLUE:
            return "blue";
        case RED:
            return "red";

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
        case DIAG_UNSET:
            return "unset";
        case DIAG_RED:
            return "red";
        case DIAG_BLUE:
            return "blue";
        case DIAG_GREEN:
            return "green";
        case DIAG_ORANGE:
            return "darkorange";
        case DIAG_VIOLET:
            return "violetred";

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


//! \brief constructor
GraphNode::GraphNode() :
    GraphNodeCommon<>(), // initialize father
    diagnosis_color(DIAG_UNSET),
    hasFinalStateInStateSet(false) {

    eventsUsed = new int [PN->getInputPlaceCount() + PN->getOutputPlaceCount()];

    for (unsigned int i = 0;
         i < PN->getInputPlaceCount() + PN->getOutputPlaceCount(); i++) {
        eventsUsed[i] = 0;
    }
}

    
//! \brief destructor
GraphNode::~GraphNode() {
    trace(TRACE_5, "GraphNode::~GraphNode() : start\n");

    if (eventsUsed != NULL) {
        delete[] eventsUsed;
    }

    numberDeletedVertices++;
    trace(TRACE_5, "GraphNode::~GraphNode() : end\n");
}


//! \param s pointer to the state that is to be added to this node
//! \brief adds the state s to the list of states
bool GraphNode::addState(State * s) {
    assert(s != NULL);
    pair<StateSet::iterator, bool> result = reachGraphStateSet.insert(s);
    return result.second; // returns whether the element got inserted (true) or not (false)
}


//! \param myclause the clause to be added to the annotation of the current node
//! \brief adds a new clause to the CNF formula of the node
void GraphNode::addClause(GraphFormulaMultiaryOr* myclause) {
    annotation->addClause(myclause);
}


void GraphNode::removeLiteralFromAnnotation(const std::string& literal) {
    trace(TRACE_5, "GraphNode::removeLiteralFromAnnotation(const string& literal) : start\n");

    //cout << "remove literal " << literal << " from annotation " << annotation->asString() << " of node number " << getName() << endl;
    annotation->removeLiteral(literal);

    trace(TRACE_5, "GraphNode::removeLiteralFromAnnotation(const string& literal) : end\n");
}


void GraphNode::removeUnneededLiteralsFromAnnotation() {
    GraphNode::LeavingEdges::ConstIterator
            edgeIter =getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        GraphEdge<>* edge = edgeIter->getNext();
        if (edge->getDstNode()->getColor() == RED) {
            annotation->removeLiteral(edge->getLabel());
        }
    }
    delete edgeIter;
}


//! \param c color of GraphNode
//! \brief sets the color of the GraphNode to the given color
GraphNodeDiagnosisColor GraphNode::setDiagnosisColor(GraphNodeDiagnosisColor c) {
    diagnosis_color = c;
    return c;
}


//! \brief returns the diagnosis color of the GraphNode
GraphNodeDiagnosisColor GraphNode::getDiagnosisColor() const {
    return diagnosis_color;
}


bool GraphNode::isToShow(const GraphNode* rootOfGraph) const {

    if (parameters[P_SHOW_ALL_NODES] || (parameters[P_SHOW_NO_RED_NODES] &&
        (getColor() != RED))|| (!parameters[P_SHOW_NO_RED_NODES] &&
        (getColor() == RED))|| (getColor() == BLUE) ||
        (this == rootOfGraph)) {

        return (parameters[P_SHOW_EMPTY_NODE] || reachGraphStateSet.size() != 0);
    } else {
        return false;
    }
}


//! \brief analyses the node and sets its color
void GraphNode::analyseNode() {

    trace(TRACE_5, "GraphNode::analyseNodeByFormula() : start\n");

    trace(TRACE_3, "\t\t\t analysing node ");
    trace(TRACE_3, this->getNumber() + "...\n");

    assert(this->getColor() == BLUE);

    // computing the assignment given by outgoing edges (to blue nodes)
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

    // evaluating the computed assignment
    bool result = this->getAnnotation()->value(*myassignment);
    delete myassignment;

    if (result) {
        trace(TRACE_3, "\t\t\t node analysed blue, formula "
                + this->getAnnotation()->asString() + "\n");
        this->setColor(BLUE);
    } else {
        trace(TRACE_3, "\t\t\t node analysed red, formula "
                + this->getAnnotation()->asString() + "\n");
        this->setColor(RED);
    }

    trace(TRACE_5, "GraphNode::analyseNodeByFormula() : end\n");
}


//! \param left left hand GraphNode
//! \param right right hand GraphNode
//! \brief implements the operator < by comparing the states of the two vertices
bool operator <(GraphNode const& left, GraphNode const& right) {
    return (left.reachGraphStateSet < right.reachGraphStateSet);
}


/*!
 * \brief  returns true iff a colored successor can be avoided
 *
 * \param  color   the color under consideration
 *
 * \return true iff there is either a sending edge to a differently colored
 *         successor or, for each external deadlock, there exists a receiving
 *         edge to a non-colored successor
 */
bool GraphNode::coloredSuccessorsAvoidable(GraphNodeDiagnosisColor_enum color) const {    
    // collect all edges to differently colored successor nodes
    set<GraphEdge<>*> edges_to_differently_colored_successors;
    bool colored_successor_present = false;
    
    GraphNode::LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge<> *element = edgeIter->getNext();
        GraphNode *vNext = element->getDstNode();
        
        if (vNext != this)
            continue;
        
        if (vNext->getDiagnosisColor() != color) {
            // if there is a sending edge to a differently colored sucessor, take this edge;
            // otherwise: store this edge for future considerations
            if (element->getType() == SENDING ) {
                return true;
            } else {
                edges_to_differently_colored_successors.insert(element);
            }
        } else {
            colored_successor_present = true;
        }
    }
    delete edgeIter;
    
    // if there is no such colored successor, it can be clearly avoided
    if (!colored_successor_present) {
        return true;
    }
    
    // if there are no differently colored successors, the colored successors can not be avoided
    if (edges_to_differently_colored_successors.empty() ) {
        return false;
    }
    
    // last chance: look if each external deadlock "enables" a receiving edge
    // to a differently colored successor
    for (StateSet::const_iterator state = reachGraphStateSet.begin();
         state != reachGraphStateSet.end(); state++) {
        (*state)->decode(PN);
        
        bool found_enabled_state = false;
        
        if ((*state)->type == DEADLOCK) {
            for (set<GraphEdge<>*>::iterator edge = edges_to_differently_colored_successors.begin();
                 edge != edges_to_differently_colored_successors.end(); edge++) {
                if ((*edge)->getType() == SENDING ) {
                    continue;
                }
                
                string edge_label = (*edge)->getLabel().substr(1, (*edge)->getLabel().length());
                
                for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
                    if (PN->getOutputPlace(i)->name == edge_label) {
                        if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
                            found_enabled_state = true;
                            break;
                        }
                    }
                }
            }
            
            // looked at all edges but did not finde an enabling state
            if (!found_enabled_state) {
                return false;
            }
        }
    }
    
    // looked at all edges but did not abort earlier
    return true;
}


/*!
 * \brief  returns true iff edge e is possible in every state
 *
 * \param  e  an edge
 *
 * \return true, iff the edge e is labeled with a sending event, or a
 *         receiving event that is present in every external deadlock state
 *
 * \note   For performance issues, it is not checked whether edge e is really
 *         leaving this node.
 */
bool GraphNode::edgeEnforcable(GraphEdge<> *e) const {
    assert (e != NULL);
    
    if (e->getType() == SENDING) {
        return true;
    } else {
        string edge_label = e->getLabel().substr(1, e->getLabel().length());
        bool edge_enforcable = true;
        
        // iterate the states and look for deadlocks where the considered
        // message is not present: then, the receiving of this message can
        // not be enforced
        for (StateSet::const_iterator state = reachGraphStateSet.begin();
             state != reachGraphStateSet.end(); state++) {
            (*state)->decode(PN);
            
            if ((*state)->type == DEADLOCK) {
                for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
                    if (PN->getOutputPlace(i)->name == edge_label) {
                        if (PN->CurrentMarking[PN->getOutputPlace(i)->index] == 0) {
                            edge_enforcable = false;
                            break;
                        }
                    }
                }
            }
        }
        
        return edge_enforcable;
    }
}


/*!
 * \brief  returns true iff e changes the color of the common successors
 *
 * \param  e  an edge
 *
 * \todo   Comment me!
 */
bool GraphNode::changes_color(GraphEdge<> *e) const {
    assert(e != NULL);
    
    GraphNode *vNext = e->getDstNode();
    
    if ( vNext->getDiagnosisColor() == DIAG_RED )
        return false;
    
    if ( !edgeEnforcable(e) )
        return false;
    
    
    map< string, GraphEdge<>* > v_edges;
    map< string, GraphEdge<>* > vNext_edges;
    
    GraphNode::LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge<> *element = edgeIter->getNext();
        v_edges[element->getLabel()] = element;
    }
    edgeIter = vNext->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge<> *element = edgeIter->getNext();
        vNext_edges[element->getLabel()] = element;
    }
    delete edgeIter;
    
    
    for (map< string, GraphEdge<>* >::const_iterator v_edge = v_edges.begin();
         v_edge != v_edges.end(); v_edge++) {
        GraphEdge<> *vNext_edge = vNext_edges[v_edge->first];
        
        if (vNext_edge == NULL)
            continue;
        
        if ( (vNext_edge->getDstNode()->getDiagnosisColor() != v_edge->second->getDstNode()->getDiagnosisColor()) &&
             (vNext_edge->getDstNode()->getDiagnosisColor() == DIAG_RED) &&
             (v_edge->second->getDstNode()->getDiagnosisColor() != DIAG_VIOLET) ) {
            return true;
        }
    }
    
    return false;
}
