// -*- C++ -*-

/*!
 * \file    util.h
 *
 * \brief   utility functions
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: $Author: niels $
 *
 * \since   2005/11/11
 *
 * \date    $Date: 2009-10-14 11:30:09 +0200 (Mi, 14. Okt 2009) $
 *
 * \version $Revision: 4827 $
 */

#ifndef PNAPI_UTIL_H
#define PNAPI_UTIL_H

#include <set>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>
#include <stack>
#include <map>

using std::vector;
using std::string;
using std::set;
using std::insert_iterator;
using std::less;
using std::stack;
using std::map;
using std::min;

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
unsigned int dfsTarjan(T n, stack<T> & S, set<T> & stacked,
    unsigned int & i, map<T, int> &index,
    map<T, unsigned int> & lowlink)
{
  unsigned int retVal = 0;

  index[n] = i;
  lowlink[n] = i;
  i++;
  S.push(n);
  stacked.insert(n);
  for (typename set<T>::const_iterator nn = n->getPostset().begin();
  nn != n->getPostset().end(); ++nn)
  {
    if (index[*nn] < 0)
    {
      retVal += dfsTarjan(*nn, S, stacked, i, index, lowlink);
      lowlink[n] = min(lowlink[n], lowlink[*nn]);
    }
    else
    {
      if (stacked.count(*nn) > 0)
        lowlink[n] = min(lowlink[n], lowlink[*nn]);
    }
  }
  if (static_cast<int>(lowlink[n]) == index[n])
  {
    retVal++;
    while (!S.empty() && lowlink[S.top()] == lowlink[n])
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
