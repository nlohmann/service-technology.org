/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/



/*****************************************************************************
 * bison options 
 ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_petrify_yy"

/* write tokens to parser-owfn.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


/*****************************************************************************
 * C declarations
 ****************************************************************************/
%{

#include "parser.h"
#include <string>
#include <sstream>
#include <iostream>
#include <set>
#include <map>
#include <cstdlib>

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error
#define yylex pnapi::parser::petrify::lex
#define yylex_destroy pnapi::parser::petrify::lex_destroy
#define yyparse pnapi::parser::petrify::parse

using namespace pnapi::parser::petrify;

%}


/*****************************************************************************
 * types, tokens, start symbol
 ****************************************************************************/

%token PLACENAME TRANSITIONNAME IDENTIFIER
%token K_MODEL K_DUMMY K_GRAPH K_MARKING K_END NEWLINE
%token OPENBRACE CLOSEBRACE

%union
{
  char * yt_str;
}

%type <yt_str> PLACENAME TRANSITIONNAME

%defines

%token_table

%%

stg:
  K_MODEL IDENTIFIER newline
  K_DUMMY transition_list newline
  K_GRAPH newline { in_arc_list = true; }
  tp_list pt_list
  K_MARKING { in_marking_list = true; } OPENBRACE place_list CLOSEBRACE newline
  K_END newline
;

transition_list:
  /* empty */
| transition_list TRANSITIONNAME
  { 
    std::string ident = $2;
    free($2);

    if (in_arc_list) 
    {
      tempNodeSet_.insert(ident);
      transitions_.insert(ident);
    } 
    else
    {
      interface_.insert(ident);
    }
  }
;

place_list:
  /* empty */
| place_list PLACENAME
  { 
    std::string ident = $2;
    free($2);

    places_.insert(ident);
    if (in_marking_list)
      initialMarked_[ident] = 1;
    else
      tempNodeSet_.insert(ident);
  }
;

tp_list:
  /* empty */
| tp_list TRANSITIONNAME place_list newline
  { 
    arcs_[$2] = tempNodeSet_;
    tempNodeSet_.clear();
    free($2);
  } 
;

pt_list:
  /* empty */
| pt_list PLACENAME transition_list newline
  { arcs_[$2] = tempNodeSet_;
    tempNodeSet_.clear();
    free($2);
  }
;

newline:
  NEWLINE
| NEWLINE NEWLINE
;
