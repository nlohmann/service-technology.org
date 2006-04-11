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
\*****************************************************************************/

/**
 * \file tarjan.cc
 *
 * \brief Implementation of Tarjan's algorithm for Petri nets
 *
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2006-04-11
 *          - last changed: \$Date: 2006/04/11 13:23:00 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.2 $
 */

#include "tarjan.h"
#include "debug.h"
#include "helpers.h"
#include <deque>
#include <list>
#include <algorithm>


using namespace std;

extern PetriNet * TheNet;

deque<Node *> tarjanStack;
map<Node*, bool> notWeiss;
map<Node*, int> dfs;
map<Node*, int> lowlink;

long maxdfs = 0;

void tarjan(Node * node)
{
  if (node == NULL)
  {
    return;
  }
  dfs[node] = lowlink[node] = maxdfs++;
  tarjanStack.push_back(node);
  notWeiss[node] = true;
  
  set<Node*> postSet = TheNet->postset(node);
  for (set<Node*>::iterator iter = postSet.begin(); iter != postSet.end(); iter++)
  {
    Node * node2 = *iter;
    if (!notWeiss[(node2)])
    {
      tarjan(node2);
      lowlink[node] = (lowlink[node] < lowlink[node2])?lowlink[node]:lowlink[node2];
    }
    else
    {
      if (find(tarjanStack.begin(), tarjanStack.end(), node2) != tarjanStack.end())
      {
	lowlink[node] = (lowlink[node] < dfs[node2])?lowlink[node]:dfs[node2];
      }	
    }
  }
  if (lowlink[node] == dfs[node])
  {
    list<Node *> scc;
    Node * node2 = NULL;
    do
    {
      node2 = tarjanStack.back();
      scc.push_back(node2);
      tarjanStack.pop_back();
    } while (node != node2);
    if (scc.size() > 2)
    {
      trace(TRACE_ALWAYS, "Warning: Found strongly connected component in the Petri net\n");
      trace(TRACE_ALWAYS, "         Reachability graph might be cyclic.\n");
      cerr << "         Size: " << scc.size() << endl;
      for (list<Node*>::iterator iter = scc.begin(); iter != scc.end(); iter++)
      {
	trace(TRACE_DEBUG, (*iter)->nodeShortName() + "\n");
	(*iter)->inSCC = true;
      }
    }
  }
}




