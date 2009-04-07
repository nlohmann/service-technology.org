// for UINT8_MAX
#define __STDC_LIMIT_MACROS

#include <cmath>
#include <stdint.h>

#include "config.h"
#include "InterfaceMarking.h"


/******************
 * STATIC MEMBERS *
 ******************/

unsigned int InterfaceMarking::message_bound = 0;
unsigned int InterfaceMarking::interface_length = 0;
unsigned int InterfaceMarking::message_bound_bits = 0;
unsigned int InterfaceMarking::bytes = 0;
unsigned int InterfaceMarking::markings_per_byte = 0;

unsigned int InterfaceMarking::memory_count = 0;
unsigned int InterfaceMarking::memory_max = 0;


/******************
 * STATIC METHODS *
 ******************/

unsigned int InterfaceMarking::initialize(unsigned int m) {
    // only a positive message bound makes sense
    if (m < 1) {
        fprintf(stderr, "%s: message bound must be at least 1 -- aborting\n",
            PACKAGE);
        exit(EXIT_FAILURE);
    }
    
    // we use bytes to store the markings, so the message bound must not exceed 255    
    if (m > UINT8_MAX) {
        fprintf(stderr, "%s: message bound must not exceed %d -- aborting\n",
            PACKAGE, UINT8_MAX);
        exit(EXIT_FAILURE);
    }
    
    message_bound = m;
    interface_length = Label::async_events;    
    message_bound_bits = ceil(log2((double)message_bound+1));
    markings_per_byte = 8 / message_bound_bits;
    bytes = ceil((double)interface_length / (double)markings_per_byte);
    
    fprintf(stderr, "%s: message bound set to %d (%d bytes/interface marking, %d bits/event)\n",
        PACKAGE, message_bound, bytes, message_bound_bits);
}


/***************
 * CONSTRUCTOR *
 ***************/

InterfaceMarking::InterfaceMarking() : storage(NULL) {
    // initialize() must be called before first object is created
    assert(interface_length);
    assert(message_bound_bits);
    assert(bytes);
    assert(markings_per_byte);

    ++memory_count;
    memory_max = std::max(memory_count, memory_max);

    // reserve memory and initialize to 0
    storage = new uint8_t[bytes];
    assert(storage);
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] = 0;
    }
}

/*!
  \deprecated I'm not sure whether I need this function
 */
InterfaceMarking::InterfaceMarking(const InterfaceMarking &other) {
    // initialize() must be called before first object is created
    assert(interface_length);
    assert(message_bound_bits);
    assert(bytes);
    assert(markings_per_byte);

    ++memory_count;
    memory_max = std::max(memory_count, memory_max);

    // reserve memory and copy values
    storage = new uint8_t[bytes];
    assert(storage);
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
InterfaceMarking::InterfaceMarking(const InterfaceMarking &other, Label_ID label, bool increase, bool &success) {
    // initialize() must be called before first object is created
    assert(interface_length);
    assert(message_bound_bits);
    assert(bytes);
    assert(markings_per_byte);
    assert(success);

    ++memory_count;
    memory_max = std::max(memory_count, memory_max);

    // reserve memory and copy values
    storage = new uint8_t[bytes];
    assert(storage);
    for (size_t i = 0; i < bytes; ++i) {
        storage[i] = other.storage[i];
    }
    
    if (increase) {
        if (!inc(label)) {
            success = false;
        }
    } else {
        if (!dec(label)) {
            success = false;
        }
    }
}


/**************
 * DESTRUCTOR *
 **************/

InterfaceMarking::~InterfaceMarking() {
    --memory_count;

    delete[] storage;
}


/*************
 * OPERATORS *
 *************/

bool InterfaceMarking::operator< (const InterfaceMarking &other) const {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] >= other.storage[i]) {
            return false;
        }
    }
    
    return true;
}

bool InterfaceMarking::operator!= (const InterfaceMarking &other) const {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] != other.storage[i]) {
            return true;
        }
    }
    
    return false;
}

bool InterfaceMarking::operator== (const InterfaceMarking &other) const {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] != other.storage[i]) {
            return false;
        }
    }
    
    return true;
}

std::ostream& operator<< (std::ostream &o, const InterfaceMarking &m) {
    o << "[";
    for (Label_ID l = 1; l <= m.interface_length; ++l) {
        if (l != 1) {
            o << ",";
        }
        o << (unsigned int)m.get(l);
    }
    return o << "]";;
}


/******************
 * MEMBER METHODS *
 ******************/

uint8_t InterfaceMarking::get(Label_ID label) const {
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
 \deprecated I'm not sure whether I need this function
 */
bool InterfaceMarking::set(Label_ID label, uint8_t &v) {
    assert(label > 0);
    assert(label <= interface_length);

    // the byte in which this interface is stored
    unsigned int byte = (label-1) / markings_per_byte;
    // the offset inside the byte, i.e. the starting position
    unsigned int offset = ((label-1) % markings_per_byte) * message_bound_bits;
    
    // create a mask to select the bits we want to modify:
    // (2**message_bound_bits)-1, then shift it to the needed position
    uint8_t mask = ((1 << message_bound_bits) - 1) << offset;
    
    // shift and mask the value to store
    uint8_t result = ((v << offset) & mask);

    // remove values of this label
    uint8_t crop = (storage[byte] & ~mask);
    
    // combine set value and the stored byte
    storage[byte] = (crop | result);
    
    return (v <= message_bound);
}

/*!
 \return whether the message bound was respected (false means violation)
 */
bool InterfaceMarking::inc(Label_ID label) {
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
    bool OK = (( (storage[byte] & mask) >> offset ) < message_bound );

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
bool InterfaceMarking::dec(Label_ID label) {
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
 \return whether this is an empty interface marking
 \deprecated I'm not sure whether I need this function
 */
bool InterfaceMarking::empty() const {
    for (size_t i = 0; i < bytes; ++i) {
        if (storage[i] != 0) {
            return false;
        }
    }
    
    return true;
}

unsigned int InterfaceMarking::hash() const {
    unsigned int result = 0;
    
    for (unsigned int i = 0; i < bytes; ++i) {
        result += (storage[i] << (7*i));
    }
    
    return result;
}
