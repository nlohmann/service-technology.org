/*****************************************************************************\
 * Copyright 2005, 2006, 2007 Niels Lohmann, Christian Gierds                *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

%{
/*!
 * \file ast-to-xml.cc
 *
 * \brief BPEL visualization unparser (implementation)
 *
 * This file defines and implements unparse rules to unparse BPEL to XML.
 * 
 * \author  Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 * 
 * \since   2007-04-18
 *
 * \date    \$Date: 2007/05/21 13:18:20 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.11 $
 *
 */

%}



/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,c,d,e,g,i,_id) ->
  [visualization: 
  
  { 
    globals::visArcs = "";
    (*output) << "digraph N {" << endl;
    (*output) << " graph [fontname=\"Helvetica\" label=\"";
    (*output) << "Visualization of the BPEL-Process generated from " << globals::filename << "\"]" << endl;
    (*output) << "compound = true;" << endl;
    globals::ASTEmap[_id->value]->visConnection = "Process_Start";
  } 
  a b c d e g 
  {
    (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"Process activity\";" << endl;  
  }
  i 
  {  
    globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[i->id]->visConnection;
    globals::ASTEmap[_id->value]->secVisConnection = globals::ASTEmap[i->id]->secVisConnection;
    (*output) << "}" << endl;    

    unsigned int linkId;
    unsigned int source;
    unsigned int target;
    for(set<string>::iterator names = globals::ASTE_linkNames.begin(); names != globals::ASTE_linkNames.end(); names++)
    {
       linkId = globals::ASTE_linkIdMap[(*names)];
     
       source = globals::ASTEmap[linkId]->sourceActivity;
       target = globals::ASTEmap[linkId]->targetActivity;

       globals::visArcs += globals::ASTEmap[source]->activityTypeName() + toString(source) + " -> " + 
                         globals::ASTEmap[target]->activityTypeName() + toString(target) + ";\n" ; 
    }
    (*output) << globals::visArcs;
    (*output) << "}" << endl;
  } ]
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

PartnerLink(_id) ->
  [visualization: ]
;


/******************************************************************************
  PARTNERS
******************************************************************************/

Partner(a,_id) ->
  [visualization: ]
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

FaultHandlers(a,b,_id) ->
  [visualization: 
  { if (! (globals::ASTEmap[_id->value]->isUserDefined) && globals::hideStandardHandlers) }
    ${ /* no Variables */ $}
  { else }
    ${
       {
         (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"FaultHandlers\";" << endl;  
       }
       a b
       {
         (*output) << "}" << endl;    
       }    
    $}
  ]
;

Catch(a,_id) ->
  [visualization: 
  {
  (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"Catch\";" << endl;  
  }
  a
  {
  (*output) << "}" << endl;    
  }
  ]
;

CatchAll(a,_id) ->
  [visualization:
  {
    (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"CatchAll\";" << endl;  
  }
  a
  {
    (*output) << "}" << endl;    
  }
  ]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

emptyEventHandlers(_id) ->
  [visualization: ]
;

EventHandlers(a,b,_id) ->
  [visualization: 
  {
    (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"EventHandlers\";" << endl;  
  }
  a b 
  {
    (*output) << "}" << endl;    
  }
  ]
;

OnMessage(a,b,c,_id) ->
  [visualization:
  {
  (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"OnMessage\";" << endl;  
  }
  a b c
  {
  (*output) << "}" << endl;    
  }
  ]
;

OnAlarm(a,b,_id) ->
  [visualization: 
  {
  (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"OnAlarm\";" << endl;  
  }
  b
  {
  (*output) << "}" << endl;    
  }
  ]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/
 
CompensationHandler(a,_id) ->
  [visualization: 
  { if (! (globals::ASTEmap[_id->value]->isUserDefined) && globals::hideStandardHandlers) }
    ${ /* no Variables */ $}
  { else }
    ${
       {
         (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"CompensationHandler\";" << endl;  
       }
       a
       {
         (*output) << "}" << endl;    
       }
    $}
  ]
;


/******************************************************************************
  TERMINATION HANDLERS
******************************************************************************/
 
TerminationHandler(a,_id) ->
  [visualization: 
  { if (! (globals::ASTEmap[_id->value]->isUserDefined) && globals::hideStandardHandlers) }
    ${ /* no Variables */ $}
  { else }
    ${
       {
         (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"TerminationHandler\";" << endl;  
       }
       a
       {
         (*output) << "}" << endl;    
       }
    $}
  ]
;


/******************************************************************************
  VARIABLES
******************************************************************************/

Variable(_id) ->
  [visualization: ]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet(_id) ->
  [visualization: ]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation(_id) ->
  [visualization: ]
;

/******************************************************************************
  TO & FROM PARTS                                               (WS-BPEL 2.0)
******************************************************************************/

ToPart(_id) ->
  [visualization: ]
;

FromPart(_id) ->
  [visualization: ]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c,_id) ->
  [visualization: 
  {
  (*output) << "receive" << toString(_id->value) << " [shape=rectangle style=filled label=\"receive\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "receive" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "receive" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,c,_id) ->
  [visualization: 
  {
  (*output) << "reply" << toString(_id->value) << " [shape=rectangle style=filled label=\"reply\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "reply" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "reply" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/

SynchronousInvoke(a,b,c,d,_id) ->
  [visualization: 
  {
  (*output) << "invoke" << toString(_id->value) << " [shape=rectangle style=filled label=\"invoke\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "invoke" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "invoke" + toString(_id->value);
  }
  ]
;

AsynchronousInvoke(a,b,c,d,_id) ->
  [visualization: 
  {
  (*output) << "invoke" << toString(_id->value) << " [shape=rectangle style=filled label=\"invoke\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "invoke" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "invoke" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,_id) ->
  [visualization: 
  {
  (*output) << "assign" << toString(_id->value) << " [shape=rectangle style=filled label=\"assign\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "assign" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "assign" + toString(_id->value);
  }
  ]
;

Copy(a,b,_id) ->
  [visualization: a b]
;

From(a) ->
  [visualization: a]
;

To(b) ->
  [visualization: b]
;


/******************************************************************************
  VALIDATE                                                      (WS-BPEL 2.0)
******************************************************************************/

Validate(a,_id) ->
  [visualization: 
  {
  (*output) << "validate" << toString(_id->value) << " [shape=rectangle style=filled label=\"validate\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "validate" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "validate" + toString(_id->value);
  }
  ]
;

/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a,_id) ->
  [visualization: 
  {
  (*output) << "empty" << toString(_id->value) << " [shape=rectangle style=filled label=\"empty\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "empty" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "empty" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  WAIT
******************************************************************************/

Wait(a,_id) ->
  [visualization: 
  {
  (*output) << "wait" << toString(_id->value) << " [shape=rectangle style=filled label=\"wait\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "wait" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "wait" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  EXIT / TERMINATE
******************************************************************************/

Exit(a,_id) ->
  [visualization: 
  {
  (*output) << "exit" << toString(_id->value) << " [shape=rectangle style=filled label=\"exit\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "exit" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "exit" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a,_id) ->
  [visualization: 
  {
  (*output) << "throw" << toString(_id->value) << " [shape=rectangle style=filled label=\"throw\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "throw" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "throw" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  RETHROW
******************************************************************************/

Rethrow(a,_id) ->
  [visualization: 
  {
  (*output) << "rethrow" << toString(_id->value) << " [shape=rectangle style=filled label=\"rethrow\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "rethrow" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "rethrow" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,_id) ->
  [visualization: 
  {
    (*output) << "compensate" << toString(_id->value) << " [shape=rectangle style=filled label=\"compensate\"";
    if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
    (*output) << "];" << endl;
    globals::ASTEmap[_id->value]->visConnection= "compensate" + toString(_id->value);
    globals::ASTEmap[_id->value]->secVisConnection= "compensate" + toString(_id->value);
  }

  ]
;

CompensateScope(a,b,_id) ->
  [visualization: 
  {
  (*output) << "compensateScope" << toString(_id->value) << " [shape=rectangle style=filled label=\"compensateScope\"";
  if(globals::ASTEmap[_id->value]->highlighted)
    (*output) << " fillcolor=red";
  (*output) << "];" << endl;
  globals::ASTEmap[_id->value]->visConnection= "compensateScope" + toString(_id->value);
  globals::ASTEmap[_id->value]->secVisConnection= "compensateScope" + toString(_id->value);
  }
  ]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b,_id) ->
  [visualization: 
  {

    (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"Sequence\";" << endl;  
  }
  a b
  {
    
    bool startlist=true;
    bool laststructured = false;
    int lastid = 0;   
    foreach(innerActivity; activity_list b)
    {
      if (startlist)
      {
        startlist=false;
        globals::ASTEmap[_id->value]->secVisConnection = globals::ASTEmap[innerActivity->id]->visConnection;
      }
      else
      {
   
        globals::visArcs += globals::ASTEmap[_id->value]->visConnection + " -> " + globals::ASTEmap[innerActivity->id]->secVisConnection + "["; 
        if (globals::ASTEmap[innerActivity->id]->structured())
        {
          globals::visArcs += "lhead=";
          globals::visArcs += "cluster";          
          globals::visArcs += toString(innerActivity->id);          
          globals::visArcs += " ";          
        }
        if (laststructured)
        {
          globals::visArcs += "ltail=";
          globals::visArcs += "cluster";          
          globals::visArcs += toString(lastid);          
        }
        globals::visArcs += "];\n";
      }
      if (globals::ASTEmap[innerActivity->id]->structured())
      {
        lastid = innerActivity->id;
        laststructured = true;
      }
      else
        laststructured = false;
      
      globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[innerActivity->id]->visConnection;
    }
    (*output) << "}" << endl;    
  }
  ]
;


/******************************************************************************
  IF / SWITCH
******************************************************************************/

If(a, b, c, _id) ->
  [visualization: 
  {
  (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"If\";" << endl;  
  (*output) << "if" << toString(_id->value) << " [shape=diamond style=filled label=\"\"];" << endl;
  globals::ASTEmap[_id->value]->secVisConnection = "if" + toString(_id->value);
  }
  a b c
  {
    int middle = 0;

    foreach(ElseIf(innerActivity); tElseIf_list b)
    {
      middle += 1;
    }

    with(c)
    {
      c=Else(innerActivity,_id2):
      { 
      middle += 1;
      }
    }

    middle = middle/2 + (middle % 2);
    int childcounter=1;

    foreach(ElseIf(innerActivity); tElseIf_list b)
    {
      globals::visArcs += "if" + toString(_id->value) + " -> " + globals::ASTEmap[innerActivity->id]->secVisConnection + "["; 
      if (globals::ASTEmap[innerActivity->id]->structured())
      {
        globals::visArcs += "lhead=";
        globals::visArcs += "cluster";          
        globals::visArcs += toString(innerActivity->id);          
        globals::visArcs += " ";          
      }
      globals::visArcs += "];\n";
      
      if (childcounter<=middle)
      {
        globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[innerActivity->id]->visConnection;
      }
      childcounter+=1;
    } 
    with(c)
    {
      c=Else(innerActivity,_id2):
      { 
        globals::visArcs += "if" + toString(_id->value) + " -> " + globals::ASTEmap[innerActivity->id]->secVisConnection + "["; 
        if (globals::ASTEmap[innerActivity->id]->structured())
        {
            globals::visArcs += "lhead=";
            globals::visArcs += "cluster";          
            globals::visArcs += toString(innerActivity->id);          
            globals::visArcs += " ";          
          }
          globals::visArcs += "];\n";

          if (childcounter<=middle)
          {
          globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[innerActivity->id]->visConnection;
          }
        }
        default: { /* do nothing */ 
      }
    }
        
    (*output) << "}" << endl;    
  }
  ]
;

ElseIf(a,_id) ->
  [visualization: a]
  
;

Else(a,_id) ->
  [visualization: a]
;


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b,_id) ->
  [visualization: 
  {
  (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"While\";" << endl;  
  }
  a b
  {
  globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[b->id]->visConnection;
  globals::ASTEmap[_id->value]->secVisConnection = globals::ASTEmap[b->id]->secVisConnection;
  (*output) << "}" << endl;    
  }
  ]
;


/******************************************************************************
  REPEATUNTIL                                                   (WS-BPEL 2.0)
******************************************************************************/

RepeatUntil(a,b,_id) ->
  [visualization: 
  {
  (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"RepeatUntil\";" << endl;  
  }
  a b
  {
  globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[b->id]->visConnection;
  globals::ASTEmap[_id->value]->secVisConnection = globals::ASTEmap[b->id]->secVisConnection;
  (*output) << "}" << endl;    
  }
  ]
;


/******************************************************************************
  FOREACH                                                       (WS-BPEL 2.0)
******************************************************************************/

ForEach(a,b,c,d,e,_id) ->
  [visualization: 
  {
  (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"ForEach\";" << endl;  
  }
  e
  {
  globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[e->id]->visConnection;
  globals::ASTEmap[_id->value]->secVisConnection = globals::ASTEmap[e->id]->secVisConnection;
  (*output) << "}" << endl;    
  }
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c,_id) ->
  [visualization: 
  {
    (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"Pick\";" << endl;  
    (*output) << "pick" << toString(_id->value) << " [shape=diamond style=filled label=\"?\"];" << endl;
    globals::ASTEmap[_id->value]->secVisConnection = "if" + toString(_id->value);
  }
  a b c
  {
    int middle = 0;
    foreach(branch=OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
    {
    middle+=1;
    }
    foreach(branch=OnAlarm(*,innerActivity,_id3); tOnAlarm_list c)
    {
    middle+=1;
    }

    middle = middle/2 + (middle % 2);
    int childcounter = 1;

    foreach(branch=OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
    {
      globals::visArcs += "pick" + toString(_id->value) + " -> " + globals::ASTEmap[innerActivity->id]->secVisConnection + "["; 
      if (globals::ASTEmap[innerActivity->id]->structured())
      {
        globals::visArcs += "lhead=";
        globals::visArcs += "cluster";          
        globals::visArcs += toString(innerActivity->id);          
        globals::visArcs += " ";          
      }
      globals::visArcs += "];\n";
      
      if (childcounter<=middle)
      {
        globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[innerActivity->id]->visConnection;
      }
      childcounter +=1;
    }
    foreach(branch=OnAlarm(*,innerActivity,_id3); tOnAlarm_list c)
    {
      globals::visArcs += "pick" + toString(_id->value) + " -> " + globals::ASTEmap[innerActivity->id]->secVisConnection + "["; 
      if (globals::ASTEmap[innerActivity->id]->structured())
      {
        globals::visArcs += "lhead=";
        globals::visArcs += "cluster";          
        globals::visArcs += toString(innerActivity->id);          
        globals::visArcs += " ";          
      }
      globals::visArcs += "];\n";
      
      if (childcounter<=middle)
      {
        globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[innerActivity->id]->visConnection;
      }
      childcounter +=1;
    }
    (*output) << "}" << endl;    
  }
  ]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,NiltLink_list(),c,_id) ->
  [visualization: 
  {
    (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"Flow\";" << endl;  
  }
  a c
  {
    int middle = 0;
    foreach(innerActivity; activity_list c)
    {
    middle+=1;
    }
    middle = middle/2 + (middle % 2);
    int childcounter = 1;
    foreach(innerActivity; activity_list c)
    {
      if (childcounter<=middle)
      {
        globals::ASTEmap[_id->value]->secVisConnection = globals::ASTEmap[innerActivity->id]->secVisConnection;
        globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[innerActivity->id]->visConnection;
      }
      childcounter +=1;
    }
    (*output) << "}" << endl;    
  }
  ]
;

Flow(a,b,c,_id) ->
  [visualization: 
  {
    (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"Flow\";" << endl;  
  }
  a b c
  {
    int middle = 0;
    foreach(innerActivity; activity_list c)
    {
    middle+=1;
    }
    middle = middle/2 + (middle % 2);
    int childcounter = 1;    

    foreach(innerActivity; activity_list c)
    {
      if (childcounter<=middle)
      {
        globals::ASTEmap[_id->value]->secVisConnection = globals::ASTEmap[innerActivity->id]->secVisConnection;
        globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[innerActivity->id]->visConnection;
      }
      childcounter +=1;
    }
    (*output) << "}" << endl;    
  }
  ]
;

Link(_id) ->
  [visualization: ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g,i,j,_id) ->
  [visualization: 
  {
    (*output) << "subgraph cluster" << toString(_id->value+1000) << " { label = \"Scope\";" << endl;  
  }
  a b c d e f g
  {
    (*output) << "subgraph cluster" << toString(_id->value) << " { label = \"Scope Activity\";" << endl;  
  }
  i 
  {  
    (*output) << "}" << endl;    

    globals::ASTEmap[_id->value]->visConnection = globals::ASTEmap[i->id]->visConnection;
    globals::ASTEmap[_id->value]->secVisConnection = globals::ASTEmap[i->id]->secVisConnection;
    (*output) << "}" << endl;    
  }
  ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

StandardElements(NiltTarget_list(), NiltSource_list(),jc) -> [visualization: ];
StandardElements(NiltTarget_list(), a,jc) -> [visualization: ];
StandardElements(a, NiltSource_list(),jc) -> [visualization: ];
StandardElements(a, b,jc) -> [visualization: ];

Target(a) ->
  [visualization: ]
;

Source(a) ->
  [visualization: ]
;
