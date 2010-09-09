/*****************************************************************************\
 Candy -- Synthesizing cost efficient partners for services

 Copyright (c) 2010 Richard MŸller

 Candy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Candy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Candy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


%option noyywrap
%option nounput
%option yylineno
%option full
%option outfile="lex.yy.c"
%option prefix="cf_yy"

%{
#include "cf-syntax.h" // from parser
#include "verbose.h"   // for verbose messages

void cf_yyerror(const char);
%}

%s COMMENT

identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);              }
<COMMENT>"}"                            { BEGIN(INITIAL);              }
<COMMENT>[^}]*                          { /* do nothing */             }

";"                                     { return SEMICOLON;            }

{number}       { cf_yylval.val = atoi(cf_yytext); return NUMBER;     }
{identifier}   { cf_yylval.str = strdup(cf_yytext); return IDENT;      }

[ \n\r\t]*                              { /* do nothing */             }

%%

__attribute__((noreturn)) void cf_yyerror(const char *msg) {
	status("error near '%s': %s", cf_yytext, msg);
  	abort(1, "error while parsing the costfile");
}
