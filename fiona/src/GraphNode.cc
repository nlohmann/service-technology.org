/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider, Leonard Kern                                 *
 *                                                                           *
 * Copyright 2008                                                            *
 *   Peter Massuthe, Daniela Weinberg                                        *
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
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "GraphNode.h"
#include "options.h"
#include <cassert>

using namespace std;


/************************
 * class GraphNodeColor *
 ************************/

//! \brief constructor
//! \param color initial colour value
GraphNodeColor::GraphNodeColor(GraphNodeColor_enum color) : color_(color) {
}


//! \brief converts this colour value to a string
//! \return "blue" if this colour is blue, "red" if it's red,
//!         "undefinded color" in any other case
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


//! \brief typecast operator
GraphNodeColor::operator GraphNodeColor_enum() const {
    return color_;
}


/*********************************
 * class GraphNodeDiagnosisColor *
 *********************************/

//! \brief constructor
//! \param color initial colour value
GraphNodeDiagnosisColor::GraphNodeDiagnosisColor(GraphNodeDiagnosisColor_enum color) :
    diagnosis_color_(color) {

}


//! \brief converts this colour value into a string
//! \return a string representation of this colour
std::string GraphNodeDiagnosisColor::toString() const {
    switch (diagnosis_color_) {
        case DIAG_UNSET:
            return "darkgreen";
        case DIAG_RED:
            return "red";
        case DIAG_BLUE:
            return "blue";
        case DIAG_ORANGE:
            return "darkorange";
        case DIAG_VIOLET:
            return "violetred";

        default:
            assert(false);
            return "undefined color";
    }
}


//! \brief typecast operator
GraphNodeDiagnosisColor::operator GraphNodeDiagnosisColor_enum() const {
    return diagnosis_color_;
}


/******************
* class GraphNode *
******************/

//! \brief constructor (no parameters)
GraphNode::GraphNode() :
        number(12345678),
        name("12345678"),
        color(BLUE),
        diagnosis_color(DIAG_UNSET),
        hasFinalStateInStateSet(false) {

    eventsUsedInput = new int[PN->getInputPlaceCount()];
    eventsUsedOutput = new int[PN->getOutputPlaceCount()];
    
    for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
        eventsUsedInput[i] = 0;
    }
    
    for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
        eventsUsedOutput[i] = 0;
    }
}


//! \brief constructor (three parameters)
//! \param _name the name of this node
//! \param _annotation the annotation of this node
//! \param _color the colour of this node
//! \param _number the number of this node
GraphNode::GraphNode(const std::string& _name,
                     GraphNodeColor _color,
                     unsigned int _number) :
    number(_number), name(_name), color(_color), eventsUsedInput(NULL),
    eventsUsedOutput(NULL) {
        
}


//! \brief destructor
GraphNode::~GraphNode() {
    
    trace(TRACE_5, "GraphNode::~GraphNode() : start\n");
    
    LeavingEdges::ConstIterator iEdge = getLeavingEdgesConstIterator();
    while (iEdge->hasNext()) {
        GraphEdge* edge = iEdge->getNext();
        delete edge;
    }
    delete iEdge;
    
    if (eventsUsedInput != NULL) {
        delete[] eventsUsedInput;
    }
    
    if (eventsUsedOutput != NULL) {
        delete[] eventsUsedOutput;
    }
    
    numberDeletedVertices++;
    
    trace(TRACE_5, "GraphNode::~GraphNode() : end\n");
}


//! \brief returns the number of this node
//! \return number of this node
unsigned int GraphNode::getNumber() const {
    return number;
}


//! \brief sets the number of this node
//! \param newNumber number of this node in the graph
void GraphNode::setNumber(unsigned int newNumber) {
    number = newNumber;
}


//! \brief returns the name of this node
//! \return name of this node
std::string GraphNode::getName() const {
    return name;
}


//! \brief sets the name of this node
//! \param newName new name of this node in the graph
void GraphNode::setName(std::string newName) {
    name = newName;
}


//! \brief returns the color of the GraphNode
//! \return color of this node
GraphNodeColor GraphNode::getColor() const {
    return color;
}


//! \brief sets the color of the GraphNode to the given color
//! \param c color of GraphNode
void GraphNode::setColor(GraphNodeColor c) {
    color = c;
}


//! \brief Is the node colour blue?
//! \return true iff this node's colour is blue
bool GraphNode::isBlue() const {
    return getColor() == BLUE;
}


