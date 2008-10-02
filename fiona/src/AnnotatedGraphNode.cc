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
 * \file    AnnotatedGraphNode.cc
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
#include "AnnotatedGraphNode.h"
#include "options.h"
#include <cassert>

using namespace std;

/******************
* class AnnotatedGraphNode *
******************/

//! \brief constructor (no parameters)
AnnotatedGraphNode::AnnotatedGraphNode() {

    annotation = new GraphFormulaCNF();
    final = false;
}


//! \brief constructor (four parameters)
//! \param _name the name of this node
//! \param _annotation the annotation of this node
//! \param _color the colour of this node
//! \param _number the number of this node
AnnotatedGraphNode::AnnotatedGraphNode(const std::string& _name,
                                       GraphFormula* _annotation,
                                       GraphNodeColor _color,
                                       unsigned int _number) : GraphNode(_name, _color, _number) {
    
    annotation = _annotation->getCNF();
    final = false;
    delete _annotation; // because getCNF() returns a newly created formula
}


//! \brief destructor
AnnotatedGraphNode::~AnnotatedGraphNode() {

    trace(TRACE_5, "AnnotatedGraphNode::~AnnotatedGraphNode() : start\n");

    LeavingEdges::ConstIterator iEdge = getLeavingEdgesConstIterator();
    while (iEdge->hasNext()) {
        AnnotatedGraphEdge* edge = iEdge->getNext();
        delete edge;
    }
    delete iEdge;

    delete annotation;

    trace(TRACE_5, "AnnotatedGraphNode::~AnnotatedGraphNode() : end\n");
}


//! \brief returns the CNF formula that is this node's annotation
//! \return this node's annotation as a GraphFormulaCNF
GraphFormulaCNF* AnnotatedGraphNode::getAnnotation() const {
    return annotation;
}


// CODE FROM PL
//! \brief sets the annotation of this node
//! \param newAnnotation annotation of the node
void AnnotatedGraphNode::setAnnotation(GraphFormulaCNF* newAnnotation) {
    if (annotation != NULL) {
        delete annotation;
    }
    annotation = newAnnotation;
}
// END OF CODE FROM PL


//! \brief returns the CNF formula that is this node's annotation as a String
//! \return this node's annotation as a String
std::string AnnotatedGraphNode::getAnnotationAsString() const {
    assert(annotation != NULL);
    return annotation->asString();
}


//! \brief adds a new clause to the CNF formula of the node
//! \param myclause the clause to be added to the annotation of the current node
void AnnotatedGraphNode::addClause(GraphFormulaMultiaryOr* myclause) {
    annotation->addClause(myclause);
}


//! \brief Adds a leaving edge to this node.
//! \param edge the new leaving edge
void AnnotatedGraphNode::addLeavingEdge(AnnotatedGraphEdge* edge) {
    leavingEdges.add(edge);
}


//! \brief Returns an iterator that can be used to traverse all leaving edges of
//!        this GraphNode. This iterator can also be used to modify the list of
//!        leaving edges, e.g., to remove an edge.
//!        Consult SList<T>::getIterator() for instructions how to use this
//!        iterator.
//! \return the iterator described above
AnnotatedGraphNode::LeavingEdges::Iterator AnnotatedGraphNode::getLeavingEdgesIterator() {
    return leavingEdges.getIterator();
}


//! \brief Returns a const iterator that can be used to traverse all leaving
//!        edges of this GraphNode. You can not modify the list of leaving edges
//!        with this const iterator. Consult SList<T>::getConstIterator() for
//!        instructions how to use this iterator.
//! \return the iterator described above
AnnotatedGraphNode::LeavingEdges::ConstIterator AnnotatedGraphNode::getLeavingEdgesConstIterator() const {
    return leavingEdges.getConstIterator();
}


//! \brief Returns the number of leaving edges.
//! \return number of leaving edges
unsigned int AnnotatedGraphNode::getLeavingEdgesCount() const {
    return leavingEdges.size();
}


