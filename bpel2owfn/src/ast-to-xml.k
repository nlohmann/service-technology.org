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
 *          - last changed: \$Date: 2005/12/04 14:00:33 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \note    This file was created using Kimwitu++ version 2.3.8 (C) 1998-2003
 *          Humboldt-University of Berlin reading file bpel-unparse.k. See
 *          http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.14 $
 *          - 2005-11-10 (nlohmann) Added doxygen comments.
 *          - 2005-11-11 (nlohmann) Moved #intToString to \ref helpers.cc.
 *          - 2005-11-18 (nlohmann) Identifiers are printed.
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



%{
/*!
 * \class kc::xml_class
 * \brief Unparse rules: XML
 */
%}
%uview xml;

/******************************************************************************/

// All the includes and variables can be used during the unparsing.
%{ KC_UNPARSE
#include <iostream>
#include "helpers.h"

/// number of spaces to be added as indention
unsigned int indent = 0;

/// number of spaces to be added when indention is increased
unsigned int indentStep = 4;


/// Adds whitespace to indent output
void in()
{
  for(unsigned int i=0; i<indent; i++)
    std::cout << " ";
}


/// Increases indention
void inup()
{
  in();
  indent += indentStep;
}


///Decreases indention
void indown()
{
  indent -= indentStep;
  in();
}





%}






/******************************************************************************/

// All the includes, variables and structs defined here can be used in the
// printers below.
%{
#include <iostream>
%}

/******************************************************************************/






Process(a,b,c,d,e,f,g,h,i) ->
  [xml: { inup(); } "<process id=\"" $0->id ">\n" 
  
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

PartnerLink() ->
  [xml: { in(); } "<partnerLink />\n"]
;


/******************************************************************************
  PARTNERS
******************************************************************************/

Partner(a) ->
  [xml: { inup(); } "<partner>\n" a { indown(); } "</partner>\n"]
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

userDefinedFaultHandler(a,b) ->
  [xml: { inup(); } "<faultHandlers>\n" a b { indown(); } "</faultHandlers>\n"]
;

Catch(a) ->
  [xml: { inup(); } "<catch>\n" a { indown(); } "</catch>\n"]
;

CatchAll(a) ->
  [xml: { inup(); } "<catchAll>\n" a { indown(); } "</catchAll>\n"]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

EventHandler(a,b) ->
  [xml: { inup(); } "<eventHandlers>\n" a b { indown(); } "</eventHandlers>\n"]
;

OnMessage(a) ->
  [xml: { inup(); } "<onMessage>\n" a { indown(); } "</onMessage>\n"]
;

OnAlarm(a) ->
  [xml: { inup(); } "<onAlarm>\n" a { indown(); } "</onAlarm>\n"]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

userDefinedCompensationHandler(a) ->
  [xml: { inup(); } "<compensationHandler>\n" a { indown(); } "</compensationHandler>\n"]
;


/******************************************************************************
  VARIABLES
******************************************************************************/

Variable() ->
  [xml: { in(); } "<variable />\n"]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet() ->
  [xml: { in(); } "<correlationSet />\n"]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation() ->
  [xml: { in(); } "<correlation />\n"]
;


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a) ->
  [xml: { inup(); } "<empty id=\"" $0->id "\">\n" a { indown(); } "</empty>\n"];


/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b) ->
  [xml: { inup(); } "<invoke id=\"" $0->id "\">\n" a b { indown(); } "</invoke>\n"]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b) ->
  [xml: { inup(); } "<receive id=\"" $0->id "\">\n" a b { indown(); } "</receive>\n"]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b) ->
  [xml: { inup(); } "<reply id=\"" $0->id "\">\n" a b { indown(); } "</reply>\n"]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b) ->
  [xml: { inup(); } "<assign id=\"" $0->id "\">\n" a b { indown(); } "</assign>\n"]
;

Copy(a,b) ->
  [xml: { inup(); } "<copy>\n" a b { indown(); } "</copy>\n"]
;

From() ->
  [xml: { in(); } "<from />\n"]
;

To() ->
  [xml: { in(); } "<to />\n"]
;


/******************************************************************************
  WAIT
******************************************************************************/

Wait(a) ->
  [xml: { inup(); } "<wait id=\"" $0->id "\">\n" a { indown(); } "</wait>\n"]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a) ->
  [xml: { inup(); } "<throw id=\"" $0->id "\">\n" a { indown(); } "</throw>\n"]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a) ->
  [xml: { inup(); } "<compensate id=\"" $0->id "\">\n" a { indown(); } "</compensate>\n"]
;


/******************************************************************************
  TERMINATE
******************************************************************************/

Terminate(a) ->
  [xml: { inup(); } "<terminate id=\"" $0->id "\">\n" a { indown(); } "</terminate>\n"]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,NiltLink_list(),c) ->
  [xml: { inup(); } "<flow id=\"" $0->id "\">\n" a c { indown(); } "</flow>\n"]
;

Flow(a,b,c) ->
  [xml: { inup(); } "<flow id=\"" $0->id "\">\n" a { inup(); } "<links>\n" b { indown(); } "</links>\n" c { indown(); } "</flow>\n"]
;

Link() ->
  [xml: { in(); } "<link />\n"]
;


/******************************************************************************
  SWITCH
******************************************************************************/

Switch(a,b,c) ->
  [xml: { inup(); } "<switch id=\"" $0->id "\">\n" a b c { indown(); } "</switch>\n"]
;

Case(a) ->
  [xml: { inup(); } "<case>\n" a { indown(); } "</case>\n"]
;

Otherwise(a) ->
  [xml: { inup(); } "<otherwise>\n" a { indown(); } "</otherwise>\n"]
;


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b) ->
  [xml: { inup(); } "<while id=\"" $0->id "\">\n" a b { indown(); } "</while>\n"]
;


/******************************************************************************
  SEQUENCE
******************************************************************************/

activitySequence(Sequence(a,b)) ->
  [xml: { inup(); } "<sequence id=\"" $0->id "\">\n" a b { indown(); } "</sequence>\n"]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c) ->
  [xml: { inup(); } "<pick id=\"" $0->id "\">\n" a b c { indown(); } "</pick>\n"]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g) ->
  [xml: { inup(); } "<scope id=\"" $0->id "\">\n" 
  
  a

  { if (b->length() == 0) }
    ${ /* no Variables */ $}
  { else }
    ${ { inup(); } "<variables id=\"" $0->id "\">\n" b { indown(); } "</variables>\n" $}

  c d e f g
    
  { indown(); } "</scope>\n" ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

Target() ->
  [xml: { in(); } "<target />\n"]
;

Source() ->
  [xml: { in(); } "<source />\n"]
;





/*****************************************************************************/
// the printer functions

/// printer-function for output on stdout
void printer(const char *s, uview v)
{
  std::cout << s;
}
