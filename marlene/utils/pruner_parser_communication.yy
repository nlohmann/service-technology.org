/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

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


%token KW_ABSTRACT_HEADING KW_REMAINING_HEADING
%token KW_IN KW_OUT KW_SYNC
%token EQUALS COMMA
%token NUMBER NAME

%expect 0
%defines
%name-prefix="communication_yy"

%{
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "Output.h"
#include "pruner-cmdline.h"
#include "pruner-verbose.h"

extern int communication_yylex();
extern int communication_yyerror(const char *);
%}

%union {
  unsigned int val;
  char* str;
}

%type <val> NUMBER
%type <str> NAME

%%

interfaceinformation:
    abstract_information remaining_information
;

abstract_information:
    /* empty */
    |
    KW_ABSTRACT_HEADING
;

remaining_information:
    /* empty */
    |
    KW_REMAINING_HEADING
;

