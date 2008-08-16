/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

/*!
 * \file    helpers.h
 *
 * \brief   helper functions
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 * 
 * \since   2005/11/11
 *
 * \date    \$Date: 2007/07/18 08:16:53 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.2 $
 */





#ifndef HELPERS_H
#define HELPERS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <set>
#include <string>
#include <vector>
#include <climits>

#ifdef USING_BPEL2OWFN
#include "ast-config.h"		// for kc::integer
#endif

using std::vector;
using std::string;
using std::set;
using std::insert_iterator;
using std::less;





/******************************************************************************
 * Conversion and error handling functions
 *****************************************************************************/

/// converts int to string
string toString(int i);

///converts a vector to a C++ string
string toString(const vector<unsigned int> &v);

#ifdef USING_BPEL2OWFN
/// converts integer to string
string toString(kc::integer i);
#endif

/// converts string to int
int toInt(string s);

/// converts string to unsigned int
unsigned int toUInt(string s);

/// strip an XML namespace
string strip_namespace(string s);

/// returns the maximum of two unsigned ints
unsigned int max(unsigned int a, unsigned int b);




/******************************************************************************
 * Set functions (since we're using templates, we cannot move this to a header)
 *****************************************************************************/

// to avoid compile errors
class Node;


/*!
 * \brief returns the union of two sets
 *
 * \param a  a set of type T
 * \param b  a set of type T
 * \returns set \f$a \cup b\f$
 */
template <class T>
set<T> setUnion(set<T> a, set<T> b)
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
set<T> setIntersection(set<T> a, set<T> b)
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
set<T> setDifference(set<T> a, set<T> b)
{
  set<T> result;
  insert_iterator<set<T, less<T> > > res_ins(result, result.begin());
  set_difference(a.begin(), a.end(), b.begin(), b.end(), res_ins);

  return result;
}





#endif
