%{
#include <map>
#include <string>
#include "Graph.h"

using std::map;
using std::string;

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
bool input = true;
%}

// Bison options
%name-prefix="og_yy"
%error-verbose

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
    Node *n;
}

%type <value> number
%type <str>   ident
%type <f>     formula
%type <n>     node

%%


og:
  interface nodes initialnode transitions
    { tempMap.clear(); }
;


interface:
  key_interface key_input places_list semicolon
  key_output { input = false; } places_list semicolon
;


places_list:
  /* empty */
| places_list comma ident
    { if (input)
          G.addLabel("?" + string($3));
      else
          G.addLabel("!" + string($3)); } 
| ident
    { if (input)
          G.addLabel("?" + string($1));
      else
          G.addLabel("!" + string($1)); } 
;


nodes:
  key_nodes nodes_list semicolon
;


nodes_list:
  /* empty */
| node
    { G.addNode($1);
      tempMap[$1->id] = $1; }
| nodes_list comma node
    { G.addNode($3);
      tempMap[$3->id] = $3; }
;


node:
  number
    { $$ = new Node($1); }
| number colon formula
    { $$ = new Node($1, $3); }
| number colon key_finalnode
    { $$ = new Node($1); }
| number colon formula colon color
    { $$ = new Node($1, $3); }
| number colon formula colon key_finalnode
    { $$ = new Node($1, $3); }
| number colon formula colon color colon key_finalnode
    { $$ = new Node($1, $3); }
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


color:
| key_blue
| key_red
;


initialnode:
  key_initialnode number
    { if (!tempMap[$2])
        og_yyerror("could not find node specified as root"); }  
  semicolon
    { G.setRoot(tempMap[$2]); }
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
  number
    { if (!tempMap[$1])
        og_yyerror("could not find source node of edge"); }  
  arrow number
    { if (!tempMap[$4])
        og_yyerror("could not find target node of edge"); }  
  colon ident
    { tempMap[$1]->addEdge($7, tempMap[$4]); }
;
