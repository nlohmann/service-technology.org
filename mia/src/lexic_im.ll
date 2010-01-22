/*****************************************************************************\
 Mia -- calculating migration information

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

 Mia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Mia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Mia.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%option noyywrap
%option nounput
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="im_"

%{
#include <cstring>
#include <string>
#include "syntax_im.h"
#include "verbose.h"

extern std::string NAME_token;

void im_error(const char *);
%}

name      [^,;:()\t \n\{\}\[\]][^,;:()\t \n\{\}\[\]]*
number    [0-9][0-9]*


%%

"INTERFACE"  { return KW_INTERFACE; }
","          { return COMMA; }
"["          { return LBRACKET; }
"]"          { return RBRACKET; }

{number}     { im_lval.val = atoi(im_text); return NUMBER; }
{name}       { NAME_token = im_text; return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { im_error("lexical error"); }

%%

void im_error(const char *msg) {
  status("%d: error near '%s': %s", im_lineno, im_text, msg);
  abort(2, "error while parsing the migration information");
}
