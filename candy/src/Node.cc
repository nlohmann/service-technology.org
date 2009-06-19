#include "Node.h"
#include <iostream>

// if you need assertions, always include these headers in _this_ order
#include "config.h"
#include <cassert>

using std::cout;
using std::endl;


//! \brief constructor (three parameters)
//! \param _number the number of this node
Node::Node(unsigned int _id) :
    id(_id) {
}


//! \brief destructor
Node::~Node() {

    successors.clear();
}


//! \brief set the color of all inefficient reachable nodes to red
//! \param inefficientSuccessors contains all reached nodes and their inefficient successors
void Node::recolorInefficientSuccessors(map< Node*, list<Node*> >& inefficientSuccessors) {

    /*
    map< AnnotatedGraphNode*, list<AnnotatedGraphNode*> >::iterator iter = inefficientSuccessors.find(this);

    if ( iter != inefficientSuccessors.end() ) {

        list<AnnotatedGraphNode*> succ = iter->second;
        inefficientSuccessors.erase(iter);

        for (list<AnnotatedGraphNode*>::iterator i = succ.begin(); i != succ.end(); ++i) {

            // current successor
            AnnotatedGraphNode* node = *i;

            // all non-efficient nodes are marked RED
            node->setColor(RED);
            //cerr << "      node " << node->getNumber() << " was marked RED" << endl;
        }

        removeUnneededLiteralsFromAnnotation();
    }

    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();
        AnnotatedGraphNode* successor = edge->getDstNode();

        // we skip the empty node and non-blue nodes
        if ( successor->getAnnotation()->equals() != TRUE && successor->getColor() == BLUE ) {

            if (successor == this) {
                // this should never happens as cost is defined for acyclic OGs
                trace("Cannot compute cost since the given OG is not acyclic\n\n");
                assert(false);
            }

            successor->recolorInefficientSuccessors(inefficientSuccessors);
        }
    }
    delete edgeIter;
    */
}


//! \brief computes a list of inefficient successors for this node and the node's minimal cost
//! \param inefficientSuccessors contains all reached nodes and their inefficient successors
//! \return returns the minimal cost of this node
unsigned int Node::computeInefficientSuccessors(map< Node*, list<Node*> >& inefficientSuccessors) {


    /*
    // if this node is a final node then we dont have to consider any leaving edge
    //cerr << "DEBUG computing costs for node " << getNumber() << endl;
    if ( isFinal() ) {
        //cerr << "      node " << getNumber() << " is final, cost is 0" << endl;
        return 0;
    }


    // first we have to compute the cost for all leaving edges which lead to
    // non-empty blue nodes (these are the nodes used for partner synthesis)
    list< pair<string, unsigned int> > labels;
    LeavingEdges::ConstIterator edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();
        AnnotatedGraphNode* successor = edge->getDstNode();

        // we skip the empty node and non-blue nodes
        if ( successor->getAnnotation()->equals() != TRUE && successor->getColor() == BLUE ) {

            if (successor == this) {
                // this should never happens as cost are defined for acyclic OGs
                trace("Cannot compute cost since the given OG is not acyclic\n\n");
                assert(false);
            }

            unsigned int successorCost = edge->cost + successor->computeInefficientSuccessors(inefficientSuccessors);
            labels.push_back( make_pair(edge->getLabel(), successorCost) );
            //cerr << "      node " << getNumber() << " has successor with cost " << successorCost << endl;
        }
    }
    delete edgeIter;


    // second we compute all cost minimal assignments for this node as well as
    // the minimal cost
    list<GraphFormulaAssignment> minimalAssignments;
    unsigned int minimalCost = getCostMinimalAssignments(labels, minimalAssignments);
    labels.clear();
    //cerr << "      node " << getNumber() << " has minimalAssignment with cost " << minimalCost << endl;


    // finally we compute a list of all inefficient successor nodes which are
    // reachable through leaving edges from this node
    list<AnnotatedGraphNode*> succ;
    edgeIter = getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();
        AnnotatedGraphNode* successor = edge->getDstNode();

        // we skip the empty node and non-blue nodes
        if ( successor->getAnnotation()->equals() != TRUE && successor->getColor() == BLUE ) {

            // a successor is inefficient iff it's edge label is assigned with
            // False in all minimal assignments
            bool inefficient = true;
            for (list<GraphFormulaAssignment>::iterator i = minimalAssignments.begin();
                 inefficient && i != minimalAssignments.end(); ++i ) {

                inefficient = !i->get( edge->getLabel() );
            }

            if ( inefficient ) {
                succ.push_back(successor);
                //cerr << "      node " << successor->getNumber() << " marked as inefficient from node " << this->getNumber() << endl;
            }
        }
    }
    inefficientSuccessors[this] = succ;
    delete edgeIter;


    return minimalCost;
    */
    return 0;
}


