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

%{
/*!
 * \file bpel-unparse-cfg.cc
 *
 * \brief CFG unparser (implementation)
 *
 * This file defines and implements the conversion of each BPEL activity to its
 * associated control flow block.
 * 
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date    
 *          - created 2006-01-19
 *          - last changed: \$Date: 2006/03/10 15:54:17 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-cfg.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.6 $
 */
%}


%{	
/*!
 * \file bpel-unparse-cfg.h
 * \brief CFG unparser (interface)
 *
 * See \ref bpel-unparse-cfg.cc for more information.
 */
%}


%uview cfg;

/******************************************************************************/

// All the includes, variables and structs defined here can be used in the
// printers below.
%{ KC_UNPARSE

#include "cfg.h"
#include "helpers.h"
#include <map>

extern CFGBlock * TheCFG;

map <std::string, CFGBlock *> cfgMap;

%}

/******************************************************************************/


/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,c,d,e,f,g,h,i) ->
  [cfg: {
	  trace(TRACE_DEBUG, "[CFG] Process\n");
	  CFGBlock * firstBlock = new CFGBlock(CFGProcess, $0->id, "Process_begin"); 
	  CFGBlock * lastBlock = new CFGBlock(CFGProcess, $0->id, "Process_end");

	  firstBlock->firstBlock = lastBlock->firstBlock = firstBlock;
	  firstBlock->lastBlock  = lastBlock->lastBlock  = lastBlock;
	  
	  TheCFG = firstBlock;
	  cfgMap[intToString($0->id->value)] = TheCFG; 
	}
        i //activity
	{
	  CFGBlock * innerFirst = cfgMap.find(intToString(i->id->value))->second->firstBlock;
	  CFGBlock * innerLast = cfgMap.find(intToString(i->id->value))->second->lastBlock;
	  
	  connectBlocks(firstBlock, innerFirst);
	  connectBlocks(innerLast, lastBlock);
	}
  ];


/******************************************************************************
  PARTNER LINKS
******************************************************************************/


/******************************************************************************
  PARTNERS
******************************************************************************/


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/


/******************************************************************************
  VARIABLES
******************************************************************************/


/******************************************************************************
  CORRELATION SETS
******************************************************************************/


/******************************************************************************
  CORRELATIONS
******************************************************************************/


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Empty\n");
	  CFGBlock * block = new CFGBlock(CFGEmpty, $0->id, "Empty"); 

	  cfgMap[intToString($0->id->value)] = block; 
	}
	a // standard elements
  ];

  
/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Invoke\n");
	  CFGBlock * block = new CFGBlock(CFGInvoke, $0->id, "Invoke");

	  block->channel_name = $0->channelID->name; 

	  cfgMap[intToString($0->id->value)] = block; 
  	}
	a // standard elements
  ];



/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Receive\n");
	  CFGBlock * block = new CFGBlock(CFGReceive, $0->id, "Receive"); 

	  block->channel_name = $0->channelID->name; 

	  cfgMap[intToString($0->id->value)] = block; 
    	}
	a // standard elements
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Reply\n");
	  CFGBlock * block = new CFGBlock(CFGReply, $0->id, "Reply"); 

	  block->channel_name = $0->channelID->name; 

	  cfgMap[intToString($0->id->value)] = block; 
  	}
	a // standard elements
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b) ->
  [cfg: {
	  trace(TRACE_DEBUG, "[CFG] Assign\n");
	  CFGBlock * firstBlock = new CFGBlock(CFGAssign, $0->id, "Assign_begin"); 
	  CFGBlock * lastBlock = new CFGBlock(CFGAssign, $0->id, "Assign_end");

	  firstBlock->firstBlock = lastBlock->firstBlock = firstBlock;
	  firstBlock->lastBlock  = lastBlock->lastBlock  = lastBlock;
	  
	  cfgMap[intToString($0->id->value)] = firstBlock; 

	  CFGBlock * last = firstBlock;
	  int i = 1;
	  foreach(copy; tCopy_list b)
	  {
	    CFGBlock * copyBlock = new CFGBlock(CFGCopy, $0->id, "Copy_" + intToString(i++));
	    connectBlocks(last, copyBlock);
	    last = copyBlock;
	  } 
	  connectBlocks(last, lastBlock);
  	}
	a // standard elements
  ]
;



/******************************************************************************
  WAIT
******************************************************************************/

Wait(a) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Wait\n");
	  CFGBlock * block = new CFGBlock(CFGWait, $0->id, "Wait"); 

	  cfgMap[intToString($0->id->value)] = block; 
  	} 
	a // standard elements
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Throw\n");
	  CFGBlock * block = new CFGBlock(CFGThrow, $0->id, "Throw"); 

	  cfgMap[intToString($0->id->value)] = block; 
  	}
	a // standard elements
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Compensate\n");
	  CFGBlock * block = new CFGBlock(CFGCompensate, $0->id, "Compensate"); 

	  cfgMap[intToString($0->id->value)] = block; 
  	}
	a // standard elements
  ]
