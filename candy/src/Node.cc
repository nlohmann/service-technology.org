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
    id(_id),
    final(false) {
}


//! \brief destructor
Node::~Node() {

    successors.clear();
    efficientSuccessors.clear();
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
unsigned int Node::computeEfficientSuccessors() {

    // if this node is a final node then we dont have to consider any leaving edge
    cout << "DEBUG computing costs for node " << getID() << endl;
    if ( final ) {
        cout << "      node " << getID() << " is final, cost is 0" << endl;
        return 0;
    }


    // first we have to compute the cost for all successors
    list< pair< pair<Node*, Event*>, unsigned int> > totalCost;
    for ( map< Node*, Event* >::const_iterator iter = successors.begin();
          iter != successors.end(); ++iter) {

    	Node* successor = iter->first;
		if (successor == this) {
			// this should never happens as cost are defined for acyclic OGs
			cout << "Cannot compute cost since the given OG is not acyclic\n\n" << endl;
			return EXIT_FAILURE;
		}

		unsigned int successorCost = (iter->second)->cost + successor->computeEfficientSuccessors();
		totalCost.push_back( pair<pair<Node*, Event*>, unsigned int>(*iter, successorCost) );
		cout << "      node " << getID() << " has successor with cost " << successorCost << endl;
    }


    // second we compute all cost minimal assignments for this node as well as
    // the minimal cost
    list<FormulaAssignment> minimalAssignments;
    unsigned int minimalCost = getCostMinimalAssignments(totalCost, minimalAssignments);
    cout << "      node " << getID() << " has minimalAssignment with cost " << minimalCost << endl;


    /*
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
    */


    return minimalCost;
}


//! \brief computes a list of all cost minimal assignments for this node and the minimal cost
//! \param labelCost a list with all edge labels and all edge cost
//! \param minimalAssignments list of all found minimal assignments
//! \return returns the minimal cost
unsigned int Node::getCostMinimalAssignments(
        list< pair< pair<Node*, Event*>, unsigned int> > totalCost,
        list< FormulaAssignment >& minimalAssignments) {

    // temporary variables for recursion
    FormulaAssignment possibleAssignment = FormulaAssignment();
    unsigned int minimalCost = UINT_MAX;

    // the literals "true" and "final" are always set to true in every assignment
    possibleAssignment.setToTrue("true");
    possibleAssignment.setToTrue("final");

    // compute minimal assignments with recursion
    getCostMinimalAssignmentsRecursively(totalCost, 0, possibleAssignment, minimalCost, minimalAssignments);
    return minimalCost;
}


//! \brief recursive helper function for getCostMinimalAssignments
//! \param labelCost a list with all edge labels and all edge cost
//! \param currentCost cost of current assignment
//! \param currentAssignment current assignment
//! \param minimalCost cost of found minimal assignment(s)
//! \param minimalAssignments list of all found minimal assignments
void Node::getCostMinimalAssignmentsRecursively(
        list< pair<pair<Node*, Event*>, unsigned int> > totalCost, // edge label and edge cost
        unsigned int currentCost,
        FormulaAssignment currentAssignment,
        unsigned int& minimalCost,
        list< FormulaAssignment >& minimalAssignments) {

    // check if there are still labels to assign and if there is still the
    // chance of finding an assignment with lesser cost than the found minimal
    // assignment(s)
    if (totalCost.empty() || currentCost > minimalCost ) {
        return;
    }

    // get current label and current label's cost
    // the list labelCost is handled as queue
    pair<Node*, Event*> currentSuccessor = totalCost.front().first;
    string currentLabel = (currentSuccessor.second)->name;
    unsigned int currentLabelCost = totalCost.front().second;
    totalCost.pop_front();

    // if this was the last label ...
    if (totalCost.empty()) {

        // set it to False ...
        currentAssignment.setToFalse(currentLabel);
        if ( formula->satisfies(currentAssignment) ) {

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
            if ( formula->satisfies(currentAssignment) ) {

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
        getCostMinimalAssignmentsRecursively(totalCost, currentCost, currentAssignment, minimalCost, minimalAssignments);

        // set it to True
        currentAssignment.setToTrue(currentLabel);
        getCostMinimalAssignmentsRecursively(totalCost, currentCost + currentLabelCost, currentAssignment, minimalCost, minimalAssignments);
    }

    // reinsert the label afterwards
    totalCost.push_front( pair<pair<Node*, Event*>, unsigned int>(currentSuccessor, currentLabelCost));
}


void Node::printToStdout() {

    cout << "node id '" << id << "' with formula '" <<
    (formula != NULL ? formula->asString() : "NULL") << "' and successors" << endl;
    for ( map< Node*, Event* >::const_iterator iter = successors.begin();
          iter != successors.end(); ++iter ) {

        Node* currentNode = iter->first;
        if ( currentNode != NULL ) {
        //if ( true ) {
            cout << "\tnode id '" << currentNode->id << "' with ";
            (iter->second)->printToStdout();
        } else {
            cout << "\tnode NULL" << endl;
        }
    }
}
