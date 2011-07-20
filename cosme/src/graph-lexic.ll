/*****************************************************************************\
 Cosme -- Checking Simulation, Matching, and Equivalence

 Copyright (c) 2010 Andreas Lehmann

 Cosme is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Cosme is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Cosme.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

%option outfile="lex.yy.c"
%option prefix="graph_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstring>
#include <string>
#include "graph-syntax.h"
#include "verbose.h"

int graph_yyerror(const char *);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    "-"?[0-9][0-9]*

%%

{number}" Places"               { /* skip */ }
{number}" Transitions"          { /* skip */ }
{number}" significant places"   { /* skip */ }
">>>>> "{number}" States, "{number}" Edges, "{number}" Hash table entries" { /* skip */ }

"STATE"      { return KW_STATE; }
"Lowlink:"   { return KW_LOWLINK; }
"SCC:"       { return KW_SCC; }
":"          { return COLON; }
","          { return COMMA; }
"->"         { return ARROW; }

{number}     { graph_yylval.value = atoi(graph_yytext); return NUMBER; }
{name}       { graph_yylval.str = graph_yytext; return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { graph_yyerror("lexical error"); }

%%

int graph_yyerror(char const *msg) {
		status("%d: error near '%s': %s", graph_yylineno, graph_yytext, msg);
  	abort(6, "error while parsing the reachability graph");
}
