%error-verbose
%token_table
%defines

%{
#include "config.h"
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
// resulting bit
std::string bit;

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
  { (*myOut) << "\nINTERFACE\n"; }
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
  NUMBER
  {
    (*myOut) << "  " << $1;
    // if we read a formula and this bit was not overwritten, we only read send events
    bit = " :: S\n";
  }
  annotation successors
;


annotation:
  /* empty; i.e. we do not write a bit */
| COLON formula { (*myOut) << bit; }
| DOUBLECOLON BIT_S { yyerror("this seems to be already a bit-OG"); }
| DOUBLECOLON BIT_F { yyerror("this seems to be already a bit-OG"); }
| DOUBLECOLON BIT_T { yyerror("this seems to be already a bit-OG"); }
;


formula:
  LPAR formula RPAR
| formula OP_AND formula
| formula OP_OR formula
| OP_NOT formula
| KEY_FINAL { bit = " :: F\n"; } 
| KEY_TRUE  { bit = " :: T\n"; }
| KEY_FALSE
| IDENT
  {
    if(!events[$1]) // if we read an event that is no output event
      bit = "\n"; // set no bit
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


