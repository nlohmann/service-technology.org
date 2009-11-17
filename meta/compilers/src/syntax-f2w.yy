/*****************************************************************************\
 Fiona2wendy -- compiling Fiona OGs to Wendy OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 Fiona2wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Fiona2wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Fiona2wendy.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


%{
#include "config.h"
#include <iostream>
#include <map>
#include <set>
#include <string>


/// output stream - set in main.cc
extern std::ostream* myOut;


/// from flex
extern char* yytext;
extern int yylex();
extern int yyerror(char const *msg);
extern int colonCount;

// data structures

std::map<unsigned int, std::string> formulae;
std::map<unsigned int, std::map<std::string, unsigned int> > transitions;
unsigned int initialNode;
std::set<std::string> inputs;
std::set<std::string> outputs;
bool interfaceInput;


%}

%error-verbose
%token_table
%defines

%token K_INTERFACE K_INPUT K_OUTPUT K_NODES K_INITIALNODE K_TRANSITIONS
%token COMMA SEMICOLON COLON ARROW
%token K_FINAL K_TRUE SEND RECEIVE LPAR RPAR 
%token IDENT NUMBER

%left OP_AND OP_OR

%union {
    char *str;
    unsigned int value;
    std::string *phi;
}

%type <value> NUMBER
%type <str>   IDENT 
%type <phi>   formula

%start og


%%

og: 
  { 
    colonCount = 0; 
    (*myOut) << "INTERFACE\n  INPUT\n    ";
    interfaceInput = true;
  }
  K_INTERFACE
  K_INPUT identlist SEMICOLON
  { 
    (*myOut) << ";\n  OUTPUT\n    ";
    interfaceInput = false;
  }
  K_OUTPUT identlist SEMICOLON
  { (*myOut) << ";\n\nNODES\n"; }
  K_NODES nodes SEMICOLON
  K_INITIALNODE NUMBER SEMICOLON
  { initialNode = $15 + 1; }
  K_TRANSITIONS transitions SEMICOLON
  {
    // initial node
    (*myOut) << "  " << initialNode << " : " << formulae[initialNode] << std::endl;
    for(std::map<std::string, unsigned int>::iterator t = transitions[initialNode].begin();
         t != transitions[initialNode].end(); ++t)
    {
      (*myOut) << "    " << t->first << " -> " << t->second << std::endl;
    }

    // other nodes
    for(std::map<unsigned int, std::string>::iterator it = formulae.begin();
         it != formulae.end(); ++it)
    {
      if(it->first == initialNode)
        continue;

      (*myOut) << "  " << it->first << " : " << it->second << std::endl;
      for(std::map<std::string, unsigned int>::iterator t = transitions[it->first].begin();
           t != transitions[it->first].end(); ++t)
      {
        (*myOut) << "    " << t->first << " -> " << t->second << std::endl;
      }
    }

    // empty node
    (*myOut) << "  0 : true\n";
    for(std::set<std::string>::iterator it = inputs.begin();
         it != inputs.end(); ++it)
    {
      (*myOut) << "    " << (*it) << " -> 0\n";
    }
    for(std::set<std::string>::iterator it = outputs.begin();
         it != outputs.end(); ++it)
    {
      (*myOut) << "    " << (*it) << " -> 0\n";
    }
  }
;

identlist:
  /* empty */
| IDENT
  { 
    if(interfaceInput)
      inputs.insert($1);
    else
      outputs.insert($1);

    (*myOut) << $1;
    free($1);
  }
| identlist COMMA IDENT
  {
    if(interfaceInput)
      inputs.insert($3);
    else
      outputs.insert($3);

    (*myOut) << ", " << $3;
    free($3);
  }
;

nodes:
  node
| nodes COMMA node
;

node:
  NUMBER COLON formula COLON
  {
    formulae[$1 + 1] = *$3;
    delete $3;
    for(std::set<std::string>::iterator it = inputs.begin();
         it != inputs.end(); ++it)
    {
      transitions[$1 + 1][*it] = 0;
    }
  }
| NUMBER COLON formula COLON COLON
  {
    formulae[$1 + 1] = *$3;
    delete $3;
    for(std::set<std::string>::iterator it = inputs.begin();
         it != inputs.end(); ++it)
    {
      transitions[$1 + 1][*it] = 0;
    }
  }
;

formula:
  LPAR formula RPAR
  {
    *$2 = "(" + (*$2 ) + ")";
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
| K_FINAL
  { $$ = new std::string("final"); }
| K_TRUE
  { $$ = new std::string("true"); }
| SEND IDENT
  {
    $$ = new std::string($2);
    delete $2;
  }
| RECEIVE IDENT
  {
    $$ = new std::string($2);
    delete $2;
  }
;

transitions:
  transition
| transitions COMMA transition
;

transition:
  NUMBER ARROW NUMBER COLON SEND IDENT
  {
    transitions[$1 + 1][$6] = $3 + 1;
    delete $6;
  }
| NUMBER ARROW NUMBER COLON RECEIVE IDENT
  {
    transitions[$1 + 1][$6] = $3 + 1;
    delete $6;
  }
;


%%

