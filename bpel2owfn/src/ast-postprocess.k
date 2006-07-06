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
 *          - last changed: \$Date: 2006/07/06 11:46:48 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.7 $
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
#include "bpel-unparse-tools.h"
#include "ast-printers.h"
#include <map>
#include "ast-details.h"

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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // set suppressJoinFailure-attribute
      if (ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }


      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_targetNamespace = mkcasestring(ASTEmap[myid]->attributes["targetNamespace"].c_str());
      $0->A_queryLanguage = mkcasestring(ASTEmap[myid]->attributes["queryLanguage"].c_str());
      $0->A_expressionLanguage = mkcasestring(ASTEmap[myid]->attributes["expressionLanguage"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_enableInstanceCompensation = mkcasestring(ASTEmap[myid]->attributes["enableInstanceCompensation"].c_str());
      $0->A_abstractProcess = mkcasestring(ASTEmap[myid]->attributes["abstractProcess"].c_str());
      $0->A_xmlns = mkcasestring(ASTEmap[myid]->attributes["xmlns"].c_str());


      ASTEmap[myid]->parentScopeId = myid;
      ASTEmap[myid]->parentActivityId = myid;


      // distribute the process' id to the enclosed FH/CH/EH/activity
      assert(ASTEmap[e->id->value] != NULL);
      ASTEmap[e->id->value]->parentActivityId = myid;
      assert(ASTEmap[f->id->value] != NULL);
      ASTEmap[f->id->value]->parentActivityId = myid;
      assert(ASTEmap[g->id->value] != NULL);
      ASTEmap[g->id->value]->parentActivityId = myid;
      assert(ASTEmap[i->id->value] != NULL);
      ASTEmap[i->id->value]->parentActivityId = myid;

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
      int myid = $0->id->value;
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
  [postprocessing: a]
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

userDefinedFaultHandler(a,b) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // cerr << "<faultHandlers>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the fault handlers's id to the enclosed catch/catchAll branches
      // also mark the activities as "inside fault handlers"
      foreach(branch=Catch(innerActivity); tCatch_list a)
      {
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
        ASTEmap[innerActivity->id->value]->controlFlow = FHCF;
      }
//      assert(ASTEmap[b->id->value] != NULL); /TODO!
//      ASTEmap[b->id->value]->parentActivityId = myid;

      // these 3 asserts show that the AST can be annotated right here
      assert($0->hasCatchAll == (b->op_name() == "CatchAll"));
      assert($0->parentScopeId->value == ASTEmap[myid]->parentScopeId);
      assert($0->inProcess == (ASTEmap[myid]->parentScopeId == 1));
    }
    a b
  ]
;

implicitFaultHandler() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // cerr << "<faultHandlers>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // these 3 asserts show that the AST can be annotated right here
      assert($0->hasCatchAll == false);
      assert($0->parentScopeId->value == ASTEmap[myid]->parentScopeId);
      assert($0->inProcess == (ASTEmap[myid]->parentScopeId == 1));
    }
  ]
;


Catch(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_faultName = mkcasestring(ASTEmap[myid]->attributes["faultName"].c_str());
      $0->A_faultVariable = mkcasestring(ASTEmap[myid]->attributes["faultVariable"].c_str());
    }
    a
  ]
;

CatchAll(a) ->
  [postprocessing:
    a
    {
      int myid = $0->id->value;
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // this assert shows that the AST can be annotated right here
      assert($0->parentScopeId->value == ASTEmap[myid]->parentScopeId);    
    }  
  ]
;

userDefinedEventHandler(a,b) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // this assert shows that the AST can be annotated right here
      assert($0->parentScopeId->value == ASTEmap[myid]->parentScopeId);    
    }
    a b
  ]
;

