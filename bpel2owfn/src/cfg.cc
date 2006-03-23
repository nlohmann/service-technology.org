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
 *          - last changed: \$Date: 2006/03/23 15:53:06 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.12 $
 *
 * \todo    - commandline option to control drawing of clusters 
 */

#include "cfg.h"
#include "symbol-table.h"

extern SymbolTable symTab;

using std::cout;
using std::endl;

map<std::string, CFGBlock*> sources;
map<std::string, CFGBlock*> targets;

CFGBlock::CFGBlock()
{
  firstBlock = this;
  lastBlock  = this;
  dotted = false;
  processed = false;
  dpe = false;
  
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
  dpe = false;
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
    (*output) << "  \"" << dot_name() << "\" [ label=\"" << label << " (" << id->value <<")";
    if (channel_name != "")
    {
      (*output) << "\\nchannel: " << channel_name;
    }
    if (! initializedVariables.empty())
    {
      (*output) << "\\ninitializedVariables: \\n ";
      for (set<std::string>::iterator iter = initializedVariables.begin(); iter != initializedVariables.end(); iter++)
      {
	(*output) << (*iter) << "\\n";
      }
    }
    (*output) << "\"";
    if (type == CFGSource)
    {
      (*output) << " color=red style=filled"; // style=filled
    }
    if (type == CFGTarget)
    {
      (*output) << " color=green style=filled"; //
    }
    if (dpe)
    {
      (*output) << " fontcolor=blue";
    }
    (*output) << " ]; " << endl;
    (*output) << "  // all outgoing edges" << endl;
    for(list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*output) << "  \"" << dot_name() << "\" -> \"" << (*iter)->dot_name() << "\";" << endl;
    }
    (*output) << endl;
    // draw link
    if (type == CFGSource)
    {
      std::string targ = "dummy";
      if (targets[dot_name()] != NULL)
	  targ = (targets[dot_name()])->dot_name();
      (*output) << "  \"" << dot_name() << "\" -> \"" << targ << "\" [ style=dotted ];" << endl;
    }

    for(list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*iter)->print_dot();
    }
  }
}

std::string CFGBlock::dot_name()
{
  if (type == CFGSource || type == CFGTarget)
  {
    return label;
  }
  else
  {
    return label + "_" + intToString(id->value);
  }
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

bool CFGBlock::needsDPE(int hasStartingBlock, list<kc::integer>& lastTargets)
{
 
  if (processed)
  {
    return dpe;
  }
  
  bool childrenDPE = false;
  int newStartingBlockNumber = hasStartingBlock;
  list<kc::integer> localTargetList;

  if (!lastTargets.empty())
  {
    for (list<kc::integer>::iterator iter = lastTargets.begin(); iter != lastTargets.end(); iter++)
    {
      localTargetList.push_back(*iter);
    }
  }

  processed = true;

  if (! nextBlocks.empty())
  {
    switch (type)
    {
      case CFGSwitch :  if (label == "Switch_begin")
			{
			  newStartingBlockNumber++;
			}
			else
			{
			  newStartingBlockNumber--;
			}
			break; 
      case CFGPick :	if (label == "Pick_begin")
			{
			  newStartingBlockNumber++;
			}
			else
			{
			  newStartingBlockNumber--;
			}
			break; 
      case CFGTarget :	localTargetList.push_back(id);
			break;
      default :		if (! localTargetList.empty())
			{
			  while ( ! localTargetList.empty() && (*(--localTargetList.end()))->value > id->value)
			  {
			    localTargetList.remove( *(--localTargetList.end()) );
			  }
			}
			break;
    }
    for (list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      bool result = (*iter)->needsDPE(newStartingBlockNumber, localTargetList);
      childrenDPE = childrenDPE || result;
    }
    if (type == CFGSource)
    {
      childrenDPE = true;
    }
    
    return dpe = (childrenDPE && ((hasStartingBlock > 0) || !localTargetList.empty()) );
  }
  else
  {
    return childrenDPE;
  }
}

/// resets the processed flag to false
void CFGBlock::resetProcessedFlag(bool withLinks, bool forward)
{
  if (! processed)
  {
    return;
  }
  processed = false;
  
  if (forward && ! nextBlocks.empty())
  {
    for (list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*iter)->resetProcessedFlag(withLinks, forward);
    }
    if (withLinks && type == CFGSource)
    {
      targets[dot_name()]->resetProcessedFlag(withLinks, forward);
    }
  }
  else if (! forward && ! prevBlocks.empty())
  {
    for (list<CFGBlock *>::iterator iter = prevBlocks.begin(); iter != prevBlocks.end(); iter++)
    {
      (*iter)->resetProcessedFlag(withLinks, forward);
    }
    if (withLinks && type == CFGTarget)
    {
      sources[dot_name()]->resetProcessedFlag(withLinks, forward);
    }
  }
  
}


