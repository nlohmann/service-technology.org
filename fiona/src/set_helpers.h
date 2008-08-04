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
template<class T> set<T> setUnion(set<T> a, set<T> b) {
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
template<class T> set<T> setIntersection(set<T> a, set<T> b) {
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
template<class T> set<T> setDifference(set<T> a, set<T> b) {
    set<T> result;
    insert_iterator<set<T, less<T> > > res_ins(result, result.begin());
    set_difference(a.begin(), a.end(), b.begin(), b.end(), res_ins);

    return result;
}

#endif
