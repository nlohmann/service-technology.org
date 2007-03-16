/*****************************************************************************
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

/*!
 * \file    syntax_og.yy
 *
 * \brief   the parser for the OG file format
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
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
extern char* og_yytext;
extern int og_yylex();


#include "mynew.h"
#include "debug.h"

#include "OGFromFile.h"

OGFromFile OGToMatch;

using namespace std;

void og_yyerror_unknown_node(const std::string& nodeName)
{
    string msg("Node ");
    msg += nodeName;
    msg += " is unknown.";
    og_yyerror(msg.c_str());
}

void og_yyerror_node_already_defined(const std::string& nodeName)
{
    string msg("Node ");
    msg += nodeName;
    msg += " already defined.";
    og_yyerror(msg.c_str());
}
%}

// Bison options
%name-prefix="og_yy"

// the terminal symbols (tokens)

%token key_nodes key_initialnode key_transitions
%token comma colon semicolon ident arrow
%token key_true key_false
%token lpar rpar

%left op_and
%left op_or


// Bison generates a list of all used tokens in file "syntax_og.h" (for flex)
%token_table


%union {
    char * str;
    CommGraphFormula* formula;
}

/* the types of the non-terminal symbols */
%type <str> ident
%type <formula> formula


%%

/* Grammar rules */

og: nodes initialnode transitions
;

nodes: key_nodes nodes_list semicolon
;

nodes_list: nodes_list comma node
| node
| /* empty */
;

node: ident colon formula
    {
        if (OGToMatch.hasNodeWithName($1)) {
            og_yyerror_node_already_defined($1);
        }

        OGToMatch.addNode($1, $3);
        free($1);
    }
;

formula: lpar formula rpar
    {
        $$ = $2;
    }
| formula op_and formula
    {
        $$ = new CommGraphFormulaMultiaryAnd($1, $3);
    }
| formula op_or formula
    {
        $$ = new CommGraphFormulaMultiaryOr($1, $3);
    }
| key_true
    {
        $$ = new CommGraphFormulaTrue;
    }
| key_false
    {
        $$ = new CommGraphFormulaFalse;
    }
| ident
    {
        $$ = new CommGraphFormulaProposition($1);
        free($1);
    }
;

initialnode: key_initialnode ident
    {
        if (!OGToMatch.hasNodeWithName($2)) {
            og_yyerror_unknown_node($2);
        }

        OGToMatch.setRootToNodeWithName($2);
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
        if (!OGToMatch.hasNodeWithName($1)) {
            og_yyerror_unknown_node($1);
        }

        if (!OGToMatch.hasNodeWithName($3)) {
            og_yyerror_unknown_node($3);
        }

        OGToMatch.addTransition($1, $3, $5);
        free($1);
        free($3);
        free($5);
    }
;
