/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2009        Niels Lohmann
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  GNU BPEL2oWFN is free software: you can redistribute it and/or modify it
  under the terms of the GNU Affero General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License
  along with GNU BPEL2oWFN. If not, see <http://www.gnu.org/licenses/>.
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
 * \date    \$Date: 2007/07/18 08:17:48 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.84 $
 *
 * \ingroup debug
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <config.h>
#include <iostream>
#include <fstream>  // (std::ofstream)
#include <sstream>  // (std::ostringstream, std::istringstream)
#include <string>
#include <vector>

#include "debug.h"  // (trace)
#include "helpers.h"
#include "options.h"

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
string toString(kc::integer i) {
    assert(i != NULL);
    return toString(i->value);
}
#endif





/*!
 * Converts a C++ int to a C++ string object.
 *
 * \param i standard C int
 * \return  C++ string object representing i
 */
string toString(int i) {
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
string toString(const vector<unsigned int> &v) {
    string result;

    for (unsigned int i = 0; i < v.size(); i++) {
        if (i != 0) {
            result += ".";
        }

        result += toString(v[i]);
    }

    return result;
}





/*!
 * Converts a C++ string object to a C++ int.
 *
 * \param s C++ string object
 * \return int representing s or INT_MAX if the conversion failed
 */
int toInt(string s) {
    int result;
    istringstream isst;
    isst.str(s);

    isst >> result;

    if (isst.fail()) {
        return INT_MAX;
    }

    return result;
}





/*!
 * Converts a C++ string object to a C++ unsigned int.
 *
 * \param s C++ string object
 * \return unsigned int representing s or UINT_MAX if the conversion failed
 *         (e.g. a negative value was passed)
 */
unsigned int toUInt(string s) {
    unsigned int result;
    istringstream isst;
    isst.str(s);

    isst >> result;

    if (isst.fail()) {
        return UINT_MAX;
    }

    return result;
}





/*!
 * \param a an unsigned int
 * \param b an unsigned int
 * \return max(a,b)
 */
unsigned int max(unsigned int a, unsigned int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}





/*!
 * \param s  a string that might be prefixed with an XML namespace
 * \return   string s without XML namespace prefix
 *
 * \note  The prefix "http:" is not removed.
 */
string strip_namespace(string s) {
    if (s.find_first_of(":") != string::npos &&
            (s.substr(0, s.find_first_of(":")) != "http")) {
        return s.substr(s.find_first_of(":") + 1, s.length());
    } else {
        return s;
    }
}
