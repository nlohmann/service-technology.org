/*****************************************************************************\
 WendyFormula2Bits -- compiling Wendy CNF OGs to Wendy Bit OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 WendyFormula2Bits is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 WendyFormula2Bits is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with WendyFormula2Bits.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%error-verbose
%token_table
%defines

%{
#include "config.h"
#include "types-wf2b.h"
#include <iostream>
#include <map>
#include <string>

// from flex
extern char* yytext;
extern int yylex();
extern int yyerror(char const *msg);

// from main
extern std::ostream * myOut;

// data structures
// whether to parse output events
bool outputEvent;
// events and whether they are input events
std::map<std::string, bool> events;


/*
 * Nodes' formulae will be evaluated as follows:
 *  - if a formula containes the condition "final",
 *    the F flag will be set
 *  - if the formula evaluates to "true", the T flag
 *    will be set (true AND x evaluates to x;
 *    true OR x evaluates to true)
 *  - if the formula can only be satisfied by sending
 *    something, the S bit will be set
 *    (S and x evaluates to S; S or x evaluates to x)
 *  - otherwise no bit will be set
 */

Bit_T bitsAnd[][4] = {{NONE, SEND, NONE, FINAL},
                     {SEND, SEND, SEND, FINAL},
                     {NONE, SEND, TRUE, FINAL},
                     {FINAL, FINAL, FINAL, FINAL}};

Bit_T bitsOr[][4] = {{NONE, NONE, TRUE, FINAL},
                    {NONE, SEND, TRUE, FINAL},
                    {TRUE, TRUE, TRUE, FINAL},
                    {FINAL, FINAL, FINAL, FINAL}};

char bitLabels[][10] = {"\n", " :: S\n", " :: T\n", " :: F\n"};

%}


%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S BIT_T
%token LPAR RPAR

%union {
    char *str;
    unsigned int value;
    Bit_T bit;
}

%type <value> NUMBER
%type <str>   IDENT
%type <bit>   formula annotation

%left OP_OR
%left OP_AND
%left OP_NOT

%start og


%%


og:
  KEY_INTERFACE
  { (*myOut) << "INTERFACE\n"; }
  input output synchronous
  { (*myOut) << "\nNODES\n"; }
  KEY_NODES nodes
  { (*myOut) << std::endl << std::flush; }
;


input:
  /* empty */
| KEY_INPUT
  {
    (*myOut) << "  INPUT\n    ";
    outputEvent = false;
  }
  identlist SEMICOLON
  { (*myOut) << ";\n"; }
;


output:
  /* empty */
| KEY_OUTPUT
  {
    (*myOut) << "  OUTPUT\n    ";
    outputEvent = true;
  }
  identlist SEMICOLON
  { (*myOut) << ";\n"; }
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS
  {
    (*myOut) << "  SYNCHRONOUS\n    ";
    outputEvent = false;
  }
  identlist SEMICOLON
  { (*myOut) << ";\n"; }
;


identlist:
  /* empty */
| IDENT
  {
    events[$1] = outputEvent;
    (*myOut) << $1;
    free($1);
  }
| identlist COMMA IDENT
  {
    events[$3] = outputEvent;
    (*myOut) << ", " << $3;
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
    (*myOut) << "  " << $1 << bitLabels[$2];
    // if we read a formula and this bit was not overwritten, we only read send events
  }
  successors
;


annotation:
  /* empty; i.e. we do not write a bit */ 
  { $$ = NONE; }
| COLON formula     { $$ = $2; }
| DOUBLECOLON BIT_S { yyerror("this seems to be already a bit-OG"); }
| DOUBLECOLON BIT_F { yyerror("this seems to be already a bit-OG"); }
| DOUBLECOLON BIT_T { yyerror("this seems to be already a bit-OG"); }
;


formula:
  LPAR formula RPAR       { $$ = $2; }
| formula OP_AND formula  { $$ = bitsAnd[$1][$3]; }
| formula OP_OR formula   { $$ = bitsOr[$1][$3]; }
| OP_NOT formula          { $$ = NONE; }
| KEY_FINAL               { $$ = FINAL; } 
| KEY_TRUE                { $$ = TRUE; }
| KEY_FALSE               { $$ = NONE; }
| IDENT
  { 
    $$ = (events[$1] ? SEND : NONE);
    free($1);
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    (*myOut) << "    " << $2 << " -> " << $4 << "\n";
    free($2);
  }
;


%%


