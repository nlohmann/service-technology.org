/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines
 
 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
 Wendy is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Wendy (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/


%option noyywrap
%option nodefault
%option outfile="lex.yy.c"
%option prefix="graph_"

%{
#include <cstring>
#include <string>
#include "syntax_graph.h"
#include "config.h"
#include "cmdline.h"

extern std::string NAME_token;
extern gengetopt_args_info args_info;

void graph_error(const char *);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    "-"?[0-9][0-9]*


%%

{number}" Places"               { /* skip */ }
{number}" Transitions"          { /* skip */ }
">>>>> "{number}" States, "{number}" Edges, "{number}" Hash table entries" { /* skip */ }

"STATE"      { return KW_STATE; }
"Prog:"      { return KW_PROG; }
":"          { return COLON; }
","          { return COMMA; }
"->"         { return ARROW; }

{number}     { graph_lval.val = atoi(graph_text); return NUMBER; }
{name}       { NAME_token = graph_text; return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { graph_error("lexical error"); }

%%

void graph_error(const char *msg) {
  if (args_info.verbose_given) {
      fprintf(stderr, "%s:%d: error near '%s': %s\n", PACKAGE, graph_lineno, graph_text, msg);
  }
  fprintf(stderr, "%s: error while parsing the reachability graph from LoLA -- aborting\n", PACKAGE);
  exit(EXIT_FAILURE);
}
