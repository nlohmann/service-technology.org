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

/* <<-- CHANGE START (parser) -->> */
%{
#include <iostream>
#include <string>

#include "config.h"

/// output stream - set in main.cc
extern std::ostream* myOut;

/// from flex
extern char* wendy_yytext;
extern int wendy_yylex();
extern int wendy_yyerror(char const *msg);

extern std::string wendy_out;

%}

%name-prefix="wendy_yy"
%error-verbose
%token_table
%defines

%token IDENT WHITE

%union {
    char *str;
}

%type <str>  IDENT
%type <str>  WHITE

%start initial
%%


initial:
  IDENT { wendy_out += $1; } initial
  | WHITE { wendy_out += $1; } initial
  | /*empty*/
;
/* <<-- CHANGE END -->> */