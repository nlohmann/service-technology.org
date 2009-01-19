/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/*!
 * \file    syntax_covog.yy
 *
 * \brief   the parser for the file format specifying an operating guideline
 *          with global constraint
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de> 
 *
 */

%{
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
extern char* covog_yytext;
extern int covog_yylex();


#include "mynew.h"
#include "debug.h"

#include "ConstraintOG.h"
#include "GraphFormula.h"

extern AnnotatedGraph* OGToParse;
GraphFormulaCNF* covConstraint;

using namespace std;

void covog_yyerror_unknown_node(const std::string& nodeName)
{
    string msg("Node ");
    msg += nodeName;
    msg += " is unknown.";
    covog_yyerror(msg.c_str());
}

void covog_yyerror_node_already_defined(const std::string& nodeName)
{
    string msg("Node ");
    msg += nodeName;
    msg += " already defined.";
    covog_yyerror(msg.c_str());
}
%}

// Bison options
%name-prefix="covog_yy"

// the terminal symbols (tokens)

%token key_nodes key_initialnode key_transitions
%token key_cover key_placestocover key_transitionstocover key_globalconstraint
%token key_actuallycovered key_none
%token key_red key_blue
%token comma colon semicolon ident arrow
%token key_true key_false
%token lpar rpar

%left op_or
%left op_and


// Bison generates a list of all used tokens in file "syntax_covog.h" (for flex)
%token_table


%union {
    char * str;
    GraphFormula* formula;
    GraphNodeColor_enum color;
}

/* the types of the non-terminal symbols */
%type <str> ident
%type <formula> formula
%type <color> color_optional;


%%

/* Grammar rules */

og: nodes initialnode transitions cover
;

nodes: key_nodes nodes_list semicolon
;

nodes_list: nodes_list comma node
| node
| /* empty */
;

node: ident colon formula color_optional
    {
        if (OGToParse->hasNodeWithName($1)) {
            covog_yyerror_node_already_defined($1);
        }

        OGToParse->addNode($1, $3, $4);
        free($1);
    }
;

formula: lpar formula rpar
    {
        $$ = $2;
    }
| formula op_and formula
    {
        $$ = new GraphFormulaMultiaryAnd($1, $3);
    }
| formula op_or formula
    {
        $$ = new GraphFormulaMultiaryOr($1, $3);
    }
| key_true
    {
        $$ = new GraphFormulaTrue;
    }
| key_false
    {
        $$ = new GraphFormulaFalse;
    }
| ident
    {
        $$ = new GraphFormulaLiteral($1);
        free($1);
    }
;

color_optional: colon key_blue
    {
        $$ = BLUE;
    }
| colon key_red
    {
        $$ = RED;
    }
|
    {
        $$ = BLUE;
    }
;

initialnode: key_initialnode ident
    {
        if (!OGToParse->hasNodeWithName($2)) {
            covog_yyerror_unknown_node($2);
        }

        OGToParse->setRootToNodeWithName($2);
        free($2);
    }
    semicolon
;

transitions: key_transitions transitions_list semicolon
;

transitions_list: transitions_list comma transition
| transition
| /* empty */
;

transition: ident arrow ident colon ident
    {
        if (!OGToParse->hasNodeWithName($1)) {
            covog_yyerror_unknown_node($1);
        }

        if (!OGToParse->hasNodeWithName($3)) {
            covog_yyerror_unknown_node($3);
        }

        OGToParse->addEdge($1, $3, $5);
        free($1);
        free($3);
        free($5);
    }
;

cover: key_cover placestocover transitionstocover globalconstraint actuallycovered semicolon
;

placestocover: key_placestocover colon idents_list comma
;

transitionstocover: key_transitionstocover colon idents_list comma
;

globalconstraint: key_globalconstraint colon formula comma
    {
        covConstraint = ($3)->getCNF();
    }
;

actuallycovered: covernodes_list 
;

covernodes_list: covernodes_list comma covernode
| covernode
| /* empty */
;

covernode: ident colon idents_list
;

idents_list: ident idents_list
| ident
| key_none
;

