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
 * \file bpel-unparse-ast.cc
 *
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date    
 *          - created: 2006-07-02
 *          - last changed: \$Date: 2006/07/11 06:53:04 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.19 $
 */
%}


%{	
/*!
 * \file bpel-unparse-ast.h
 *
 * See \ref bpel-unparse-ast.cc for more information.
 */
%}


/******************************************************************************/

// All the includes and variables can be used during the unparsing.
%{ KC_UNPARSE
#include <iostream>
#include <map>
#include <set>
#include "bpel-unparse-tools.h"
#include "ast-printers.h"
#include "ast-details.h"
#include "symbol-table.h"

using namespace std;

extern map<unsigned int, ASTE*> ASTEmap;

// global sets
set<string> ASTE_inputChannels;
set<string> ASTE_outputChannels;
set<string> ASTE_variables;
set<string> ASTE_links;


map<string, int> linkIdMap;

map<int, int> PPhasCompensateMap;

unsigned int PPcurrentScope = 0;
%}






/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,c,d,e,f,g,h,i) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // set suppressJoinFailure-attribute
      if (ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      $0->hasEH = (string(g->op_name()) == "userDefinedEventHandler");

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_targetNamespace = ASTEmap[myid]->attributes["targetNamespace"];
      $0->A_queryLanguage = ASTEmap[myid]->attributes["queryLanguage"];
      $0->A_expressionLanguage = ASTEmap[myid]->attributes["expressionLanguage"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_enableInstanceCompensation = ASTEmap[myid]->attributes["enableInstanceCompensation"];
      $0->A_abstractProcess = ASTEmap[myid]->attributes["abstractProcess"];
      $0->A_xmlns = ASTEmap[myid]->attributes["xmlns"];
      /* here we can check the attributes */


      ASTEmap[myid]->parentScopeId = myid;
      ASTEmap[myid]->parentActivityId = myid;


      // tag the (useless) compensation handler
      f->inProcess = true;


      // distribute the process' id to the enclosed FH/CH/EH/activity
      assert(ASTEmap[e->id] != NULL);
      ASTEmap[e->id]->parentActivityId = myid;
      assert(ASTEmap[f->id] != NULL);
      ASTEmap[f->id]->parentActivityId = myid;
      assert(ASTEmap[g->id] != NULL);
      ASTEmap[g->id]->parentActivityId = myid;
      assert(ASTEmap[i->id] != NULL);
      ASTEmap[i->id]->parentActivityId = myid;

      PPcurrentScope = myid;
    }
    a b c d e f g h i
    {
      // collect enclosed source links
      assert(ASTEmap[i->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks = ASTEmap[i->id]->enclosedSourceLinks;

      // add an attribute to the compensation handler
      if(PPhasCompensateMap[myid] == 1)
        f->hasCompensateWithoutScope = true;
      if(PPhasCompensateMap[myid] == 2)
        f->hasCompensateWithScope = true;

//      cerr << myid << " has " << ASTEmap[myid]->enclosedSourceLinks.size() << " indirectly enclosed source links" << endl;
    }
  ]
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

PartnerLink() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  PARTNERS
******************************************************************************/

Partner(a) ->
  [postprocessing:
    {
      foreach(partner; tPartnerLink_list a)
      {
        partner->inPartners = true;
      }
    }
    a
  ]
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

userDefinedFaultHandler(a,b) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->hasCatchAll = (b->op_name() == "CatchAll");
      $0->parentScopeId = PPcurrentScope;
      $0->inProcess = (ASTEmap[myid]->parentScopeId == 1);

      // distribute the fault handlers's id to the enclosed catch/catchAll branches
      // also mark the activities as "inside fault handlers"
      foreach(branch=Catch(innerActivity); tCatch_list a)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = FHCF;
      }
//      assert(ASTEmap[b->id] != NULL); /TODO!
//      ASTEmap[b->id]->parentActivityId = myid;
    }
    a b
  ]
;

implicitFaultHandler() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->hasCatchAll = false;
      $0->parentScopeId = PPcurrentScope;
      $0->inProcess = (ASTEmap[myid]->parentScopeId == 1);
    }
  ]
;


Catch(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_faultName = ASTEmap[myid]->attributes["faultName"];
      $0->A_faultVariable = ASTEmap[myid]->attributes["faultVariable"];
      /* here we can check the attributes */
    }
    a
  ]
;

CatchAll(a) ->
  [postprocessing:
    a
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
    }
  ]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