;


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Terminate\n");
	  CFGBlock * block = new CFGBlock(CFGTerminate, $0->id, "Terminate"); 

	  cfgMap[intToString($0->id->value)] = block; 
  	}
	a // standard elements
  ]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,b,c) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Flow\n");
	  CFGBlock * firstBlock = new CFGBlock(CFGFlow, $0->id, "Flow_begin"); 
	  CFGBlock * lastBlock = new CFGBlock(CFGFlow, $0->id, "Flow_end");

	  firstBlock->firstBlock = lastBlock->firstBlock = firstBlock;
	  firstBlock->lastBlock  = lastBlock->lastBlock  = lastBlock;
	  
	  cfgMap[intToString($0->id->value)] = firstBlock; 
	}
	c
	{
	  foreach(innerActivity; activity_list c)
	  {
 	    CFGBlock * innerFirst = cfgMap.find(intToString(innerActivity->id->value))->second->firstBlock;
	    CFGBlock * innerLast = cfgMap.find(intToString(innerActivity->id->value))->second->lastBlock;
	  
	    connectBlocks(firstBlock, innerFirst);
	    connectBlocks(innerLast, lastBlock);
	  } 
  	}
	a // standard elements
	{
	  trace(TRACE_DEBUG, "[CFG] ~Flow\n");
	}
  ]
;


/******************************************************************************
  SWITCH
******************************************************************************/

This=Switch(a,b,Otherwise(c)) ->
  [cfg: {
	  trace(TRACE_DEBUG, "[CFG] Switch\n");
	  CFGBlock * firstBlock = new CFGBlock(CFGSwitch, $0->id, "Switch_begin"); 
	  CFGBlock * lastBlock = new CFGBlock(CFGSwitch, $0->id, "Switch_end");

	  firstBlock->firstBlock = lastBlock->firstBlock = firstBlock;
	  firstBlock->lastBlock  = lastBlock->lastBlock  = lastBlock;
	  
	  cfgMap[intToString($0->id->value)] = firstBlock; 
	}
	b // tCase_list
	c // tOtherwise
	{
	  CFGBlock * last = firstBlock;
	  int i = 1;
	  foreach(caseBranch=Case(innerActivity); tCase_list b)
	  {
	    CFGBlock * caseBlock = new CFGBlock(CFGCase, This->id, "Case_" + intToString(i));
	    
 	    CFGBlock * innerFirst = cfgMap.find(intToString(innerActivity->id->value))->second->firstBlock;
	    CFGBlock * innerLast = cfgMap.find(intToString(innerActivity->id->value))->second->lastBlock;

	    connectBlocks(last, caseBlock);
	    connectBlocks(caseBlock, innerFirst);
	    connectBlocks(innerLast, lastBlock);

	    last = caseBlock;	    
	    i++;
	  } 
          CFGBlock * otherwiseBlock = new CFGBlock(CFGOtherwise, $0->id, "Otherwise");
 	  CFGBlock * innerFirst = cfgMap.find(intToString(c->id->value))->second->firstBlock;
	  CFGBlock * innerLast = cfgMap.find(intToString(c->id->value))->second->lastBlock;

	  connectBlocks(last, otherwiseBlock);
	  connectBlocks(otherwiseBlock, innerFirst);
	  connectBlocks(innerLast, lastBlock);
  	}
	a // standard elements
  ]
