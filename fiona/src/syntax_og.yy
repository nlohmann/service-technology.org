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

#include <map>

#include "mynew.h"
#include "debug.h"

#include "AnnotatedGraph.h"
#include "GraphFormula.h"

using namespace std;

AnnotatedGraph* OGToParse;
map<string, AnnotatedGraphNode*> nodes;

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
%token key_red key_blue
%token comma colon semicolon ident arrow
%token key_true key_false key_finalnode
%token lpar rpar

%left op_or
%left op_and


// Bison generates a list of all used tokens in file "syntax_og.h" (for flex)
%token_table


%union {
    char * str;
    GraphFormula* formula;
    GraphNodeColor_enum color;
    bool boolVal;
}

/* the types of the non-terminal symbols */
%type <str> ident
%type <formula> formula
%type <color> color_optional;
%type <boolVal> finalnode_optional;


%%

/* Grammar rules */

og: nodes initialnode transitions
    {
        nodes.clear();
    }
;

nodes: key_nodes nodes_list semicolon
;

nodes_list: nodes_list comma node
| node
| /* empty */
;

node: ident colon formula color_optional finalnode_optional
    {
        if (nodes[$1] != NULL) {
            og_yyerror_node_already_defined($1);
        }

        AnnotatedGraphNode* currentNode = OGToParse->addNode($1, $3, $4);
        nodes[$1] = currentNode;
        currentNode->setFinal($5);
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
| colon key_blue colon
    {
        $$ = BLUE;
    }
| colon key_red colon
    {
        $$ = RED;
    }
| colon
    {
        $$ = BLUE;
    }
|
    {
        $$ = BLUE;
    }
;

finalnode_optional: key_finalnode
    {
        $$ = true;
    }
|
    {
        $$ = false;
    }
;

initialnode: key_initialnode ident
    {
        if (nodes[$2] == NULL) {
            og_yyerror_unknown_node($2);
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
        if (nodes[$1] == NULL) {
            og_yyerror_unknown_node($1);
        }
    
        if (nodes[$3] == NULL) {
            og_yyerror_unknown_node($3);
        }

        OGToParse->addEdge(nodes[$1], nodes[$3], $5);
        free($1);
        free($3);
        free($5);
    }
;

