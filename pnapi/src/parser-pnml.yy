 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_pnml_yy"

/* write tokens to parser-pnml.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{
#include "parser.h"
#include <string>
#include <sstream>

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error
#define yylex pnapi::parser::pnml::lex
#define yylex_destory pnapi::parser::pnml::lex_destroy
#define yyparse pnapi::parser::pnml::parse

using namespace pnapi;
using namespace pnapi::parser::pnml;

#include <map>
#include <set>
#include <string>
std::map<std::string, std::string> currentAttributes;
pnapi::Transition *currentTransition;
pnapi::Marking* currentMarking;
%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%token X_OPEN X_SLASH X_CLOSE X_NEXT X_STRING X_EQUALS
%token NUMBER IDENT
%token KEY_PNML KEY_MODULE KEY_PORT KEY_PORTS KEY_NET KEY_TEXT KEY_PLACE
%token KEY_INITIALMARKING KEY_TRANSITION KEY_ARC KEY_FINALMARKINGS KEY_MARKING
%token KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS KEY_SEND KEY_RECEIVE KEY_SYNCHRONIZE
%token KEY_NAME KEY_VALUE KEY_INSCRIPTION

%union
{
  int yt_int;
  char * yt_str;
}

%type <yt_int> NUMBER
%type <yt_int> initialmarking
%type <yt_int> inscription
%type <yt_int> text
%type <yt_str> IDENT
%type <yt_str> X_STRING

%start petrinet


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

petrinet:
  X_OPEN KEY_PNML arbitraryAttributes X_NEXT module X_SLASH KEY_PNML X_CLOSE
| X_OPEN KEY_PNML arbitraryAttributes X_NEXT net X_SLASH KEY_PNML X_CLOSE
;

module:
  KEY_MODULE arbitraryAttributes X_NEXT ports net finalmarkings X_SLASH KEY_MODULE X_NEXT
;

ports:
  KEY_PORTS arbitraryAttributes X_NEXT port_list X_SLASH KEY_PORTS X_NEXT
;

port_list:
  /* empty */
| port_list port
;

port:
  KEY_PORT arbitraryAttributes X_NEXT channels X_SLASH KEY_PORT X_NEXT
;

channels:
  /* empty */
| input_channel channels
| output_channel channels
| synchronous_channel channels
;

input_channel:
  KEY_INPUT arbitraryAttributes X_SLASH X_NEXT
    { pnapi_pnml_yynet.createPlace(currentAttributes["id"], Node::INPUT);
      currentAttributes.clear(); }
;

output_channel:
  KEY_OUTPUT arbitraryAttributes X_SLASH X_NEXT
    { pnapi_pnml_yynet.createPlace(currentAttributes["id"], Node::OUTPUT);
      currentAttributes.clear(); }
;

synchronous_channel:
  KEY_SYNCHRONOUS arbitraryAttributes X_SLASH X_NEXT
    { pnapi_pnml_yynet.addSynchronousLabel(currentAttributes["id"]);
      currentAttributes.clear(); }
;

net:
  KEY_NET arbitraryAttributes X_NEXT nodes X_SLASH KEY_NET X_NEXT
;

nodes:
  /* empty */
| nodes node
;

node:
  place
| transition
| arc
;

place:
  KEY_PLACE arbitraryAttributes X_NEXT initialmarking X_SLASH KEY_PLACE X_NEXT
    { pnapi_pnml_yynet.createPlace(currentAttributes["id"], Node::INTERNAL, $4);
      currentAttributes.clear(); }
| KEY_PLACE arbitraryAttributes X_SLASH X_NEXT
    { pnapi_pnml_yynet.createPlace(currentAttributes["id"]);
      currentAttributes.clear(); }
;

initialmarking:
  /* empty */
    { $$ = 0; }
| KEY_INITIALMARKING arbitraryAttributes X_NEXT text X_SLASH KEY_INITIALMARKING X_NEXT
    { $$ = $4; }
;

