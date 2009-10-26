/* *****************************************************************************\
   * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf,      *
   *                      Jan Bretschneider, Christian Gierds                  *
   *                                                                           *
   * This file is part of Fiona.                                               *
   *                                                                           *
   * Fiona is free software; you can redistribute it and/or modify it          *
   * under the terms of the GNU General Public License as published by the     *
   * Free Software Foundation; either version 2 of the License, or (at your    *
   * option) any later version.                                                *
   *                                                                           *
   * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
   * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
   * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
   * more details.                                                             *
   *                                                                           *
   * You should have received a copy of the GNU General Public License along   *
   * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
   * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
   *****************************************************************************/

/*!
 * \file    syntax_owfn.yy
 *
 * \brief   the parser
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */


%{
/* Prologue: Syntax and usage of the prologue.
 * Bison Declarations: Syntax and usage of the Bison declarations section.
 * Grammar Rules: Syntax and usage of the grammar rules section.
 * Epilogue: Syntax and usage of the epilogue.  */

// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 1  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


// from flex
extern char* frontend_owfn_text;
extern int frontend_owfn_lex();

// defined in "debug.h"
extern int frontend_owfn_error(const char *);




#include <stdio.h>
#include "globals.h"
#include "petrinet.h"
#include "helpers.h"
#include <limits.h>
#include <set>
#include <string>
using namespace std;
using namespace PNapi;

extern PetriNet PN;

set<string> in;
set<string> out;
set<Place*> finalMarking;
string nodename;
string current_port;
int readmode=0;
PNapi::Transition *t = NULL; 



%}


// Bison options
%name-prefix="frontend_owfn_"

// Bison generates a list of all used tokens in file "syntax.h" (for flex)
%token_table

%defines

%union {
    kc::casestring yt_casestring;
}

%type <yt_casestring> IDENT
%type <yt_casestring> nodeident
%type <yt_casestring> NUMBER
%type <yt_casestring> NEGATIVE_NUMBER


// the terminal symbols (tokens)

%token KEY_SAFE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT KEY_PORTS
%token KEY_MARKING KEY_FINALMARKING KEY_FINALCONDITION
%token KEY_TRANSITION KEY_CONSUME KEY_PRODUCE
%token KEY_ALL_OTHER_PLACES_EMPTY
%token KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
%token KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
%token KEY_MAX_UNIQUE_EVENTS KEY_ON_LOOP KEY_MAX_OCCURRENCES
%token KEY_TRUE KEY_FALSE LCONTROL RCONTROL
%token COMMA COLON SEMICOLON IDENT NUMBER NEGATIVE_NUMBER
%token LPAR RPAR

%left OP_OR
%left OP_AND
%left OP_NOT
%nonassoc OP_EQ OP_NE OP_GT OP_LT OP_GE OP_LE


// the start symbol of the grammar
%start net



%%

/* Grammar rules */

net:
    KEY_PLACE place_area port_area KEY_MARKING
    markinglist SEMICOLON final transitionlist
;

final: KEY_FINALMARKING finalmarkinglist SEMICOLON
| KEY_FINALCONDITION statepredicate SEMICOLON
;

place_area: place_area_internal place_area_input  place_area_output
	  | place_area_lola
;

place_area_input: 
  {readmode = 0;}
  KEY_INPUT placelists SEMICOLON
| /* empty */
;

place_area_output: 
  {readmode = 1;}
  KEY_OUTPUT placelists SEMICOLON
| /* empty */
;

place_area_internal: 
  {readmode = 2;}
  KEY_INTERNAL placelists SEMICOLON
| /* empty */
;

place_area_lola: 
  {readmode = 2;}
  placelists SEMICOLON
;

placelists: capacity placelist 
| placelists SEMICOLON capacity placelist
;

capacity: 
| KEY_SAFE    COLON  
| KEY_SAFE NUMBER  COLON 
;

placelist:  
   placelist COMMA place 
|  place
|  /* empty */ 
;

place: 
  nodeident controlcommands
  {
    switch (readmode)
    {
      case 0:    PN.newPlace(nodename, IN); break;
      case 1:    PN.newPlace(nodename, OUT); break;
      case 2:    PN.newPlace(nodename); break;
      case 3:    break;
    }
  }
;

nodeident: IDENT {nodename = strip_namespace($1->name);}
| NUMBER  {nodename = strip_namespace($1->name);}
;

controlcommands:
  /* emtpy */
| LCONTROL commands RCONTROL
;

commands:
  /* empty */
| KEY_MAX_UNIQUE_EVENTS OP_EQ NUMBER commands
    {
      globals::owfn_commands[nodename] = globals::owfn_commands[nodename] + "MAX_UNIQUE_EVENTS = " + strip_namespace($3->name) + " ";
    }
| KEY_ON_LOOP OP_EQ KEY_TRUE commands
    {
      globals::owfn_commands[nodename] = globals::owfn_commands[nodename] + "ON_LOOPS = TRUE ";
    }
| KEY_ON_LOOP OP_EQ KEY_FALSE commands
    {
      globals::owfn_commands[nodename] = globals::owfn_commands[nodename] + "ON_LOOPS = FALSE ";
    }
| KEY_MAX_OCCURRENCES OP_EQ NUMBER commands
    {
      globals::owfn_commands[nodename] = globals::owfn_commands[nodename] + "MAX_OCCURRENCES = " + strip_namespace($3->name) + " ";
    }
