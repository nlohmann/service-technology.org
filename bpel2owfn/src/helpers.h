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
 * \file helpers.h
 *
 * \brief Helper functions (interface)
 *
 * This file defines several small helper functions that do not belong to any
 * other file.
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes by: \$Author: gierds $
 *          
 * \date    
 *          - created: 2005/11/11
 *          - last changed: \$Date: 2006/04/05 12:11:58 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.27 $
 */





#ifndef HELPERS_H
#define HELPERS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <set>
#include <string>
// #include "petrinet.h"
#include "exception.h"	// exception handling
// #include "check-symbols.h"
#include "bpel2owfn.h"

using namespace std;





// to avoid compile errors
class Node;

/// Returns the union of two sets of Petri net nodes.
set<Node *> setUnion(set<Node *> a, set<Node *> b);

/**
 * Returns the union of two sets of T's.
 *
 * \par a the first set
 * \par b the second set
 * \returns the set of a united with b (a \\/ b)
 */
template <class T>
set<T> setUnion(set<T> & a, set<T> & b)
{
  set<T> resultSet;
  resultSet.insert(a.begin(), a.end());
  resultSet.insert(b.begin(), b.end());

  return resultSet;
}

/**
 * Returns the intersection of two sets of T's.
 *
 * \par a the first set
 * \par b the second set
 * \returns the set of a intersected with b (a /\\ b)
 */
template <class T>
set<T> setIntersection(set<T> & a, set<T> & b)
{
  std::set<T> resultSet;
  if (! (a.empty() || b.empty()))
  {
    for (typename set<T>::iterator iter = a.begin(); iter != a.end(); iter++)
    {
      if (b.find(*iter) != b.end())
      {
	resultSet.insert(*iter);
      }
    }
  }

  return resultSet;
}

/**
 * Returns the difference of two sets of T's.
 *
 * \par a the first set
 * \par b the second set
 * \returns the set of a minus b (a \\ b)
 */
template <class T>
set<T> setDifference(set<T> & a, set<T> & b)
{
  set<T> resultSet (a);
  if (! (a.empty() || b.empty()))
  {
    for (typename set<T>::iterator iter = b.begin(); iter != b.end(); iter++)
    {
      resultSet.erase(*iter);
    }
  }

  return resultSet;
}

/// Converts int to string.
string intToString(int i);

/// Formats a debug information.
string pos(const char *file, int line, const char *function);

/// Prints help for command line arguments
void print_help();

/// Prints version information
void print_version(std::string name);

/// Evaluates command line arguments
// void parse_command_line(int argc, char * argv[]);

/// Calls #cleanup(), then exits
void error();

/// Prints information about #Exception e, calls #cleanup() and exits 
void error(Exception e);

/// Close all open files, delete all pointers.
void cleanup();

#endif
