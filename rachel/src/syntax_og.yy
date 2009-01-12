/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008, 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 Copyright (C) 2005, 2006  Jan Bretschneider (Fiona parser)
 
 Rachel is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Rachel (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/


%{
#include <libgen.h>
#include "cmdline.h"
#include "Graph.h"
#include "Formula.h"

extern Graph G_parsedGraph;

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
extern gengetopt_args_info args_info;
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
}

%type <value> number
%type <str>   ident
%type <f>     formula

%%


og:
 interface nodes initialnode transitions
   { // Do not reenumerate nodes in annotation mode, because there the
     // node numbers are printed and reenumeration would be confusing.
     if (args_info.mode_arg != mode_arg_annotation)
       G_parsedGraph.reenumerate();
   }
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
| number colon formula
    { G_parsedGraph.addNode($1);
      G_parsedGraph.addFormula($1, $3); }
| number colon key_finalnode
    { G_parsedGraph.addNode($1);
      G_parsedGraph.setFinal($1); }
| number colon formula colon color
    { G_parsedGraph.addNode($1);
      G_parsedGraph.addFormula($1, $3); }
| number colon formula colon key_finalnode
    { G_parsedGraph.addNode($1);
      G_parsedGraph.addFormula($1, $3);
      G_parsedGraph.setFinal($1); }
| number colon formula colon color colon key_finalnode
    { G_parsedGraph.addNode($1);
      G_parsedGraph.addFormula($1, $3);
      G_parsedGraph.setFinal($1); }
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
