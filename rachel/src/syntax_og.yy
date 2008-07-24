/*****************************************************************************
 * Copyright 2008       Niels Lohmann                                        *
 * Copyright 2005, 2006 Jan Bretschneider                                    *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

%{
#include "Graph.h"
#include "Formula.h"
#include "helpers.h"

Graph G_parsedGraph("parsed");
extern string G_filename;

/* Prologue: Syntax and usage of the prologue.
 * Bison Declarations: Syntax and usage of the Bison declarations section.
 * Grammar Rules: Syntax and usage of the grammar rules section.
 * Epilogue: Syntax and usage of the epilogue.  */

// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


// from flex
extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char *msg);
%}

// Bison options
%name-prefix="og_yy"

%token key_nodes key_initialnode key_finalnode key_transitions
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
   { G_parsedGraph = Graph(crop_filename(G_filename)); }
 nodes initialnode transitions
   { G_parsedGraph.reenumerate(); }
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
  number colon formula color_optional
    { G_parsedGraph.addNode($1);
      G_parsedGraph.addFormula($1, $3); }
| number
    { G_parsedGraph.addNode($1); }
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
| transitions_list comma transition
| transition
;


transition:
  number arrow number colon ident
    { G_parsedGraph.addEdge($1, $3, $5); }
;
