#ifndef _UTIL_H
#define _UTIL_H

#include <set>
#include <string>
#include <fstream>
#include <algorithm>

/*!
  \def FOREACH(I, C)
  \brief a shortcut for STL iterations
  \details Assume you have the code:
  \code
  for (std::set<std::string>::iterator it = mySet.begin(); it != mySet.end(); ++it) {
    // stuff
  }
  \endcode
  Then this code can be replaced by the following code
  \code
  FOREACH(it, mySet) {
    // stuff
  }
  \endcode
  \param I an iterator variable
  \param C an container to iterate
  \note The code for the FOREACH macro was taken from a post on Stack Overflow
        http://stackoverflow.com/questions/197375/visual-c-for-each-portability/197926#197926
  \note The type of the iterator is derived from the begin() function of the
        passed container C. There is no need to care about constness.
*/
#define FOREACH(I,C) for(__typeof((C).begin()) I=((C).begin()); I != (C).end(); ++I)

/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}

/*!
 * \brief returns the union of two sets
 *
 * \param a  a set of type T
 * \param b  a set of type T
 * \returns set \f$a \cup b\f$
 */
template <class T>
std::set<T> setUnion(const std::set<T> & a, const std::set<T> & b)
{
  std::set<T> result;
  std::insert_iterator<std::set<T, std::less<T> > > res_ins(result, result.begin());
  set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);

  return result;
}

template <class T>
std::set<T> setIntersection(const std::set<T> & a, const std::set<T> & b)
{
  std::set<T> result;
  std::insert_iterator<std::set<T, std::less<T> > > res_ins(result, result.begin());
  set_intersection(a.begin(), a.end(), b.begin(), b.end(), res_ins);

  return result;
}

#endif
