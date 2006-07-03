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
 *          - last changed: \$Date: 2006/07/03 13:16:17 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.2 $
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

      cerr << "read process: " << ASTEmap[myid]->attributes["name"] << endl;

      if (ASTEmap[myid]->attributes["suppressJoinFailure"] == "yes")
      {
        $0->suppressJF = true;
        ASTEmap[myid]->suppressJF = true;
        cerr << "suppressJoinFailure = \"yes\"" << endl;
      }

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

      cerr << "read partner link: " << ASTEmap[myid]->attributes["name"] << endl;

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

      // these 3 asserts show that the AST can be annotated right here
      assert($0->hasCatchAll == false);
      assert($0->parentScopeId->value == ASTEmap[myid]->parentScopeId);
      assert($0->inProcess == (ASTEmap[myid]->parentScopeId == 1));
    }
  ]
;


Catch(a) ->
  [postprocessing: a]
;

CatchAll(a) ->
  [postprocessing: a]
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
  [postprocessing: a]
;

OnAlarm(a) ->
  [postprocessing: a]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

userDefinedCompensationHandler(a) ->
  [postprocessing: a]
;


/******************************************************************************
  VARIABLES
******************************************************************************/

Variable() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      cerr << "read variable: " << PPcurrentScope << "." << ASTEmap[myid]->attributes["name"] << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet() ->
  [postprocessing: ]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation() ->
  [postprocessing: ]
;


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      cerr << "<empty>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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

      cerr << "<invoke>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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

      cerr << "<receive>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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

      cerr << "<reply>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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

      cerr << "<assign>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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
      $0->variable = mkcasestring(ASTEmap[myid]->attributes["name"].c_str());
      $0->part = mkcasestring(ASTEmap[myid]->attributes["part"].c_str());
      $0->partnerLink = mkcasestring(ASTEmap[myid]->attributes["partnerLink"].c_str());
      $0->endpointReference = mkcasestring(ASTEmap[myid]->attributes["endpointReference"].c_str());
      $0->property = mkcasestring(ASTEmap[myid]->attributes["property"].c_str());
      $0->expression = mkcasestring(ASTEmap[myid]->attributes["expression"].c_str());
    }
  ]
;

To() ->
  [postprocessing: ]
;


/******************************************************************************
  WAIT
******************************************************************************/

WaitFor(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      cerr << "<wait>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
    a
  ]
;

WaitUntil(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      cerr << "<wait>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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

      cerr << "<throw>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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

      cerr << "<compensate>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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

      cerr << "<terminate>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
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

      cerr << "<flow>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the flow's id to the enclosed activities
      foreach(innerActivity; activity_list c)
      {
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
      }
    }
    a b c
  ]
;

Link() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      cerr << "read link: " << PPcurrentFlow << "." << ASTEmap[myid]->attributes["name"] << endl;

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

      cerr << "<switch>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the switch's id to the enclosed activities
      foreach(branch=Case(innerActivity); tCase_list b)
      {
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
      }
      assert(ASTEmap[d->id->value] != NULL);
      ASTEmap[d->id->value]->parentActivityId = myid;
    }
    a b c
  ]
;

Case(a) ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

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

      cerr << "<while>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the while's id to the enclosed activity
      assert(ASTEmap[b->id->value] != NULL);
      ASTEmap[b->id->value]->parentActivityId = myid;
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

      cerr << "<sequence>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the sequence's id to the enclosed activities
      foreach(innerActivity; activity_list b)
      {
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
      }
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

      cerr << "<pick>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the pick's id to the enclosed activities
      foreach(branch=OnMessage(innerActivity); tOnMessage_list b)
      {
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id->value] != NULL);
        ASTEmap[innerActivity->id->value]->parentActivityId = myid;
      }
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

      cerr << "<scope>: " << myid << "@" << ASTEmap[myid]->parentActivityId << endl;

      // distribute the scope's id to the enclosed activity
      assert(ASTEmap[g->id->value] != NULL);
      ASTEmap[g->id->value]->parentActivityId = myid;
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

Target() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      cerr << "read target link: " << ASTEmap[myid]->attributes["linkName"] << endl;
    }
  ]
;

Source() ->
  [postprocessing:
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      cerr << "read source link: " << ASTEmap[myid]->attributes["linkName"] << endl;
    }
  ]
;

