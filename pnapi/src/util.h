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
#define PNAPI_FOREACH(I,C) for(__typeof((C).begin()) I=((C).begin()); I != (C).end(); ++I)

#include <set>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>
#include <stack>
#include <map>


namespace pnapi
{

/*!
 * \brief   Generic Utility Functions
 *
 * Converting strings and integers, set operations, ...
 */
namespace util
{

/*
 * \brief   DFS using Tarjan's algorithm.
 *
 * \param   Node n which is to check
 * \param   Stack S is the stack used in the Tarjan algorithm
 * \param   Set stacked which is needed to identify a node which is already
 *          stacked
 * \param   int \f$i \in \fmathbb{N}\f$ which is the equivalent to Tarjan's
 *          index variable
 * \param   Map index which describes the index property of a node
 * \param   Map lowlink which describes the lowlink property of a node
 *
 * \return  the number of strongly connected components reachable from the
 *          start node.
 *
 * \note    Used by method isWorkflowNet() to check condition (3) - only
 *          working for this method.
 */
template <typename T>
unsigned int dfsTarjan(T n, std::stack<T> & S, std::set<T> & stacked,
                       unsigned int & i, std::map<T, int> & index,
                       std::map<T, unsigned int> & lowlink)
{
  unsigned int retVal = 0;

  index[n] = i;
  lowlink[n] = i;
  ++i;
  S.push(n);
  stacked.insert(n);
  
  PNAPI_FOREACH(nn, n->getPostset())
  {
    if (index[*nn] < 0)
    {
      retVal += dfsTarjan(*nn, S, stacked, i, index, lowlink);
      lowlink[n] = std::min(lowlink[n], lowlink[*nn]);
    }
    else
    {
      if (stacked.count(*nn) > 0)
        lowlink[n] = std::min(lowlink[n], lowlink[*nn]);
    }
  }
  
  if(static_cast<int>(lowlink[n]) == index[n])
  {
    ++retVal;
    while(!S.empty() && (lowlink[S.top()] == lowlink[n]))
    {
      S.pop();
    }
  }

  return retVal;
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


/*!
 * \brief returns the intersection of two sets
 *
 * \param a  a set of type T
 * \param b  a set of type T
 * \returns set \f$a \cap b\f$
 */
template <class T>
std::set<T> setIntersection(const std::set<T> & a, const std::set<T> & b)
{
  std::set<T> result;
  std::insert_iterator<std::set<T, std::less<T> > > res_ins(result, result.begin());
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
std::set<T> setDifference(const std::set<T> & a, const std::set<T> & b)
{
  std::set<T> result;
  std::insert_iterator<std::set<T, std::less<T> > > res_ins(result, result.begin());
  set_difference(a.begin(), a.end(), b.begin(), b.end(), res_ins);

  return result;
}

} /* namespace util */

} /* namespace pnapi */

#endif /* PNAPI_UTIL_H */
