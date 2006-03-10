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
 * \file cfg.cc
 *
 * \brief Functions for the Control Flow Graph (implementation)
 *
 * This file implements the class defined in cfg.h
 * 
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2006-01-19
 *          - last changed: \$Date: 2006/03/10 10:42:53 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.5 $
 *
 * \todo    - commandline option to control drawing of clusters 
 */

#include "cfg.h"

using std::cout;
using std::endl;

CFGBlock::CFGBlock()
{
  firstBlock = this;
  lastBlock  = this;
  dotted = false;
  processed = false;
  
}

CFGBlock::CFGBlock(CFGBlockType pType, kc::integer pId = kc::mkinteger(0), std::string pLabel = "")
{
  firstBlock = this;
  lastBlock  = this;
  type = pType;
  id = pId;
  label = pLabel;
  dotted = false;
  processed = false;
}

CFGBlock::~CFGBlock()
{

}

void CFGBlock::print_dot()
{
  if (!dotted)
  {
    dotted = true;

    (*output) << "  // " << dot_name() << endl;
    (*output) << "  " << dot_name() << " [ label=\"" << label << " (" << id->value <<")\" ]; " << endl;
    (*output) << "  // all outgoing edges" << endl;
    for(list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*output) << "  " << dot_name() << " -> " << (*iter)->dot_name() << ";" << endl;
    }
    (*output) << endl;

    for(list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*iter)->print_dot();
    }
  }
}

std::string CFGBlock::dot_name()
{
  return label + "_" + intToString(id->value);
}

void cfgDot(CFGBlock * block)
{
  (*output) << "digraph{" << endl;
  (*output) << "  graph ["
            << " label=\"CFG generated from '" << filename <<"'\" ]; " << endl;
  (*output) << "  node [ fontsize=10 shape=box fontname=Helvetica ]; " << endl;
  (*output) << "  edge [ fontsize=10 ]; " << endl << endl;

  // print all the nodes and edges
  block->print_dot();
  
  (*output) << "}" << endl;

}

void connectBlocks(CFGBlock * from, CFGBlock * to)
{
  from->nextBlocks.push_back(to);
  to->prevBlocks.push_back(from);
}

