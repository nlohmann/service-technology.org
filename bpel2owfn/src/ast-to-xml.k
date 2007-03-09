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
 * \brief XML unparser (implementation)
 *
 * This file defines and implements unparse rules to unparse BPEL to XML.
 * 
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2005-11-01
 *
 * \date    \$Date: 2007/03/09 12:59:02 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.75 $
 *
 * \todo    Add support for the <terminationHandler>
 */

%}



/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,c,d,e,g,i,_id) ->
  [xml: { inup(); } "<process id=\"" _id "\"" { listAttributes( _id ); } ">\n" 
  
  { if (a->length() == 0) }
    ${ /* no Partner Links */ $}
  { else }
    ${ { inup(); } "<partnerLinks>\n" a { indown(); } "</partnerLinks>\n" $}
  
  { if (b->length() == 0) }
    ${ /* no Partners */ $}
  { else }
    ${ { inup(); } "<partners>\n" b { indown(); } "</partners>\n" $}
  
  { if (c->length() == 0) }
    ${ /* no Variables */ $}
  { else }
    ${ { inup(); } "<variables>\n" c { indown(); } "</variables>\n" $}

  { if (d->length() == 0) }
    ${ /* no Correlation Sets */ $}
  { else }
    ${ { inup(); } "<correlationSets>\n" d { indown(); } "</correlationSets>\n" $}

  e g i { indown(); } "</process>\n"]
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

PartnerLink(_id) ->
  [xml: { in(); } "<partnerLink id=\"" _id "\"" { listAttributes( _id ); } " />\n"]
;


/******************************************************************************
  PARTNERS
******************************************************************************/

Partner(a,_id) ->
  [xml: { inup(); } "<partner id=\"" _id "\">\n" a { indown(); } "</partner>\n"]
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

FaultHandlers(a,b,_id) ->
  [xml: { inup(); } "<faultHandlers id=\"" _id "\">\n" a b { indown(); } "</faultHandlers>\n"]
;

Catch(a,_id) ->
  [xml: { inup(); } "<catch id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</catch>\n"]
;

CatchAll(a,_id) ->
  [xml: { inup(); } "<catchAll id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</catchAll>\n"]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

emptyEventHandlers(_id) ->
  [xml: ]
;

EventHandlers(a,b,_id) ->
  [xml: { inup(); } "<eventHandlers id=\"" _id "\">\n" a b { indown(); } "</eventHandlers>\n"]
;

OnMessage(a,b,c,_id) ->
  [xml: { inup(); } "<onMessage id=\"" _id "\"" { listAttributes( _id ); } ">\n"

  { if (a->length() == 0) }
    ${ /* no correlation sets */ $}
  { else }
    ${ { inup(); } "<correlations>\n" a { indown(); } "</correlations>\n" $}
  
  { if (b->length() == 0) }
    ${ /* no from parts */ $}
  { else }
    ${ { inup(); } "<fromParts>\n" b { indown(); } "</fromParts>\n" $}
  
  c { indown(); } "</onMessage>\n"]
;

OnAlarm(a,_id) ->
  [xml: { inup(); } "<onAlarm" { listAttributes( _id ); } ">\n" a { indown(); } "</onAlarm>\n"]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/
 
CompensationHandler(a,_id) ->
  [xml: { inup(); } "<compensationHandler id=\"" _id "\">\n" a { indown(); } "</compensationHandler>\n"]
;


/******************************************************************************
  VARIABLES
******************************************************************************/

Variable(_id) ->
  [xml: { in(); } "<variable id=\"" _id "\"" { listAttributes( _id ); } " />\n"]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet(_id) ->
  [xml: { in(); } "<correlationSet" { listAttributes( _id ); } " />\n"]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation(_id) ->
  [xml: { in(); } "<correlation" { listAttributes( _id ); } " />\n"]
;

/******************************************************************************
  TO & FROM PARTS                                               (WS-BPEL 2.0)
******************************************************************************/

ToPart(_id) ->
  [xml: { in(); } "<toPart" { listAttributes( _id ); } " />\n"]
;

