/*****************************************************************************\
 * Copyright 2005, 2006, 2007 Niels Lohmann, Christian Gierds                *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    helpers.cc
 *
 * \brief   helper functions
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2005/11/11
 *
 * \date    \$Date: 2007/03/04 14:31:59 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.74 $
 *
 * \ingroup conversion
 * \ingroup debug
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <fstream>	// (std::ofstream)
#include <sstream>	// (std::ostringstream, std::istringstream)

#include "debug.h"	// (trace)
#include "helpers.h"
#include "options.h"

using std::istringstream;
using std::ostringstream;
using std::cout;
using std::cerr;
using std::clog;
using std::flush;
using std::ofstream;





/******************************************************************************
 * Conversion functions
 *****************************************************************************/

#ifdef USING_BPEL2OWFN
/*!
 * Converts a Kimwitu++ kc::integer to a C++ string.
 *
 * \param i Kimwitu++ integer
 * \return  C++ string representing i
 *
 * \ingroup conversion
 */
string toString(kc::integer i)
{
  return toString(i->value);
}
#endif




/*!
 * Converts a C++ int to a C++ string.
 *
 * \param i standard C int
 * \return  C++ string representing i
 *
 * \pre \f$i < 10^{20} \f$
 *
 * \ingroup conversion
 */
string toString(int i)
{
  std::ostringstream buffer;
  
  buffer << i;
  
  return buffer.str();
}





/*!
 * Convers an STL vector of unsigned ints to a C++ string representation. The
 * integers are seperated by a period.
 *
 * \param v  an STL vector of unsigned ints
 * \return   C++ string representation of v
 *
 * \ingroup conversion
 */
string toString(const vector<unsigned int> &v)
{
  string result;

  for (unsigned int i = 0; i < v.size(); i++)
  {
    if (i != 0)
      result += ".";

    result += toString(v[i]);
  }

  return result;
}





/*!
 * Converts a C++ string to a C++ int.
 *
 * \param s C++ string
 * \return int representing s or INT_MAX if the conversion failed
 *
 * \ingroup conversion
 */
int toInt(string s)
{
  int result;
  istringstream isst;
  isst.str(s);
 
  isst >> result;

  if (isst.fail())
    return INT_MAX;

  return result;
}





/*!
 * Converts a C++ string to a C++ unsigned int.
 *
 * \param s C++ string
 * \return unsigned int representing s or UINT_MAX if the conversion failed
 *         (e.g. a negative value was passed)
 *
 * \ingroup conversion
 */
unsigned int toUInt(string s)
{
  unsigned int result;
  istringstream isst;
  isst.str(s);
 
  isst >> result;

  if (isst.fail())
    return UINT_MAX;

  return result;
}





/*!
 * \param a an unsigned int
 * \param b an unsigned int
 * \return max(a,b)
 */
unsigned int max(unsigned int a, unsigned int b)
{
  if (a > b)
    return a;
  else
    return b;
}
