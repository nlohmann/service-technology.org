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
 *          - last changed: \$Date: 2006/01/26 15:15:34 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.3 $
 *
 * \todo    - commandline option to control drawing of clusters 
 */

#include "cfg.h"

using std::cout;
using std::endl;

/// generic constructor
CFGBlock::CFGBlock() 
{
  type = "generic";	
  label = "0";
  prevBlock = NULL;
  nextBlock = NULL;
}

/// generic constructor
CFGBlock::CFGBlock(std::string id) 
{
  type = "generic";	
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

/// generic constructor
CFGBlock::CFGBlock(kc::integer id) 
{
  type = "generic";	
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}

/// generic destructor
CFGBlock::~CFGBlock()
{
  trace(TRACE_DEBUG, "[CFG] Deleting " + type + "(" + label + ")\n");
  if (nextBlock != NULL)
  {
    delete(nextBlock);
  }
}
    
/// returns the concrete type
std::string CFGBlock::getType()
{
  return type;
}

/// generic dot printout
void CFGBlock::print_dot()
{
  // cout << "    node [shape=box]" << endl;
  cout << "    " << type << label << " [label=\"" << type << " (" << label << ")\"]" << endl;

  dot_nextBlock();
}

void CFGBlock::dot_nextBlock()
{
  if (nextBlock != NULL)
  {
    nextBlock->print_dot();
    cout << "    " << lastNodeName() << " -> " << nextBlock->firstNodeName() << ";" << endl;
  }
  cout << endl;
}

/// returns the name of the first node in this block
std::string CFGBlock::firstNodeName()
{
  return type + label;
}

/// returns the name of the last node in this block
std::string CFGBlock::lastNodeName()
{
  return type + label;
}


/******************************************************************************
  PROCESS
******************************************************************************/

CFGProcess::CFGProcess(std::string id)
{
  type = "Process";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGProcess::CFGProcess(kc::integer id)
{
  type = "Process";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}

void CFGProcess::print_dot()
{
  cout << "digraph{" << endl;
  cout << "  graph [" 
       << " label=\"CFG generated from '" << filename << "'\"];" << endl; //fontname=\"Helvetica-Oblique\"
  cout << "  node [fontsize=10 fixedsize];" << endl; //fontname=\"Helvetica-Oblique\" 
  cout << "  edge [fontsize=10];" << endl << endl; //fontname=\"Helvetica-Oblique\" 

  cout << "    node [shape=box];" << endl;  //,regular=true
  cout << "    " << firstNodeName() << " [label=\"" << type << " (" << label << "_entry)\"];" << endl;
  cout << "    " << lastNodeName()  << " [label=\"" << type << " (" << label << "_exit)\"];" << endl;

  if (nextBlock != NULL)
  {
    nextBlock->print_dot();
    cout << "    " << firstNodeName() << " -> " << nextBlock->firstNodeName() << ";" << endl;
    cout << "    " << nextBlock->lastNodeName() << " -> " << lastNodeName() << ";" << endl;
  }
  
  cout << "}" << endl;
}

/// returns the name of the first node in this block
std::string CFGProcess::firstNodeName()
{
  return type + label + "entry";
}

/// returns the name of the last node in this block
std::string CFGProcess::lastNodeName()
{
  return type + label + "exit";
}


/******************************************************************************
  EMPTY
******************************************************************************/

CFGEmpty::CFGEmpty(std::string id)
{
  type = "Empty";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGEmpty::CFGEmpty(kc::integer id)
{
  type = "Empty";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  INVOKE
******************************************************************************/

CFGInvoke::CFGInvoke(std::string id)
{
  type = "Invoke";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGInvoke::CFGInvoke(kc::integer id)
{
  type = "Invoke";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  RECEIVE
******************************************************************************/

CFGReceive::CFGReceive(std::string id)
{
  type = "Receive";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGReceive::CFGReceive(kc::integer id)
{
  type = "Receive";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  REPLY
******************************************************************************/

CFGReply::CFGReply(std::string id)
{
  type = "Reply";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGReply::CFGReply(kc::integer id)
{
  type = "Reply";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  ASSIGN
******************************************************************************/

CFGAssign::CFGAssign(std::string id)
{
  type = "Assign";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGAssign::CFGAssign(kc::integer id)
{
  type = "Assign";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGAssign::~CFGAssign()
{
  if (!copyList.empty()) 
  {
    delete(*(copyList.begin()));
  }
}

void CFGAssign::print_dot()
{
  cout << endl;
/*
  cout << "  subgraph cluster" << type << label << " {" << endl;
  cout << "    label=\"" << type << " (" << label << ")\";" << endl;
  cout << "    labelangle=90.0;" << endl;
  cout << "    labeljust=l;" << endl;
  cout << "    color=blue;" << endl;
  cout << "    style=dotted;" << endl;
*/
/*
  for(list<CFGCopy *>::iterator iter = copyList.begin(); iter != copyList.end(); iter++)
  {
    (*iter)->print_dot();
    if ((*iter)->prevBlock != NULL)
    {
      cout << "    " << (*iter)->prevBlock->lastNodeName() 
	   << " -> " << (*iter)->firstNodeName() << ";" << endl;
    }
  }
*/
  if (! copyList.empty())
  {
    (*(copyList.begin()))->print_dot();
  }
/*  
  cout << "  }" << endl;
*/

  dot_nextBlock();
  cout << endl;
}

std::string CFGAssign::firstNodeName()
{
  if (!copyList.empty())
  {
    return ((*(copyList.begin()))->firstNodeName());
  }
  else
  {
    return "gaga";
  }
}

std::string CFGAssign::lastNodeName()
{
  if (!copyList.empty())
  {
    return ((*(--(copyList.end())))->lastNodeName());
  }
  else
  {
    return "gaga";
  }
}


CFGCopy::CFGCopy(std::string id)
{
  type = "Copy";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGCopy::CFGCopy(kc::integer id)
{
  type = "Copy";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  WAIT
******************************************************************************/

CFGWait::CFGWait(std::string id)
{
  type = "Wait";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGWait::CFGWait(kc::integer id)
{
  type = "Wait";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  THROW
******************************************************************************/

CFGThrow::CFGThrow(std::string id)
{
  type = "Throw";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGThrow::CFGThrow(kc::integer id)
{
  type = "Throw";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  COMPENSATE
******************************************************************************/

CFGCompensate::CFGCompensate(std::string id)
{
  type = "Compensate";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGCompensate::CFGCompensate(kc::integer id)
{
  type = "Compensate";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  TERMINATE
******************************************************************************/

CFGTerminate::CFGTerminate(std::string id)
{
  type = "Terminate";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGTerminate::CFGTerminate(kc::integer id)
{
  type = "Terminate";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}


/******************************************************************************
  FLOW
******************************************************************************/

CFGFlow::CFGFlow(std::string id)
{
  type = "Flow";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGFlow::CFGFlow(kc::integer id)
{
  type = "Flow";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGFlow::~CFGFlow()
{
  if (!activityList.empty()) 
  {
    for (list<CFGBlock *>::iterator iter = activityList.begin(); iter != activityList.end(); iter++)
    {
      delete(*(iter));
    }
  }
}

void CFGFlow::print_dot()
{
  cout << endl;
/*
  cout << "  subgraph cluster" << type << label << " {" << endl;
  cout << "    label=\"" << type << " (" << label << ")\";" << endl;
  cout << "    labelangle=90.0;" << endl;
  cout << "    labeljust=l;" << endl;
  cout << "    color=blue;" << endl;
  cout << "    style=dotted;" << endl;
  cout << "    node [shape=box]" << endl;
*/
  cout << "    " << firstNodeName() << " [label=\"" << type << " (" << label << "_entry)\"]" << endl;
  cout << "    " << lastNodeName()  << " [label=\"" << type << " (" << label << "_exit)\"]" << endl;

  for(list<CFGBlock *>::iterator iter = activityList.begin(); iter != activityList.end(); iter++)
  {
    (*iter)->print_dot();
    cout << "    " << firstNodeName() 
         << " -> " << (*iter)->firstNodeName() << ";" << endl;
    cout << "    " << (*iter)->lastNodeName()
	 << " -> " << lastNodeName() << ";" << endl;
  }
/*  
  cout << "  }" << endl;
*/
  dot_nextBlock();
  cout << endl;
}

std::string CFGFlow::firstNodeName()
{
  return type + label + "entry";
}

std::string CFGFlow::lastNodeName()
{
  return type + label + "exit";
}


/******************************************************************************
  SWITCH
******************************************************************************/

CFGSwitch::CFGSwitch(std::string id)
{
  type = "Switch";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;

  otherwise = NULL;
}

CFGSwitch::CFGSwitch(kc::integer id)
{
  type = "Switch";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;

  otherwise = NULL;
}

CFGSwitch::~CFGSwitch()
{
  if (!caseList.empty()) 
  {
    for (list<CFGBlock *>::iterator iter = caseList.begin(); iter != caseList.end(); iter++)
    {
      delete(*(iter));
    }
  }
  if (otherwise != NULL)
  {
    delete(otherwise);
  }
}

void CFGSwitch::print_dot()
{
  cout << endl;
/*
  cout << "  subgraph cluster" << type << label << " {" << endl;
  cout << "    label=\"" << type << " (" << label << ")\";" << endl;
  cout << "    labelangle=90.0;" << endl;
  cout << "    labeljust=l;" << endl;
  cout << "    color=blue;" << endl;
  cout << "    style=dotted;" << endl;
  cout << "    node [shape=box];" << endl;
*/
  cout << "    " << firstNodeName() << " [label=\"" << type << " (" << label << "_entry)\"];" << endl;
  cout << "    " << lastNodeName()  << " [label=\"" << type << " (" << label << "_exit)\"];" << endl;

  int i = 1;
  for(list<CFGBlock *>::iterator iter = caseList.begin(); iter != caseList.end(); iter++)
  {
    (*iter)->print_dot();
    cout << "    " << firstNodeName() << "Case" << i 
	 << " [label=\"Case " << i << "(" << label << ")\"]" 
	 << ";" << endl;
    if (i > 1)
    {
      cout << "    " << firstNodeName() << "Case" << (i - 1) 
           << " -> " << firstNodeName() << "Case" << i  
	   << " [taillabel=\"false\" labelangle=-45.0 fontsize=8]"
	   << ";" << endl;
    }
    else
    {
      cout << "    " << firstNodeName() 
           << " -> " << firstNodeName() << "Case" << i  
	   // << " [taillabel=\"false\" labelangle=-45.0  fontsize=8]"
	   << ";" << endl;
    }
    cout << "    " << firstNodeName() << "Case" << i 
         << " -> " << (*iter)->firstNodeName() 
	 << " [taillabel=\"true\" labelangle=-45.0 fontsize=8]"
	 << ";" << endl;
    cout << "    " << (*iter)->lastNodeName()
	 << " -> " << lastNodeName() << ";" << endl;
    i++;
/*
    cout << "    " << firstNodeName() 
         << " -> " << (*iter)->firstNodeName() 
	 << " [label=\"Case " << i++ << "\" fontsize=8]"
	 << ";" << endl;
    cout << "    " << (*iter)->lastNodeName()
	 << " -> " << lastNodeName() << ";" << endl;
*/
  }
  if (otherwise != NULL)
  {
    otherwise->print_dot();
    cout << "    " << firstNodeName() << "Case" << (i - 1) 
         << " -> " << otherwise->firstNodeName()  
	 << " [taillabel=\"otherwise\" labelangle=-45.0 fontsize=8]"
	 << ";" << endl;
    cout << "    " << otherwise->lastNodeName()
	 << " -> " << lastNodeName() << ";" << endl;
  }
/*  
  cout << "  }" << endl;
*/
  dot_nextBlock();
  cout << endl;
}

std::string CFGSwitch::firstNodeName()
{
  return type + label + "entry";
}

std::string CFGSwitch::lastNodeName()
{
  return type + label + "exit";
}


/******************************************************************************
  WHILE
******************************************************************************/

CFGWhile::CFGWhile(std::string id)
{
  type = "While";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;

  loopActivity = NULL;
}

CFGWhile::CFGWhile(kc::integer id)
{
  type = "While";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;

  loopActivity = NULL;
}

CFGWhile::~CFGWhile()
{
  if (loopActivity != NULL)
  {
    delete(loopActivity);
  }
}

void CFGWhile::print_dot()
{
  cout << endl;
/*
  cout << "  subgraph cluster" << type << label << " {" << endl;
  cout << "    label=\"" << type << " (" << label << ")\";" << endl;
  cout << "    labelangle=90.0;" << endl;
  cout << "    labeljust=l;" << endl;
  cout << "    color=blue;" << endl;
  cout << "    style=dotted;" << endl;
  cout << "    node [shape=box];" << endl;
*/
  cout << "    " << firstNodeName() << " [label=\"" << type << " (" << label << "_cond)\"];" << endl;

  int i = 1;
  if (loopActivity != NULL)
  {
    loopActivity->print_dot();
    cout << "    " << firstNodeName() 
         << " -> " << loopActivity->firstNodeName()
	 << " [taillabel=\"true\" labelangle=-45.0 fontsize=\"8\"]"
	 << ";" << endl;
    cout << "    " << loopActivity->lastNodeName()
	 << " -> " << lastNodeName() << ";" << endl;
  }
/*  
  cout << "  }" << endl;
*/
  dot_nextBlock();
  cout << endl;
}

std::string CFGWhile::firstNodeName()
{
  return type + label + "cond";
}

std::string CFGWhile::lastNodeName()
{
  return type + label + "cond";
}


/******************************************************************************
  SEQUENCE
******************************************************************************/

CFGSequence::CFGSequence(std::string id)
{
  type = "Sequence";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGSequence::CFGSequence(kc::integer id)
{
  type = "Sequence";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGSequence::~CFGSequence()
{
  if (!activityList.empty()) 
  {
    delete(*(activityList.begin()));
  }
}

void CFGSequence::print_dot()
{
  cout << endl;
/*
  cout << "  subgraph cluster" << type << label << " {" << endl;
  cout << "    label=\"" << type << " (" << label << ")\";" << endl;
  cout << "    labelangle=90.0;" << endl;
  cout << "    labeljust=l;" << endl;
  cout << "    color=blue;" << endl;
  cout << "    style=dotted;" << endl;
*/
  if(!activityList.empty())
  {
    (*(activityList.begin()))->print_dot();
  }
/*
  for(list<CFGBlock *>::iterator iter = activityList.begin(); iter != activityList.end(); iter++)
  {
    (*iter)->print_dot();
    if ((*iter)->prevBlock != NULL)
    {
      cout << "    " << (*iter)->prevBlock->lastNodeName() 
	   << " -> " << (*iter)->firstNodeName() << ";" << endl;
    }
  }
*/
/*  
  cout << "  }" << endl;
*/
  dot_nextBlock();
  cout << endl;
}

std::string CFGSequence::firstNodeName()
{
  if (!activityList.empty())
  {
    return ((*(activityList.begin()))->firstNodeName());
  }
  else
  {
    return "gaga";
  }
}

std::string CFGSequence::lastNodeName()
{
  if (!activityList.empty())
  {
    return ((*(--(activityList.end())))->lastNodeName());
  }
  else
  {
    return "gaga";
  }
}


/******************************************************************************
  PICK
******************************************************************************/

CFGPick::CFGPick(std::string id)
{
  type = "Pick";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGPick::CFGPick(kc::integer id)
{
  type = "Pick";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGPick::~CFGPick()
{
  if (!messageList.empty()) 
  {
    for (list<CFGBlock *>::iterator iter = messageList.begin(); iter != messageList.end(); iter++)
    {
      delete(*(iter));
    }
  }
  if (!alarmList.empty()) 
  {
    for (list<CFGBlock *>::iterator iter = alarmList.begin(); iter != alarmList.end(); iter++)
    {
      delete(*(iter));
    }
  }
}

void CFGPick::print_dot()
{
  cout << endl;
/*
  cout << "  subgraph cluster" << type << label << " {" << endl;
  cout << "    label=\"" << type << " (" << label << ")\";" << endl;
  cout << "    labelangle=90.0;" << endl;
  cout << "    labeljust=l;" << endl;
  cout << "    color=blue;" << endl;
  cout << "    style=dotted;" << endl;
  cout << "    node [shape=box];" << endl;
*/
  cout << "    " << firstNodeName() << " [label=\"" << type << " (" << label << "_entry)\"];" << endl;
  cout << "    " << lastNodeName()  << " [label=\"" << type << " (" << label << "_exit)\"];" << endl;

  int i = 1;
  for(list<CFGBlock *>::iterator iter = messageList.begin(); iter != messageList.end(); iter++)
  {
    (*iter)->print_dot();
    cout << "    " << firstNodeName() 
         << " -> " << (*iter)->firstNodeName() 
	 << " [label=\"onMessage " << i++ << "\" fontsize=8]"
	 << ";" << endl;
    cout << "    " << (*iter)->lastNodeName()
	 << " -> " << lastNodeName() << ";" << endl;
  }
  i = 1;
  for(list<CFGBlock *>::iterator iter = alarmList.begin(); iter != alarmList.end(); iter++)
  {
    (*iter)->print_dot();
    cout << "    " << firstNodeName() 
         << " -> " << (*iter)->firstNodeName() 
	 << " [label=\"onAlarm " << i++ << "\" fontsize=8]"
	 << ";" << endl;
    cout << "    " << (*iter)->lastNodeName()
	 << " -> " << lastNodeName() << ";" << endl;
  }
/*  
  cout << "  }" << endl;
*/
  dot_nextBlock();
  cout << endl;
}

std::string CFGPick::firstNodeName()
{
  return type + label + "entry";
}

std::string CFGPick::lastNodeName()
{
  return type + label + "exit";
}


/******************************************************************************
  SCOPE
******************************************************************************/

CFGScope::CFGScope(std::string id)
{
  type = "Scope";
  label = id;
  prevBlock = NULL;
  nextBlock = NULL;
}

CFGScope::CFGScope(kc::integer id)
{
  type = "Scope";
  label = intToString(id->value);
  prevBlock = NULL;
  nextBlock = NULL;
}

void CFGScope::print_dot()
{
  cout << endl;
/*
  cout << "  subgraph cluster" << type << label << " {" << endl;
  cout << "    label=\"" << type << " (" << label << ")\";" << endl;
  cout << "    labelangle=90.0;" << endl;
  cout << "    labeljust=l;" << endl;
  cout << "    color=blue;" << endl;
  cout << "    style=dotted;" << endl;
  cout << "    node [shape=box];" << endl;
*/
  cout << "    " << firstNodeName() << " [label=\"" << type << " (" << label << "_entry)\"];" << endl;
  cout << "    " << lastNodeName()  << " [label=\"" << type << " (" << label << "_exit)\"];" << endl;

  if (innerActivity != NULL)
  {
    innerActivity->print_dot();
    cout << "    " << firstNodeName() << " -> " << innerActivity->firstNodeName() << ";" << endl;
    cout << "    " << innerActivity->lastNodeName() << " -> " << lastNodeName() << ";" << endl;
  }
/*  
  cout << "}" << endl;
*/
  dot_nextBlock();
}

/// returns the name of the first node in this block
std::string CFGScope::firstNodeName()
{
  return type + label + "entry";
}

/// returns the name of the last node in this block
std::string CFGScope::lastNodeName()
{
  return type + label + "exit";
}



