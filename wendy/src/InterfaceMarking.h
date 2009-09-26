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


#ifndef _INTERFACEMARKING_H
#define _INTERFACEMARKING_H

#include <ostream>
#include "types.h"


/*!
  \brief an interface marking

  Interface markings are stored as a mapping from an input/output label to
  integer numbers that range from 0 to the message bound. To save space,
  several numbers are stored within a byte when possible.

  The set(), inc(), and dec() operator check whether the stored values stay
  in their bounds and return false if they detect a problem (i.e. violation
  of the message bound or decrement of 0). Once such a problem was detected,
  the whole marking might be invalid and should not be used any more.

  \note The positions of the interface markings are the same as for the
        respective labels. That is, they range from Label::first_receive to
        Label::last_send. Internally, they are stored in the range from 0
        to Label::last_send-1. The operators take care of this, so get(1)
        returns the marking of label 1 which is stored at position 0.

  \note As the values are stored as uint8_t, they are treated like unsigned
        char. Hence, you need to cast the result of get() to unsigned int in
        order to display the marking count in a stream operator. The
        operator<< does this voluntarily.

  \todo see http://c-faq.com/misc/bitsets.html for possible improvements --
        especially using preprocessor macros
*/
class InterfaceMarking {
    public: /* static functions */
        /// initializes the class InterfaceMarking
        static void initialize();

        /// comparison function to use std::sort
        static bool sort_cmp(const InterfaceMarking*, const InterfaceMarking*);

    private: /* static attributes */
        /// the message bound
        static uint8_t message_bound;

        /// the size of the asynchronous interface (send + receive)
        static uint8_t interface_length;

        /// how many bits are needed to store a single message bound
        static uint8_t message_bound_bits;

        /// how many bytes are needed to store an interface marking
        static uint8_t bytes;

        /// how many markings can be stored in one byte
        static uint8_t markings_per_byte;

    public: /* member functions */
        /// constructor
        InterfaceMarking();

        /// copy constructor
        InterfaceMarking(const InterfaceMarking&);

        /// copy constructor with effect on given label
        InterfaceMarking(const InterfaceMarking&, const Label_ID&, const bool&, bool&);

        /// destructor
        ~InterfaceMarking();


        /// comparison operator
        bool operator< (const InterfaceMarking&) const;

        /// comparison operator
        bool operator== (const InterfaceMarking&) const;

        /// comparison operator
        bool operator!= (const InterfaceMarking&) const;

        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const InterfaceMarking&);


        /// increments the value at the given label
        bool inc(const Label_ID&);

        /// decrements the value at the given label
        bool dec(const Label_ID&);

        /// returns whether the whole interface is unmarked
        bool unmarked() const;

        /// returns whether the interface is marked at the given label
        bool marked(const Label_ID&) const;

        /// returns whether message bound is respected
        bool sane() const;

        /// returns whether output messages are pending
        bool pendingOutput() const;

        /// returns the hash value of this marking
        hash_t hash() const;

    private: /* member functions */
        /// returns the marking value for the given label
        uint8_t get(const Label_ID&) const;

    private: /* member attributes */
        /// a byte array to store the interface markings
        uint8_t* storage;
};

#endif
