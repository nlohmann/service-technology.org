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
 * \file bpel-unparse-xml.cc
 *
 * \brief XML unparser (implementation)
 *
 * This file defines and implements unparse rules to unparse BPEL to XML.
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date    
 *          - created: 2005-11-01
 *          - last changed: \$Date: 2006/11/05 12:05:57 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.58 $
 */
%}





/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,c,d,e,f,g,h,i,_id) ->
  [xml: { inup(); } "<process id=\"" _id "\">\n" 
  
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

  e f g h i { indown(); } "</process>\n"]
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

PartnerLink(_id) ->
  [xml: { in(); } "<partnerLink id=\"" _id "\"/>\n"]
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

userDefinedFaultHandler(a,b,_id) ->
  [xml: { inup(); } "<faultHandlers id=\"" _id "\">\n" a b { indown(); } "</faultHandlers>\n"]
;

Catch(a,_id) ->
  [xml: { inup(); } "<catch id=\"" _id "\">\n" a { indown(); } "</catch>\n"]
;

CatchAll(a,_id) ->
  [xml: { inup(); } "<catchAll>\n" _id { indown(); } "</catchAll>\n"]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

userDefinedEventHandler(a,b,_id) ->
  [xml: { inup(); } "<eventHandlers id=\"" _id "\">\n" a b { indown(); } "</eventHandlers>\n"]
;

OnMessage(a,b,c,d) ->
  [xml: { inup(); } "<onMessage>\n" c { indown(); } "</onMessage>\n"]
;

OnAlarm(a,b) ->
  [xml: { inup(); } "<onAlarm>\n" a { indown(); } "</onAlarm>\n"]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

userDefinedCompensationHandler(a,_id) ->
  [xml: { inup(); } "<compensationHandler id=\"" _id "\">\n" a { indown(); } "</compensationHandler>\n"]
;


/******************************************************************************
  VARIABLES
******************************************************************************/

Variable(_id) ->
  [xml: { in(); } "<variable id=\"" _id "\"/>\n"]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet(a) ->
  [xml: { in(); } "<correlationSet />\n"]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation(a) ->
  [xml: { in(); } "<correlation />\n"]
;


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a,_id) ->
  [xml: { inup(); } "<empty id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a { indown(); } "</empty>\n"];


/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b,c,d,_id) ->
  [xml: { inup(); } "<invoke id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a b { indown(); } "</invoke>\n"]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c,_id) ->
  [xml: { inup(); } "<receive id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a b { indown(); } "</receive>\n"]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,c,_id) ->
  [xml: { inup(); } "<reply id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a b { indown(); } "</reply>\n"]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,_id) ->
  [xml: { inup(); } "<assign id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a b { indown(); } "</assign>\n"]
;

Copy(a,b,c) ->
  [xml: { inup(); } "<copy>\n" a b { indown(); } "</copy>\n"]
;

From(a) ->
  [xml: { in(); } "<from />\n"]
;

To(b) ->
  [xml: { in(); } "<to />\n"]
;


/******************************************************************************
  WAIT
******************************************************************************/

Wait(a,_id) ->
  [xml: { inup(); } "<wait id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a { indown(); } "</wait>\n"]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a,_id) ->
  [xml: { inup(); } "<throw id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a { indown(); } "</throw>\n"]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,_id) ->
  [xml: { inup(); } "<compensate id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a { indown(); } "</compensate>\n"]
;


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a,_id) ->
  [xml: { inup(); } "<terminate id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a { indown(); } "</terminate>\n"]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,NiltLink_list(),c,_id) ->
  [xml: { inup(); } "<flow id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a c { indown(); } "</flow>\n"]
;

Flow(a,b,c,_id) ->
  [xml: { inup(); } "<flow id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a { inup(); } "<links>\n" b { indown(); } "</links>\n" c { indown(); } "</flow>\n"]
;

Link(_id) ->
  [xml: { in(); } "<link id=\"" _id "\"/>\n"]
;


/******************************************************************************
  SWITCH
******************************************************************************/

Switch(a,b,c,_id) ->
  [xml: { inup(); } "<switch id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a b c { indown(); } "</switch>\n"]
;

Case(a,b) ->
  [xml: { inup(); } "<case>\n" a { indown(); } "</case>\n"]
;

Otherwise(a,b) ->
  [xml: { inup(); } "<otherwise>\n" a { indown(); } "</otherwise>\n"]
;


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b,_id) ->
  [xml: { inup(); } "<while id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a b { indown(); } "</while>\n"]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b,_id) ->
  [xml: { inup(); } "<sequence id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a b { indown(); } "</sequence>\n"]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c,_id) ->
  [xml: { inup(); } "<pick id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" a b c { indown(); } "</pick>\n"]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g,h,i,_id) ->
  [xml: { inup(); } "<scope id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" 
  
  a

  { if (b->length() == 0) }
    ${ /* no Variables */ $}
  { else }
    ${ { inup(); } "<variables id=\"" _id "\" ncf=\"" { ASTEmap[_id->value]->controlFlow; } "\">\n" b { indown(); } "</variables>\n" $}

  c d e f g h i
    
  { indown(); } "</scope>\n" ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

Target(a) ->
  [xml: { in(); } "<target ncf=\"" { ASTEmap[a->value]->controlFlow; } "\" />\n"]
;

Source(a) ->
  [xml: { in(); } "<source ncf=\"" { ASTEmap[a->value]->controlFlow; } "\" />\n"]
;
