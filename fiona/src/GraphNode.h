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
#include "debug.h"
#include "state.h"
#include "options.h"
#include <set>
#include <cassert>
#include "SinglyLinkedList.h"
#include "GraphFormula.h"
#include "GraphEdge.h"

//class State;
class literal;
class OGFromFileNode;
//class GraphFormula;
//class GraphFormulaAssignment;
//class GraphFormulaMultiaryOr;
//class GraphFormulaCNF;

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
template<typename GraphNodeType = GraphNode> class GraphNodeCommon {
    public:

        /// Type of the container that holds all leaving edges of this GraphNode.
        typedef SList<GraphEdge<GraphNodeType>*> LeavingEdges;

    protected:

        /// Number of this GraphNode in the graph.
        unsigned int number;

        /// Name of this GraphNode in the graph.
        std::string name;

        /// Color of this GraphNode.
        GraphNodeColor color;

        //! Diagnosis color of this GraphNode.
        GraphNodeDiagnosisColor diagnosis_color;

        /// Annotation of this node (a CNF) as a formula.
        GraphFormulaCNF* annotation;

        /// Contains all leaving edges.
        LeavingEdges leavingEdges;

    public:

        bool hasFinalStateInStateSet;
        int* eventsUsed;

        // this set contains only a reduced number of states in case the state
        // reduced graph is to be build.
        StateSet reachGraphStateSet;

        /// constructor
        GraphNodeCommon();

        //! constructor
        //! \param _name
        //! \param _annotation
        GraphNodeCommon(const std::string& _name,
                        GraphFormula* _annotation);

        //! constructor
        //! \param _name
        //! \param _annotation
        //! \param _color
        GraphNodeCommon(const std::string& _name,
                        GraphFormula* _annotation,
                        GraphNodeColor _color);


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

        /// get the annotation
        GraphFormulaCNF* getAnnotation() const;

        /// get the annotation as a string
        std::string getAnnotationAsString() const;

        /// Destroys this GraphNodeCommon.
        ~GraphNodeCommon();

        /// Adds a leaving edge to this node.
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

        /// Returns the number of leaving edges.
        unsigned int getLeavingEdgesCount() const;


        
        // originate from GraphNode

        /// adds a state to the states of a GraphNode
        bool addState(State *);

        void addClause(GraphFormulaMultiaryOr*);

        /// analyses the node and sets its color
        void analyseNode();

        /// get the node diagnosis color
        GraphNodeDiagnosisColor getDiagnosisColor() const;

        /// set the diagnosis color
        GraphNodeDiagnosisColor setDiagnosisColor(GraphNodeDiagnosisColor c);

        void removeLiteralFromAnnotation(const std::string& literal);

        /// Removes unneeded literals from the node's annotation. Labels of edges to
        /// red nodes are unneeded.
        void removeUnneededLiteralsFromAnnotation();
        
        /// returns true iff a colored successor of v can be avoided
        bool coloredSuccessorsAvoidable(GraphNodeDiagnosisColor_enum color) const;        
        
        /// returns true iff edge e is possible in every state
        bool edgeEnforcable(GraphEdge<> *e) const;
        
        /// returns true iff e changes the color of the common successors
        bool changes_color(GraphEdge<> *e) const;
        
        /// returns true iff node should be shown according to the "show" parameter
        bool isToShow(const GraphNodeCommon<GraphNode>* rootOfGraph) const;

        /// compare operator to enable use of sets of GraphNodes
//        friend bool operator <(GraphNodeCommon<GraphNode> const&, GraphNodeCommon<GraphNode> const&);





        // originate from OGFromFileNode
        bool hasTransitionWithLabel(const std::string&) const;
        bool hasBlueTransitionWithLabel(const std::string&) const;

        GraphEdge<OGFromFileNode>* getTransitionWithLabel(const std::string&) const;
        GraphNodeCommon<OGFromFileNode>* fireTransitionWithLabel(const std::string&);

        bool assignmentSatisfiesAnnotation(const GraphFormulaAssignment&) const;

        GraphFormulaAssignment* getAssignment() const;

        void removeTransitionsToNode(const GraphNodeCommon<OGFromFileNode>*);

//#undef new
//        /// Provides user defined operator new. Needed to trace all new operations
//        /// on this class.
//        NEW_OPERATOR(GraphNode)
//#define new NEW_NEW
};


