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
#include <cstdlib>

#include "cmdline.h"
#include "InterfaceMarking.h"
#include "Label.h"
#include "verbose.h"

extern gengetopt_args_info args_info;


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
    if ((m < 1)) {
        abort(9, "message queue length must be at least 1");
    }

    message_bound = m;

    status("message queue length set to %d", message_bound);
}


/***************
 * CONSTRUCTOR *
 ***************/

InterfaceMarking::InterfaceMarking() :
    storage() {
}

InterfaceMarking::InterfaceMarking(const InterfaceMarking &other) :
    storage(other.storage) {
}

InterfaceMarking::InterfaceMarking(const InterfaceMarking &other, const Label_ID label, const bool &increase, bool &success) :
    storage(other.storage) {
    assert(success);

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
}


/*************
 * OPERATORS *
 *************/

bool InterfaceMarking::operator< (const InterfaceMarking &other) const {
    return (storage < other.storage);
}

bool InterfaceMarking::operator!= (const InterfaceMarking &other) const {
    return (storage != other.storage);
}

bool InterfaceMarking::operator== (const InterfaceMarking &other) const {
    return (storage == other.storage);
}

std::ostream& operator<< (std::ostream &o, const InterfaceMarking &m) {
    std::queue<Label_ID> temp(m.storage);
    
    o << "<";
    
    while (not temp.empty()) {
        o << Label::id2name[temp.front()];
        temp.pop();
        if (not temp.empty()) {
            o << ",";
        }
    }
    
    return o << ">";;
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
 \return whether the message bound was respected (false means violation)
 */
bool InterfaceMarking::inc(const Label_ID label) {
    storage.push(label);

    return (storage.size() <= message_bound);
}

/*!
 \return whether the result is positive (false means decrement of 0)
 */
bool InterfaceMarking::dec(const Label_ID label) {
    if (not storage.empty() and storage.front() == label) {
        storage.pop();
        return true;
    } else {
        return false;
    }
}

/*!
 \return whether the whole interface is unmarked
 */
bool InterfaceMarking::unmarked() const {
    return storage.empty();
}

/*
 \return whether the interface is marked at the given label
*/
bool InterfaceMarking::marked(const Label_ID label) const {
    return (storage.front() == label);
}


hash_t InterfaceMarking::hash() const {
    hash_t result = 0;

    return result;
}
