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
// initial node ID
unsigned int initialNode;
// list of successors
std::map<unsigned int, std::map<std::string, unsigned int> > succ;
// output labels (event type has to be switched)
std::set<std::string> outputs;
// synchronous labels
std::set<std::string> synchronous;
// whether reading later output places
bool interfaceOutput;
// recent node ID
unsigned int nodeID;
// whether node is finalnode
std::map<unsigned int, bool> finalNodes;


%}

%error-verbose
%token_table
%defines

%token K_INTERFACE K_INPUT K_OUTPUT K_NODES K_INITIALNODE K_TRANSITIONS
%token COMMA SEMICOLON COLON ARROW
%token K_FINAL K_TRUE K_FALSE SEND RECEIVE SYNCHRONOUS LPAR RPAR 
%token IDENT NUMBER

%left OP_AND OP_OR

%union {
    char *str;
    unsigned int value;
}

%type <value> NUMBER
%type <str>   IDENT

%start og


%%

og: 
  K_INTERFACE
  { 
    colonCount = 0; // initialize for flex
    interfaceOutput = true;
    (*myOut) << "INTERFACE\n  INPUT\n    ";
  }
  K_INPUT identlist SEMICOLON
  { interfaceOutput = false ;}
  K_OUTPUT identlist SEMICOLON
  K_NODES nodes SEMICOLON
  K_INITIALNODE NUMBER SEMICOLON
  { initialNode = $14; }
  K_TRANSITIONS transitions SEMICOLON
  {
    (*myOut) << ";\n  OUTPUT\n    ";
    std::string delim = ""; // delimeter
    for(std::set<std::string>::iterator o = outputs.begin();
         o != outputs.end(); ++o)
    {
      (*myOut) << delim << *o;
      delim = ", ";
    }
    (*myOut) << ";\n\nNODES\n";



    for(std::map<unsigned int, std::map<std::string, unsigned int> >::iterator it = succ.begin();
         it != succ.end(); ++it)
    {
      std::string delim = " : "; // delimeter
      (*myOut) << "  " << it->first;
      if(it->first == initialNode)
      {
        (*myOut) << delim << "INITIAL";
        delim = ", ";
      }
      if(finalNodes[it->first])
        (*myOut) << delim << "FINAL";
      (*myOut) << "\n";

      for(std::map<std::string, unsigned int>::iterator s = it->second.begin();
           s != it->second.end(); ++s)
      {
        (*myOut) << "    " << s->first << " -> " << s->second << "\n";
      }
    }
    (*myOut) << std::endl << std::flush;
  }
;

identlist:
  /* empty */
| IDENT
  {
    if(interfaceOutput)
    {
      outputs.insert($1);
    }
    else
    {
      (*myOut) << $1;
    }
    free($1);
  }
| identlist COMMA IDENT
  {
    if(interfaceOutput)
    {
      outputs.insert($3);
    }
    else
    {
      (*myOut) << ", " << $3;
    }
    free($3);
  }
;

nodes:
  node
| nodes COMMA node
;

node:
  NUMBER
  {
    succ[$1]; // add node to list
    nodeID = $1;
  }
  COLON formula colon
;

colon:
  COLON
| COLON COLON
;

formula:
  LPAR formula RPAR
| formula OP_AND formula
| formula OP_OR formula
| K_FINAL { finalNodes[nodeID] = true; }
| K_TRUE
| K_FALSE
| SEND IDENT { free($2); }
| RECEIVE IDENT { free($2); }
| SYNCHRONOUS IDENT { synchronous.insert($2); free($2); }
;

transitions:
  transition
| transitions COMMA transition
;

transition:
  NUMBER ARROW NUMBER COLON SEND IDENT
  {
    succ[$1][$6] = $3;
    free($6);
  }
| NUMBER ARROW NUMBER COLON RECEIVE IDENT
  {
    succ[$1][$6] = $3;
    free($6);
  }
| NUMBER ARROW NUMBER COLON SYNCHRONOUS IDENT
  {
    succ[$1][$6] = $3;
    synchronous.insert($6);
    free($6);
  }
;


%%