///*******************
// * class GraphNode *
// *******************/
//
class GraphNode : public GraphNodeCommon<> {

};


/*************************
 * class GraphNodeCommon *
 *************************/


//! \brief constructor
template<typename GraphNodeType> GraphNodeCommon<GraphNodeType>::GraphNodeCommon() :
    number(12345678),
    name("12345678"),
    color(BLUE),
    diagnosis_color(DIAG_UNSET),
    hasFinalStateInStateSet(false) {

    annotation = new GraphFormulaCNF();

    eventsUsed = new int [PN->getInputPlaceCount() + PN->getOutputPlaceCount()];

    for (unsigned int i = 0;
         i < PN->getInputPlaceCount() + PN->getOutputPlaceCount(); i++) {
        eventsUsed[i] = 0;
    }
}


//! \brief constructor
template<typename GraphNodeType> GraphNodeCommon<GraphNodeType>::GraphNodeCommon(const std::string& _name,
                                                                                 GraphFormula* _annotation) :
    number(12345678), name(_name), color(BLUE) {

    annotation = _annotation->getCNF();
    delete _annotation; // because getCNF() returns a newly create formula
}


//! \brief constructor
template<typename GraphNodeType> GraphNodeCommon<GraphNodeType>::GraphNodeCommon(const std::string& _name,
                                                                                 GraphFormula* _annotation,
                                                                                 GraphNodeColor _color) :
    number(12345678), name(_name), color(_color) {

    annotation = _annotation->getCNF();
    delete _annotation; // because getCNF() returns a newly create formula
}


//! returns the number of this node
//! \return number of this node
template<typename GraphNodeType> unsigned int GraphNodeCommon<GraphNodeType>::getNumber() const {
    return number;
}


//! sets the number of this node
//! \param newNumber number of this node in the graph
template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::setNumber(unsigned int newNumber) {
    number = newNumber;
}


//! returns the name of this node
//! \return name of this node
template<typename GraphNodeType> std::string GraphNodeCommon<GraphNodeType>::getName() const {
    return name;
}


//! sets the name of this node
//! \param newName new name of this node in the graph
template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::setName(std::string newName) {
    name = newName;
}


//! returns the color of the GraphNode
//! \return color of this node
template<typename GraphNodeType> GraphNodeColor GraphNodeCommon<GraphNodeType>::getColor() const {
    return color;
}


//! sets the color of the GraphNode to the given color
//! \param c color of GraphNode
template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::setColor(GraphNodeColor c) {
    color = c;
}


template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::isBlue() const {
    return getColor() == BLUE;
}


template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::isRed() const {
    return getColor() == RED;
}


// returns the CNF formula that is the annotation of a node as a Boolean formula
template<typename GraphNodeType> GraphFormulaCNF* GraphNodeCommon<GraphNodeType>::getAnnotation() const {
    return annotation;
}


template<typename GraphNodeType> std::string GraphNodeCommon<GraphNodeType>::getAnnotationAsString() const {
    assert(annotation != NULL);
    return annotation->asString();
}


template<typename GraphNodeType> GraphNodeCommon<GraphNodeType>::~GraphNodeCommon<GraphNodeType>() {

    trace(TRACE_5, "GraphNode::~GraphNode() : start\n");

    typename LeavingEdges::ConstIterator iEdge = getLeavingEdgesConstIterator();
    while (iEdge->hasNext()) {
        GraphEdge<GraphNodeType>* edge = iEdge->getNext();
        delete edge;
    }
    delete iEdge;

    delete annotation;

    if (eventsUsed != NULL) {
        delete[] eventsUsed;
    }

    numberDeletedVertices++;

    trace(TRACE_5, "GraphNode::~GraphNode() : end\n");
}