//! \brief Is the node colour ret?
//! \return true iff this node's colour is red
bool GraphNode::isRed() const {
    return getColor() == RED;
}


//! \brief Adds a leaving edge to this node.
//! \param edge the new leaving edge
void GraphNode::addLeavingEdge(GraphEdge* edge) {
    leavingEdges.add(edge);
}


//! \brief Returns an iterator that can be used to traverse all leaving edges of
//!        this GraphNode. This iterator can also be used to modify the list of
//!        leaving edges, e.g., to remove an edge.
//!        Consult SList<T>::getIterator() for instructions how to use this
//!        iterator.
//! \return the iterator described above
GraphNode::LeavingEdges::Iterator GraphNode::getLeavingEdgesIterator() {
    return leavingEdges.getIterator();
}


//! \brief Returns a const iterator that can be used to traverse all leaving
//!        edges of this GraphNode. You can not modify the list of leaving edges
//!        with this const iterator. Consult SList<T>::getConstIterator() for
//!        instructions how to use this iterator.
//! \return the iterator described above
GraphNode::LeavingEdges::ConstIterator GraphNode::getLeavingEdgesConstIterator() const {
    return leavingEdges.getConstIterator();
}


//! \brief Returns the number of leaving edges.
//! \return number of leaving edges
unsigned int GraphNode::getLeavingEdgesCount() const {
    return leavingEdges.size();
}


//! \brief adds the state s to the list of states
//! \param s pointer to the state that is to be added to this node
bool GraphNode::addState(State* s) {
    assert(s != NULL);
    pair<StateSet::iterator, bool> result = reachGraphStateSet.insert(s);
    return result.second; // returns whether the element got inserted (true) or not (false)
}


//! \brief returns the diagnosis colour of the GraphNode
//! \return the diagnosis colour
GraphNodeDiagnosisColor GraphNode::getDiagnosisColor() const {
    return diagnosis_color;
}


//! \brief sets the colour of the GraphNode to the given colour
//! \param c colour of GraphNode
//! \return the new diagnosis colour
GraphNodeDiagnosisColor GraphNode::setDiagnosisColor(GraphNodeDiagnosisColor c) {
    diagnosis_color = c;
    return c;
}


