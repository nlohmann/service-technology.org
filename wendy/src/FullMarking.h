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


#ifndef _FULLMARKING_H
#define _FULLMARKING_H

#include "InnerMarking.h"
#include "InterfaceMarking.h"

/*!
 \brief full marking (a pair of an inner and an interface marking)
 */
class FullMarking {
    public: /* member functions */

        /// constructor
        FullMarking(InnerMarking_ID);

        /// constructor
        FullMarking(InnerMarking_ID, InterfaceMarking);

        /// comparison operator
        bool operator< (const FullMarking &other) const;

        /// comparison operator
        bool operator!= (const FullMarking &other) const;

        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const FullMarking&);

    public: /* member attributes */

        /// an inner marking
        InnerMarking_ID inner;

        /// an interface marking
        InterfaceMarking interface;
};

#endif
