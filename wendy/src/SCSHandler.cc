/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include "SCSHandler.h"
#include "Label.h"
#include "StoredKnowledge.h"


/***************************
 * class Edge              *
 ***************************/

/*******************************************
 * CONSTRUCTOR, DESTRUCTOR, INITIALIZATION *
 *******************************************/

/*!
    constructor
*/
Edge::Edge(const StoredKnowledge* _knowledge, const Label_ID & _edge) : knowledge(_knowledge), edge(_edge) {

}




/***************************
 * class SetOfEdgesHandler *
 ***************************/


/*********************
 * STATIC ATTRIBUTES *
 *********************/

unsigned int SCSHandler::countBadSubsystems = 0;


/*******************************************
 * CONSTRUCTOR, DESTRUCTOR, INITIALIZATION *
 *******************************************/

/*!
    constructor
*/
SCSHandler::SCSHandler() : mandatoryEdges(NULL), numberOfAllEdges(0), minNumberOfEdges(0), currentBitMask(0), numberOfMandatoryEdges(0) {

}


/*!
     destructor
 */
SCSHandler::~SCSHandler() {

    // delete all stored edges
    for (unsigned int i = 0; i < numberOfAllEdges; ++i) {
        delete edges[i];
    }

    // delete the array itself
    delete[] edges;

    delete[] mandatoryEdges;
}



/*!
    initializes the set of edges handler by calculating the set of mandatory edges of the currently considered SCS
    (if one of those mandatory edges are left out in a subsystem, the SCS is not connected anymore)
    \param setOfEdges the SCS to be considered
    \param _numberOfAllEdges number of edges contained within the SCS
*/
void SCSHandler::initialize(SetOfEdges & SCS, unsigned int & _numberOfAllEdges) {

    assert(mandatoryEdges == NULL);

    numberOfAllEdges = _numberOfAllEdges;
    numberOfMandatoryEdges = 0;

    // minimal sub set size is the size of knowledges
    minNumberOfEdges = SCS.size();

    // C array of edges needed to generate a subsystem of edges quickly
    edges = new Edge*[numberOfAllEdges];

    unsigned int i = 0;
    unsigned int index = 0;
    Label_ID countEdgesOfKnowledge;
    Label_ID lastEdge;

    std::map<StoredKnowledge* , std::set< unsigned int > > incomingEdges;

    // initialize vector of mandatory edges
    if (not (mandatoryEdges = (unsigned int*) calloc(1, sizeof(unsigned int)))) {
        return ;
    }

    // create C array of the set of edges and detect mandatory edges
    for (SetOfEdges::iterator iter = SCS.begin(); iter != SCS.end(); ++iter) {

        countEdgesOfKnowledge = 0;
        lastEdge = 0;
        index = 0;

        for (std::set<Label_ID>::iterator iterEdges = (*iter).second.begin(); iterEdges != (*iter).second.end(); ++iterEdges) {

            Edge * edgeObject = new Edge((*iter).first, *iterEdges);

            edges[i] = edgeObject;

            index = i;
            incomingEdges[((*iter).first)->successors[*iterEdges]].insert(i);

            ++i;

            ++countEdgesOfKnowledge;
            lastEdge = *iterEdges;
        }

        // knowledge has only one outgoing edge, so we can't skip this edge when calculating a subsystem --> it is a mandatory edge
        if (countEdgesOfKnowledge == 1) {

            ++numberOfMandatoryEdges;
            if (not (mandatoryEdges = (unsigned int*) realloc(mandatoryEdges, numberOfMandatoryEdges * sizeof(unsigned int)))) {
                return ;
            }

            mandatoryEdges[numberOfMandatoryEdges - 1] = 1 << index;
        }
    }

    // iterate over the map of incoming edges, to find more mandatory edges
    for (std::map<StoredKnowledge* , std::set< unsigned int > >::iterator iterIncomingEdges = incomingEdges.begin();
                                                                          iterIncomingEdges != incomingEdges.end();
                                                                          ++iterIncomingEdges) {

        // knowledge has only one incoming edge, so this edge is mandatory as well
        if ((*iterIncomingEdges).second.size() == 1) {

            ++numberOfMandatoryEdges;
            if (not (mandatoryEdges = (unsigned int*) realloc(mandatoryEdges, numberOfMandatoryEdges * sizeof(unsigned int)))) {
                return ;
            }

            mandatoryEdges[numberOfMandatoryEdges - 1] = 1 << (*(*iterIncomingEdges).second.begin());
        }
    }

}




/*********************
 * MEMBER FUNCTIONS  *
 *********************/


/*!
    how many bits are set to true in the given bitmask
    \param n bitmask
*/
unsigned int SCSHandler::bitCount(unsigned int n) {
    return (n==0) ? 0 : (n & 1) + bitCount( n / 2 );
}


/*!
    set the current bitmask to 0, then we can start with the subsystem calculation
*/
void SCSHandler::initializeSubsystemCalculation() {
    currentBitMask = 0;
}


/*!
    \return true, iff the current bitmask contains all mandatory edges; false, otherwise
*/
bool SCSHandler::containsMandatoryEdges() const {

    for (unsigned int i = 0; i < numberOfMandatoryEdges; ++i) {
        if ((currentBitMask | mandatoryEdges[i]) != currentBitMask) {
            ++countBadSubsystems;
            return false;
        }
    }

    return true;
}


/*!
    \return true, if there exists a next subsystem; false, otherwise
*/
bool SCSHandler::nextSubsystem() const {
    return (currentBitMask < (1 << numberOfAllEdges));
}


/*!
    \return returns the next subsystem as a "set of edges"
*/
SetOfEdges SCSHandler::getNextSubsystem() {

    // current bitmask is valid
    assert(currentBitMask < (1 << numberOfAllEdges));

    SetOfEdges subsystem;

    if (bitCount(currentBitMask) < minNumberOfEdges or not containsMandatoryEdges()) {
         ++currentBitMask;
         return subsystem;
    }

    for (unsigned int i = 0; i < numberOfAllEdges; ++i) {
        if ( (currentBitMask & (1 << i)) > 0 ) {
            subsystem[edges[i]->knowledge].insert(edges[i]->edge);
        }
    }

    ++currentBitMask;

    return subsystem;
}


