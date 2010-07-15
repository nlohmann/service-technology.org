%{
#include <iostream>
#include <map>
#include <string>
#include "Graph.h"

using std::map;
using std::string;

enum Interface { INPUT, OUTPUT, SYNCHRONOUS };

/* Prologue: Syntax and usage of the prologue.
 * Bison Declarations: Syntax and usage of the Bison declarations section.
 * Grammar Rules: Syntax and usage of the grammar rules section.
 * Epilogue: Syntax and usage of the epilogue.  */

// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// from flex
extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);

Graph G;
map<unsigned int, Node*> tempMap;
Interface interface = INPUT;
Node *temp = NULL;
%}

%name-prefix="og_yy"
%error-verbose

%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S BIT_T
%token LPAR RPAR

%left OP_OR
%left OP_AND
%left OP_NOT

%token-table
%defines

%union {
    char *str;
    unsigned int value;
    Formula *f;
    Node *n;
}

%type <value> NUMBER
%type <str>   IDENT
%type <f>     formula
%type <n>     node

%start og
%%


og:
  KEY_INTERFACE input output synchronous KEY_NODES nodes
  { tempMap.clear(); }
;


input:
  /* empty */
| KEY_INPUT identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT { interface = OUTPUT; } identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS {interface = SYNCHRONOUS; } identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  { 
    if (interface == INPUT) G.addLabel("?" + string($1));
    else if (interface == OUTPUT) G.addLabel("!" + string($1));
    else G.addLabel("#" + string($1));
  }
| identlist COMMA IDENT
  { 
    if (interface == INPUT) G.addLabel("?" + string($3));
    else if (interface == OUTPUT) G.addLabel("!" + string($3));
    else G.addLabel("#" + string($3));
  }
;


nodes:
  node
  {
    G.addNode($1);
    G.setRoot($1);
    tempMap[$1->id] = $1;
  }
| nodes node
  { 
    G.addNode($2);
    tempMap[$2->id] = $2;
  }
;


node:
  NUMBER 
  { 
    if (tempMap.count($1) > 0) 
      temp = tempMap[$1]; 
    else 
      temp = new Node($1); 
  } 
  annotation successors
  { $$ = temp; }
;


annotation:
  /* empty */
| COLON formula
  {
    G.setType(Graph::FORMULA);
    temp->setFormula($2); 
  }
| DOUBLECOLON BIT_S
  { 
    G.setType(Graph::BIT);
    temp->setBit(Node::S); 
  }
| DOUBLECOLON BIT_F
  { 
    G.setType(Graph::BIT);
    temp->setBit(Node::F); 
  }
| DOUBLECOLON BIT_T
  { 
    G.setType(Graph::BIT);
    temp->setBit(Node::T); 
  }
;


formula:
  LPAR formula RPAR
  { $$ = $2; }
| formula OP_AND formula
  { $$ = new FormulaAND($1, $3); }
| formula OP_OR formula
  { $$ = new FormulaOR($1, $3); }
| OP_NOT formula
  { $$ = new FormulaNeg($2); }
| KEY_FINAL
  { $$ = new FormulaFinal(); }
| KEY_TRUE
  { $$ = new FormulaTrue(); }
| KEY_FALSE
  { $$ = new FormulaFalse(); }
| IDENT
  { $$ = new FormulaLit($1); }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    if (!tempMap.count($4))
    {
      Node *h = new Node($4);
      tempMap[h->id] = h;
    }
    temp->addEdge($2, tempMap[$4]);
  }
;