OnMessage(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_partnerLink = mkcasestring(ASTEmap[myid]->attributes["partnerLink"].c_str());
      $0->A_portType = mkcasestring(ASTEmap[myid]->attributes["portType"].c_str());
      $0->A_operation = mkcasestring(ASTEmap[myid]->attributes["operation"].c_str());
      $0->A_variable = mkcasestring(ASTEmap[myid]->attributes["variable"].c_str());
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // cerr << "<compensationHandler>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the compensation handlers's id to the enclosed activity
      // also mark the activities as "inside compensation handlers"
      assert(ASTEmap[a->id->value] != NULL);
      ASTEmap[a->id->value]->parentActivityId = myid;
      ASTEmap[a->id->value]->controlFlow = CHCF;
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
      int myid = $0->id->value;
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_properties = mkcasestring(ASTEmap[myid]->attributes["properties"].c_str());
     }
  ]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_set = mkcasestring(ASTEmap[myid]->attributes["set"].c_str());
      $0->A_initiate = mkcasestring(ASTEmap[myid]->attributes["initiate"].c_str());
     }
  ]
;


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<empty>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_partnerLink = mkcasestring(ASTEmap[myid]->attributes["partnerLink"].c_str());
      $0->A_portType = mkcasestring(ASTEmap[myid]->attributes["portType"].c_str());
      $0->A_operation = mkcasestring(ASTEmap[myid]->attributes["operation"].c_str());
      $0->A_inputVariable = mkcasestring(ASTEmap[myid]->attributes["inputVariable"].c_str());
      $0->A_outputVariable = mkcasestring(ASTEmap[myid]->attributes["outputVariable"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<invoke>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      
      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_partnerLink = mkcasestring(ASTEmap[myid]->attributes["partnerLink"].c_str());
      $0->A_portType = mkcasestring(ASTEmap[myid]->attributes["portType"].c_str());
      $0->A_operation = mkcasestring(ASTEmap[myid]->attributes["operation"].c_str());
      $0->A_variable = mkcasestring(ASTEmap[myid]->attributes["variable"].c_str());
      $0->A_createInstance = mkcasestring(ASTEmap[myid]->attributes["createInstance"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<receive>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_partnerLink = mkcasestring(ASTEmap[myid]->attributes["partnerLink"].c_str());
      $0->A_portType = mkcasestring(ASTEmap[myid]->attributes["portType"].c_str());
      $0->A_operation = mkcasestring(ASTEmap[myid]->attributes["operation"].c_str());
      $0->A_variable = mkcasestring(ASTEmap[myid]->attributes["variable"].c_str());
      $0->A_faultName = mkcasestring(ASTEmap[myid]->attributes["faultName"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<reply>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<assign>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_variable = mkcasestring(ASTEmap[myid]->attributes["variable"].c_str());
      $0->A_part = mkcasestring(ASTEmap[myid]->attributes["part"].c_str());
      $0->A_partnerLink = mkcasestring(ASTEmap[myid]->attributes["partnerLink"].c_str());
      $0->A_endpointReference = mkcasestring(ASTEmap[myid]->attributes["endpointReference"].c_str());
      $0->A_property = mkcasestring(ASTEmap[myid]->attributes["property"].c_str());
      $0->A_expression = mkcasestring(ASTEmap[myid]->attributes["expression"].c_str());
    }
  ]
;

To() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_variable = mkcasestring(ASTEmap[myid]->attributes["variable"].c_str());
      $0->A_part = mkcasestring(ASTEmap[myid]->attributes["part"].c_str());
      $0->A_partnerLink = mkcasestring(ASTEmap[myid]->attributes["partnerLink"].c_str());
      $0->A_property = mkcasestring(ASTEmap[myid]->attributes["property"].c_str());
    }
  ]
;
;


/******************************************************************************
  WAIT
******************************************************************************/

WaitFor(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_for = mkcasestring(ASTEmap[myid]->attributes["for"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<wait>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
    }
    a
  ]
;

WaitUntil(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_until = mkcasestring(ASTEmap[myid]->attributes["until"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<wait>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_faultName = mkcasestring(ASTEmap[myid]->attributes["faultName"].c_str());
      $0->A_faultVariable = mkcasestring(ASTEmap[myid]->attributes["faultVariable"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<throw>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_scope = mkcasestring(ASTEmap[myid]->attributes["scope"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<compensate>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<terminate>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      PPcurrentFlow = myid; //TODO reset

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());

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
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
        ASTEmap[innerActivity->id->value]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
    }
    a b c
  ]
;

Link() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());

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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());

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
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
        ASTEmap[innerActivity->id->value]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      assert(ASTEmap[d->id->value] != NULL);
      ASTEmap[d->id->value]->parentActivityId = myid;
      ASTEmap[d->id->value]->controlFlow = ASTEmap[myid]->controlFlow;

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
    }
    a b c
  ]
;

Case(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // copy attributes to AST
      $0->A_condition = mkcasestring(ASTEmap[myid]->attributes["condition"].c_str());

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
    a
  ]
;

Otherwise(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_condition = mkcasestring(ASTEmap[myid]->attributes["condition"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<while>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the while's id to the enclosed activity
      assert(ASTEmap[b->id->value] != NULL);
      ASTEmap[b->id->value]->parentActivityId = myid;
      ASTEmap[b->id->value]->controlFlow = ASTEmap[myid]->controlFlow;

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());

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
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
        ASTEmap[innerActivity->id->value]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());

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
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
        ASTEmap[innerActivity->id->value]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
        ASTEmap[innerActivity->id->value]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
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
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      PPcurrentScope = myid;

      // copy attributes to AST
      $0->A_name = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->A_joinCondition = mkcasestring(ASTEmap[myid]->attributes["joinCondition"].c_str());
      $0->A_suppressJoinFailure = mkcasestring(ASTEmap[myid]->attributes["suppressJoinFailure"].c_str());
      $0->A_variableAccessSerializable = mkcasestring(ASTEmap[myid]->attributes["variableAccessSerializable"].c_str());

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // cerr << "<scope>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the scope's id to the enclosed activity
      assert(ASTEmap[g->id->value] != NULL);
      ASTEmap[g->id->value]->parentActivityId = myid;

      // assert(ASTEmap[myid]->controlFlow == $0->negativeControlFlow->value);
      $0->negativeControlFlow = mkinteger(ASTEmap[myid]->controlFlow);
    }
    a b c d e f g
    {
      PPcurrentScope = ASTEmap[myid]->parentScopeId;
      assert($0->parentScopeId->value == ASTEmap[myid]->parentScopeId);
    }
  ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

StandardElements(a,b,c) ->
  [postprocessing:
    {
      $0->negativeControlFlow = mkinteger(ASTEmap[$0->parentId->value]->controlFlow);
      $0->suppressJF = ASTEmap[$0->parentId->value]->suppressJF;

      foreach(targetLink; tTarget_list a)
      {
        assert(ASTEmap[targetLink->id->value] != NULL);
        ASTEmap[targetLink->id->value]->controlFlow = ASTEmap[$0->parentId->value]->controlFlow;
        targetLink->negativeControlFlow = mkinteger(ASTEmap[$0->parentId->value]->controlFlow);
        ASTEmap[targetLink->id->value]->parentActivityId = $0->parentId->value;
      }

      foreach(sourceLink; tSource_list b)
      {
        assert(ASTEmap[sourceLink->id->value] != NULL);
        ASTEmap[sourceLink->id->value]->controlFlow = ASTEmap[$0->parentId->value]->controlFlow;
        sourceLink->negativeControlFlow = mkinteger(ASTEmap[$0->parentId->value]->controlFlow);
        ASTEmap[sourceLink->id->value]->parentActivityId = $0->parentId->value;
      }
    }
    a b c
  ]
;

Target() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // cerr << "<target>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;
      // cerr << "\tread target link: " << ASTEmap[myid]->attributes["linkName"] << endl;
    }
  ]
;

Source() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      // cerr << "<source>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;
      // cerr << "\tread source link: " << ASTEmap[myid]->attributes["linkName"] << endl;
    }
  ]
;

