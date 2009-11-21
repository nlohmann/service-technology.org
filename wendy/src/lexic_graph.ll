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
%option nodefault
%option nounput
%option full
%option outfile="lex.yy.c"
%option prefix="graph_"

%{
#include <cstring>
#include "syntax_graph.h"
#include "verbose.h"

void graph_error(const char*);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    [0-9][0-9]*

%%

"STATE"      { return KW_STATE; }
"Lowlink:"   { return KW_LOWLINK; }
"SCC:"       { return KW_SCC; }
":"          { return COLON; }
","          { return COMMA; }
"->"         { return ARROW; }

{number}     { graph_lval.val = atoi(graph_text); return NUMBER; }
{name}       { graph_lval.str = strdup(graph_text); return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { graph_error("lexical error"); }

%%

void graph_error(const char* msg) {
  status("error near '%s': %s", graph_text, msg);
  abort(6, "error while parsing the reachability graph");
}