text:
  KEY_TEXT arbitraryAttributes X_CLOSE NUMBER X_OPEN X_SLASH KEY_TEXT X_NEXT
    { $$ = $4; }
| KEY_TEXT arbitraryAttributes X_SLASH X_NEXT
    { $$ = 0; }
;

transition:
  KEY_TRANSITION arbitraryAttributes X_SLASH X_NEXT
    { pnapi_pnml_yynet.createTransition(currentAttributes["id"]);
      currentAttributes.clear(); }
| KEY_TRANSITION arbitraryAttributes X_NEXT
    { currentTransition = &pnapi_pnml_yynet.createTransition(currentAttributes["id"]);
      currentAttributes.clear(); }
  events X_SLASH KEY_TRANSITION X_NEXT
;

events:
  /* empty */
| event events
;

event:
  KEY_RECEIVE arbitraryAttributes X_SLASH X_NEXT
    { Node *source = pnapi_pnml_yynet.findNode(currentAttributes["id"]);
      pnapi_pnml_yynet.createArc(*source, *currentTransition);
      currentAttributes.clear(); }
| KEY_SEND arbitraryAttributes X_SLASH X_NEXT
    { Node *target = pnapi_pnml_yynet.findNode(currentAttributes["id"]);
      pnapi_pnml_yynet.createArc(*currentTransition, *target);
      currentAttributes.clear(); }
| KEY_SYNCHRONIZE arbitraryAttributes X_SLASH X_NEXT
    { std::set<std::string> labels = currentTransition->getSynchronizeLabels();
      labels.insert(currentAttributes["id"]);
      currentTransition->setSynchronizeLabels(labels);
      currentAttributes.clear(); }
| KEY_NAME X_NEXT value text X_SLASH KEY_NAME X_NEXT
;

arc:
  KEY_ARC arbitraryAttributes X_SLASH X_NEXT
    { Node *source = pnapi_pnml_yynet.findNode(currentAttributes["source"]);
      Node *target = pnapi_pnml_yynet.findNode(currentAttributes["target"]);
      pnapi_pnml_yynet.createArc(*source, *target);
      currentAttributes.clear(); }
| KEY_ARC arbitraryAttributes X_NEXT inscription X_SLASH KEY_ARC X_NEXT
    { Node *source = pnapi_pnml_yynet.findNode(currentAttributes["source"]);
      Node *target = pnapi_pnml_yynet.findNode(currentAttributes["target"]);
      pnapi_pnml_yynet.createArc(*source, *target, $4);
      currentAttributes.clear(); }
;

inscription:
  KEY_INSCRIPTION arbitraryAttributes X_NEXT value text X_SLASH KEY_INSCRIPTION X_NEXT
    { $$ = $5; }
;

value:
  /* empty */
| KEY_VALUE arbitraryAttributes X_SLASH X_NEXT
| KEY_VALUE arbitraryAttributes X_CLOSE NUMBER X_OPEN X_SLASH KEY_VALUE X_NEXT
;

finalmarkings:
  KEY_FINALMARKINGS arbitraryAttributes X_NEXT markings X_SLASH KEY_FINALMARKINGS X_NEXT
;

markings:
  /* empty */
| markings marking
;

marking:
  KEY_MARKING arbitraryAttributes X_NEXT
    { currentMarking = new pnapi::Marking(pnapi_pnml_yynet); }
  mappings X_SLASH KEY_MARKING X_NEXT
    { pnapi_pnml_yynet.finalCondition().addMarking(*currentMarking);
        delete currentMarking;
      currentAttributes.clear(); }
;

mappings:
  /* empty */
| mappings mapping
;

mapping:
  KEY_PLACE arbitraryAttributes X_NEXT text X_SLASH KEY_PLACE X_NEXT
    { Node *place = pnapi_pnml_yynet.findNode(currentAttributes["id"]);
      (*currentMarking)[(const pnapi::Place&)*place] = $4;
      currentAttributes.clear(); }
;

arbitraryAttributes:
  /* empty */
| IDENT X_EQUALS X_STRING { currentAttributes[$1] = $3; } arbitraryAttributes
;