//! \brief computes a list of all cost minimal assignments for this node and the minimal cost
//! \param labelCost a list with all edge labels and all edge cost
//! \param minimalAssignments list of all found minimal assignments
//! \return returns the minimal cost
unsigned int Node::getCostMinimalAssignments(
        list< pair<string, unsigned int> > labelCost,
        list< FormulaAssignment >& minimalAssignments) {

    /*
    // temporary variables for recursion
    GraphFormulaAssignment possibleAssignment = GraphFormulaAssignment();
    unsigned int minimalCost = UINT_MAX;

    // the literals "true" and "final" are always set to true in every assignment
    possibleAssignment.setToTrue("true");
    possibleAssignment.setToTrue("final");


    // compute minimal assignments with recursion
    getCostMinimalAssignmentsRecursively(labelCost, 0, possibleAssignment, minimalCost, minimalAssignments);
    return minimalCost;
    */
    return 0;
}


//! \brief recursive helper function for getCostMinimalAssignments
//! \param labelCost a list with all edge labels and all edge cost
//! \param currentCost cost of current assignment
//! \param currentAssignment current assignment
//! \param minimalCost cost of found minimal assignment(s)
//! \param minimalAssignments list of all found minimal assignments
void Node::getCostMinimalAssignmentsRecursively(
        list< pair<string, unsigned int> > labelCost, // edge label and edge cost
        unsigned int currentCost,
        FormulaAssignment currentAssignment,
        unsigned int& minimalCost,
        list< FormulaAssignment >& minimalAssignments) {

    /*
    // check if there are still labels to assign and if there is still the
    // chance of finding an assignment with lesser cost than the found minimal
    // assignment(s)
    if (labelCost.empty() || currentCost > minimalCost ) {
        return;
    }

    // get current label and current label's cost
    // the list labelCost is handled as queue
    string currentLabel = labelCost.front().first;
    unsigned int currentLabelCost = labelCost.front().second;
    labelCost.pop_front();

    // if this was the last label ...
    if (labelCost.empty()) {

        // set it to False ...
        currentAssignment.setToFalse(currentLabel);
        if ( assignmentSatisfiesAnnotation(currentAssignment) ) {

            if ( currentCost < minimalCost ) {
                // we found a minimal assignment with lesser cost
                minimalCost = currentCost;
                minimalAssignments.clear();
                minimalAssignments.push_back( currentAssignment );
            } else if ( currentCost == minimalCost ) {
                // we found a minimal assignment with equal cost
                minimalAssignments.push_back( currentAssignment );
            }

        // and if its not satisfying ...
        } else {

            // set it to True
            // only for true labels the label's cost are added
            currentAssignment.setToTrue(currentLabel);
            if ( assignmentSatisfiesAnnotation(currentAssignment) ) {

                if ( currentCost + currentLabelCost < minimalCost ) {
                    // we found a minimal assignment with lesser cost
                    minimalCost = currentCost + currentLabelCost;
                    minimalAssignments.clear();
                    minimalAssignments.push_back( currentAssignment );
                } else if ( currentCost + currentLabelCost == minimalCost ) {
                    // we found a minimal assignment with equal cost
                    minimalAssignments.push_back( currentAssignment );
                }
            }
        }

    // if this is a label inbetween or at the start
    } else {

        // set it to False
        currentAssignment.setToFalse(currentLabel);
        getCostMinimalAssignmentsRecursively(labelCost, currentCost, currentAssignment, minimalCost, minimalAssignments);

        // set it to True
        currentAssignment.setToTrue(currentLabel);
        getCostMinimalAssignmentsRecursively(labelCost, currentCost + currentLabelCost, currentAssignment, minimalCost, minimalAssignments);
    }

    // reinsert the label afterwards
    labelCost.push_front( make_pair(currentLabel, currentLabelCost));
    */
}


void Node::printToStdout() {

    cout << "node id '" << id << "' with formula '" << formula->asString() << "' and successors" << endl;
    for ( list< pair<string, Node*> >::iterator iter = successors.begin();
          iter != successors.end(); ++iter ) {

        Node* currentNode = iter->second;
        //if ( currentNode != NULL ) {
        if ( true ) {
            cout << "\tnode id '" << currentNode->id << "' with edge label '" << iter->first << "'" << endl;
        } else {
            cout << "\tnode NULL" << endl;
        }
    }
}
