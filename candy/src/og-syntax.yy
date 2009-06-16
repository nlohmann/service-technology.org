%{
// map library
#include <map>
// string library
#include <string>

// header from configure
#include "config.h"
// header for graph class
#include "Graph.h"

#include "Formula.h"

// from main.cc
extern Graph* ogGraph;


// from flex
extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);

using std::pair;
using std::string;


bool foundRootNode;
Node* currentNode;
Node* currentSuccessor;
Formula* currentFormula;
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
    Formula* form;
}

%type <value> NUMBER
%type <str>   IDENT
%type <form> formula

%left OP_OR
%left OP_AND
%left OP_NOT

%start og
%%


og:
  KEY_INTERFACE input output synchronous KEY_NODES
  {
    foundRootNode = false;
    currentNode = NULL;
    currentSuccessor = NULL;
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
    currentNode->formula = currentFormula;
    ogGraph->nodes[$1] = currentNode;
    if ( foundRootNode == false ) {
        // first found node is per definition the root node
        foundRootNode = true;
        ogGraph->root = currentNode;
    }
  }
;


annotation:
  /* empty */
| COLON formula
  {
    currentFormula = $2;
  }
| DOUBLECOLON BIT_S
  {
    // parsing 2-bit OGs is not supported
    og_yyerror("read a 2-bit annotation; only formulae are supported");
    return EXIT_FAILURE;
  }
| DOUBLECOLON BIT_F
  {
    // parsing 2-bit OGs is not supported
    og_yyerror("read a 2-bit annotation; only formulae are supported");
    return EXIT_FAILURE;
  }
;


formula:
  LPAR formula RPAR
  {
    $$ = $2;
  }
| formula OP_AND formula
  {
    $$ = new FormulaMultiaryAnd($1, $3);
  }
| formula OP_OR formula
  {
    $$ = new FormulaMultiaryOr($1, $3);
  }
| OP_NOT formula
  {
    //$$ = new pnapi::formula::Negation(*$2);
    $$ = NULL;
  }
| KEY_FINAL
  {
    $$ = new FormulaLiteralFinal();
  }
| KEY_TRUE
  {
    $$ = new FormulaTrue();
  }
| KEY_FALSE
  {
    $$ = new FormulaFalse();
  }
| IDENT
  { 
    $$ = new FormulaLiteral($1);

    // since lexer uses strdup() to copy the ident it has to be freed
    free($1); 
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    // find known successor node or create it
    if ( ogGraph->nodes.find($4) != ogGraph->nodes.end() ) {
        currentSuccessor = (ogGraph->nodes.find($4))->second;
    } else {
        currentSuccessor = new Node($4);
    }

    // register successor node as successor
    currentNode->successors.push_back( pair<string, Node*>($2, currentSuccessor) );
    free($2);
  }
;
