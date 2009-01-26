/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2008 Dirk Fahland <dirk.fahland@service-technolog.org>,
                     Martin Znamirowski <znamirow@informatik.hu-berlin.de>
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/




/******************************************************************************
 * Headers
 *****************************************************************************/

#include <fstream>	// (std::ofstream)
#include <sstream>	// (std::ostringstream, std::istringstream)
#include <cassert>

#include "debug.h"	// (trace)
#include "helpers.h"
#include "options.h"

using std::istringstream;
using std::ostringstream;
using std::flush;
using std::ofstream;






/*!
 * Converts a C++ int to a C++ string object.
 *
 * \param i standard C int
 * \return  C++ string object representing i
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
 * \return   C++ string object representation of v
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
 * Converts a C++ float to a C++ string object.
 *
 * \param f standard C float
 * \return  C++ string object representing f
 */
string floatToString(float f)
{
  std::ostringstream buffer;
  buffer << f;
  return buffer.str();
}



/*!
 * Converts a C++ string object to a C++ int.
 *
 * \param s C++ string object
 * \return int representing s or INT_MAX if the conversion failed
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
 * Converts a C++ string object to a C++ unsigned int.
 *
 * \param s C++ string object
 * \return unsigned int representing s or UINT_MAX if the conversion failed
 *         (e.g. a negative value was passed)
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





/*!
 * \param s  a string that might be prefixed with an XML namespace
 * \return   string s without XML namespace prefix
 *
 * \note  The prefix "http:" is not removed.
 */
string strip_namespace(string s)
{
  if (s.find_first_of(":") != string::npos &&
      (s.substr(0, s.find_first_of(":")) != "http"))
    return s.substr(s.find_first_of(":")+1, s.length());
  else
    return s;
}

string id(string s)
{
    return s;
}