//! \brief  returns true iff a colored successor can be avoided
//! \param  color   the color under consideration
//! \return true iff there is either a sending edge to a differently colored
//!         successor or, for each external deadlock, there exists a receiving
//!         edge to a non-colored successor
bool AnnotatedGraphNode::coloredSuccessorsAvoidable(GraphNodeDiagnosisColor_enum color) const {    
    // collect all edges to differently colored successor nodes
    set<AnnotatedGraphEdge*> edges_to_differently_colored_successors;
    bool colored_successor_present = false;
    
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge *element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();
        
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
            for (set<AnnotatedGraphEdge*>::iterator edge = edges_to_differently_colored_successors.begin();
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
bool AnnotatedGraphNode::edgeEnforcable(AnnotatedGraphEdge* e) const {
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
bool AnnotatedGraphNode::changes_color(AnnotatedGraphEdge* e) const {
    assert(e != NULL);
    
    AnnotatedGraphNode* vNext = e->getDstNode();
    
    if ( vNext->getDiagnosisColor() == DIAG_RED )
        return false;
    
    if ( !edgeEnforcable(e) )
        return false;
    
    
    map< string, AnnotatedGraphEdge* > v_edges;
    map< string, AnnotatedGraphEdge* > vNext_edges;
    
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        v_edges[element->getLabel()] = element;
    }
    delete edgeIter;
    edgeIter = vNext->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        vNext_edges[element->getLabel()] = element;
    }
    delete edgeIter;
    
    
    for (map< string, AnnotatedGraphEdge* >::const_iterator v_edge = v_edges.begin();
         v_edge != v_edges.end(); v_edge++) {
        AnnotatedGraphEdge* vNext_edge = vNext_edges[v_edge->first];
        
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


//! \brief determines whether this node has a edge with the given label
//! \param edgeLabel the label of the edge
//! \return true iff this node has a edge with the given label
bool AnnotatedGraphNode::hasEdgeWithLabel(const std::string& edgeLabel) const {
    return getEdgeWithLabel(edgeLabel) != NULL;
}


//! \brief determines whether this node has a blue edge with a given label
//! \param edgeLabel the label of the blue edge
//! \return true iff this node has a blue edge with the given label
bool AnnotatedGraphNode::hasBlueEdgeWithLabel(const std::string& edgeLabel) const {
    
    AnnotatedGraphEdge* edge = getEdgeWithLabel(edgeLabel);
    if (edge == NULL)
        return false;
    
    return edge->getDstNode()->isBlue();
}


//! \brief returns a edge of this node with the given label
//! \param edgeLabel the label of the edge that we look for
//! \return a edge of this node with the given label, NULL if no such
//!         edge exists
AnnotatedGraphEdge* AnnotatedGraphNode::getEdgeWithLabel(const std::string& edgeLabel) const {
    
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();
        
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
AnnotatedGraphNode* AnnotatedGraphNode::followEdgeWithLabel(const std::string& edgeLabel) {
    
    if (!(parameters[P_PV])) {
        assert(edgeLabel != GraphFormulaLiteral::TAU);
    }
    
    AnnotatedGraphEdge* edge = getEdgeWithLabel(edgeLabel);
    if (edge == NULL) {
        return NULL;
    }
    
    return edge->getDstNode();
}


//! \brief removes all edge from this node to the given one
//! \param nodeToDelete the target node
void AnnotatedGraphNode::removeEdgesToNode(const AnnotatedGraphNode* nodeToDelete) {
    LeavingEdges::Iterator iEdge = getLeavingEdgesIterator();
    while (iEdge->hasNext()) {
        AnnotatedGraphEdge* edge = iEdge->getNext();
        if (edge->getDstNode() == nodeToDelete) {
            removeLiteralFromAnnotation(edge->getLabel());
            delete edge;
            iEdge->remove();
        }
    }
    delete iEdge;
}


//! \brief analyses the node and sets its color
void AnnotatedGraphNode::analyseNode(bool ignoreFinal) {
    
    trace(TRACE_5, "AnnotatedGraphNode::analyseNodeByFormula() : start\n");
    
    trace(TRACE_3, "\t\t\t analysing node ");
    trace(TRACE_3, this->getName() + "...\n");
    assert(this->getColor() == BLUE);
    
    // computing the assignment given by outgoing edges (to blue nodes)
    GraphFormulaAssignment* myassignment = new GraphFormulaAssignment();
    
    // traverse outgoing edges and set the corresponding literals
    // to true if the respective node is BLUE
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();
        if (edge->getDstNode()->getColor() == BLUE) {
            myassignment->setToTrue(edge->getLabel());
        }
    }
    delete edgeIter;
    
    // only if node has final state, set assignment of literal final to true
    if ((this->hasFinalStateInStateSet == true) || ignoreFinal) {
        myassignment->setToTrue(GraphFormulaLiteral::FINAL);
    }
    
//    cout << "*myassignment: " << *myassignment->asString << endl;
    
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
    
    trace(TRACE_5, "AnnotatedGraphNode::analyseNodeByFormula() : end\n");
}


//! \brief removes the given literal from this node's annotation
//! \param literal the literal which is to be removed
void AnnotatedGraphNode::removeLiteralFromAnnotation(const std::string& literal) {
    trace(TRACE_5, "AnnotatedGraphNode::removeLiteralFromAnnotation(const string& literal) : start\n");
    
    //cout << "remove literal " << literal << " from annotation " << annotation->asString() << " of node number " << getName() << endl;
    annotation->removeLiteral(literal);
    
    trace(TRACE_5, "AnnotatedGraphNode::removeLiteralFromAnnotation(const string& literal) : end\n");
}


//! \brief Removes unneeded literals from the node's annotation. Labels of
//!        edges to red nodes are unneeded.
void AnnotatedGraphNode::removeUnneededLiteralsFromAnnotation() {
    LeavingEdges::ConstIterator
    edgeIter = getLeavingEdgesConstIterator();
    
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();
        if (edge->getDstNode()->getColor() == RED) {
            annotation->removeLiteral(edge->getLabel());
        }
    }
    delete edgeIter;
}


//! \brief returns true iff node should be shown according to the "show" parameter
//! \param rootOfGraph the root node of the current graph
//! \param hasOWFN if the oWFN is given, the knowledge stored in the nodes can be
//!                used to distinguish the empty node from other true nodes
//! \return true iff this node should be shown according to the "show" parameter
bool AnnotatedGraphNode::isToShow(const AnnotatedGraphNode* rootOfGraph, bool hasOWFN) const {
// always call this method like: v->isToShow(root, (PN != NULL))
    
#undef TRUE

    return ((parameters[P_SHOW_ALL_NODES]) ||
            (getColor() == BLUE && (!hasOWFN || 
                                    reachGraphStateSet.size() != 0 || 
                                    parameters[P_SHOW_EMPTY_NODE])) ||
            (getColor() == RED && parameters[P_SHOW_RED_NODES]) ||
            (this == rootOfGraph));

#define TRUE 1
}


//! \brief determines whether the given assignment satisfies this node's
//!        annotation
//! \param assignment the assignment that satisfies the annotation or not
//! \return true iff the assignment satisfies the annotation
bool AnnotatedGraphNode::assignmentSatisfiesAnnotation(const GraphFormulaAssignment& assignment) const {
    
    trace(TRACE_5, "AnnotatedGraphNode::assignmentSatisfiesAnnotation(const GraphFormulaAssignment& assignment) const: start\n");
    assert(annotation != NULL);
    trace(TRACE_5, "AnnotatedGraphNode::assignmentSatisfiesAnnotation(const GraphFormulaAssignment& assignment) const: end\n");
    return annotation->satisfies(assignment);
}


//! \brief return the assignment that is imposed by present or absent arcs
//!        leaving the node
//! \return the assignment described above
GraphFormulaAssignment* AnnotatedGraphNode::getAssignment() const {
    
    trace(TRACE_5, "AnnotatedGraphNode::getAssignment(): start\n");
    trace(TRACE_5, "computing annotation of node " + getName() + "\n");
    
    GraphFormulaAssignment* myassignment = new GraphFormulaAssignment();
    
    // traverse outgoing edges and set the corresponding literals
    // to true if the respective node is BLUE
    
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();
        myassignment->setToTrue(edge->getLabel());
    }
    delete edgeIter;
    
    // we assume that literal final is always true
    myassignment->setToTrue(GraphFormulaLiteral::FINAL);
    
    trace(TRACE_5, "AnnotatedGraphNode::getAssignment(): end\n");
    return myassignment;
}


//! \brief fills a given set of strings with all events appearing either
//!        in the nodes formula or in one of it's leaving edges
//! \param events set of string to fill
void AnnotatedGraphNode::getEvents(set<string>& events) {
    
    trace(TRACE_5, "AnnotatedGraphNode::getEvents(): start\n");
    trace(TRACE_5, "computing annotation of node " + getName() + "\n");
    
    // Adds all event-representing literals from the formula of the node
    // to the set of events
    annotation->getEventLiterals(events);    
    
    // Adds the labels of alle edges to the set
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();
        if (edge->getLabel() != GraphFormulaLiteral::TAU) {
            events.insert(edge->getLabel());            
        }
    }
    delete edgeIter;

    trace(TRACE_5, "AnnotatedGraphNode::getEvents(): end\n");
}