| KEY_MAX_OCCURRENCES OP_EQ NEGATIVE_NUMBER commands
    {
      globals::owfn_commands[nodename] = globals::owfn_commands[nodename] + "MAX_OCCURRENCES = " + strip_namespace($3->name) + " ";
    }
;

markinglist:
  /* empty */ 
| marking
| markinglist COMMA marking
;

marking: 
  nodeident COLON NUMBER 
      {
       (PN.findPlace(nodename))->mark(toUInt(strip_namespace($3->name)));
      } 
| nodeident
      {
       (PN.findPlace(nodename))->mark();
      }
;

finalmarkinglist: 
  /* empty */
| 
  finalmarking
    {
      if (!finalMarking.empty())
      {
        PN.final_set_list.push_back(finalMarking);
        finalMarking.clear();
      }
    }

| 
  finalmarkinglist COMMA finalmarking
    {
      if (!finalMarking.empty())
      {
        PN.final_set_list.push_back(finalMarking);
        finalMarking.clear();
      }
    }
;

finalmarking: 
  nodeident COLON NUMBER 
    {
      (PN.findPlace(nodename))->isFinal = true; // BAM
      finalMarking.insert(PN.findPlace(nodename));
    }
| 
  nodeident
    {
      (PN.findPlace(nodename))->isFinal = true;
      finalMarking.insert(PN.findPlace(nodename));
    }
;





/*************
 * the ports *
 *************/

port_area:
  /* empty */
| KEY_PORTS port_list
;

port_list:
  port_definition
| port_list port_definition
;

port_definition:
  nodeident
    { current_port = std::string($1->name); }
  COLON port_participant_list SEMICOLON
;

port_participant_list:
  nodeident
    { PN.setPlacePort(PN.findPlace(string($1->name)), current_port); }
| port_participant_list COMMA nodeident
    { PN.setPlacePort(PN.findPlace(string($3->name)), current_port); }
;






transitionlist: transitionlist transition
| /* empty */
;

transition: KEY_TRANSITION tname
	{
	  t = PN.newTransition(nodename);
	}
        annotation
	KEY_CONSUME
	{
	  readmode = 4;
	}
	arclist SEMICOLON KEY_PRODUCE
	{
	  readmode = 5;
	}
	arclist SEMICOLON
	{
	}
;


tname:   IDENT {nodename = strip_namespace($1->name);}
| NUMBER {nodename = strip_namespace($1->name);}
;

annotation:
  /* empty */
| LCONTROL annotation_list RCONTROL
;

annotation_list:
  IDENT { t->add_label(string($1->name)); }
| IDENT { t->add_label(string($1->name)); } COMMA annotation_list
;

arclist: 
  /* empty */
| arc 
| arc COMMA arclist 
;

arc: 
  nodeident COLON NUMBER 
    { 
      if (readmode == 4)
        PN.newArc(PN.findPlace(nodename), t, STANDARD, toInt(strip_namespace($3->name)) );
      if (readmode == 5) 
        PN.newArc(t, PN.findPlace(nodename), STANDARD, toInt(strip_namespace($3->name)) );
    }
| nodeident
    { 
      if (readmode == 4)
        PN.newArc(PN.findPlace(nodename), t, STANDARD, 1);
      if (readmode == 5) 
        PN.newArc(t, PN.findPlace(nodename), STANDARD, 1);
    }
;

statepredicate:
  /* empty */
| LPAR 
  {
    globals::owfn_statepredicate += "(";  
  }
  statepredicate 
  RPAR 
  {
    globals::owfn_statepredicate += ")";  
  }

| statepredicate OP_AND  
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " AND ";  
  }
  statepredicate
| statepredicate OP_AND KEY_ALL_OTHER_PLACES_EMPTY 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " AND ALL_OTHER_PLACES_EMPTY";  
  }
| statepredicate OP_AND KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " AND ALL_OTHER_INTERNAL_PLACES_EMPTY";  
  }
| statepredicate OP_AND KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " AND ALL_OTHER_EXTERNAL_PLACES_EMPTY";  
  }
| statepredicate OP_OR 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " OR ";  
  }
  statepredicate 
| OP_NOT 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + "NOT ";  
  }
  statepredicate 
| nodeident OP_EQ NUMBER 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " " + nodename + " = " + strip_namespace($3->name); 
    Place * p = PN.findPlace(nodename);
    assert(p != NULL);
    p->isFinal = true;
  }
| nodeident OP_NE NUMBER 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " " + nodename + " != " + strip_namespace($3->name);  
    Place * p = PN.findPlace(nodename);
    assert(p != NULL);
    p->isFinal = true;
  }
| nodeident OP_LT NUMBER 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " " + nodename + " < " + strip_namespace($3->name);  
    Place * p = PN.findPlace(nodename);
    assert(p != NULL);
    p->isFinal = true;
  }
| nodeident OP_GT NUMBER 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " " + nodename + " > " + strip_namespace($3->name);  
    Place * p = PN.findPlace(nodename);
    assert(p != NULL);
    p->isFinal = true;
  }
| nodeident OP_GE NUMBER 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " " + nodename + " >= " + strip_namespace($3->name);  
    Place * p = PN.findPlace(nodename);
    assert(p != NULL);
    p->isFinal = true;
  }
| nodeident OP_LE NUMBER 
  {
    globals::owfn_statepredicate = globals::owfn_statepredicate + " " + nodename + " <= " + strip_namespace($3->name);  
    Place * p = PN.findPlace(nodename);
    assert(p != NULL);
    p->isFinal = true;
  }
;
