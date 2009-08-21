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


#ifndef _LABEL_H
#define _LABEL_H

#include <map>
#include <string>
#include "types.h"


/*! \def SILENT(L)
    \brief shorthand notation to find out whether a label is tau */
#define SILENT(L) (L == 0)

/*! \def SENDING(L)
    \brief shorthand notation to find out whether a label belongs to a send action */
#define SENDING(L) (L >= Label::first_send and L <= Label::last_send)

/*! \def RECEIVING(L)
    \brief shorthand notation to find out whether a label belongs to a receive action */
#define RECEIVING(L) (L >= Label::first_receive and L <= Label::last_receive)

/*! \def SYNC(L)
    \brief shorthand notation to find out whether a label belongs to a synchronous action */
#define SYNC(L) (L >= Label::first_sync and L <= Label::last_sync)

/*! \def PREFIX(L)
    \brief the prefix of the given label, depending on the communication type */
#define PREFIX(L) (SENDING(L) ? "!" : (RECEIVING(L) ? "?" : "#"))


/*!
 \brief communication labels
 
 Communication labels are described from the point of view of the environment;
 that is, not from the point of view of the net. If the net sends a message,
 the respective communication label for us would be to receive.
 
 - silent action (tau) has label 0
 - asynchronous receive events have labels first_receive (=1) to last_receive
 - asynchronous send events have labels first_send to last_send
 - synchronous events have labels first_sync to last_sync
 */
class Label {
    public: /* static functions */

        /// initialize the interface labels and prepare the necessary mappings
        static void initialize();

    public: /* static attributes */

        /// label of first receive (?) event
        static Label_ID first_receive;
        /// label of last receive (?) event
        static Label_ID last_receive;

        /// label of first send (!) event
        static Label_ID first_send;
        /// label of last send (!) event
        static Label_ID last_send;

        /// label of first synchronous (#) event
        static Label_ID first_sync;
        /// label of last synchronous (#) event
        static Label_ID last_sync;

        /// the number of asynchronous send (!) events
        static Label_ID send_events;
        /// the number of asynchronous receive (?) events
        static Label_ID receive_events;
        /// the number of synchronous (#) events
        static Label_ID sync_events;
        /// the number of all events
        static Label_ID events;

        /// given a communication label, returns a string representation
        static std::map<Label_ID, std::string> id2name;
        /// given a transition name, the communication label
        static std::map<std::string, Label_ID> name2id;
};

#endif
