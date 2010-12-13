/*****************************************************************************\
 Hello -- <<-- Hello World -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Hello is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Hello is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/* <<-- CHANGE START (lexer) -->> */
%option outfile="lex.yy.c"
%option prefix="wendy_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <iostream>
#include <cstring>

#include "wendy_syntax.h"
#include "config.h"

extern int wendy_yyerror(char const *msg);
%}

whitespace		[\n\r\t ]+
identifier     [^\t \n\r]+

%%

{identifier}   { wendy_yylval.str = strdup(wendy_yytext); return IDENT; }

{whitespace}	{ wendy_yylval.str = strdup(wendy_yytext); return WHITE; }

.		{ wendy_yyerror("lexical error"); }

%%

int wendy_yyerror(char const *msg) {
    std::cerr << PACKAGE << ": " << wendy_yylineno
              << ": ERROR near '" << wendy_yytext 
              << "': " << msg << std::endl;
    exit(EXIT_FAILURE);
}
/* <<-- CHANGE END -->> */
