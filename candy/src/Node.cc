#include "settings.h"
#include "Node.h"
#include "verbose.h"



// set flag for this node and all successor nodes
void Node::setFlagRecursively(bool _flag) {

    flag = _flag;
    for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
          i != successors.end(); ++i ) {

        if ( (i->first)->flag != _flag ) {

            (i->first)->setFlagRecursively(_flag);
        }
    }
}


//! \brief computes the node's minimal cost and cut connections to inefficient successor nodes
//! \return returns the minimal cost of this node
unsigned int Node::computeCost() {

    // return computed cost if we handled this node before
    debug("computing cost for node %d", getID());
    if ( computedCost ) {
    	debug("node %d, %d already computed, cost is %d", getID(), this, cost);
        return cost;
    }

    // if this node has ID 0 we dont have to consider leaving edges
    // reason: per definition from the tool wendy is ID 0 the empty node
    if ( getID() == 0 ) {
    	debug("node %d, %d is the empty node, cost is 0", getID(), this);
        computedCost = true;
        cost = 0;
        return 0;
    }

    // if this node is a final node then we dont have to consider any leaving edge
    // reason: per definition has final node in an OG no successors
    if ( final ) {
		debug("node %d, %d is a final node, cost is 0", getID(), this);
        computedCost = true;
        cost = 0;
        return 0;
    }

    // first we have to compute the cost for all outgoing edges
    // this is critical as cost are defined for acyclic OGs only!!!
    // this is written for deterministic graphs only!!!
    list< pair<Event*, unsigned int> > edgeCost;
    debug("node %d, %d has %d successors", getID(), this, successors.size());
    for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
          i != successors.end(); ++i ) {

    	Node* successor = i->first;
        assert(successor != NULL);

        // forget about the empty node
		if ( successor->getID() != 0 ) {

            unsigned int successorCost = successor->computeCost();
            debug("node %d, %d has successor with cost %d", getID(), this, successorCost);
            for ( list<Event*>::const_iterator j = i->second.begin(); j != i->second.end(); ++j ) {

                unsigned int currentEdgeCost = (*j)->getCost() + successorCost;
                edgeCost.push_back( pair<Event*, unsigned int>(*j, currentEdgeCost) );
                debug("node %d, %d has edge with cost %d", getID(), this, currentEdgeCost);
            }
		}
    }


    // second we compute all cost minimal assignments for this node as well as
    // the minimal cost
    list<FormulaAssignment> minimalAssignments;
    unsigned int minimalCost = 0;
    if ( negation ) {
        // run-time complexity O(2^n)
        minimalCost = getCostMinimalAssignments(edgeCost, minimalAssignments);
        debug("node %d, %d has formula with negation", getID(), this);
    } else {
        // run-time complexity O(n^2)
        minimalCost = getCostMinimalAssignmentsWithoutNegation(edgeCost, minimalAssignments);
		debug("node %d, %d has formula without negation", getID(), this);
    }
    debug("node %d, %d has minimal assignment with cost %d", getID(), this, minimalCost);


    // finally we cut the connection to all inefficient successor nodes
    for ( map< Node*, list<Event*> >::iterator i = successors.begin();
          i != successors.end(); /*inside*/ ) {

        Node* currentSuccessor = i->first;
        assert(currentSuccessor != NULL);

        // forget about the empty node
		if ( currentSuccessor->getID() != 0 ) {

            debug("node %d, %d is checking successor %d", getID(), this, currentSuccessor->getID());
            for ( list<Event*>::iterator j = i->second.begin(); j != i->second.end(); /*inside*/ ) {

                // an edge is inefficient iff it's edge label is assigned with
                // False in all minimal assignments
                bool inefficient = true;
                for ( list<FormulaAssignment>::const_iterator a = minimalAssignments.begin();
                      inefficient && a != minimalAssignments.end(); ++a ) {

                    inefficient = not a->get( (*j)->getName() );
                }

                if ( inefficient ) {

                    // remove unnecessary literal
                    formula->removeLiteral( (*j)->getName() );
                    debug("node %d, %d has lost edge %s", getID(), this, (*j)->getName().c_str() );

                    // the erase returns incremented iterator, which is invalid for last element
                    i->second.erase( j++ );

                } else {
                    ++j;
                }
            }

            // check if there is a valid formula
            // this is possible, e.g. with a formula ~a
            if ( formula->size() == 0 ) {
                delete formula;
                formula = new FormulaTrue();
				debug("node %d, %d has lost formula and is set to TRUE", getID(), this);
            }

            // check if there are edges to currentSuccessor left
            if ( i->second.empty() ) {
                successors.erase( i++ );
				debug("node %d, %d has lost successor %d", getID(), this, currentSuccessor->getID());
            } else {
                ++i;
            }
        } else {
            ++i;
        }
    }

    // simplify this node's formula
    formula->simplify();

    // mark this node as already handled and store computed cost
    computedCost = true;
    cost = minimalCost;

    // return minimal cost
    return minimalCost;
}