;


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] While\n");
	  CFGBlock * block = new CFGBlock(CFGWhile, $0->id, "While"); 

	  cfgMap[intToString($0->id->value)] = block; 
  	}
	b
	{
	  CFGBlock * innerFirst = cfgMap.find(intToString(b->id->value))->second->firstBlock;
	  CFGBlock * innerLast = cfgMap.find(intToString(b->id->value))->second->lastBlock;
	  
	  connectBlocks(block, innerFirst);
	  connectBlocks(innerLast, block);
	}
	a // standard elements
  ]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b) ->
  [cfg: {
	  trace(TRACE_DEBUG, "[CFG] Sequence\n");
	  CFGBlock * firstBlock = new CFGBlock(CFGSequence, $0->id, "Sequence_begin"); 
	  CFGBlock * lastBlock = new CFGBlock(CFGSequence, $0->id, "Sequence_end");

	  firstBlock->firstBlock = lastBlock->firstBlock = firstBlock;
	  firstBlock->lastBlock  = lastBlock->lastBlock  = lastBlock;
	  
	  cfgMap[intToString($0->id->value)] = firstBlock; 
	}
	b
	{
	  CFGBlock * last = firstBlock;
	  foreach(innerActivity; activity_list b)
	  {
 	    CFGBlock * current = cfgMap.find(intToString(innerActivity->id->value))->second;
	  
	    connectBlocks(last, current->firstBlock);
	    last = current->lastBlock;
	  } 
	  connectBlocks(last, lastBlock);
  	}
	a // standard elements
	{
	  trace(TRACE_DEBUG, "[CFG] ~Sequence\n");
	}
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Pick\n");
	  CFGBlock * firstBlock = new CFGBlock(CFGPick, $0->id, "Pick_begin"); 
	  CFGBlock * lastBlock = new CFGBlock(CFGPick, $0->id, "Pick_end");

	  firstBlock->firstBlock = lastBlock->firstBlock = firstBlock;
	  firstBlock->lastBlock  = lastBlock->lastBlock  = lastBlock;
	  
	  cfgMap[intToString($0->id->value)] = firstBlock; 
	}
	b // tOnMessage_list
	c // tOnAlarm_list
	{
	  foreach(message=OnMessage(innerActivity); tOnMessage_list b)
	  {
 	    CFGBlock * innerFirst = cfgMap.find(intToString(innerActivity->id->value))->second->firstBlock;
	    CFGBlock * innerLast = cfgMap.find(intToString(innerActivity->id->value))->second->lastBlock;
	  
	    connectBlocks(firstBlock, innerFirst);
	    connectBlocks(innerLast, lastBlock);
	  } 
	  foreach(alarm=OnAlarm(innerActivity); tOnAlarm_list c)
	  {
 	    CFGBlock * innerFirst = cfgMap.find(intToString(innerActivity->id->value))->second->firstBlock;
	    CFGBlock * innerLast = cfgMap.find(intToString(innerActivity->id->value))->second->lastBlock;
	  
	    connectBlocks(firstBlock, innerFirst);
	    connectBlocks(innerLast, lastBlock);
	  } 
  	}
	a // standard elements
  ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g) ->
  [cfg: { 
	  trace(TRACE_DEBUG, "[CFG] Scope\n");
	  CFGBlock * firstBlock = new CFGBlock(CFGScope, $0->id, "Scope_begin"); 
	  CFGBlock * lastBlock = new CFGBlock(CFGScope, $0->id, "Scope_end");

	  firstBlock->firstBlock = lastBlock->firstBlock = firstBlock;
	  firstBlock->lastBlock  = lastBlock->lastBlock  = lastBlock;
	  
	  cfgMap[intToString($0->id->value)] = firstBlock; 
	}
	g // activity
	{
	  CFGBlock * innerFirst = cfgMap.find(intToString(g->id->value))->second->firstBlock;
	  CFGBlock * innerLast = cfgMap.find(intToString(g->id->value))->second->lastBlock;
	  
	  connectBlocks(firstBlock, innerFirst);
	  connectBlocks(innerLast, lastBlock);
	}
	a // standard elements
	{
	  trace(TRACE_DEBUG, "[CFG] ~Scope\n");
	}
  ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

This=StandardElements(a,b) ->
  [cfg:
    {

      CFGBlock * innerFirst = cfgMap.find(intToString(This->parentId->value))->second->firstBlock;
      CFGBlock * innerLast = cfgMap.find(intToString(This->parentId->value))->second->lastBlock;
      foreach (link; tTarget_list a)
      {
	  trace(TRACE_DEBUG, "[CFG] Target\n");
	  CFGBlock * target = new CFGBlock(CFGTarget, $0->parentId, "Target_" + std::string(link->linkName->name));
	  /// \todo: use link to connect source and target
/*	  for (list<CFGBlock *>::iterator iter = innerFirst->prevBlocks.begin(); iter != innerFirst->prevBlocks.end(); iter++)
	  {
	    // really necessary?
	    (*iter)->nextBlocks.remove(innerFirst);
	    innerFirst->prevBlocks.remove((*iter));
	    connectBlocks((*iter), target);
	  }
*/	  connectBlocks(target, innerFirst->firstBlock);
	  innerFirst->firstBlock = target;
	  target->lastBlock = innerLast->lastBlock;
      }

      foreach (link; tSource_list b)
      {
	  trace(TRACE_DEBUG, "[CFG] Source\n");
	  CFGBlock * source = new CFGBlock(CFGSource, $0->parentId, "Source_" + std::string(link->linkName->name));
	  /// \todo: use link to connect source and target

/*	  for (list<CFGBlock *>::iterator iter = innerLast->nextBlocks.begin(); iter != innerLast->nextBlocks.end(); iter++)
	  {
	    trace("i");
	    // really necessary?
	    CFGBlock * next = (*iter);
	    next->prevBlocks.remove(innerLast);
	    innerLast->nextBlocks.remove(next);
	    connectBlocks(source, next);
	  }
*/
	  connectBlocks(innerLast->lastBlock, source);
	  innerLast->lastBlock = innerFirst->lastBlock = source;
	  // trace("set last block again\n");
	  innerFirst->firstBlock->lastBlock = source;
	  trace(TRACE_DEBUG, "[CFG] ~Source\n");

      }


    }
  ]
;


