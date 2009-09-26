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


#ifndef _POSSIBLESENDEVENTS_H
#define _POSSIBLESENDEVENTS_H

#include "types.h"

/*!
 \brief storage for sending events reachable by an inner marking
 */
class PossibleSendEvents {
    public: /* static functions */
        /// initializes the class PossibleSendEvents
        static void initialize();

    private: /* static attributes */
        /// the number of bytes needed
        static uint8_t bytes;

    public: /* member functions */
        /// constructor
        PossibleSendEvents();

        /// constructor with initialization
        PossibleSendEvents(const bool&, const Label_ID&);

        /// destructor; also cleans decodedLabels
        ~PossibleSendEvents();

        /// overloaded bit-wise AND operator
        void operator&=(const PossibleSendEvents&);

        /// overloaded bit-wise OR operator
        void operator|=(const PossibleSendEvents&);

        /// copies storage
        void copy(const PossibleSendEvents&);

        /// returns array of all sending events (possible or not)
        char* decode();

    private:
        /// set a label to be possible
        void labelPossible(const Label_ID&);

    private: /* member attributes */
        /// a byte array to store the possible sending events
        uint8_t* storage;

        /// array of all sending events (possible or not); built by decode()
        char* decodedLabels;
};

#endif
