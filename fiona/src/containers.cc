/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    containers.cc
 *
 * \brief   functions for containers used at different locations
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include "containers.h"
#include "AnnotatedGraphNode.h"

//! \brief 'left' is less than 'right', if the reachGraphStateSet of 'left' is
//!        lexicographical less than 'right's reachGraphStateSet. Consequently,
//!        'left' and 'right' are equal iff both their reachGraphStateSets contain
//!        the same states.
//! \param left left node
//! \param right right node
//! \returns true left is less than right.
//! \returns false left is greater or equal to right.
bool CompareGraphNodes::operator()(AnnotatedGraphNode const* left, AnnotatedGraphNode const* right) {
    TRACE(TRACE_5, "CompareGraphNodes::operator()() : start\n");
    TRACE(TRACE_5, "CompareGraphNodes::operator()() : end\n");

    return (left->reachGraphStateSet < right->reachGraphStateSet);
}
