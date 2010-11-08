// -*- C++ -*-

/*!
 * \file    util.h
 *
 * \brief   utility functions
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: $Author: cas $
 *
 * \since   2005/11/11
 *
 * \date    $Date: 2010-06-14 15:12:32 +0200 (Mon, 14 Jun 2010) $
 *
 * \version $Revision: 5831 $
 */

#ifndef PNAPI_UTIL_H
#define PNAPI_UTIL_H

/*!
  \def PNAPI_FOREACH(I, C)
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
 * \brief   modified Tarjan's algorithm.
 *
 * \param   n Node which is to check
 * \param   last node without postset
 * \param   stacked Set which is needed to identify a node which is already
 *          stacked
 * \param   index the equivalent to Tarjan's index variable
 * \param   indices map which describes the index property of a node
 * \param   lowlink map which describes the lowlink property of a node
 * \param   nodeCount amount of nodes in the net
 *
 * \return  whether the net has workflow structure
 *
 * \note    Used by method isWorkflow() to check condition (3) - only
 *          working for this method.
 * 
 * To Tarjan's algorithm the following modifications have been applied:
 * 1.: If the recent node is the "last" node, the lowlink will be set to 0,
 *     since the method PetrNet::isWorkflow() actually checks, whether
 *     the net is strongly connected when a single transition, consuming
 *     from the last place and producing to the first place, is added.
 *     So instead of adding this transition, the lowlink value 0 will
 *     directly be assigned.
 * 2.: If a scc has been found, the index has to be 0 and the stack must
 *     hold nodeCount nodes. Otherwise the net is not strongly connected.
 */
template <typename T>
bool dfsTarjan(T n, T last, std::set<T> & stacked,
               unsigned int & index, std::map<T, int> & indices,
               std::map<T, unsigned int> & lowlink, unsigned int nodeCount)
{
  indices[n] = lowlink[n] = index++;
  if(n == last)
  {
    lowlink[n] = 0;
  }
  stacked.insert(n);
  
  PNAPI_FOREACH(nn, n->getPostset())
  {
    if(indices[*nn] < 0)
    {
      // new node in dfs-tree
      if(!dfsTarjan(*nn, last, stacked, index, indices, lowlink, nodeCount))
      {
        // scc found whithin the net
        return false;
      }
      lowlink[n] = std::min(lowlink[n], lowlink[*nn]);
    }
    else
    {
      if(stacked.count(*nn) > 0)
      {
        lowlink[n] = std::min(lowlink[n], static_cast<unsigned int>(indices[*nn]));
      }
    }
  }
  
  if(static_cast<int>(lowlink[n]) == indices[n])
  {
    // scc found
    if((indices[n] == 0) && // this node is the start node
       (stacked.size() == nodeCount)) // all nodes belong to this scc
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  return true;
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
