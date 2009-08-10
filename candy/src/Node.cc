#include "settings.h"
#include "Node.h"

extern gengetopt_args_info args_info;


//! \brief constructor (three parameters)
//! \param _number the number of this node
Node::Node(unsigned int _id) :
    id(_id),
    final(false) {
}


//! \brief destructor
Node::~Node() {

    successors.clear();
}


//! \brief computes a list of inefficient successors for this node and the node's minimal cost
//! \param inefficientSuccessors contains all reached nodes and their inefficient successors
//! \return returns the minimal cost of this node
unsigned int Node::computeEfficientSuccessors() {

    // if this node is a final node then we dont have to consider any leaving edge
    if (args_info.debug_flag) cout << "DEBUG computing costs for node " << getID() << endl;
    if (args_info.debug_flag) printToStdout();
    if ( final ) {
    	if (args_info.debug_flag) cout << "      node '" << getID() << "', " << this << " is final, cost is 0" << endl;
        return 0;
    }


    // first we have to compute the cost for all successors
    list< pair< pair<Node*, Event*>, unsigned int> > totalCost;
    if (args_info.debug_flag) cout << "      node " << getID() << " has " << successors.size() << " successors" << endl;
    for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
          i != successors.end(); ++i) {

    	Node* successor = i->first;
		if (successor == this) {
			// this should never happens as cost are defined for acyclic OGs
			if (args_info.debug_flag) cout << "Cannot compute cost since the given OG is not acyclic\n\n" << endl;
			return EXIT_FAILURE;
		}

        Event* maxEvent = i->second.front();
        unsigned int maxEventCost = maxEvent->cost;
        for ( list<Event*>::const_iterator j = i->second.begin();
              j != i->second.end(); ++j ) {

            if ( maxEventCost < (*j)->cost ) {
                maxEvent = *j;
                maxEventCost = (*j)->cost;
            }
        }
        assert( maxEvent != NULL );

        if (args_info.debug_flag) cout << "bla" << endl;
        assert(successor != NULL);
		unsigned int successorCost = maxEventCost + successor->computeEfficientSuccessors();
		totalCost.push_back( pair< pair<Node*, Event*>, unsigned int >( pair<Node*, Event*>(successor, maxEvent) , successorCost) );
		if (args_info.debug_flag) cout << "      node " << getID() << " has successor with cost " << successorCost << endl;
    }


    // second we compute all cost minimal assignments for this node as well as
    // the minimal cost
    list<FormulaAssignment> minimalAssignments;
    unsigned int minimalCost = getCostMinimalAssignments(totalCost, minimalAssignments);
    if (args_info.debug_flag) cout << "      node " << getID() << " has minimalAssignment with cost " << minimalCost << endl;


    // finally we cut the connection to all inefficient successor nodes
    for ( map< Node*, list<Event*> >::iterator i = successors.begin();
          i != successors.end(); ) { //increment inside because of erase

        Node* currentSuccessor = i->first;
        assert(currentSuccessor != NULL);
        if (args_info.debug_flag) cout << "      node " << getID() << " is checking successor " << currentSuccessor->getID() << endl;

        Event* maxEvent = i->second.front();
        assert(maxEvent != NULL);
        unsigned int maxEventCost = maxEvent->cost;
        for ( list<Event*>::const_iterator j = i->second.begin();
              j != i->second.end(); ++j ) {

            if ( maxEventCost < (*j)->cost ) {
                maxEvent = *j;
                maxEventCost = (*j)->cost;
            }
        }
        assert( maxEvent != NULL );

        // a successor is inefficient iff it's edge label is assigned with
        // False in all minimal assignments
        bool inefficient = true;
        for (list<FormulaAssignment>::const_iterator j = minimalAssignments.begin();
             inefficient && j != minimalAssignments.end(); ++j ) {

            inefficient = not j->get( maxEvent->name );
        }

        if ( inefficient ) {

            successors.erase( i++ ); // erase returns incremented iterator, which is invalid for last element
            formula->removeLiteralForReal( maxEvent->name );
            if (args_info.debug_flag) cout << "      node " << getID() << " has lost successor " << currentSuccessor->getID() << endl;
        } else {
        	++i;
        }
    }


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

    cout << "node id '" << id << "', " << this << " with formula '" <<
    (formula != NULL ? formula->asString() : "NULL") << "', final " << final << " and successors" << endl;
    for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
          i != successors.end(); ++i ) {

        Node* currentNode = i->first;
        if ( currentNode != NULL ) {
            for ( list<Event*>::const_iterator j = i->second.begin();
                  j != i->second.end(); ++j ) {

                cout << "\tnode id '" << currentNode->id << "', " << currentNode << " with ";
                (*j)->printToStdout();
            }
        } else {
            cout << "\tnode NULL" << endl;
        }
    }
}


void Node::printToStdoutRecursively() {

    printToStdout();
    for ( map< Node*, list<Event*> >::const_iterator iter = successors.begin();
          iter != successors.end(); ++iter ) {
        (iter->first)->printToStdoutRecursively();
    }
}


// WARNING: fails for cyclic og
void Node::output(std::ostream& file, bool isRootNode) {

    assert(formula != NULL);

    file << "  " << id;
    if ( args_info.automata_given ) {

        if ( isRootNode ) {
            if ( final ) {
                file << " : INITIAL, FINAL" << endl;
            } else {
                file << " : INITIAL" << endl;
            }
        } else {
            if ( final ) {
                file << " : FINAL" << endl;
            } else {
                file << endl;
            }
        }
    } else {

        if ( final ) {
            file << " : FINAL" << endl;
        } else {
            file << " : " << formula->asString() << endl;
        }
    }

    for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
          i != successors.end(); ++i ) {

        Node* currentNode = i->first;
        if ( currentNode != NULL ) {
            for ( list<Event*>::const_iterator j = i->second.begin();
                  j != i->second.end(); ++j ) {

                file << "    " << (*j)->name << " -> " << currentNode->id << endl;
            }
        } else {
            file << "\tnode NULL" << endl;
        }
    }

    for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
          i != successors.end(); ++i ) {
        // no other node is a root node
        (i->first)->output(file, false);
    }
}