void connectBlocks(CFGBlock * from, CFGBlock * to)
{
  from->nextBlocks.push_back(to);
  to->prevBlocks.push_back(from);
}

/***************************** Program Analysis *******************************/

/**
 * Checks for uninitialized variables.
 * This is a forward-must analysis of the data flow.
 *
 *
 */
void CFGBlock::checkForUninitializedVariables()
{
  if (processed)
  {
    return;
  }
  processed = true;
  
  bool allPrerequisites = true;
  // check if all entries are allready processed
  if (!prevBlocks.empty())
  {
    /// for a while, we assume, that the body is never executed, so we drop that set
    list<CFGBlock *>::iterator blockBegin = prevBlocks.begin();
    if (type == CFGWhile)
    {
      blockBegin++;
    }
    for (list<CFGBlock *>::iterator iter = blockBegin; iter != prevBlocks.end(); iter++)
    {
      allPrerequisites = allPrerequisites && (*iter)->processed;
    }
  }
  if (type == CFGTarget)
  {
    allPrerequisites = allPrerequisites && sources[dot_name()]->processed;
  }
  if (allPrerequisites)
  {
    if (!prevBlocks.empty())
    {
      list<CFGBlock *>::iterator blockBegin = prevBlocks.begin();
      if (type == CFGWhile)
      {
	blockBegin++;
      }
      initializedVariables = (*blockBegin)->initializedVariables;
      for (list<CFGBlock *>::iterator iter = blockBegin; iter != prevBlocks.end(); iter++)
      {
	if (type == CFGFlow && label == "Flow_end")
	{
	  initializedVariables = setUnion(initializedVariables, (*iter)->initializedVariables);
	}
	else
	{
	  initializedVariables = setIntersection(initializedVariables, (*iter)->initializedVariables);
	}
      }
    }
    if (type == CFGTarget)
    {
      initializedVariables = setUnion(initializedVariables, sources[dot_name()]->initializedVariables);
    }
  }
  else
  {
    processed = false;
    return;
  }

  // checking Variables for this Block
  STVariable * stVar = NULL;
  std::string attributeName = "variable";
  try
  {
    switch(type)
    {
      case CFGReply     : stVar = (dynamic_cast<STReply*>     (symTab.lookup(id)))->variable; break;
      case CFGFrom      : stVar = (dynamic_cast<STFromTo*>    (symTab.lookup(id)))->variable; break;
      case CFGInvoke    : stVar = (dynamic_cast<STInvoke*>    (symTab.lookup(id)))->inputVariable; 
			  attributeName = "inputVariable";
			  break;
    }
  }
  catch (bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR, "Could not cast correctly", pos(__FILE__, __LINE__, __FUNCTION__));
  }
  
  if ((type == CFGReply || type == CFGFrom || type == CFGInvoke) && stVar != NULL)
  {
    std::string var = stVar->name;
    if (var != "")
    {
      if (initializedVariables.find(var) == initializedVariables.end())
      {
	trace("[CFG] Warning: Variable \"" + symTab.readAttributeValue(id, attributeName) 
//	      + "\" ("+ var +") (activity ID = " 
//	      + intToString(id->value)  
	      + "\" in line " 
	      + intToString(symTab.readAttribute(id, attributeName)->line) 
	      + " might be undefined!\n");
      }
    }
  }
  
  // adding Variables for this Block
  stVar = NULL;
  attributeName = "variable";
  try
  {
    switch(type)
    {
      case CFGReceive   : stVar = (dynamic_cast<STReceive*>   (symTab.lookup(id)))->variable; break;
      case CFGTo        : stVar = (dynamic_cast<STFromTo*>    (symTab.lookup(id)))->variable; break;
      case CFGInvoke    : stVar = (dynamic_cast<STInvoke*>    (symTab.lookup(id)))->outputVariable;
			  attributeName = "outputVariable";
			  break;
      case CFGOnMessage : stVar = (dynamic_cast<STOnMessage*> (symTab.lookup(id)))->variable; break;
    }
  }
  catch (bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR, "Could not cast correctly", pos(__FILE__, __LINE__, __FUNCTION__));
  }
  
  if ((type == CFGReceive || type == CFGTo || type==CFGInvoke || type == CFGOnMessage) && stVar != NULL)
  {
    std::string var = stVar->name;
    if (var != "")
    {
      initializedVariables.insert(var); 
    }
  }
  
  if (! nextBlocks.empty())
  {
    for (list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*iter)->checkForUninitializedVariables();
    }
  }  
}

