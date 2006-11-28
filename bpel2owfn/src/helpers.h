/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

/*!
 * \file    helpers.h
 *
 * \brief   helper functions
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 * 
 * \since   2005/11/11
 *
 * \date    \$Date: 2006/11/28 12:39:57 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.40 $
 *
 * \ingroup conversion
 * \ingroup debug
 */





#ifndef HELPERS_H
#define HELPERS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <set>
#include <string>
#include <vector>
#include "bpel2owfn.h"

#ifdef USING_BPEL2OWFN
#include "ast-config.h"		// all you need from Kimwitu++
#endif

using namespace std;




/******************************************************************************
 * Conversion and error handling functions
 *****************************************************************************/

string toString(int i);
string toString(vector<unsigned int> &v);
#ifdef USING_BPEL2OWFN
string toString(kc::integer i);
#endif
int toInt(string s);
unsigned int toUInt(string s);
void error();
void cleanup();
void next_index(vector<unsigned int> &current_index, vector<unsigned int> &max_index);





/******************************************************************************
 * Set functions (since we're using templates, we cannot move this to a header
 *****************************************************************************/

// to avoid compile errors
class Node;





/*!
 * \brief Returns the union of two sets of T's.
 *
 * \par a the first set
 * \par b the second set
 * \returns the set of a united with b
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
 * \brief Returns the intersection of two sets of T's.
 *
 * \par a the first set
 * \par b the second set
 * \returns the set of a intersected with b
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
 * \brief Returns the difference of two sets of T's.
 *
 * \par a the first set
 * \par b the second set
 * \returns the set of a minus b (a \\ b)
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



/*!
 *\defgroup conversion Conversion Functions
 */