template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::addLeavingEdge(GraphEdge<GraphNodeType>* edge) {
    leavingEdges.add(edge);
}


template<typename GraphNodeType> typename GraphNodeCommon<GraphNodeType>::LeavingEdges::Iterator 
        GraphNodeCommon<GraphNodeType>::getLeavingEdgesIterator() {
    return leavingEdges.getIterator();
}


template<typename GraphNodeType> typename GraphNodeCommon<GraphNodeType>::LeavingEdges::ConstIterator
        GraphNodeCommon<GraphNodeType>::getLeavingEdgesConstIterator() const {
    return leavingEdges.getConstIterator();
}


template<typename GraphNodeType> unsigned int GraphNodeCommon<GraphNodeType>::getLeavingEdgesCount() const {
    return leavingEdges.size();
}



//*********************************************************************


// originate from GraphNode


//! \param s pointer to the state that is to be added to this node
//! \brief adds the state s to the list of states
template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::addState(State * s) {
    assert(s != NULL);
    pair<StateSet::iterator, bool> result = reachGraphStateSet.insert(s);
    return result.second; // returns whether the element got inserted (true) or not (false)
}


//! \param myclause the clause to be added to the annotation of the current node
//! \brief adds a new clause to the CNF formula of the node
template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::addClause(GraphFormulaMultiaryOr* myclause) {
    annotation->addClause(myclause);
}


template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::removeLiteralFromAnnotation(const std::string& literal) {
    trace(TRACE_5, "GraphNode::removeLiteralFromAnnotation(const string& literal) : start\n");

    //cout << "remove literal " << literal << " from annotation " << annotation->asString() << " of node number " << getName() << endl;
    annotation->removeLiteral(literal);

    trace(TRACE_5, "GraphNode::removeLiteralFromAnnotation(const string& literal) : end\n");
}


