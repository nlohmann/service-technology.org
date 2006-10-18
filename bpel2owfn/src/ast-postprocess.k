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
 *          - last changed: \$Date: 2006/10/18 09:37:41 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.42 $
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
#include "helpers.h"

using namespace std;




/******************************************************************************
 * External variables
 *****************************************************************************/

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

Process(a,b,c,d,e,f,g,h,i,j) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = myid;
      ASTEmap[myid]->parentActivityId = myid;
      PPcurrentScope = myid;

      // set suppressJoinFailure-attribute
      if (ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->hasEH = (string(g->op_name()) == "userDefinedEventHandler");


      // tag the (useless) compensation handler
      f->inProcess = true;


      // distribute the process' id to the enclosed FH/CH/EH/activity
      assert(ASTEmap[e->id] != NULL);
      assert(ASTEmap[f->id] != NULL);
      assert(ASTEmap[g->id] != NULL);
      assert(ASTEmap[i->id] != NULL);
      ASTEmap[e->id]->parentActivityId = myid;
      ASTEmap[f->id]->parentActivityId = myid;
      ASTEmap[g->id]->parentActivityId = myid;
      ASTEmap[i->id]->parentActivityId = myid;
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

      
      // pass list of enclosed scopes to the compensation handler
      ASTEmap[f->id]->enclosedScopes = ASTEmap[myid]->enclosedScopes;


      // this assertion guarantees that we did not forget a link and encounter
      // problems generating the Petri net
      assert(ASTEmap[myid]->enclosedSourceLinks.size() == ASTE_links.size());

//      cerr << myid << " has " << ASTEmap[myid]->enclosedSourceLinks.size() << " indirectly enclosed source links" << endl;
    }
  ]
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

PartnerLink(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  PARTNERS
******************************************************************************/

Partner(a,b) ->
  [postprocessing:
    {
      // annotate all enclosed partner links
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

userDefinedFaultHandler(a,b,_id) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // annotate AST 
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

      // check whether a <catchAll>-branch with an enclosed activity is present
      // and if yes, annotate it
      with (b)
      {
        CatchAll(c,_id) :
        {
          assert(ASTEmap[c->id] != NULL);
          ASTEmap[c->id]->parentActivityId = myid;
        }
        default :
        {
          // triggers SA00080
          if (a->length() == 0)
            SAerror(80, "", toInt(ASTEmap[parentScopeId]->attributes["referenceLine"]));
        }
      }
    }
    a b
  ]
;

implicitFaultHandler(_id) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // annotate AST 
      $0->hasCatchAll = false;
      $0->parentScopeId = PPcurrentScope;
      $0->inProcess = (ASTEmap[myid]->parentScopeId == 1);
    }
  ]
;


Catch(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
    }
    a
  ]
;

CatchAll(a,b) ->
  [postprocessing:
    a
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
    }
  ]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

implicitEventHandler(_id) ->
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

userDefinedEventHandler(a,b,_id) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      $0->parentScopeId = PPcurrentScope;

      $0->inWhile = true;

      // triggers SA00083
      if (a->length() == 0 && b->length() == 0)
        SAerror(83, "", toInt(ASTEmap[parentScopeId]->attributes["referenceLine"]));

      // distribute the event handlers's id to the enclosed activities
      foreach(branch=OnMessage(innerActivity); tOnMessage_list a)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
    }
    a b
  ]
;

OnMessage(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      $0->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->channelName = ASTEmap[myid]->createChannel();
      $0->variableName = ASTEmap[myid]->checkVariable();
    }
    a
  ]
;

OnAlarm(a,b) ->
  [postprocessing: a]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

implicitCompensationHandler(_id) ->
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

userDefinedCompensationHandler(a,_id) ->
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

Variable(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();

      ASTEmap[myid]->defineVariable();
    }
  ]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
     }
  ]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation(a) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
     }
  ]
;


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }
    }
    a
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b,c) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      $0->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->isSynchronousInvoke = ($0->attributes["outputVariable"] != "");
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      $0->channelName = ASTEmap[myid]->createChannel($0->isSynchronousInvoke);
      $0->inputVariableName = ASTEmap[myid]->checkInputVariable();
      $0->outputVariableName = ASTEmap[myid]->checkOutputVariable();

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }
    }
    a b
  ]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;

// TODO: replace $0->id by c->value...
assert($0->id == c->value);

      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      $0->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      $0->channelName = ASTEmap[myid]->createChannel();
      $0->variableName = ASTEmap[myid]->checkVariable();

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }
    }
    a b
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,c) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      $0->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;
      $0->channelName = ASTEmap[myid]->createChannel();
      $0->variableName = ASTEmap[myid]->checkVariable();

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }
    }
    a b
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,c) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }
    }
    a b
  ]
;

Copy(a,b,c) ->
  [postprocessing: a b]
;

From(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->variableName = ASTEmap[myid]->checkVariable();
    }
  ]
;

To(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->variableName = ASTEmap[myid]->checkVariable();
    }
  ]
;



/******************************************************************************
  WAIT
******************************************************************************/

