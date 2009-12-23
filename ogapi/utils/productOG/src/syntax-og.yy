/*****************************************************************************\
 ProductOG -- Computing product OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 ProductOG is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 ProductOG is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with ProductOG.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


%error-verbose
%token_table
%defines

%{

#include "config.h"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <map>
#include <set>

// from flex
extern char* yytext;
extern int yylex();
extern int yyerror(char const *msg);

// from main
extern std::map<unsigned int, std::map<std::string, unsigned int> > succ;
extern std::map<unsigned int, std::string> formulae;
extern std::set<std::string> inputs;
extern std::set<std::string> outputs;
extern std::set<std::string> synchronous;
extern std::map<unsigned int, std::set<std::string> > presentLabels;
extern unsigned int * initialID;

// data structures
std::set<std::string> * labelSet;
unsigned int nodeID;

%}


%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S BIT_T
%token LPAR RPAR

%union {
    char *str;
    unsigned int value;
    std::string * phi;
}

%type <value> NUMBER
%type <str>   IDENT
%type <phi>   annotation formula

%left OP_OR
%left OP_AND
%left OP_NOT

%start og


%%


og:
  KEY_INTERFACE input output synchronous KEY_NODES nodes
;


input:
  /* empty */
| KEY_INPUT
  { labelSet = &inputs; }
  identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT
  { labelSet = &outputs; }
  identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS
  { labelSet = &synchronous; }
  identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  {
    labelSet->insert($1);
    free($1);
  }
| identlist COMMA IDENT
  {
    labelSet->insert($3);
    free($3);
  }
;


nodes:
  node
| nodes node
;


node:
  NUMBER annotation
  {
    if(initialID == NULL)
      initialID = new unsigned int($1);
    nodeID = $1;
    formulae[$1] = (*$2);
    delete $2;
  }
  successors
;


annotation:
  /* empty */        { $$ = new std::string(); }
| COLON formula      { $$ = $2; }
| DOUBLECOLON BIT_S  { yyerror("can only produce formula OGs"); }
| DOUBLECOLON BIT_F  { yyerror("can only produce formula OGs"); }
| DOUBLECOLON BIT_T  { yyerror("can only produce formula OGs"); }
;


formula:
  LPAR formula RPAR
  {
    (*$2) = "(" + (*$2) + ")";
    $$ = $2;
  }
| formula OP_AND formula
  {
    (*$1) += " * " + (*$3);
    delete $3;
    $$ = $1;
  }
| formula OP_OR formula
  {
    (*$1) += " + " + (*$3);
    delete $3;
    $$ = $1;
  }
| OP_NOT formula
  {
    (*$2) = "~" + (*$2);
    $$ = $2;
  }
| KEY_FINAL
  { $$ = new std::string("final"); }
| KEY_TRUE
  { $$ = new std::string("true"); }
| KEY_FALSE
  { $$ = new std::string("false"); }
| IDENT
  {
    $$ = new std::string($1);
    free($1);
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    succ[nodeID][$2] = $4;
    presentLabels[nodeID].insert($2);
    free($2);
  }
;


%%