//! \brief Delivers the element from the priority map with the highest priority.
//!        The element with the highest priority will be in a clause with minimal length
//!        and will have the maximal count of occurrences throughout the annotation
//!        in regards to other elements with minimal clause length. 
//!        The element will be removed afterwards. 
PriorityMap::KeyType PriorityMap::pop() {
    KeyType key;

    // initialize with minimum priority values
    int min_depth = INT_MAX;
    int max_occ = 0;

    // iterate over the mapped set of interface places
    MapType::iterator i;
    for (i = pm.begin(); i != pm.end(); i++) {

        // consider the element with a minimal depth clause (to catch the init values) 
        if (i->second.first ==  min_depth) {
            // consider the second moment of priority (occurrence count)
            if (i->second.second >= max_occ) {
                key = i->first;
                min_depth = i->second.first;
                max_occ   = i->second.second;
            }
        }
        // first moment of priority suffices 
        else if (i->second.first < min_depth) {
            key = i->first;
            min_depth = i->second.first;
            max_occ   = i->second.second;
        }
    }

    // remove popped element
    pm.erase(key);

    return key;
}

void PriorityMap::adjustPM(oWFN * PN, const std::string & eventName) {
    
    trace(TRACE_5, "PriorityMap::adjustPM(oWFN * PN, const std::string & eventName): start\n");
    
    MapClauseEvents::iterator iterClauses;
    set<std::string>::iterator iterLiterals;
    MapTypeIG::iterator iterPM;
    
    set<int> clausesToDelete;

    messageMultiSet key;
    
    // consider each clause
    for (iterClauses = pmClauseEvents.begin(); iterClauses != pmClauseEvents.end(); iterClauses++) {
        
        // the given event is found in the currently considered clause
        if (iterClauses->second.find(eventName) != iterClauses->second.end()) {
            
            // now check each literal that is in the clause as well
            for (iterLiterals = iterClauses->second.begin(); iterLiterals != iterClauses->second.end();
                    iterLiterals++) {

                // iterate over the mapped set of interface places
                for (iterPM = pmIG.begin(); iterPM != pmIG.end(); iterPM++) {
                    if (PN->createLabel(iterPM->first) == *iterLiterals) {
                        iterPM->second.second--;
                        if (iterPM->second.second <= 0) {
                            key = iterPM->first;
                        }
                        break;
                    }
                }
                pmIG.erase(key);
            }
            clausesToDelete.insert(iterClauses->first);
        }
    }
    
    for (set<int>::iterator iterClausesToDelete = clausesToDelete.begin(); 
            iterClausesToDelete != clausesToDelete.end(); iterClausesToDelete++) {
        
        pmClauseEvents.erase(*iterClausesToDelete);
    }

    trace(TRACE_5, "PriorityMap::adjustPM(oWFN * PN, const std::string & eventName): end\n");
}

