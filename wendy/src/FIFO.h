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

#include <queue>
#include <map>
#include <ostream>
#include "types.h"


class InterfaceMarking {
    public: /* static functions */

        /// initializes the class InterfaceMarking
        static void initialize(unsigned int);

    private: /* static attributes */

        /// the message bound
        static unsigned int message_bound;

        /// the size of the asynchronous interface (send + receive)
        static unsigned int interface_length;

        /// how many bits are needed to store a single message bound
        static unsigned int message_bound_bits;

        /// how many bytes are needed to store an interface marking
        static unsigned int bytes;

        /// how many markings can be stored in one byte
        static unsigned int markings_per_byte;

    public: /* member functions */

        /// constructor
        InterfaceMarking();

        /// copy constructor
        InterfaceMarking(const InterfaceMarking&);

        /// copy constructor with effect on given label
        InterfaceMarking(const InterfaceMarking&, const Label_ID, const bool&, bool&);

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
        bool inc(const Label_ID);

        /// decrements the value at the given label
        bool dec(const Label_ID);

        /// returns whether the whole interface is unmarked
        bool unmarked() const;

        /// returns whether the interface is marked at the given label
        bool marked(const Label_ID) const;

        /// returns the hash value of this marking
        hash_t hash() const;

    private: /* member attributes */

        /// the InterfaceMarking queue
        std::queue<Label_ID> storage;
};

#endif
