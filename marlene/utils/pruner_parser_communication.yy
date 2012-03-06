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
%token EQUALS COMMA SEMICOLON
%token NUMBER NAME

%expect 0
%defines
%name-prefix="communication_yy"

%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages

#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "Output.h"
#include "pruner-cmdline.h"
#include "pruner-statespace.h"
#include "pruner-verbose.h"

extern int communication_yylex();
extern int communication_yyerror(const char *);

std::map< std::string, std::string > transitionMapping;
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
    {
        transitionMapping.clear();
    }
    input
    {
        State::abstractedIn = transitionMapping;
        transitionMapping.clear();
    }
    output
    {
        State::abstractedOut = transitionMapping;
        transitionMapping.clear();
    }
    sync
    {
        State::abstractedSync = transitionMapping;
        transitionMapping.clear();
    }
;

remaining_information:
    /* empty */
    |
    KW_REMAINING_HEADING
    {
        transitionMapping.clear();
    }
    input
    {
        State::remainingIn = transitionMapping;
        transitionMapping.clear();
    }
    output
    {
        State::remainingSync = transitionMapping;
        transitionMapping.clear();
    }
    sync
    {
        State::remainingSync = transitionMapping;
        transitionMapping.clear();
    }
;

input:
    /* empty */
    | 
    KW_IN
    transitions SEMICOLON
;

output:
    /* empty */
    | 
    KW_OUT
    transitions SEMICOLON
;

sync:
    /* empty */
    | 
    KW_SYNC
    transitions SEMICOLON
;

transitions:
    transition
    |
    transitions COMMA transition
;

transition:
    NAME EQUALS NAME
    {
        transitionMapping[$1] = $3;
        free($1);
        free($3);
    }
;