//! \brief Delivers the element from the priority map with the highest priority.
//!        The element with the highest priority will be in a clause with minimal length
//!        and will have the maximal count of occurrences throughout the annotation
//!        in regards to other elements with minimal clause length. 
//!        The element will be removed afterwards. 
messageMultiSet PriorityMap::popIG() {
    messageMultiSet key;

    MapTypeIG::iterator i;
    
    // initialize with minimum priority values
    int min_depth = INT_MAX;
    int max_occ = 0;

    // iterate over the mapped set of interface places
    for (i = pmIG.begin(); i != pmIG.end(); i++) {
        // consider the element with a minimal depth clause (to catch the init values) 
        if (i->second.first == min_depth) {
            // consider the second moment of priority (occurrence count)
            if (i->second.second >= max_occ) {
                key = i->first;
                min_depth = i->second.first;
                max_occ   = i->second.second;
            }
        }
        // first moment of priority suffices 
        else if (i->second.first < min_depth) {
            key = i->first;
            min_depth = i->second.first;
            max_occ   = i->second.second;
        }
    }

    // remove popped element
    pmIG.erase(key);

    return key;
}

//! \brief Fills the priority map according to the given annotation with interface places 
//!           and their corresponding priority.
//!        NOTE: All interface places will be considered; places not in the
//!        annotation will have a minimal priority.
//! \param annotation the annotation, from which the priority map will be extracted.
void PriorityMap::fill(oWFN * PN, GraphFormulaCNF *annotation) {
    trace(TRACE_5, "PriorityMap::fill(GraphFormulaCNF *annotation): start\n");

    oWFN::Places_t::size_type i;
    KeyType key;

    // iterate over all places in the net
    for (i = 0; i < PN->getPlaceCount(); i++) {

        key = PN->getPlace(i);
        // only consider interface places
        if (key != NULL && (key->getType() == INPUT || key->getType() == OUTPUT)) {
            // initialize with a minimal priority
            pm[key].first  = INT_MAX; // minimal clause length
            pm[key].second = 0;       // maximal occurrence in the annotation

            // iterate over the annotation (in cnf) in regards to a specific interface place
            for (GraphFormulaMultiaryAnd::iterator j = annotation->begin();
                 j != annotation->end(); j++) {

                GraphFormulaMultiaryOr* clause = dynamic_cast<GraphFormulaMultiaryOr*>(*j);
                // iterate over disjunctive clauses
                for (GraphFormulaMultiaryOr::iterator k = clause->begin(); k != clause->end(); k++) {

                    GraphFormulaLiteral* lit = dynamic_cast<GraphFormulaLiteral*>(*k);
                    // interface place found in the clause?
                    if (lit->asString() == key->getLabelForCommGraph()) {
                        // for every literal found, increase the count of occurrence
                        pm[key].second++;

                        // if this label's actual clause is shorter than the former minimum, set the new minimum
                        if (clause->size() < pm[key].first) {
                            pm[key].first = clause->size();
                        }
                    }
                }
            }
        }
    }

    trace(TRACE_5, "PriorityMap::fill(GraphFormulaCNF *annotation): end\n");
}

