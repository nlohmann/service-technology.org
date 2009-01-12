/*!
 * \file    util.cc
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





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <fstream>	// (std::ofstream)
#include <sstream>	// (std::ostringstream, std::istringstream)
#include <cassert>
#include <string>
#include <vector>

//#include "debug.h"	// (trace)
#include "util.h"
//#include "options.h"

using std::istringstream;
using std::ostringstream;
using std::cout;
using std::cerr;
using std::clog;
using std::flush;
using std::ofstream;
using std::string;
using std::vector;





/******************************************************************************
 * Conversion functions
 *****************************************************************************/

#ifdef USING_BPEL2OWFN
/*!
 * Converts a Kimwitu++ kc::integer to a C++ string object.
 *
 * \param i Kimwitu++ integer
 * \return  C++ string object representing i
 */
/*
string toString(kc::integer i)
{
  assert(i!=NULL);
  return toString(i->value);
}
*/
#endif





/*!
 * Converts a C++ int to a C++ string object.
 *
 * \param i standard C int
 * \return  C++ string object representing i
 *
 * \note most uses of this function can be avoided by using stream operators 
 *       consequently
 */
string pnapi::util::toString(int i)
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
  /*
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
  */




/*!
 * Converts a C++ string object to a C++ int.
 *
 * \param s C++ string object
 * \return int representing s or INT_MAX if the conversion failed
 */
   /*
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

   */



/*!
 * Converts a C++ string object to a C++ unsigned int.
 *
 * \param s C++ string object
 * \return unsigned int representing s or UINT_MAX if the conversion failed
 *         (e.g. a negative value was passed)
 */
    /*
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
    */




/*!
 * \param a an unsigned int
 * \param b an unsigned int
 * \return max(a,b)
 */
    unsigned int pnapi::util::max(unsigned int a, unsigned int b)
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
      /*
string strip_namespace(string s)
{
  if (s.find_first_of(":") != string::npos &&
      (s.substr(0, s.find_first_of(":")) != "http"))
    return s.substr(s.find_first_of(":")+1, s.length());
  else
    return s;
}
      */
