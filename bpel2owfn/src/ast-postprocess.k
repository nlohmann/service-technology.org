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
 *          - last changed: \$Date: 2006/07/02 17:39:36 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.1 $
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

      ASTEmap[myid]->parentScopeId = myid;
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
  [postprocessing: a
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b) ->
  [postprocessing: a b]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b) ->
  [postprocessing: a b
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b) ->
  [postprocessing: a b
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b) ->
  [postprocessing: a b
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;

Copy(a,b) ->
  [postprocessing: a b]
;

From() ->
  [postprocessing: ]
;

To() ->
  [postprocessing: ]
;


/******************************************************************************
  WAIT
******************************************************************************/

WaitFor(a) ->
  [postprocessing: a
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;

WaitUntil(a) ->
  [postprocessing: a
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a) ->
  [postprocessing: a
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a) ->
  [postprocessing: a
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a) ->
  [postprocessing: a
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
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
    }
  a b c]
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

Switch(a,b,c) ->
  [postprocessing: a b c
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;

Case(a) ->
  [postprocessing: a
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;

Otherwise(a) ->
  [postprocessing: a
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b) ->
  [postprocessing: a b
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b) ->
  [postprocessing: a b
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c) ->
  [postprocessing: a b c
    {
      int myid = $0->id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
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

