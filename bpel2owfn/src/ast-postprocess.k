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
 *          - last changed: \$Date: 2006/07/08 12:15:52 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.13 $
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
/* These lines are from bpel-unparse-ast.k */
#include <iostream>
#include <map>
#include "bpel-unparse-tools.h"
#include "ast-printers.h"
#include "ast-details.h"
#include "symbol-table.h"

using namespace std;

extern map<unsigned int, ASTE*> ASTEmap;


unsigned int PPcurrentFlow = 0;
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
//      STProcess* myself = dynamic_cast<STProcess*> (symTab.lookup($0->id));
//      assert($0->hasEH == myself->hasEventHandler);

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_targetNamespace = ASTEmap[myid]->attributes["targetNamespace"];
      $0->A_queryLanguage = ASTEmap[myid]->attributes["queryLanguage"];
      $0->A_expressionLanguage = ASTEmap[myid]->attributes["expressionLanguage"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_enableInstanceCompensation = ASTEmap[myid]->attributes["enableInstanceCompensation"];
      $0->A_abstractProcess = ASTEmap[myid]->attributes["abstractProcess"];
      $0->A_xmlns = ASTEmap[myid]->attributes["xmlns"];


      ASTEmap[myid]->parentScopeId = myid;
      ASTEmap[myid]->parentActivityId = myid;


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

      // cerr << "read partner link: " << ASTEmap[myid]->attributes["name"] << endl;

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

      // cerr << "<faultHandlers>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

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

      // these 3 asserts show that the AST can be annotated right here
//      assert($0->hasCatchAll == (b->op_name() == "CatchAll"));
      assert($0->parentScopeId == ASTEmap[myid]->parentScopeId);
      assert($0->inProcess == (ASTEmap[myid]->parentScopeId == 1));
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

      // cerr << "<faultHandlers>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // these 3 asserts show that the AST can be annotated right here
//      assert($0->hasCatchAll == false);
      assert($0->parentScopeId == ASTEmap[myid]->parentScopeId);
      assert($0->inProcess == (ASTEmap[myid]->parentScopeId == 1));
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
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // this assert shows that the AST can be annotated right here
      assert($0->parentScopeId == ASTEmap[myid]->parentScopeId);    
    }  
  ]
;

userDefinedEventHandler(a,b) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // this assert shows that the AST can be annotated right here
      assert($0->parentScopeId == ASTEmap[myid]->parentScopeId);    
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
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // cerr << "<compensationHandler>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

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

      // cerr << "\tread variable: " << PPcurrentScope << "." << ASTEmap[myid]->attributes["name"] << endl;

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
     }
  ]
;


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<empty>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

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

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<invoke>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
//      assert(a->parentId == myid);
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

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<receive>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
//      assert(a->parentId == myid);
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

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<reply>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
//      assert(a->parentId == myid);
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
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<assign>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
//      assert(a->parentId == myid);
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
    }
  ]
;



/******************************************************************************
  WAIT
******************************************************************************/

WaitFor(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_for = ASTEmap[myid]->attributes["for"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<wait>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
//      assert(a->parentId == myid);
    }
    a
  ]
;

WaitUntil(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_until = ASTEmap[myid]->attributes["until"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<wait>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
//      assert(a->parentId == myid);
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

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<throw>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
//      assert(a->parentId == myid);
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
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_scope = ASTEmap[myid]->attributes["scope"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<compensate>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
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
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<terminate>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
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
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;
      PPcurrentFlow = myid; //TODO reset

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<flow>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the flow's id to the enclosed activities
      // also distribute the control flow type
      foreach(innerActivity; activity_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b c
  ]
;

Link() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];

      // cerr << "\tread link: " << PPcurrentFlow << "." << ASTEmap[myid]->attributes["name"] << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  SWITCH
******************************************************************************/

Switch(a,b,c=Otherwise(d)) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<switch>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

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

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b c
  ]
;

Case(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_condition = ASTEmap[myid]->attributes["condition"];

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
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_condition = ASTEmap[myid]->attributes["condition"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<while>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the while's id to the enclosed activity
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[b->id]->parentActivityId = myid;
      ASTEmap[b->id]->controlFlow = ASTEmap[myid]->controlFlow;

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b
  ]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<sequence>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the sequence's id to the enclosed activities
      foreach(innerActivity; activity_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
    }
    a b
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<pick>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

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

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      assert(a->parentId == myid);
    }
    a b c
  ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      PPcurrentScope = myid;

      $0->hasEH = (string(e->op_name()) == "userDefinedEventHandler");
//      STScope* myself = dynamic_cast<STScope*> (symTab.lookup($0->id));
//      assert($0->hasEH == myself->hasEventHandler);

      // copy attributes to AST
      $0->A_name = ASTEmap[myid]->attributes["name"];
      $0->A_joinCondition = ASTEmap[myid]->attributes["joinCondition"];
      $0->A_suppressJoinFailure = ASTEmap[myid]->attributes["suppressJoinFailure"];
      $0->A_variableAccessSerializable = ASTEmap[myid]->attributes["variableAccessSerializable"];

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<scope>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the scope's id to the enclosed activity
      assert(ASTEmap[g->id] != NULL);
      ASTEmap[g->id]->parentActivityId = myid;

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow);
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      assert(a->parentId == myid);
    }
    a b c d e f g
    {
      PPcurrentScope = ASTEmap[myid]->parentScopeId;
      assert($0->parentScopeId == ASTEmap[myid]->parentScopeId);
    }
  ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

StandardElements(a,b,c) ->
  [postprocessing:
    {
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
  ]
;

Target() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // cerr << "<target>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;
      // cerr << "\tread target link: " << ASTEmap[myid]->attributes["linkName"] << endl;
    }
  ]
;

Source() ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // cerr << "<source>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;
      // cerr << "\tread source link: " << ASTEmap[myid]->attributes["linkName"] << endl;
    }
  ]
;

