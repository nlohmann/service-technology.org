%{
// map library
#include <map>

// header from configure
#include "config.h"
// header for graph class
#include "Graph.h"

// from main.cc
extern Graph* og;


// from flex
extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);


bool foundRootNode;
Node* currentNode;
%}

%name-prefix="og_yy"
%error-verbose
%token_table
%defines

%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S
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
  KEY_INTERFACE input output synchronous KEY_NODES
  {
    foundRootNode = false;
  }
  nodes
;


input:
  /* empty */
| KEY_INPUT identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
| identlist COMMA IDENT
;


nodes:
  node
| nodes node
;


node:
  NUMBER
  {
    currentNode = new Node($1);
  }
  annotation successors
  {
    og->nodes[$1] = currentNode;
  }
;


annotation:
  /* empty */
| COLON formula
| DOUBLECOLON BIT_S
| DOUBLECOLON BIT_F
;


formula:
  LPAR formula RPAR
| formula OP_AND formula
| formula OP_OR formula
| OP_NOT formula
| KEY_FINAL
| KEY_TRUE
| KEY_FALSE
| IDENT
  { 
    // since lexer uses strdup() to copy the ident it has to be freed
    free($1); 
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
;
