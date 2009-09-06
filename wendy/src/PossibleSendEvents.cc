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


#include "config.h"
#include <cassert>

#include "PossibleSendEvents.h"
#include "InterfaceMarking.h"
#include "Label.h"
#include "verbose.h"



/******************
 * STATIC MEMBERS *
 ******************/

unsigned int PossibleSendEvents::bytes = 0;


/******************
 * STATIC METHODS *
 ******************/

void PossibleSendEvents::initialize() {
    bytes = ((Label::send_events-1) / 8) +1;

    status("send event detection requires %d bytes per inner marking", bytes);
}


/***************
 * CONSTRUCTOR *
 ***************/

/*
  \brief all values are initialized with 0
*/
PossibleSendEvents::PossibleSendEvents() : decodedLabels(NULL) {
    assert(bytes > 0);

    // reserve memory
    storage = new uint8_t[bytes];
    for (size_t i = 0; i < bytes; ++i) {
        // initially, all sending events are not reachable
        storage[i] = 0;
    }
}

/*
  \param allValues all sending events are to be initialized with label (0 or 1)
  \param label in case allValues is set to true, then it has to be 0 or 1;
                 otherwise if allValues is set to false, label represents the
                 label that is to be set to one, all others are initialized
                 with 0
 */
PossibleSendEvents::PossibleSendEvents(bool allValues, Label_ID label) : decodedLabels(NULL) {
    assert(bytes > 0);
    assert((allValues and label <= 1) or (not allValues and SENDING(label)));

    // reserve memory
    storage = new uint8_t[bytes];
    for (size_t i = 0; i < bytes; ++i) {
        // if allValues is set, initially all sending events are reachable
        // otherwise they are set to 1
        storage[i] = allValues ? 255 : 0;
    }

    // set one particular label to 1
    if (not allValues) {
        labelPossible(label);
    }
}

/**************
 * DESTRUCTOR *
 **************/

/*!
 \note decodedLabels is deleted, but there might be still pointers pointing
       at this memory; don't try to delete these pointers (double free)!
*/
PossibleSendEvents::~PossibleSendEvents() {
    delete[] storage;
    delete[] decodedLabels;
}


/*************
 * OPERATORS *
 *************/

void PossibleSendEvents::operator&=(const PossibleSendEvents &other) {
    assert(bytes > 0);

    for (size_t i = 0; i < bytes; ++i) {
        storage[i] &= other.storage[i];
    }
}

void PossibleSendEvents::operator|=(const PossibleSendEvents &other) {
    assert(bytes > 0);

    for (size_t i = 0; i < bytes; ++i) {
        storage[i] |= other.storage[i];
    }
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
  the values of other are copied into current storage
  \param other the values of the other storage to be copied into the current one
*/
void PossibleSendEvents::copy(const PossibleSendEvents &other) {
    assert(bytes > 0);

    // reserve memory and copy values
    storage = new uint8_t[bytes];
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] = other.storage[i];
    }
}

void PossibleSendEvents::labelPossible(Label_ID l) {
    assert(storage != NULL);
    assert(SENDING(l));

    unsigned int myByte = (l - Label::first_send) / 8;
    unsigned int myBit  = (l - Label::first_send) % 8;

    storage[myByte] += (1 << myBit);
}

char * PossibleSendEvents::decode() {
    assert(bytes > 0);

    // only reserve memory on first decoding
    if (decodedLabels == NULL) {
        decodedLabels = new char[Label::send_events];
    }

    // decode and store values
    for (Label_ID l = 0; l < Label::send_events; ++l) {
        unsigned int myByte = l / 8;
        unsigned int myBit  = l % 8;

        decodedLabels[l] = (storage[myByte] & (1 << myBit)) >> myBit;
    }

    return decodedLabels;
}