Wait(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // switch to discriminate waitFor and waitUntil
      $0->isWaitFor = ($0->attributes["for"] != "");
    }
    a
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }
    }
    a
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // attribute to discriminate the <compensate>-patterns
      $0->hasScopeAttribute = ($0->attributes["scope"] != "");

      // register this compensate at the current scope
      /* here we can check whether it is the only compensate activity */
      if ($0->attributes["scope"] == "")
        PPhasCompensateMap[PPcurrentScope] = 1;
      else
        PPhasCompensateMap[PPcurrentScope] = 2;
    }
    a
  ]
;


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a,b) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }
    }
    a
  ]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,b,c,d) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

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
    }
    a b c
    {
      // collect enclosed source links
      foreach(innerActivity; activity_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
      
      /* here we can check whether each link was used as source and target */
    }
  ]
;

Link(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();

      /* here we can check whether this link was not defined before */
      linkIdMap[$0->attributes["name"]] = myid;

      // cerr << "defined a link " << $0->A_name << " (" << myid << ")" << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      ASTE_links.insert($0->attributes["name"]);
    }
  ]
;


/******************************************************************************
  SWITCH
******************************************************************************/

Switch(a,b,c,_id) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

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

      // process Otherwise-branch
      with(c)
      {
        c=Otherwise(d,e):
        { 
          assert(ASTEmap[d->id] != NULL);
          ASTEmap[d->id]->parentActivityId = myid;
          ASTEmap[d->id]->controlFlow = ASTEmap[myid]->controlFlow;
        }
        default: { /* do nothing */ }
      }
    }
    a b c
    {
      // collect enclosed source links
      foreach(branch=Case(innerActivity); tCase_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }

      with(c)
      {
        c=Otherwise(d,e):
        {
          assert(ASTEmap[d->id] != NULL);
          ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[d->id]->enclosedSourceLinks);
        }
        default: { /* do nothing */ }
      }
    }
  ]
;

Case(a,b) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
    }
    a
  ]
;

Otherwise(a,b) ->
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

While(a,b,c) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      ASTEmap[myid]->inWhile = true;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

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
    }
    a b
    {
      // collect enclosed source links
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[b->id]->enclosedSourceLinks);
    }
  ]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b,c) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

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
    }
    a b
    {
      // collect enclosed source links
      foreach(innerActivity; activity_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
    }
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c,d) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
assert($0->id == d->value);
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      a->parentId = myid;

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

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
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
    }
    a b c
    {
      // collect enclosed source links
      foreach(branch=OnMessage(innerActivity); tOnMessage_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
    }
  ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g,h,i) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);
      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      $0->parentScopeId = PPcurrentScope;
      PPcurrentScope = myid;
      a->parentId = myid;

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();
      $0->negativeControlFlow = ASTEmap[myid]->controlFlow;

      // register at parent scope
      ASTEmap[ASTEmap[myid]->parentScopeId]->enclosedScopes.insert(myid);

      $0->hasEH = (string(e->op_name()) == "userDefinedEventHandler");

      // set or inherit suppressJoinFailure-attribute
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      if (ASTEmap[ASTEmap[myid]->parentActivityId]->suppressJF || ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
      }

      // distribute the scope's id to the enclosed activity
      assert(ASTEmap[h->id] != NULL);
      ASTEmap[h->id]->parentActivityId = myid;
    }
    a b c d e f g h
    {
      PPcurrentScope = ASTEmap[myid]->parentScopeId;

      assert($0->parentScopeId == ASTEmap[myid]->parentScopeId);

      // add an attribute to the compensation handler
      if(PPhasCompensateMap[myid] == 1)
        d->hasCompensateWithoutScope = true;
      if(PPhasCompensateMap[myid] == 2)
        d->hasCompensateWithScope = true;


      // pass list of enclosed scopes to the compensation handler
      ASTEmap[d->id]->enclosedScopes = ASTEmap[myid]->enclosedScopes;


      // collect enclosed source links
      assert(ASTEmap[h->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[h->id]->enclosedSourceLinks);
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
      /* here we can check the links */
    }
  ]
;

Target(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // find the id of the link used as target here
      int linkId = linkIdMap[ASTEmap[myid]->attributes["linkName"]];
      /* here we can check whether this link was defined before */


      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();


/*TODO
      // remember that that link was used as a target
      assert(ASTEmap[linkId] != NULL);
      kc::impl_tLink *theLink = (kc::impl_tLink*)(ASTEmap[linkId]->node);
      assert(theLink != NULL);
      assert(theLink->id == linkId);
      // here we can check whether this link was not used as target before
      theLink->usedAsTarget = true;
*/

      $0->linkName = $0->attributes["linkName"];
    }
  ]
;

Source(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // find the id of the link used as source here
      int linkId = linkIdMap[ASTEmap[myid]->attributes["linkName"]];
      /* here we can check whether this link was defined before */


      // annotate AST and check information
      $0->attributes = ASTEmap[myid]->getAttributes();

/*TODO
      // remember that that link was used as a source
      assert(ASTEmap[linkId] != NULL);
      kc::impl_tLink *theLink = (kc::impl_tLink*)(ASTEmap[linkId]->node);
      assert(theLink != NULL);
      assert(theLink->id == linkId);
      // here we can check whether this link was not used as target before
      theLink->usedAsSource = true;
*/

      // add that link to the parent activity
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[ASTEmap[myid]->parentActivityId]->enclosedSourceLinks.insert(linkId);

      $0->linkName = $0->attributes["linkName"];
    }
  ]
;
