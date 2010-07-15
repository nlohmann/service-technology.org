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


#include <config.h>
#include <cstdlib>

#include "PossibleSendEvents.h"
#include "InterfaceMarking.h"
#include "Label.h"
#include "verbose.h"


/******************
 * STATIC MEMBERS *
 ******************/

uint8_t PossibleSendEvents::bytes = 0;


/******************
 * STATIC METHODS *
 ******************/

void PossibleSendEvents::initialize() {
    bytes = ((Label::send_events - 1) / 8) + 1;
    status("send event detection requires %d bytes per inner marking", bytes);
}


/***************
 * CONSTRUCTOR *
 ***************/

/*
  \brief all values are initialized with 0
  \note we assume sizeof(uint8_t) == 1
*/
PossibleSendEvents::PossibleSendEvents()
    : storage((uint8_t*)calloc(bytes, 1)), decodedLabels(NULL) {
    assert(bytes > 0);
}

/*
  \param allValues all sending events are to be initialized with label (0 or 1)
  \param label in case allValues is set to true, then it has to be 0 or 1;
                 otherwise if allValues is set to false, label represents the
                 label that is to be set to one, all others are initialized
                 with 0
  \note we assume sizeof(uint8_t) == 1
*/
PossibleSendEvents::PossibleSendEvents(const bool& allValues, const Label_ID& l)
    : storage((uint8_t*)malloc(bytes)), decodedLabels(NULL) {
    assert(bytes > 0);
    assert((allValues and l <= 1) or(not allValues and SENDING(l)));

    // if allValues is set, initially all sending events are reachable
    memset(storage, (allValues ? 255 : 0), bytes);

    // set one particular label to 1
    if (not allValues) {
        labelPossible(l);
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
    free(storage);
    delete[] decodedLabels;
}


/*************
 * OPERATORS *
 *************/

void PossibleSendEvents::operator&=(const PossibleSendEvents& other) {
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] &= other.storage[i];
    }
}

void PossibleSendEvents::operator|=(const PossibleSendEvents& other) {
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] |= other.storage[i];
    }
}

bool PossibleSendEvents::operator==(const PossibleSendEvents& other) {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] != other.storage[i]) {
            return false;
        }
    }
    return true;
}

/******************
 * MEMBER METHODS *
 ******************/

void PossibleSendEvents::labelPossible(const Label_ID& l) {
    assert(storage != NULL);
    assert(SENDING(l));

    const uint8_t myByte = (l - Label::first_send) / 8;
    const uint8_t myBit  = (l - Label::first_send) % 8;
    storage[myByte] += (1 << myBit);
}

/*!
 decode the bit array into a char array (function is called more than once)
*/
char* PossibleSendEvents::decode() {
    assert(bytes > 0);

    // only reserve memory and encode on first decoding
    if (decodedLabels == NULL) {
        decodedLabels = new char[Label::send_events];

        // decode and store values
        for (Label_ID l = 0; l < Label::send_events; ++l) {
            const uint8_t myByte = l / 8;
            const uint8_t myBit  = l % 8;
            decodedLabels[l] = (storage[myByte] & (1 << myBit)) >> myBit;
        }
    }

    return decodedLabels;
}

/*!
 reduction rule: smart sending event
 set all bits to false (needed when analyzing an SCC of inner markings)
*/
void PossibleSendEvents::setFalse() {
    memset(storage, 0, bytes);
}

