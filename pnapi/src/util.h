// -*- C++ -*-

/*!
 * \file    util.h
 *
 * \brief   utility functions
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: $Author$
 * 
 * \since   2005/11/11
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#ifndef PNAPI_UTIL_H
#define PNAPI_UTIL_H

#include <set>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>

using std::vector;
using std::string;
using std::set;
using std::insert_iterator;
using std::less;


namespace pnapi
{

  /*!
   * \brief   Generic Utility Functions
   *
   * Converting strings and integers, set operations, ...
   */
  namespace util
  {

    /// returns maximum of two numbers
    unsigned int max(unsigned int, unsigned int);

    /// converts int to string
    string toString(int);

    
    /*!
     * \brief returns the union of two sets
     *
     * \param a  a set of type T
     * \param b  a set of type T
     * \returns set \f$a \cup b\f$
     */
    template <class T>
    set<T> setUnion(const set<T> & a, const set<T> & b)
    {
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
    template <class T>
    set<T> setIntersection(const set<T> & a, const set<T> & b)
    {
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
    template <class T>
    set<T> setDifference(const set<T> & a, const set<T> & b)
    {
      set<T> result;
      insert_iterator<set<T, less<T> > > res_ins(result, result.begin());
      set_difference(a.begin(), a.end(), b.begin(), b.end(), res_ins);
      
      return result;
    }

  }

}

#endif
