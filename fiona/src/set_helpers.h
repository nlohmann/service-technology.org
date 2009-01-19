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

#ifndef SET_HELPERS_H
#define HELPERS_H

/******************************************************************************
 * Headers
 *****************************************************************************/

#include <set>
#include <algorithm>

using std::set;
using std::insert_iterator;
using std::less;


/******************************************************************************
 * Set functions (since we're using templates, we cannot move away)
 *****************************************************************************/

/*!
 * \brief returns the union of two sets
 *
 * \param a  a set of type T
 * \param b  a set of type T
 * \returns set \f$a \cup b\f$
 */
template<class T> set<T> setUnion(const set<T>& a, const set<T>& b) {
    set<T> result;
    insert_iterator<set<T, less<T> > > res_ins(result, result.begin());
    set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);

    return result;
}


/*!
 * \brief returns the intersection of two sets
 *
 * \param a  a set of type T
 * \param b  a set of type T
 * \returns set \f$a \cap b\f$
 */
template<class T> set<T> setIntersection(const set<T>& a, const set<T>& b) {
    set<T> result;
    insert_iterator<set<T, less<T> > > res_ins(result, result.begin());
    set_intersection(a.begin(), a.end(), b.begin(), b.end(), res_ins);

    return result;
}


/*!
 * \brief returns the difference of two sets
 *
 * \param a  a set of type T
 * \param b  a set of type T
 * \returns set \f$a \;\backslash\; b\f$
 */
template<class T> set<T> setDifference(const set<T>& a, const set<T>& b) {
    set<T> result;
    insert_iterator<set<T, less<T> > > res_ins(result, result.begin());
    set_difference(a.begin(), a.end(), b.begin(), b.end(), res_ins);

    return result;
}

#endif
