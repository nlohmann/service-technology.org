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
#include "util.h"


/***************************
 * class Edge              *
 ***************************/

/*******************************************
 * CONSTRUCTOR, DESTRUCTOR, INITIALIZATION *
 *******************************************/

/*!
    constructor
*/
Edge::Edge(const StoredKnowledge* _knowledge, const Label_ID& _edge) : knowledge(_knowledge), edge(_edge) {

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
SCSHandler::SCSHandler() : edges(NULL), mandatoryEdges(NULL), numberOfAllEdges(0), minNumberOfEdges(0), currentBitMask(0), numberOfMandatoryEdges(0) {

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

    free(mandatoryEdges);
}



/*!
    Optimization of the algorithm for finding all strongly connected subsystems:
    initializes the set of edges handler by calculating the set of mandatory edges of the currently considered SCS
    (if one of those mandatory edges are left out in a subsystem, the SCS is not connected anymore)
    \param SCS the SCS to be considered
    \param _numberOfAllEdges number of edges contained within the SCS
    \todo  What happens if edgeBitmask runs out of bits? #edges > sizeof(unsigned int)
    \todo  Check if edgeBitmask can reach uint_max (overflow)
    \todo  call error() in case no memory is returned from calloc/realloc
    \todo  Check if adding 1 element to mandatoryEdges can be improved
*/
void SCSHandler::initialize(SetOfEdges& SCS, unsigned int& _numberOfAllEdges) {

    assert(mandatoryEdges == NULL);

    numberOfAllEdges = _numberOfAllEdges;
    numberOfMandatoryEdges = 0;

    // minimal sub set size is the size of knowledges
    minNumberOfEdges = SCS.size();

    // C array of edges needed to generate a subsystem of edges quickly
    edges = new Edge*[numberOfAllEdges];

    unsigned int i = 0;

    // collect all incoming edges of one knowledge
    std::map<StoredKnowledge* , std::set< unsigned int > > incomingEdges;

    // initialize vector of mandatory edges
    if (not(mandatoryEdges = (unsigned int*) calloc(1, sizeof(unsigned int)))) {
        return ;
    }

    // create C array of the set of edges and detect mandatory edges
    FOREACH(iter, SCS) {
        unsigned int edgeBitmask = 0;

        FOREACH(iterEdges, iter->second) {
            Edge* edgeObject = new Edge((*iter).first, *iterEdges);

            edges[i] = edgeObject;

            incomingEdges[((*iter).first)->successors[*iterEdges]].insert(i);

            // collect the incoming edges; each bit of the bitmask points to the edge in the edge array
            // each edge has a unique index, thus we can add the value of each edge
            edgeBitmask += (1 << i);

            ++i;
        }

        // we can't skip the outgoing edges of the current knowledge when calculating a subsystem --> they are mandatory
        ++numberOfMandatoryEdges;
        if (not(mandatoryEdges = (unsigned int*) realloc(mandatoryEdges, numberOfMandatoryEdges * sizeof(unsigned int)))) {
            return ;
        }

        // the bitmask talks about the edges of the edges array, thus we store the index of the mandatory edge in the array
        mandatoryEdges[numberOfMandatoryEdges - 1] = edgeBitmask;
    }

    // iterate over the map of incoming edges, to find more mandatory edges
    FOREACH(iterIncomingEdges, incomingEdges) {
        unsigned int edgeBitmask = 0;

        // knowledge has incoming edges, so all those edges are mandatory as well
        FOREACH(iterIncomingEdge, iterIncomingEdges->second) {
            // collect the incoming edges; each bit of the bitmask points to the edge in the edge array
            // each edge has a unique index, thus we can add the value of each edge
            edgeBitmask += 1 << (*iterIncomingEdge);
        }

        ++numberOfMandatoryEdges;
        if (not(mandatoryEdges = (unsigned int*) realloc(mandatoryEdges, numberOfMandatoryEdges * sizeof(unsigned int)))) {
            return ;
        }

        // the bitmask talks about the edges of the edges array, thus we store the index of the mandatory edge in the array
        mandatoryEdges[numberOfMandatoryEdges - 1] = edgeBitmask;
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
    return (n == 0) ? 0 : (n & 1) + bitCount(n / 2);
}


/*!
    set the current bitmask to 0, then we can start with the subsystem calculation
*/
void SCSHandler::initializeSubsystemCalculation() {
    currentBitMask = 0;
}


/*!
    Optimization of the algorithm for finding all strongly connected subsystems:
    \return true, iff the current bitmask contains all mandatory edges; false, otherwise
*/
bool SCSHandler::containsMandatoryEdges() const {

    for (unsigned int i = 0; i < numberOfMandatoryEdges; ++i) {

        // in the current bitmask no mandatory edge is set
        if ((currentBitMask & mandatoryEdges[i]) == 0) {
            ++countBadSubsystems;
            return false;
        }
    }

    return true;
}


/*!
    \note always call this function to find out whether there exists a next subsystem or not
    \return true, if there exists a next subsystem; false, otherwise
*/
bool SCSHandler::nextSubsystem() const {
    return (currentBitMask < (unsigned int)(1 << numberOfAllEdges));
}


/*!
    \return returns the next subsystem as a "set of edges"
*/
SetOfEdges SCSHandler::getNextSubsystem() {

    // current bitmask is valid
    assert(currentBitMask < (unsigned int)(1 << numberOfAllEdges));

    SetOfEdges subsystem;

    // Optimization of the algorithm for finding all strongly connected subsystems:
    // a subsystem has to have a minimum number of edges and
    // has to contain all mandatory edges
    if (bitCount(currentBitMask) < minNumberOfEdges or not containsMandatoryEdges()) {
        ++currentBitMask;
        return subsystem;
    }

    for (unsigned int i = 0; i < numberOfAllEdges; ++i) {
        if ((currentBitMask & (1 << i)) > 0) {
            subsystem[edges[i]->knowledge].insert(edges[i]->edge);
        }
    }

    // get ready for the next subsystem
    ++currentBitMask;

    return subsystem;
}


