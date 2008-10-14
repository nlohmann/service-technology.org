%{
#include <libgen.h>
#include "Graph.h"
#include "Formula.h"

Graph G_parsedGraph("parsed");
extern char *G_filename;

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
extern int og_yyerror(char *msg);
%}

// Bison options
%name-prefix="og_yy"

%token key_nodes key_initialnode key_finalnode key_transitions
%token key_interface key_input key_output
%token key_red key_blue
%token comma colon semicolon ident arrow number
%token key_true key_false key_final
%token lpar rpar

%left op_or
%left op_and


// Bison generates a list of all used tokens in file "syntax_og.h" (for flex)
%token_table
%defines

%union {
    char *str;
    unsigned int value;
    Formula *f;
}

%type <value> number
%type <str>   ident
%type <f>     formula

%%


og:
   { G_parsedGraph = Graph(basename(G_filename)); }
 interface nodes initialnode transitions
   { G_parsedGraph.reenumerate(); }
;


interface:
  /* for backwards compatibility, the interface is optional */
| key_interface key_input places_list semicolon
  key_output places_list semicolon
;


places_list:
  /* empty */
| places_list comma ident
| ident
;


nodes:
  key_nodes nodes_list semicolon
;


nodes_list:
  /* empty */
| node
| nodes_list comma node
;


node:
  number
    { G_parsedGraph.addNode($1); }
| number colon formula color_optional
    { G_parsedGraph.addNode($1);
      G_parsedGraph.addFormula($1, $3); }
;


formula:
  lpar formula rpar
    { $$ = $2; }
| formula op_and formula
    { $$ = new FormulaAND($1, $3); }
| formula op_or formula
    { $$ = new FormulaOR($1, $3); }
| key_final
    { $$ = new FormulaFinal(); }
| key_true
    { $$ = new FormulaTrue(); }
| key_false
    { $$ = new FormulaFalse(); }
| ident
    { $$ = new FormulaLit($1); }
;


color_optional:
  /* empty */
| colon key_finalnode
| colon key_blue
| colon key_red
| colon key_blue colon key_finalnode
| colon key_red colon key_finalnode
;


initialnode:
  key_initialnode number semicolon
    { G_parsedGraph.setRoot($2); }
;


transitions:
  key_transitions transitions_list semicolon
;


transitions_list:
  /* empty */
| transition
| transitions_list comma transition
;


transition:
  number arrow number colon ident
    { G_parsedGraph.addEdge($1, $3, $5); }
;
