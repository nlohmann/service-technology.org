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


#include "FullMarking.h"


/***************
 * CONSTRUCTOR *
 ***************/

FullMarking::FullMarking(InnerMarking_ID _inner) :
    inner(_inner), interface() {}

FullMarking::FullMarking(InnerMarking_ID _inner, InterfaceMarking _interface) :
    inner(_inner), interface(_interface) {}


/*************
 * OPERATORS *
 *************/

bool FullMarking::operator< (const FullMarking &other) const {
    // first compare inner markings
    if (inner < other.inner) {
        return true;
    }
    if (inner > other.inner) {
        return false;
    }
    
    // in case the inner markings are equal, compare interface markings
    return (interface < other.interface);
}

bool FullMarking::operator!= (const FullMarking &other) const {
    // first compare inner markings
    if (inner != other.inner) {
        return true;
    }

    // in case the inner markings are equal, compare interface markings
    return (interface != other.interface);
}

std::ostream& operator<< (std::ostream &o, const FullMarking &m) {
    return o << "[m" << static_cast<unsigned int>(m.inner) << ", " << m.interface << "]";
}
