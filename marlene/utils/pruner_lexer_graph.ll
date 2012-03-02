/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


%option noyywrap
%option nounput
%option full
%option outfile="lex.yy.c"
%option prefix="graph_yy"

%{
#include <cstring>
#include "pruner_parser_graph.h"
#include "pruner-verbose.h"

void graph_yyerror(const char*);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    [0-9][0-9]*

%%

"Formula with\n"{number}" subformula(s)." { /* skip */ }

"STATE"      { return KW_STATE; }
"Lowlink:"   { return KW_LOWLINK; }
"SCC:"       { return KW_SCC; }
":"          { return COLON; }
","          { return COMMA; }
"->"         { return ARROW; }

{number}     { graph_yylval.val = atoi(graph_yytext); return NUMBER; }
{name}       { graph_yylval.str = strdup(graph_yytext); return NAME; }

[ \t\r\n]*   { /* skip */ }

%%

__attribute__((noreturn)) void graph_yyerror(const char* msg) {
  status("error near '%s': %s", graph_yytext, msg);
  abort(6, "error while parsing the reachability graph");
}
