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
 *          - responsible: Christian Gierds <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date    
 *          - created 2006-01-19
 *          - last changed: \$Date: 2006/01/26 10:07:44 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit&auml;t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \note    This file was created using Kimwitu++ version 2.3.8 (C) 1998-2003
 *          Humboldt-University of Berlin reading file bpel-unparse-cfg.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.2 $
 * 
 * \todo
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
//	  trace("[CFG] Process\n"); 
	  TheCFG = new CFGProcess($0->id);
	  cfgMap[intToString($0->id->value)] = TheCFG; 
	}
        i //activity
	{
	  TheCFG->nextBlock = cfgMap.find(intToString(i->id->value))->second;
	  (cfgMap.find(intToString(i->id->value))->second)->prevBlock = TheCFG;
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
	  cfgMap[intToString($0->id->value)] = new CFGEmpty($0->id); 
	}
  ];

  
/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b) ->
  [cfg: { 
	  cfgMap[intToString($0->id->value)] = new CFGInvoke($0->id); 
  	}
  ];



/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b) ->
  [cfg: { 
	  cfgMap[intToString($0->id->value)] = new CFGReceive($0->id); 
    	}
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b) ->
  [cfg: { 
	  cfgMap[intToString($0->id->value)] = new CFGReply($0->id); 
  	}
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b) ->
  [cfg: {
	  CFGAssign * assign = new CFGAssign($0->id);
	  cfgMap[intToString($0->id->value)] = assign;
	  CFGBlock * last = NULL;
	  int i = 1;
	  std::string prefix = intToString($0->id->value);
	  
	  foreach(copy; tCopy_list b)
	  {
	    CFGCopy * copyBlock = new CFGCopy(prefix + "_" + intToString(i++));
	    copyBlock->prevBlock = last;
	    if (last != NULL)
	    {
              last->nextBlock = copyBlock;
	    }
	    assign->copyList.push_back(copyBlock);
	    last = copyBlock;
	  } 
  	}
  ]
;



/******************************************************************************
  WAIT
******************************************************************************/

Wait(a) ->
  [cfg: { 
	  cfgMap[intToString($0->id->value)] = new CFGWait($0->id); 
  	} 
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a) ->
  [cfg: { 
	  cfgMap[intToString($0->id->value)] = new CFGThrow($0->id); 
  	}
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a) ->
  [cfg: { 
	  cfgMap[intToString($0->id->value)] = new CFGCompensate($0->id); 
  	}
  ]
;


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a) ->
  [cfg: { 
	  cfgMap[intToString($0->id->value)] = new CFGTerminate($0->id); 
  	}
  ]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,b,c) ->
  [cfg: { 
	  CFGFlow * flow = new CFGFlow($0->id);
	  cfgMap[intToString($0->id->value)] = flow;
	}
	c
	{
	  foreach(innerActivity; activity_list c)
	  {
	    std::string label = intToString(innerActivity->id->value);
	    CFGBlock * activity = cfgMap.find(label)->second;
	    activity->prevBlock = flow;
	    flow->activityList.push_back(activity);
	  } 
  	}
  ]
;


/******************************************************************************
  SWITCH
******************************************************************************/

Switch(a,b,Otherwise(c)) ->
  [cfg: {
	  CFGSwitch * cfgSwitch = new CFGSwitch($0->id);
	  cfgMap[intToString($0->id->value)] = cfgSwitch;
	}
	b // tCase_list
	c // tOtherwise
	{
	  foreach(caseBranch=Case(innerActivity); tCase_list b)
	  {
	    std::string label = intToString(innerActivity->id->value);
	    CFGBlock * activity = cfgMap.find(label)->second;
	    activity->prevBlock = cfgSwitch;
	    cfgSwitch->caseList.push_back(activity);
	  }
	  std::string label = intToString(c->id->value);
	  CFGBlock * otherwise = cfgMap.find(label)->second;
	  otherwise->prevBlock = cfgSwitch;
	  cfgSwitch->otherwise = otherwise;
  	}
  ]
;


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b) ->
  [cfg: { 
	  CFGWhile * cfgWhile = new CFGWhile($0->id);
	  cfgMap[intToString($0->id->value)] = cfgWhile;
  	}
	b
	{
          std::string label = intToString(b->id->value);
          CFGBlock * activity = cfgMap.find(label)->second;
          activity->prevBlock = cfgWhile;
	  cfgWhile->loopActivity = activity;
	}
  ]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b) ->
  [cfg: {
	  CFGSequence * sequence = new CFGSequence($0->id);
	  cfgMap[intToString($0->id->value)] = sequence;
	  CFGBlock * last = NULL;
  	}
	b
	{	
	  foreach(innerActivity; activity_list b)
	  {
	    std::string label = intToString(innerActivity->id->value);
	    CFGBlock * current = cfgMap.find(label)->second;
	    current->prevBlock = last;
	    if (last != NULL)
	    {
              last->nextBlock = current;
	    }
	    sequence->activityList.push_back(current);
	    last = current;
	  } 
  	}
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c) ->
  [cfg: { 
	  CFGPick * cfgPick = new CFGPick($0->id);
	  cfgMap[intToString($0->id->value)] = cfgPick;
	}
	b // tOnMessage_list
	c // tOnAlarm_list
	{
	  foreach(message=OnMessage(innerActivity); tOnMessage_list b)
	  {
	    std::string label = intToString(innerActivity->id->value);
	    CFGBlock * activity = cfgMap.find(label)->second;
	    activity->prevBlock = cfgPick;
	    cfgPick->messageList.push_back(activity);
	  }
	  foreach(alarm=OnAlarm(innerActivity); tOnAlarm_list c)
	  {
	    std::string label = intToString(innerActivity->id->value);
	    CFGBlock * activity = cfgMap.find(label)->second;
	    activity->prevBlock = cfgPick;
	    cfgPick->alarmList.push_back(activity);
	  }
  	}
  ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g) ->
  [cfg: { 
	  CFGScope * scope = new CFGScope($0->id);
	  cfgMap[intToString($0->id->value)] = scope; 
	}
        g //activity
	{
	  scope->nextBlock = cfgMap.find(intToString(g->id->value))->second;
	  (cfgMap.find(intToString(g->id->value))->second)->prevBlock = scope;
	}
  ]
;