//! \brief  returns true iff a colored successor can be avoided
//! \param  color   the color under consideration
//! \return true iff there is either a sending edge to a differently colored
//!         successor or, for each external deadlock, there exists a receiving
//!         edge to a non-colored successor
bool GraphNode::coloredSuccessorsAvoidable(GraphNodeDiagnosisColor_enum color) const {    
    // collect all edges to differently colored successor nodes
    set<GraphEdge*> edges_to_differently_colored_successors;
    bool colored_successor_present = false;
    
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge *element = edgeIter->getNext();
        GraphNode* vNext = element->getDstNode();
        
        if (vNext == this)
            continue;
        
        if (vNext->getDiagnosisColor() != color) {
            // if there is a sending edge to a differently colored sucessor, take this edge;
            // otherwise: store this edge for future considerations
            if (element->getType() == SENDING ) {
                delete edgeIter;
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
            for (set<GraphEdge*>::iterator edge = edges_to_differently_colored_successors.begin();
                 edge != edges_to_differently_colored_successors.end(); edge++) {
                if ((*edge)->getType() == SENDING ) {
                    continue;
                }
                
                string edge_label = (*edge)->getLabel().substr(1, (*edge)->getLabel().length());
                
                for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
                    if (PN->getOutputPlace(i)->name == edge_label) {
                        if (PN->CurrentMarking[PN->getPlaceIndex(PN->getOutputPlace(i))] > 0) {
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


//! \brief  returns true iff edge e is possible in every state
//! \param  e a GraphEdge
//! \return true, iff the edge e is labeled with a sending event, or a
//!         receiving event that is present in every external deadlock state
//! \note   For performance issues, it is not checked whether edge e is really
//!         leaving this node.
bool GraphNode::edgeEnforcable(GraphEdge* e) const {
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
                        if (PN->CurrentMarking[PN->getPlaceIndex(PN->getOutputPlace(i))] == 0) {
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


//! \brief returns true iff e changes the color of the common successors
//! \param e a GraphEdge
//! \return true iff the given edge changes the color of the common successors
//! \todo Whoever implemented me, please comment me!
bool GraphNode::changes_color(GraphEdge* e) const {
    assert(e != NULL);
    
    GraphNode* vNext = e->getDstNode();
    
    if ( vNext->getDiagnosisColor() == DIAG_RED )
        return false;
    
    if ( !edgeEnforcable(e) )
        return false;
    
    
    map< string, GraphEdge* > v_edges;
    map< string, GraphEdge* > vNext_edges;
    
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge* element = edgeIter->getNext();
        v_edges[element->getLabel()] = element;
    }
    delete edgeIter;
    edgeIter = vNext->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge* element = edgeIter->getNext();
        vNext_edges[element->getLabel()] = element;
    }
    delete edgeIter;
    
    
    for (map< string, GraphEdge* >::const_iterator v_edge = v_edges.begin();
         v_edge != v_edges.end(); v_edge++) {
        GraphEdge* vNext_edge = vNext_edges[v_edge->first];
        
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


//! \brief returns true iff node should be shown according to the "show" parameter
//! \param rootOfGraph the root node of the current graph
//! \return true iff this node should be shown according to the "show" parameter
bool GraphNode::isToShow(const GraphNode* rootOfGraph) const {
#undef TRUE

    return ((parameters[P_SHOW_ALL_NODES]) ||
            (getColor() == BLUE) ||
            (getColor() == RED && parameters[P_SHOW_RED_NODES]) ||
            (this == rootOfGraph));

#define TRUE 1
}


//! \brief determines whether this node has a edge with the given label
//! \param edgeLabel the label of the edge
//! \return true iff this node has a edge with the given label
bool GraphNode::hasEdgeWithLabel(const std::string& edgeLabel) const {
    return getEdgeWithLabel(edgeLabel) != NULL;
}


//! \brief determines whether this node has a blue edge with a given label
//! \param edgeLabel the label of the blue edge
//! \return true iff this node has a blue edge with the given label
bool GraphNode::hasBlueEdgeWithLabel(const std::string& edgeLabel) const {
    
    GraphEdge* edge = getEdgeWithLabel(edgeLabel);
    if (edge == NULL)
        return false;
    
    return edge->getDstNode()->isBlue();
}


//! \brief returns a edge of this node with the given label
//! \param edgeLabel the label of the edge that we look for
//! \return a edge of this node with the given label, NULL if no such
//!         edge exists
GraphEdge* GraphNode::getEdgeWithLabel(const std::string& edgeLabel) const {
    
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge* edge = edgeIter->getNext();
        
        if (edge->getLabel() == edgeLabel) {
            delete edgeIter;
            return edge;
        }
    }
    
    delete edgeIter;
    return NULL;
}


//! \brief "fires" the edge of this node with the given label, that is
//!        it returns the destination node of that edge
//! \param edgeLabel the label of this edge that is to be fired
//! \return the destination node of the labeled edge, NULL if the
//!         edge could not be found
GraphNode* GraphNode::followEdgeWithLabel(const std::string& edgeLabel) {
    
    if (!(parameters[P_PV])) {
        assert(edgeLabel != GraphFormulaLiteral::TAU);
    }
    
    GraphEdge* edge = getEdgeWithLabel(edgeLabel);
    if (edge == NULL) {
        return NULL;
    }
    
    return edge->getDstNode();
}


//! \brief removes all edge from this node to the given one
//! \param nodeToDelete the target node
void GraphNode::removeEdgesToNode(const GraphNode* nodeToDelete) {
    LeavingEdges::Iterator iEdge = getLeavingEdgesIterator();
    while (iEdge->hasNext()) {
        GraphEdge* edge = iEdge->getNext();
        if (edge->getDstNode() == nodeToDelete) {
            // -"ByHiding" does not work in the numberOfServices() function
            // since an OG without any strategies will become a single root
            // node with the annotation "true", when calling the 
            // removeFalseNodes() function beforehand
            // removeLiteralFromAnnotationByHiding(edge->getLabel());
            //removeLiteralFromAnnotation(edge->getLabel());
            delete edge;
            iEdge->remove();
        }
    }
    delete iEdge;
}

//! \brief set the node to be final or not
//! \param finalValue the truth value of the node being final
void GraphNode::setFinal(bool finalValue) {
    final = finalValue;
}

//! \brief returns true, if the node is a final node
//! \return true if the node is final
bool GraphNode::isFinal() {
    return final;
}

