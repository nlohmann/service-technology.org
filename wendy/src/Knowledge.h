/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines
 
 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
 Wendy is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Wendy (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/


#ifndef _KNOWLEDGE_H
#define _KNOWLEDGE_H

#include <queue>
#include "FullMarking.h"


/*!
 \brief knowledge (data structure for building knowledges)
 */
class Knowledge {
    public: /* member functions */

        /// construct knowledge from (initial) inner marking
        Knowledge(InnerMarking_ID);

        /// construct knowledge from a given knowledge and a label
        Knowledge(const Knowledge* const, Label_ID);

        /// destructor
        ~Knowledge();

        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const Knowledge&);

    public: /* attributes */

        /// whether this knowledge is sane
        unsigned is_sane : 1;

        /// the number of markings stored in the bubble
        unsigned int size;

        /// primary data structure
        std::map<InnerMarking_ID, std::vector<InterfaceMarking*> > bubble;

    private: /* member functions */

        /// calculate the closure of this knowledge
        void closure();

    private: /* attributes */

        /// a queue of recently added markings that might have successors
        std::queue<FullMarking> todo;
};

#endif
