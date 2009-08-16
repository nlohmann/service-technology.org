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


%token KEY_PLACES KEY_TRANSITIONS KEY_SYNCHRONOUS COMMA SEMICOLON NAME

%defines
%name-prefix="cover_"

%{
#include <vector>
#include <string>
#include "Cover.h"

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/// the current NAME token as string
std::string cover_NAME_token;

/* Type of read labels:
 * 0 - places
 * 1 - transition
 * 2 - synchronous labels
 */
unsigned short labelType;

/// places to cover
std::vector<std::string> cover_placeNames;

/// transitions to cover
std::vector<std::string> cover_transitionNames;

extern int cover_lex();
extern int cover_error(const char *);
%}

%%

cover:
  places transitions synchronous
  {
    Cover::initialize(cover_placeNames, cover_transitionNames);
    cover_placeNames.clear();
    cover_transitionNames.clear();
  }
;

places:
  /* empty */
| KEY_PLACES { labelType = 0; } names SEMICOLON
;

transitions:
  /* empty */
| KEY_TRANSITIONS { labelType = 1; } names SEMICOLON
;

synchronous:
  /* empty */
| KEY_SYNCHRONOUS { labelType = 2; } names SEMICOLON
;

names:
  /* empty */
| NAME
  {
    switch(labelType)
    {
    case 0: cover_placeNames.push_back(cover_NAME_token); break;
    case 1: cover_transitionNames.push_back(cover_NAME_token); break;
    case 2: Cover::synchronousLabels.push_back(cover_NAME_token); break;
    default: /* ignore */ ;
    }
  }
| names COMMA NAME
  {
    switch(labelType)
    {
    case 0: cover_placeNames.push_back(cover_NAME_token); break;
    case 1: cover_transitionNames.push_back(cover_NAME_token); break;
    case 2: Cover::synchronousLabels.push_back(cover_NAME_token); break;
    default: /* ignore */ ;
    }
  }
;