//! \brief Fills the priority map according to the given annotation with interface places 
//!           and their corresponding priority.
//!        NOTE: All interface places will be considered; places not in the
//!        annotation will have a minimal priority.
//! \param annotation the annotation, from which the priority map will be extracted.
void PriorityMap::fillForIG(setOfMessages &activatedEvents, oWFN * PN, GraphFormulaCNF *annotation) {
    
    trace(TRACE_5, "PriorityMap::fillForIG(GraphFormulaCNF *annotation): start\n");

    // just to remember which event we have considered already, 
    // needed for initialising the priority map correctly
    map<messageMultiSet, bool> eventFound;    

    int clauseNo = 0;  // clause counter needed to give each clause a unique number
    
    // iterate over all activated events stored in the set
    for (setOfMessages::iterator activatedEvent = activatedEvents.begin();
                                    activatedEvent != activatedEvents.end();
                                    activatedEvent++) {

        // check, if we have not considered this event yet
        map<messageMultiSet, bool>::iterator found = eventFound.find(*activatedEvent); 
        if (found != eventFound.end()) {       
            // initialize with a minimal priority
            pmIG[*activatedEvent].first  = INT_MAX; // minimal clause length
            pmIG[*activatedEvent].second = 0;       // maximal occurrences in the annotation

            // remember this that we took a look at this event
            eventFound[*activatedEvent] = true;
        }

        // iterate over the annotation (in cnf) with respect to a specific interface place
        for (GraphFormulaMultiaryAnd::iterator j = annotation->begin();
                                                j != annotation->end(); j++) {

            GraphFormulaMultiaryOr* clause = dynamic_cast<GraphFormulaMultiaryOr*>(*j);

            // iterate over disjunctive clauses
            for (GraphFormulaMultiaryOr::iterator k = clause->begin(); k != clause->end(); k++) {

                GraphFormulaLiteral* lit = dynamic_cast<GraphFormulaLiteral*>(*k);

                // activated event found in the clause?
                if (lit->asString() == PN->createLabel(*activatedEvent)) {

                    // for every literal found, increase the number of occurrences
                    pmIG[*activatedEvent].second++;

                    // if this label's actual clause is shorter than the former minimum, 
                    // set the new minimum
                    if (clause->size() < pmIG[*activatedEvent].first) {
                        pmIG[*activatedEvent].first = clause->size();
                    }
                }
                
                // currently considered clause with number clauseNo contains the literal lit
                pmClauseEvents[clauseNo].insert(lit->asString());    
            }
            
            clauseNo++;    // we consider a new clause now, so increase the clause counter
        }
    }

    trace(TRACE_5, "PriorityMap::fillForIG(GraphFormulaCNF *annotation): end\n");
}

//! \brief returns whether the priority map is empty
//! \return returns true if the priority map is empty, false otherwise
bool PriorityMap::empty() const {
    return pm.empty();
}

//! \brief returns whether the priority map for IGs is empty 
//! \return returns true if the priority map is empty, false otherwise 
bool PriorityMap::emptyIG() const {
    return pmIG.empty();
}