implicitEventHandler() ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      $0->parentScopeId = PPcurrentScope;
    }  
  ]
;

userDefinedEventHandler(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      $0->parentScopeId = PPcurrentScope;

      // distribute the event handlers's id to the enclosed activities
      foreach(branch=OnMessage(innerActivity); tOnMessage_list a)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
    }
    a b
  ]
;

OnMessage(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_partnerLink = ASTEmap[myid]->attributes["partnerLink"];
      $0->A_portType = ASTEmap[myid]->attributes["portType"];
      $0->A_operation = ASTEmap[myid]->attributes["operation"];
      $0->A_variable = ASTEmap[myid]->attributes["variable"];
      /* here we can check the attributes */

      // create a channel
//      $0->channelName = $0->A_partnerLink + "." + $0->A_portType + "." + $0->A_operation;
      $0->channelName = $0->A_operation; //TODO
      ASTE_inputChannels.insert($0->channelName);


      $0->variableName = $0->A_variable;
    }
    a
  ]
;

OnAlarm(a) ->
  [postprocessing: a]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

userDefinedCompensationHandler(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      $0->parentScopeId = PPcurrentScope;

      // distribute the compensation handlers's id to the enclosed activity
      // also mark the activities as "inside compensation handlers"
      assert(ASTEmap[a->id] != NULL);
      ASTEmap[a->id]->parentActivityId = myid;
      ASTEmap[a->id]->controlFlow = CHCF;
    }
    a
  ]
;


/******************************************************************************
  VARIABLES
******************************************************************************/

Variable() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_messageType = ASTEmap[myid]->attributes["messageType"];
      $0->A_type = ASTEmap[myid]->attributes["type"];
      $0->A_element = ASTEmap[myid]->attributes["element"];
      /* here we can check the attributes */

      // register this variable
//TODO      assert($0->A_name != "");
      /* here we can check whether this variable was not defined before */
      ASTE_variables.insert($0->A_name);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_properties = ASTEmap[myid]->attributes["properties"];
      /* here we can check the attributes */
     }
  ]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_set = ASTEmap[myid]->attributes["set"];
      $0->A_initiate = ASTEmap[myid]->attributes["initiate"];
      /* here we can check the attributes */
     }
  ]
;


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_partnerLink = ASTEmap[myid]->attributes["partnerLink"];
      $0->A_portType = ASTEmap[myid]->attributes["portType"];
      $0->A_operation = ASTEmap[myid]->attributes["operation"];
      $0->A_inputVariable = ASTEmap[myid]->attributes["inputVariable"];
      $0->A_outputVariable = ASTEmap[myid]->attributes["outputVariable"];
      /* here we can check the attributes */

      // switch to discriminate asynchronous and synchronous invokes
      $0->isAsynchronousInvoke = ($0->A_inputVariable == "");

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }
      
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // create a channel
//      $0->channelName = $0->A_partnerLink + "." + $0->A_portType + "." + $0->A_operation;
      $0->channelName = $0->A_operation; //TODO
      ASTE_outputChannels.insert($0->channelName);
      if ($0->isAsynchronousInvoke)
        ASTE_inputChannels.insert($0->channelName);

      $0->inputVariableName = $0->A_inputVariable;
      $0->outputVariableName = $0->A_outputVariable;
    }
    a b
  ]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_partnerLink = ASTEmap[myid]->attributes["partnerLink"];
      $0->A_portType = ASTEmap[myid]->attributes["portType"];
      $0->A_operation = ASTEmap[myid]->attributes["operation"];
      $0->A_variable = ASTEmap[myid]->attributes["variable"];
      $0->A_createInstance = ASTEmap[myid]->attributes["createInstance"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // create a channel
//      $0->channelName = $0->A_partnerLink + "." + $0->A_portType + "." + $0->A_operation;
      $0->channelName = $0->A_operation; //TODO
      ASTE_inputChannels.insert($0->channelName);

      $0->variableName = $0->A_variable;
    }
    a b
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_partnerLink = ASTEmap[myid]->attributes["partnerLink"];
      $0->A_portType = ASTEmap[myid]->attributes["portType"];
      $0->A_operation = ASTEmap[myid]->attributes["operation"];
      $0->A_variable = ASTEmap[myid]->attributes["variable"];
      $0->A_faultName = ASTEmap[myid]->attributes["faultName"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // create a channel
//      $0->channelName = $0->A_partnerLink + "." + $0->A_portType + "." + $0->A_operation;
      $0->channelName = $0->A_operation; //TODO
      ASTE_outputChannels.insert($0->channelName);


      $0->variableName = $0->A_variable;
    }
    a b
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b
  ]