//! \brief computes a list of all cost minimal assignments for this node and the minimal cost
//! \param edgeCost a list with all outgoing events and their cost
//! \param minimalAssignments list of all found minimal assignments
//! \return returns the cost of a minimal assignment
unsigned int Node::getCostMinimalAssignments(
        list< pair<Event*, unsigned int> > edgeCost,
        list< FormulaAssignment >& minimalAssignments) {

    // temporary variables for recursion
    FormulaAssignment possibleAssignment = FormulaAssignment();
    unsigned int minimalCost = UINT_MAX;

    // compute minimal assignments with recursion
    getCostMinimalAssignmentsRecursively(edgeCost, 0, possibleAssignment, minimalCost, minimalAssignments);
    return minimalCost;
}


//! \brief recursive helper function for getCostMinimalAssignments
//! \param edgeCost a list with all outgoing events and their cost
//! \param currentCost cost of current assignment
//! \param currentAssignment current assignment
//! \param minimalCost cost of found minimal assignment(s)
//! \param minimalAssignments list of all found minimal assignments
void Node::getCostMinimalAssignmentsRecursively(
        list< pair<Event*, unsigned int> > edgeCost,
        unsigned int currentCost,
        FormulaAssignment currentAssignment,
        unsigned int& minimalCost,
        list< FormulaAssignment >& minimalAssignments) {

    // check if there are still labels to assign and if there is still the
    // chance of finding an assignment with lesser cost than the found minimal
    // assignment(s)
    if (edgeCost.empty() || currentCost > minimalCost ) {
        return;
    }

    // get current label and current label's cost
    // the list labelCost is handled as queue
    Event* currentEvent = edgeCost.front().first;
    string currentLabel = currentEvent->getName();
    unsigned int currentLabelCost = edgeCost.front().second;
    edgeCost.pop_front();

    // if this was the last label ...
    if ( edgeCost.empty() ) {

        // set it to false ...
        currentAssignment.set(currentLabel, false);
        if ( formula->value(currentAssignment) ) {

            if ( currentCost < minimalCost ) {
                // we found a minimal assignment with lesser cost
                minimalCost = currentCost;
                minimalAssignments.clear();
                minimalAssignments.push_back( currentAssignment );
            } else if ( currentCost == minimalCost ) {
                // we found a minimal assignment with equal cost
                minimalAssignments.push_back( currentAssignment );
            }
        }

        // set it to true
        // only for true labels the label's cost are regarded
        currentAssignment.set(currentLabel, true);
        if ( formula->value(currentAssignment) ) {

            unsigned int newCurrentCost = currentCost < currentLabelCost ? currentLabelCost : currentCost;
            if ( newCurrentCost < minimalCost ) {
                // we found a minimal assignment with lesser cost
                minimalCost = newCurrentCost;
                minimalAssignments.clear();
                minimalAssignments.push_back( currentAssignment );
            } else if ( newCurrentCost == minimalCost ) {
                // we found a minimal assignment with equal cost
                minimalAssignments.push_back( currentAssignment );
            }
        }

    // if this is a label inbetween or at the start
    } else {

        // set it to False
        currentAssignment.set(currentLabel, false);
        getCostMinimalAssignmentsRecursively(edgeCost,
                                             currentCost,
                                             currentAssignment, minimalCost, minimalAssignments);

        // set it to True
        currentAssignment.set(currentLabel, true);
        getCostMinimalAssignmentsRecursively(edgeCost,
                                             (currentCost < currentLabelCost ? currentLabelCost : currentCost),
                                             currentAssignment, minimalCost, minimalAssignments);
    }

    // reinsert the label afterwards
    // TODO we dont need this, do we?
    edgeCost.push_front( pair<Event*, unsigned int>(currentEvent, currentLabelCost));
}


