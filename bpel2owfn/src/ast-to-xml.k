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
 *          - last changed: \$Date: 2006/10/18 09:37:41 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.45 $
 */
%}


%{	
/*!
 * \file bpel-unparse-xml.h
 * \brief XML unparser (interface)
 *
 * See \ref bpel-unparse-xml.cc for more information.
 */
%}


/******************************************************************************/

// All the includes and variables can be used during the unparsing.
%{ KC_UNPARSE
/* These lines are from bpel-unparse-xml.k */
#include <iostream>
#include "bpel-unparse-tools.h"
#include "ast-printers.h"
%}





Process(a,b,c,d,e,f,g,h,i,j) ->
  [xml: { inup(); } "<process id=\"" $0->id "\">\n" 
  
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

PartnerLink(a) ->
  [xml: { in(); } "<partnerLink id=\"" $0->id "\"/>\n"]
;


/******************************************************************************
  PARTNERS
******************************************************************************/

Partner(a,b) ->
  [xml: { inup(); } "<partner id=\"" $0->id "\">\n" a { indown(); } "</partner>\n"]
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

userDefinedFaultHandler(a,b,_id) ->
  [xml: { inup(); } "<faultHandlers id=\"" $0->id "\">\n" a b { indown(); } "</faultHandlers>\n"]
;

Catch(a,b) ->
  [xml: { inup(); } "<catch id=\"" $0->id "\">\n" a { indown(); } "</catch>\n"]
;

CatchAll(a,b) ->
  [xml: { inup(); } "<catchAll>\n" a { indown(); } "</catchAll>\n"]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

userDefinedEventHandler(a,b,_id) ->
  [xml: { inup(); } "<eventHandlers id=\"" $0->id "\">\n" a b { indown(); } "</eventHandlers>\n"]
;

OnMessage(a,b) ->
  [xml: { inup(); } "<onMessage>\n" a { indown(); } "</onMessage>\n"]
;

OnAlarm(a,b) ->
  [xml: { inup(); } "<onAlarm>\n" a { indown(); } "</onAlarm>\n"]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

userDefinedCompensationHandler(a,_id) ->
  [xml: { inup(); } "<compensationHandler id=\"" $0->id "\">\n" a { indown(); } "</compensationHandler>\n"]
;


/******************************************************************************
  VARIABLES
******************************************************************************/

Variable(a) ->
  [xml: { in(); } "<variable id=\"" $0->id "\"/>\n"]
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

Empty(a,b) ->
  [xml: { inup(); } "<empty id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a { indown(); } "</empty>\n"];


/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b,c) ->
  [xml: { inup(); } "<invoke id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a b { indown(); } "</invoke>\n"]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c) ->
  [xml: { inup(); } "<receive id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a b { indown(); } "</receive>\n"]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,c) ->
  [xml: { inup(); } "<reply id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a b { indown(); } "</reply>\n"]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,c) ->
  [xml: { inup(); } "<assign id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a b { indown(); } "</assign>\n"]
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

Wait(a,b) ->
  [xml: { inup(); } "<wait id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a { indown(); } "</wait>\n"]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a,b) ->
  [xml: { inup(); } "<throw id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a { indown(); } "</throw>\n"]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,b) ->
  [xml: { inup(); } "<compensate id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a { indown(); } "</compensate>\n"]
;


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a,b) ->
  [xml: { inup(); } "<terminate id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a { indown(); } "</terminate>\n"]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,NiltLink_list(),c,d) ->
  [xml: { inup(); } "<flow id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a c { indown(); } "</flow>\n"]
;

Flow(a,b,c,d) ->
  [xml: { inup(); } "<flow id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a { inup(); } "<links>\n" b { indown(); } "</links>\n" c { indown(); } "</flow>\n"]
;

Link(a) ->
  [xml: { in(); } "<link id=\"" $0->id "\"/>\n"]
;


/******************************************************************************
  SWITCH
******************************************************************************/

Switch(a,b,c,d) ->
  [xml: { inup(); } "<switch id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a b c { indown(); } "</switch>\n"]
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

While(a,b,c) ->
  [xml: { inup(); } "<while id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a b { indown(); } "</while>\n"]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b,c) ->
  [xml: { inup(); } "<sequence id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a b { indown(); } "</sequence>\n"]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c,d) ->
  [xml: { inup(); } "<pick id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" a b c { indown(); } "</pick>\n"]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g,h,i) ->
  [xml: { inup(); } "<scope id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" 
  
  a

  { if (b->length() == 0) }
    ${ /* no Variables */ $}
  { else }
    ${ { inup(); } "<variables id=\"" $0->id "\" ncf=\"" $0->negativeControlFlow "\">\n" b { indown(); } "</variables>\n" $}

  c d e f g h
    
  { indown(); } "</scope>\n" ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

Target(a) ->
  [xml: { in(); } "<target ncf=\"" $0->negativeControlFlow "\" />\n"]
;

Source(a) ->
  [xml: { in(); } "<source ncf=\"" $0->negativeControlFlow "\" />\n"]
;
