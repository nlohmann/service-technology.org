/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

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

PossibleSendEvents::PossibleSendEvents() : decodedLabels(NULL) {
    assert(bytes > 0);

    // reserve memory
    storage = new uint8_t[bytes];
    for (size_t i = 0; i < bytes; ++i) {
        // initially, all sending events are not reachable
        storage[i] = 0;
    }
}

PossibleSendEvents::PossibleSendEvents(const PossibleSendEvents &other) : decodedLabels(NULL) {
    assert(bytes > 0);

    // reserve memory and copy values
    storage = new uint8_t[bytes];
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] = other.storage[i];
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


/******************
 * MEMBER METHODS *
 ******************/

void PossibleSendEvents::labelPossible(Label_ID l) {
    assert(storage != NULL);

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