//! \brief computes a list of the cost minimal assignment for this node and the minimal cost
//!        we explore the fact that there is no negation inside the formula
//! \param edgeCost a list with all outgoing events and their cost
//! \param minimalAssignments list of all found minimal assignments
//! \return returns the cost of a minimal assignment
unsigned int Node::getCostMinimalAssignmentsWithoutNegation(
        list< pair<Event*, unsigned int> > edgeCost,
        list< FormulaAssignment >& minimalAssignments) {

    assert( not edgeCost.empty() );
    assert( minimalAssignments.empty() );


    // initial assignment is true for all literals
    FormulaAssignment minimalAssignment = FormulaAssignment();
    for ( list< pair<Event*, unsigned int> >::const_iterator i = edgeCost.begin();
            i != edgeCost.end(); ++i ) {

        minimalAssignment.set( (*i).first->getName(), true );
    }
    if ( not formula->value(minimalAssignment) ) {
        // the formula is unsatisfiable, so the cost are maximal to prevent the
        // previous node of choosing an edge to the current node
        return UINT_MAX;
    }


    // try to compute assignment for formula with lesser costs
    bool found = false;
    unsigned int maximalEdgeCost = 0;
    while ( not found and not edgeCost.empty() ) {

        // find maximal edge cost
        maximalEdgeCost = 0;
        for ( list< pair<Event*, unsigned int> >::iterator i = edgeCost.begin(); i != edgeCost.end(); ++i ) {

            maximalEdgeCost = ( (*i).second > maximalEdgeCost ) ? (*i).second : maximalEdgeCost;
        }

        // find edges with maximal cost and remove them from edgeCost
        list< Event* > maximalEdges;
        for ( list< pair<Event*, unsigned int> >::iterator i = edgeCost.begin(); i != edgeCost.end(); /**/ ) {

            if ( (*i).second == maximalEdgeCost ) {

                maximalEdges.push_back( (*i).first );
                edgeCost.erase( i++ );
            } else {

                ++i;
            }
        }
        debug("found %d edges with maximal cost %d", maximalEdges.size(), maximalEdgeCost);

        // set literals corresponding to edges with maximal cost to false
        for ( list< Event* >::const_iterator i = maximalEdges.begin(); i != maximalEdges.end(); ++i ) {

            minimalAssignment.set( (*i)->getName(), false );
        }

        if ( not formula->value( minimalAssignment ) ) {

            // restore original assignment and exit loop
            for ( list< Event* >::const_iterator i = maximalEdges.begin(); i != maximalEdges.end(); ++i ) {

                minimalAssignment.set( (*i)->getName(), true );
            }
            found = true;
        }
    }


    minimalAssignments.push_back( minimalAssignment );
    return maximalEdgeCost;
}


void Node::outputDebug(std::ostream& file) {

    file << "node id '" << id << "', " << this << " with formula '" <<
    (formula != NULL ? formula->asString() : "NULL") << "', final " << final << " and successors" << endl;
    for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
          i != successors.end(); ++i ) {

        Node* currentNode = i->first;
        if ( currentNode != NULL ) {
            for ( list<Event*>::const_iterator j = i->second.begin();
                  j != i->second.end(); ++j ) {

                cout << "\tnode id '" << currentNode->id << "', " << currentNode << " with ";
                (*j)->outputDebug(file);
            }
        } else {
            cout << "\tnode NULL" << endl;
        }
    }
}


void Node::outputDebugRecursively(std::ostream& file, map<Node*, bool>& printed) {

    outputDebug( cout );
    printed[this] = true;

    for ( map< Node*, list<Event*> >::const_iterator iter = successors.begin();
          iter != successors.end(); ++iter ) {

        if ( printed.find( iter->first ) == printed.end() ) {
            (iter->first)->outputDebugRecursively(file, printed);
        }
    }
}


void Node::outputOG(std::ostream& file, map<Node*, bool>& printed, bool isRootNode) {

    assert(formula != NULL);

    if ( getID() != 0 ) {

        printed[this] = true;
        file << "  " << id << " : " << formula->asString() << endl;

        for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
            i != successors.end(); ++i ) {

            Node* currentNode = i->first;
            if ( currentNode != NULL and currentNode->getID() != 0 ) {
                for ( list<Event*>::const_iterator j = i->second.begin();
                    j != i->second.end(); ++j ) {

                    file << "    " << (*j)->getName() << " -> " << currentNode->id << endl;
                }
            }
        }

        // print all successor nodes which were not printed yet
        for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
            i != successors.end(); ++i ) {

            if ( printed.find( i->first ) == printed.end() and getID() != 0 ) {
                // no other node is a root node
                (i->first)->outputOG(file, printed, false);
            }
        }
    }
}


void Node::outputSA(std::ostream& file, map<Node*, bool>& printed, bool isRootNode) {

    assert(formula != NULL);

    if ( getID() != 0 ) {

        printed[this] = true;
        file << "  " << id;

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

        for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
            i != successors.end(); ++i ) {

            Node* currentNode = i->first;
            if ( currentNode != NULL and currentNode->getID() != 0 ) {
                for ( list<Event*>::const_iterator j = i->second.begin();
                    j != i->second.end(); ++j ) {

                    file << "    " << (*j)->getName() << " -> " << currentNode->id << endl;
                }
            }
        }

        // print all successor nodes which were not printed yet
        for ( map< Node*, list<Event*> >::const_iterator i = successors.begin();
            i != successors.end(); ++i ) {

            if ( printed.find( i->first ) == printed.end() and getID() != 0 ) {
                // no other node is a root node
                (i->first)->outputSA(file, printed, false);
            }
        }
    }
}
