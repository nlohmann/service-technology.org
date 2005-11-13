/*!
 * \file helpers.cc
 *
 * \brief Helper functions (implementation)
 *
 * This file implements several small helper functions that do not belong to
 * any other file.
 * 
 * \author  
 *          - Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          
 * \date    2005-11-11
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version
 *          - 2005-11-11 (nlohmann) Initial version.
 */





#include "helpers.h"


/*!
 * \param a set of Petri net nodes
 * \param b set of Petri net nodes
 * \return union of the sets a and b
 */
set<Node *> setUnion(set<Node *> a, set<Node *> b)
{
  set<Node *> result;
  insert_iterator<set<Node *, less<Node *> > > res_ins(result, result.begin());
  set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);
  
  return result;
}





/*!
 * \param i standard C int
 * \return  C++ string representing i
 */
string intToString(int i)
{
  char buffer[20];
  sprintf(buffer, "%d", i);
  
  return string(buffer);
}