FromPart(_id) ->
  [xml: { in(); } "<fromPart" { listAttributes( _id ); } " />\n"]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c,_id) ->
  [xml: { inup(); } "<receive id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b c { indown(); } "</receive>\n"]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,c,_id) ->
  [xml: { inup(); } "<reply id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b c { indown(); } "</reply>\n"]
;


/******************************************************************************
  INVOKE
******************************************************************************/

SynchronousInvoke(a,b,c,d,_id) ->
  [xml: { inup(); } "<invoke id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b c d { indown(); } "</invoke>\n"]
;

AsynchronousInvoke(a,b,c,d,_id) ->
  [xml: { inup(); } "<invoke id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b c d { indown(); } "</invoke>\n"]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,_id) ->
  [xml: { inup(); } "<assign id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b { indown(); } "</assign>\n"]
;

Copy(a,b,_id) ->
  [xml: { inup(); } "<copy id=\"" _id "\">\n" a b { indown(); } "</copy>\n"]
;

From(a) ->
  [xml: { in(); } "<from />\n"]
;

To(b) ->
  [xml: { in(); } "<to />\n"]
;


/******************************************************************************
  VALIDATE                                                      (WS-BPEL 2.0)
******************************************************************************/

Validate(a,_id) ->
  [xml: { inup(); } "<validate id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</validate>\n"];


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a,_id) ->
  [xml: { inup(); } "<empty id=\"" _id "\"" { listAttributes( _id ); } 
    {  
      with(a)
      {
        a = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition()): { *output << " />";  indown(); *output << endl; }
        default: { *output << ">\n"; }
      }
    }
    a
    {  
      with(a)
      {
        a = StandardElements(NiltTarget_list(), NiltSource_list(), standardJoinCondition()): { /* nothing more to do */ }
        default: { indown(); *output << "</empty>\n"; }
      }
    }
  ]
;


/******************************************************************************
  WAIT
******************************************************************************/

Wait(a,_id) ->
  [xml: { inup(); } "<wait id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</wait>\n"]
;


/******************************************************************************
  EXIT / TERMINATE
******************************************************************************/

Exit(a,_id) ->
  [xml: { inup(); } "<exit id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</exit>\n"]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a,_id) ->
  [xml: { inup(); } "<throw id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</throw>\n"]
;


/******************************************************************************
  RETHROW
******************************************************************************/

Rethrow(a,_id) ->
  [xml: { inup(); } "<rethrow id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</rethrow>\n"]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,_id) ->
  [xml: { inup(); } "<compensate id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</compensate>\n"]
;

CompensateScope(a,b,_id) ->
  [xml: { inup(); } "<compensateScope id=\"" _id "\" target=\"" b "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</compensateScope>\n"]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b,_id) ->
  [xml: { inup(); } "<sequence id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b { indown(); } "</sequence>\n"]
;


/******************************************************************************
  IF / SWITCH
******************************************************************************/

If(a, ConstElseIf_list(ElseIf(b,_id2), c),d,_id) ->
  [xml: { inup(); } "<if id=\"" _id "\"" { listAttributes( _id ); } ">\n" {  } a b c d { indown(); } "</if>\n"]
;

ElseIf(a,_id) ->
  [xml: { inup(); } "<elseif id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { indown(); } "</elseif>\n"]
;

Else(a,_id) ->
  [xml: { inup(); } "<else id=\"" _id "\">\n" a { indown(); } "</else>\n"]
;


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b,_id) ->
  [xml: { inup(); } "<while id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b { indown(); } "</while>\n"]
;


/******************************************************************************
  REPEATUNTIL                                                   (WS-BPEL 2.0)
******************************************************************************/

RepeatUntil(a,b,_id) ->
  [xml: { inup(); } "<repeatUntil id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b { indown(); } "</repeatUntil>\n"]
;


/******************************************************************************
  FOREACH                                                       (WS-BPEL 2.0)
******************************************************************************/

ForEach(a,b,c,d,e,_id) ->
  [xml: { inup(); } "<foreach id=\"" _id "\"" { listAttributes( _id ); } ">\n" 
  a 
  { inup(); } "<startCounterValue>\n" b { indown(); } "</startCounterValue>\n"
  { inup(); } "<finalCounterValue>\n" c { indown(); } "</finalCounterValue>\n"
  { inup(); } "<completionCondition>\n" d { indown(); } "</completionCondition>\n"
  e
  { indown(); } "</repeatUntil>\n"]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c,_id) ->
  [xml: { inup(); } "<pick id=\"" _id "\"" { listAttributes( _id ); } ">\n" a b c { indown(); } "</pick>\n"]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,NiltLink_list(),c,_id) ->
  [xml: { inup(); } "<flow id=\"" _id "\"" { listAttributes( _id ); } ">\n" a c { indown(); } "</flow>\n"]
;

Flow(a,b,c,_id) ->
  [xml: { inup(); } "<flow id=\"" _id "\"" { listAttributes( _id ); } ">\n" a { inup(); } "<links>\n" b { indown(); } "</links>\n" c { indown(); } "</flow>\n"]
;

Link(_id) ->
  [xml: { in(); } "<link id=\"" _id "\"" { listAttributes( _id ); } " />\n"]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g,i,j,_id) ->
  [xml: { inup(); } "<scope id=\"" _id "\"" { listAttributes( _id ); } ">\n" 
  
  a

  { if (b->length() == 0) }
    ${ /* no Variables */ $}
  { else }
    ${ { inup(); } "<variables id=\"" _id "\">\n" b { indown(); } "</variables>\n" $}

  c d e f g /* h */ i
    
  { indown(); } "</scope>\n" ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

StandardElements(NiltTarget_list(), NiltSource_list(),jc) -> [xml: ];
StandardElements(NiltTarget_list(), a,jc) -> [xml: { inup(); } "<sources>\n" a { indown(); } "</sources>\n" ];
StandardElements(a, NiltSource_list(),jc) -> [xml: { inup(); } "<targets>\n" a { indown(); } "</targets>\n" ];
StandardElements(a, b,jc) -> [xml: { inup(); } "<targets>\n" a { indown(); } "</targets>\n" { inup(); } "<sources>\n" b { indown(); } "</sources>\n" ];

Target(a) ->
  [xml: { in(); } "<target" { listAttributes( a ); } " />\n"]
;

Source(a) ->
  [xml: { in(); } "<source" { listAttributes( a ); } " />\n"]
;

