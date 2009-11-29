/*****************************************************************************\
 WendyExpandBits -- expanding Wendy Bit OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 WendyExpandBits is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 WendyExpandBits is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with WendyExpandBits.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%error-verbose
%token_table
%defines

%{

/* 
 * According to [?pnog?] Wendy Bit OGs can be reduced.
 * By the following rules these reductions are reverted:
 *
 * 1.: For each state without successors
 *     the F bit can be added.
 *
 * 2.: For each state only enabeling sending events
 *     the S bit can be added.
 *
 * For both rules: Transitions leading to the Empty Node will be ignored.
 */


#include "config.h"
#include <iostream>
#include <map>
#include <sstream>
#include <string>

// from flex
extern char* yytext;
extern int yylex();
extern int yyerror(char const *msg);

// from main
extern std::ostream * myOut;

// data structures
// list of successors
std::stringstream succ;
// add bit
bool addF;
bool addS;
// read bit
char bit;
// sending events
std::map<std::string, bool> outputs;
// whether reading sending events
bool readOutput;

%}


%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S BIT_T
%token LPAR RPAR

%union {
    char *str;
    unsigned int value;
}

%type <value> NUMBER
%type <str>   IDENT

%left OP_OR
%left OP_AND
%left OP_NOT

%start og


%%


og:
  KEY_INTERFACE
  {
    (*myOut) << "INTERFACE\n";
    addF = addS = readOutput = false;
  }
  input output synchronous 
  KEY_NODES
  { (*myOut) << "\nNODES\n"; }
  nodes
  { (*myOut) << std::endl << std::flush; }
;


input:
  /* empty */
| KEY_INPUT
  { (*myOut) << "  INPUT\n    "; }
  identlist SEMICOLON
  { (*myOut) << ";\n"; }
;


output:
  /* empty */
| KEY_OUTPUT
  {
    (*myOut) << "  OUTPUT\n    ";
    readOutput = true;
  }
  identlist SEMICOLON
  {
    (*myOut) << ";\n";
    readOutput = false;
  }
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS
  { (*myOut) << "  SYNCHRONOUS\n    "; }
  identlist SEMICOLON
  { (*myOut) << ";\n"; }
;


identlist:
  /* empty */
| IDENT
  {
    (*myOut) << $1;
    if(readOutput)
      outputs[$1] = true;
    free($1);
  }
| identlist COMMA IDENT
  {
    (*myOut) << ", " << $3;
    if(readOutput)
      outputs[$3] = true;
    free($3);
  }
;


nodes:
  node
| nodes node
;


node:
  NUMBER annotation successors
  {
    (*myOut) << "  " << $1;
    if(addF)
      bit = 'f';
    else
      if(addS)
        bit = 's';
    switch(bit)
    {
      case 'f' : (*myOut) << " :: F\n"; break;
      case 's' : (*myOut) << " :: S\n"; break;
      case 't' : (*myOut) << " :: T\n"; break;
      case '-' : (*myOut) << "\n"; break;
      default : assert(false);
    }
    (*myOut) << succ.str();
    succ.clear();
    succ.str("");
    addF = addS = false;
  }
;


annotation:
  /* empty */        { bit = '-'; addF = addS = true; }
| COLON formula
  { yyerror("read a formula; can only expand Bit OGs"); }
| DOUBLECOLON BIT_S  { bit = 's'; }
| DOUBLECOLON BIT_F  { bit = 'f'; }
| DOUBLECOLON BIT_T  { bit = 't'; }
;


formula:
  LPAR formula RPAR
| formula OP_AND formula
| formula OP_OR formula
| OP_NOT formula
| KEY_FINAL
| KEY_TRUE
| KEY_FALSE
| IDENT { free($1); }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    succ << "    " << $2 << " -> " << $4 << "\n";

    if($4 != 0) // ignore empty node
    {
      addF = false;
      if(!outputs[$2])
        addS = false;
    }

    free($2);
  }
;


%%


