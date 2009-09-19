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


// for UINT8_MAX
#define __STDC_LIMIT_MACROS

#include "config.h"
#include <cassert>

#include <cmath>
#include <cstdlib>
#include "cmdline.h"
#include "InterfaceMarking.h"
#include "Label.h"
#include "verbose.h"

extern gengetopt_args_info args_info;


/*! \def LOG2(i)
    \brief base 2 logarithm as lookup */
#define LOG2(i) ((i <   2) ? 1 : \
                ((i <   4) ? 2 : \
                ((i <   8) ? 3 : \
                ((i <  16) ? 4 : \
                ((i <  32) ? 5 : \
                ((i <  64) ? 6 : \
                ((i < 128) ? 7 : 8 )))))))


/******************
 * STATIC MEMBERS *
 ******************/

unsigned int InterfaceMarking::message_bound = 0;
unsigned int InterfaceMarking::interface_length = 0;
unsigned int InterfaceMarking::message_bound_bits = 0;
unsigned int InterfaceMarking::bytes = 0;
unsigned int InterfaceMarking::markings_per_byte = 0;


/******************
 * STATIC METHODS *
 ******************/

void InterfaceMarking::initialize(unsigned int m) {
    // check the message bound
    if ((m < 1) or (m > UINT8_MAX)) {
        abort(9, "message bound must be between 1 and %d", UINT8_MAX);
    }

    message_bound = m;
    interface_length = Label::send_events + Label::receive_events;
    message_bound_bits = LOG2(message_bound);

    // allow to store a larger interface marking to display diagnosis results
    if (args_info.diagnose_given) {
        ++message_bound_bits;
    }

    markings_per_byte = 8 / message_bound_bits;
    bytes = (unsigned int)(ceil((double)interface_length / (double)markings_per_byte));

    status("message bound set to %d (%d bytes/interface marking, %d bits/event)",
        message_bound, bytes, message_bound_bits);
}


/*!
 This function is necessary to sort vectors or sets of pointers to
 InterfaceMarking objects using sort functions from the STL Algorithms. The
 result must be any ordering.
 */
bool InterfaceMarking::sort_cmp(const InterfaceMarking* a, const InterfaceMarking* b) {
    assert(a);
    assert(b);

    return (*a < *b);
}



/***************
 * CONSTRUCTOR *
 ***************/

InterfaceMarking::InterfaceMarking() {
    // if no (asynchronous) events are present, we are done here
    if (interface_length == 0) {
        return;
    }

    // initialize() must be called before first object is created
    assert(interface_length);
    assert(message_bound_bits);
    assert(bytes);
    assert(markings_per_byte);

    // reserve memory and initialize to 0
    storage = new uint8_t[bytes];
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] = 0;
    }
}

InterfaceMarking::InterfaceMarking(const InterfaceMarking& other) {
    // if no (asynchronous) events are present, we are done here
    if (interface_length == 0) {
        return;
    }

    // initialize() must be called before first object is created
    assert(interface_length);
    assert(message_bound_bits);
    assert(bytes);
    assert(markings_per_byte);

    // reserve memory and copy values
    storage = new uint8_t[bytes];
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] = other.storage[i];
    }
}


/*!
  This is a special copy constructor that not only makes a copy from a given
  object, but als performs a manipulation operation such as inc() or dec().
  As the result cannot be returned, a bidrectional parameter "success" is
  used to inform the called.

  \param[in] other        the interface marking to copy from

  \param[in] label        the label on which the manipulation should be
                          executed

  \param[in] increase     whether to call inc(label): when set to false,
                          dec(label) is called

  \param[in,out] success  a Boolean that must be true as input and that is
                          changed according to the success of the executed
                          manipulation: true means successful increment or
                          decrement, false means unsuccessful increment
                          (message bound violation) or decrement (no message
                          present) 
 */
InterfaceMarking::InterfaceMarking(const InterfaceMarking& other, const Label_ID& label, const bool& increase, bool& success) {
    // initialize() must be called before first object is created
    assert(interface_length);
    assert(message_bound_bits);
    assert(bytes);
    assert(markings_per_byte);
    assert(success);

    // reserve memory and copy values
    storage = new uint8_t[bytes];
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] = other.storage[i];
    }

    if (increase) {
        if (not inc(label)) {
            success = false;
        }
    } else {
        if (not dec(label)) {
            success = false;
        }
    }
}


/**************
 * DESTRUCTOR *
 **************/

InterfaceMarking::~InterfaceMarking() {
    if (interface_length > 0) {
        delete[] storage;
    }
}


/*************
 * OPERATORS *
 *************/

bool InterfaceMarking::operator< (const InterfaceMarking& other) const {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] < other.storage[i]) {
            return true;
        }
    }

    return false;
}

bool InterfaceMarking::operator!= (const InterfaceMarking& other) const {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] != other.storage[i]) {
            return true;
        }
    }

    return false;
}

