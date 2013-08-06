/*****************************************************************************\
 Tara

 Copyright (c) 20XX Authors

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
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
%option prefix="costfunction_"
%option case-insensitive

%{
#include <cstring>
#include "syntax_costfunction.hh"
#include "verbose.h"

void graph_error(const char*);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    [0-9][0-9]*

%%


"RESET"       { return CF_RESET; }
":"          { return CF_COLON; }

{number}     { costfunction_lval.val = atoi(costfunction_text); return CF_NUMBER; }
{name}       { costfunction_lval.str = strdup(costfunction_text); return CF_NAME; }

[ \t\r\n]*   { /* skip */ }

%%

__attribute__((noreturn)) void costfunction_error(const char* msg) {
  status("error near '%s': %s", costfunction_text, msg);
  abort(6, "error while parsing the costs file");
}