template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::removeUnneededLiteralsFromAnnotation() {
    typename LeavingEdges::ConstIterator
            edgeIter = getLeavingEdgesConstIterator();

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
template<typename GraphNodeType> GraphNodeDiagnosisColor GraphNodeCommon<GraphNodeType>::setDiagnosisColor(GraphNodeDiagnosisColor c) {
    diagnosis_color = c;
    return c;
}


//! \brief returns the diagnosis color of the GraphNode
template<typename GraphNodeType> GraphNodeDiagnosisColor GraphNodeCommon<GraphNodeType>::getDiagnosisColor() const {
    return diagnosis_color;
}


//! \brief analyses the node and sets its color
template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::analyseNode() {

    trace(TRACE_5, "GraphNode::analyseNodeByFormula() : start\n");

    trace(TRACE_3, "\t\t\t analysing node ");
    trace(TRACE_3, this->getNumber() + "...\n");

    assert(this->getColor() == BLUE);

    // computing the assignment given by outgoing edges (to blue nodes)
    GraphFormulaAssignment* myassignment = new GraphFormulaAssignment();

    // traverse outgoing edges and set the corresponding literals
    // to true if the respective node is BLUE
    typename LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
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


/*!
 * \brief  returns true iff a colored successor can be avoided
 *
 * \param  color   the color under consideration
 *
 * \return true iff there is either a sending edge to a differently colored
 *         successor or, for each external deadlock, there exists a receiving
 *         edge to a non-colored successor
 */
template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::coloredSuccessorsAvoidable(GraphNodeDiagnosisColor_enum color) const {    
    // collect all edges to differently colored successor nodes
    set<GraphEdge<>*> edges_to_differently_colored_successors;
    bool colored_successor_present = false;
    
    typename LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge<> *element = edgeIter->getNext();
        GraphNodeCommon<GraphNodeType>* vNext = element->getDstNode();
        
        if (vNext == this)
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
template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::edgeEnforcable(GraphEdge<> *e) const {
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
template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::changes_color(GraphEdge<> *e) const {
    assert(e != NULL);
    
    GraphNodeCommon<GraphNodeType>* vNext = e->getDstNode();
    
    if ( vNext->getDiagnosisColor() == DIAG_RED )
        return false;
    
    if ( !edgeEnforcable(e) )
        return false;
    
    
    map< string, GraphEdge<>* > v_edges;
    map< string, GraphEdge<>* > vNext_edges;
    
    typename LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
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


template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::isToShow(const GraphNodeCommon<GraphNode>* rootOfGraph) const {

    if (parameters[P_SHOW_ALL_NODES] || (parameters[P_SHOW_NO_RED_NODES] &&
        (getColor() != RED))|| (!parameters[P_SHOW_NO_RED_NODES] &&
        (getColor() == RED))|| (getColor() == BLUE) ||
        (this == rootOfGraph)) {

        return (parameters[P_SHOW_EMPTY_NODE] || reachGraphStateSet.size() != 0);
    } else {
        return false;
    }
}


//! \param left left hand GraphNode
//! \param right right hand GraphNode
//! \brief implements the operator < by comparing the states of the two vertices
//bool operator <(GraphNodeCommon<GraphNode> const& left, GraphNodeCommon<GraphNode> const& right) {
//    return (left.reachGraphStateSet < right.reachGraphStateSet);
//}


//*********************************************************************



// originate from OGFromFileNode

template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::hasTransitionWithLabel(const std::string& transitionLabel) const {

    return getTransitionWithLabel(transitionLabel) != NULL;
}


template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::hasBlueTransitionWithLabel(const std::string& transitionLabel) const {

    GraphEdge<GraphNodeType>* transition = getTransitionWithLabel(transitionLabel);
    if (transition == NULL)
        return false;

    return transition->getDstNode()->isBlue();
}


template<typename GraphNodeType> GraphEdge<OGFromFileNode>* GraphNodeCommon<GraphNodeType>::getTransitionWithLabel(const std::string& transitionLabel) const {

    typename LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge<OGFromFileNode>* edge = edgeIter->getNext();

        if (edge->getLabel() == transitionLabel) {
            delete edgeIter;
            return edge;
        }
    }

    delete edgeIter;
    return NULL;
}


template<typename GraphNodeType> GraphNodeCommon<OGFromFileNode>* GraphNodeCommon<GraphNodeType>::fireTransitionWithLabel(const std::string& transitionLabel) {

    assert(transitionLabel != GraphFormulaLiteral::TAU);

    GraphEdge<OGFromFileNode>* transition = getTransitionWithLabel(transitionLabel);
    if (transition == NULL) {
        return NULL;
    }

    return transition->getDstNode();
}


template<typename GraphNodeType> bool GraphNodeCommon<GraphNodeType>::assignmentSatisfiesAnnotation(const GraphFormulaAssignment& assignment) const {

    assert(annotation != NULL);
    return annotation->satisfies(assignment);
}


// return the assignment that is imposed by present or absent arcs leaving the node
template<typename GraphNodeType> GraphFormulaAssignment* GraphNodeCommon<GraphNodeType>::getAssignment() const {

    trace(TRACE_5, "computing annotation of node " + getName() + "\n");

    GraphFormulaAssignment* myassignment = new GraphFormulaAssignment();

    // traverse outgoing edges and set the corresponding literals
    // to true if the respective node is BLUE

    typename LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge<OGFromFileNode>* edge = edgeIter->getNext();
        myassignment->setToTrue(edge->getLabel());
    }
    delete edgeIter;

    // we assume that literal final is always true
    myassignment->setToTrue(GraphFormulaLiteral::FINAL);

    return myassignment;
}


template<typename GraphNodeType> void GraphNodeCommon<GraphNodeType>::removeTransitionsToNode(const GraphNodeCommon<OGFromFileNode>* nodeToDelete) {
    typename LeavingEdges::Iterator iEdge = getLeavingEdgesIterator();
    while (iEdge->hasNext()) {
        GraphEdge<OGFromFileNode>* edge = iEdge->getNext();
        if (edge->getDstNode() == nodeToDelete) {
            delete edge;
            iEdge->remove();
        }
    }
    delete iEdge;
}


#endif /*GraphNode_H_*/
