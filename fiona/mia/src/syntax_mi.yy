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

%token KW_STATE KW_PROG COLON DOT COMMA ARROW NUMBER NAME TGT_NAME MPP_NAME

%defines
%name-prefix="mi_"

%{
#include <string>
#include <map>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

std::map<unsigned int, std::string> id2marking;
std::string temp;

/// the current NAME token as string
extern std::string NAME_token;

extern int mi_lex();
extern int mi_error(const char *);
extern const char* mi_text;
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%%

markings:
  marking
| markings marking
;

marking:
  NUMBER COLON places
    { id2marking[$1] = temp; temp.clear(); }
;

places:
  place
| places COMMA place;

place:
  /* empty */
| NAME COLON NUMBER
    { 
      temp += NAME_token;
      if ($3 > 1) {
        temp += ":" + std::string(mi_text);
      }
    }
;