bool InterfaceMarking::operator== (const InterfaceMarking& other) const {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] != other.storage[i]) {
            return false;
        }
    }

    return true;
}

std::ostream& operator<< (std::ostream& o, const InterfaceMarking& m) {
    o << "[";
    for (Label_ID l = 1; l <= m.interface_length; ++l) {
        if (l != 1) {
            o << ",";
        }
        o << static_cast<unsigned int>(m.get(l));
    }
    return o << "]";;
}


/******************
 * MEMBER METHODS *
 ******************/

uint8_t InterfaceMarking::get(const Label_ID& label) const {
    assert(label > 0);
    assert(label <= interface_length);

    // the byte in which this interface is stored
    unsigned int byte = (label-1) / markings_per_byte;
    // the offset inside the byte, i.e. the starting position
    unsigned int offset = ((label-1) % markings_per_byte) * message_bound_bits;

    // first, create (2**message_bound_bits)-1, then shift it to the needed position
    uint8_t mask = ((1 << message_bound_bits) - 1) << offset;

    // get the result by masking the respective byte
    uint8_t result = storage[byte] & mask;

    // shift back the result
    return (result >> offset);
}

/*!
 \return whether the message bound was respected (false means violation)
 */
bool InterfaceMarking::inc(const Label_ID& label) {
    assert(label > 0);
    assert(label <= interface_length);

    // the byte in which this interface is stored
    unsigned int byte = (label-1) / markings_per_byte;
    // the offset inside the byte, i.e. the starting position
    unsigned int offset = ((label-1) % markings_per_byte) * message_bound_bits;

    // create a mask to select the bits we want to modify:
    // (2**message_bound_bits)-1, then shift it to the needed position
    uint8_t mask = ((1 << message_bound_bits) - 1) << offset;

    // before increment, the value for this label should be smaller than the message bound
    bool OK = ( (unsigned)( (storage[byte] & mask) >> offset) < message_bound );

    // use the mask to get the current value from the byte,
    // then shift and increment the value
    uint8_t update = ((storage[byte] & mask) >> offset) + 1;

    // shift back and mask the value to store
    uint8_t result = ((update << offset) & mask);

    // remove values of this label
    uint8_t crop = (storage[byte] & ~mask);

    // combine incremented value and the stored byte
    storage[byte] = (crop | result);

    // return previously checked status
    return OK;
}

/*!
 \return whether the result is positive (false means decrement of 0)
 */
bool InterfaceMarking::dec(const Label_ID& label) {
    assert(label > 0);
    assert(label <= interface_length);

    // the byte in which this interface is stored
    unsigned int byte = (label-1) / markings_per_byte;
    // the offset inside the byte, i.e. the starting position
    unsigned int offset = ((label-1) % markings_per_byte) * message_bound_bits;

    // create a mask to select the bits we want to modify:
    // (2**message_bound_bits)-1, then shift it to the needed position
    uint8_t mask = ((1 << message_bound_bits) - 1) << offset;

    // before decrement, the value for this label should be positive
    bool OK = (( (storage[byte] & mask) >> offset ) > 0 );

    // use the mask to get the current value from the byte,
    // then shift and increment the value
    uint8_t update = ((storage[byte] & mask) >> offset) - 1;

    // shift back and mask the value to store
    uint8_t result = ((update << offset) & mask);

    // remove values of this label
    uint8_t crop = (storage[byte] & ~mask);

    // combine incremented value and the stored byte
    storage[byte] = (crop | result);

    // return previously checked status
    return OK;
}

/*!
 \return whether the whole interface is unmarked
 */
bool InterfaceMarking::unmarked() const {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] != 0) {
            return false;
        }
    }

    return true;
}

/*
 \return whether the interface is marked at the given label
*/
bool InterfaceMarking::marked(const Label_ID& label) const {
    assert(label > 0);
    assert(label <= interface_length);

    // the byte in which this interface is stored
    unsigned int byte = (label-1) / markings_per_byte;
    // the offset inside the byte, i.e. the starting position
    unsigned int offset = ((label-1) % markings_per_byte) * message_bound_bits;

    // first, create (2**message_bound_bits)-1, then shift it to the needed position
    uint8_t mask = ((1 << message_bound_bits) - 1) << offset;

    // get the result by masking the respective byte
    uint8_t result = storage[byte] & mask;

    // shift back the result
    return ((result >> offset) > 0);
}


bool InterfaceMarking::sane() const {
    for (Label_ID l = 1; l <= interface_length; ++l) {
        if (get(l) > message_bound) {
            return false;
        }
    }
    return true;
}

bool InterfaceMarking::pendingOutput() const {
    for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
        if (marked(l)) {
            return true;
        }
    }
    return false;
}

hash_t InterfaceMarking::hash() const {
    hash_t result = 0;

    for (unsigned int i = 0; i < bytes; ++i) {
        result += (storage[i] << (7*i));
    }

    return result;
}