;

Copy(a,b) ->
  [postprocessing: a b]
;

From() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_variable = ASTEmap[myid]->attributes["variable"];
      $0->A_part = ASTEmap[myid]->attributes["part"];
      $0->A_partnerLink = ASTEmap[myid]->attributes["partnerLink"];
      $0->A_endpointReference = ASTEmap[myid]->attributes["endpointReference"];
      $0->A_property = ASTEmap[myid]->attributes["property"];
      $0->A_expression = ASTEmap[myid]->attributes["expression"];
      /* here we can check the attributes */

      $0->variableName = $0->A_variable;
    }
  ]
;

To() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_variable = ASTEmap[myid]->attributes["variable"];
      $0->A_part = ASTEmap[myid]->attributes["part"];
      $0->A_partnerLink = ASTEmap[myid]->attributes["partnerLink"];
      $0->A_property = ASTEmap[myid]->attributes["property"];
      /* here we can check the attributes */

      $0->variableName = $0->A_variable;
    }
  ]
;



/******************************************************************************
  WAIT
******************************************************************************/

Wait(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_for = ASTEmap[myid]->attributes["for"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // switch to discriminate waitFor and waitUntil
      $0->isWaitFor = ($0->A_for != "");
    }
    a
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_faultName = ASTEmap[myid]->attributes["faultName"];
      $0->A_faultVariable = ASTEmap[myid]->attributes["faultVariable"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_scope = ASTEmap[myid]->attributes["scope"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // register this compensate at the current scope
      /* here we can check whether it is the only compensate activity */
      if ($0->A_scope == "")
        PPhasCompensateMap[PPcurrentScope] = 1;
      else
        PPhasCompensateMap[PPcurrentScope] = 2;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a
  ]
;


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a
  ]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,b,c) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // distribute the flow's id to the enclosed activities
      // also distribute the control flow type
      foreach(innerActivity; activity_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      foreach(link; tLink_list b)
      {
        assert(ASTEmap[link->id] != NULL);
        ASTEmap[link->id]->parentActivityId = myid;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b c
    {
      // collect enclosed source links
      foreach(innerActivity; activity_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks.splice(ASTEmap[myid]->enclosedSourceLinks.end(), ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
    }
  ]
;

Link() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      /* here we can check the attributes */

      /* here we can check whether this link was not defined before */
      linkIdMap[$0->A_name] = myid;

      // cerr << "defined a link " << $0->A_name << " (" << myid << ")" << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      ASTE_links.insert($0->A_name);
    }
  ]
;


/******************************************************************************
  SWITCH
******************************************************************************/

Switch(a,b,c=Otherwise(d)) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // distribute the switch's id to the enclosed activities
      // also distribute the control flow
      foreach(branch=Case(innerActivity); tCase_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      assert(ASTEmap[d->id] != NULL);
      ASTEmap[d->id]->parentActivityId = myid;
      ASTEmap[d->id]->controlFlow = ASTEmap[myid]->controlFlow;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b c
    {
      // collect enclosed source links
      foreach(branch=Case(innerActivity); tCase_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks.splice(ASTEmap[myid]->enclosedSourceLinks.end(), ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
      assert(ASTEmap[d->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks.splice(ASTEmap[myid]->enclosedSourceLinks.end(), ASTEmap[d->id]->enclosedSourceLinks);
    }
  ]
;

Case(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_condition = ASTEmap[myid]->attributes["condition"];
      /* here we can check the attributes */

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
    a
  ]
;

Otherwise(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
    a
  ]
;


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_condition = ASTEmap[myid]->attributes["condition"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // distribute the while's id to the enclosed activity
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[b->id]->parentActivityId = myid;
      ASTEmap[b->id]->controlFlow = ASTEmap[myid]->controlFlow;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b
    {
      // collect enclosed source links
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks.splice(ASTEmap[myid]->enclosedSourceLinks.end(), ASTEmap[b->id]->enclosedSourceLinks);
    }
  ]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // distribute the sequence's id to the enclosed activities
      foreach(innerActivity; activity_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b
    {
      // collect enclosed source links
      foreach(innerActivity; activity_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks.splice(ASTEmap[myid]->enclosedSourceLinks.end(), ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
    }
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // distribute the pick's id to the enclosed activities
      foreach(branch=OnMessage(innerActivity); tOnMessage_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b c
    {
      // collect enclosed source links
      foreach(branch=OnMessage(innerActivity); tOnMessage_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks.splice(ASTEmap[myid]->enclosedSourceLinks.end(), ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks.splice(ASTEmap[myid]->enclosedSourceLinks.end(), ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
    }
  ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      PPcurrentScope = myid;
      a->parentId = myid;

      $0->hasEH = (string(e->op_name()) == "userDefinedEventHandler");

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_variableAccessSerializable = ASTEmap[myid]->attributes["variableAccessSerializable"];
      /* here we can check the attributes */

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // distribute the scope's id to the enclosed activity
      assert(ASTEmap[g->id] != NULL);
      ASTEmap[g->id]->parentActivityId = myid;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b c d e f g
    {
      PPcurrentScope = ASTEmap[myid]->parentScopeId;
      assert($0->parentScopeId == ASTEmap[myid]->parentScopeId);

      // add an attribute to the compensation handler
      if(PPhasCompensateMap[myid] == 1)
        d->hasCompensateWithoutScope = true;
      if(PPhasCompensateMap[myid] == 2)
        d->hasCompensateWithScope = true;

      // collect enclosed source links
      assert(ASTEmap[g->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks.splice(ASTEmap[myid]->enclosedSourceLinks.end(), ASTEmap[g->id]->enclosedSourceLinks);
    }
  ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

StandardElements(a,b,c) ->
  [postprocessing:
    {
      assert(ASTEmap[$0->parentId] != NULL);
      $0->negativeControlFlow = ASTEmap[$0->parentId]->controlFlow;
      $0->suppressJF = ASTEmap[$0->parentId]->suppressJF;

      foreach(targetLink; tTarget_list a)
      {
        assert(ASTEmap[targetLink->id] != NULL);
        ASTEmap[targetLink->id]->controlFlow = ASTEmap[$0->parentId]->controlFlow;
        targetLink->negativeControlFlow = ASTEmap[$0->parentId]->controlFlow;
        ASTEmap[targetLink->id]->parentActivityId = $0->parentId;
      }

      foreach(sourceLink; tSource_list b)
      {
        assert(ASTEmap[sourceLink->id] != NULL);
        ASTEmap[sourceLink->id]->controlFlow = ASTEmap[$0->parentId]->controlFlow;
        sourceLink->negativeControlFlow = ASTEmap[$0->parentId]->controlFlow;
        ASTEmap[sourceLink->id]->parentActivityId = $0->parentId;
      }
    }
    a b c
    {
      /* here we can check whether each link was used as source and target */
    }
  ]
;

Target() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // find the id of the link used as target here
      int linkId = linkIdMap[ASTEmap[myid]->attributes["linkName"]];
      /* here we can check whether this link was defined before */


      // copy attributes to AST
      $0->A_linkName = ASTEmap[myid]->attributes["linkName"];
      /* here we can check the attributes */


      // remember that that link was used as a target
      assert(ASTEmap[linkId] != NULL);
      kc::impl_tLink *theLink = (kc::impl_tLink*)(ASTEmap[linkId]->node);
      assert(theLink != NULL);
      assert(theLink->id == linkId);
      /* here we can check whether this link was not used as target before */
      theLink->usedAsTarget = true;

      $0->linkName = $0->A_linkName;
    }
  ]
;

Source() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // find the id of the link used as source here
      int linkId = linkIdMap[ASTEmap[myid]->attributes["linkName"]];
      /* here we can check whether this link was defined before */


      // copy attributes to AST
      $0->A_linkName = ASTEmap[myid]->attributes["linkName"];
      $0->A_transitionCondition = ASTEmap[myid]->attributes["transitionCondition"];
      $0->A_variableAccessSerializable = ASTEmap[myid]->attributes["variableAccessSerializable"];
      /* here we can check the attributes */


      // remember that that link was used as a source
      assert(ASTEmap[linkId] != NULL);
      kc::impl_tLink *theLink = (kc::impl_tLink*)(ASTEmap[linkId]->node);
      assert(theLink != NULL);
      assert(theLink->id == linkId);
      /* here we can check whether this link was not used as target before */
      theLink->usedAsSource = true;


      // cerr << ASTEmap[myid]->parentActivityId << " has a source link (" << linkId << ")" << endl;


      // add that link to the parent activity
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[ASTEmap[myid]->parentActivityId]->enclosedSourceLinks.push_back(linkId);

      $0->linkName = $0->A_linkName;
    }
  ]
;
