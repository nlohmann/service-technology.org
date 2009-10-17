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
%name-prefix="pnapi_pn_yy"

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
#define yylex pnapi::parser::pn::lex
#define yylex_destroy pnapi::parser::pn::lex_destroy
#define yyparse pnapi::parser::pn::parse

using namespace pnapi::parser::pn;

%}


/*****************************************************************************
 * types, tokens, start symbol
 ****************************************************************************/

%token PLACENAME TRANSITIONNAME IDENTIFIER WEIGHT
%token K_MODEL K_DUMMY K_OUTPUTS K_GRAPH K_MARKING K_END NEWLINE
%token OPENBRACE CLOSEBRACE LPAR RPAR

%union
{
  char * yt_str;
  unsigned int yt_uInt;
}

%type <yt_str> PLACENAME TRANSITIONNAME
%type <yt_uInt> WEIGHT weight

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
| K_OUTPUTS transition_list newline
  K_GRAPH newline { in_arc_list = true; }
  pt_list tp_list
  K_MARKING { in_marking_list = true; } OPENBRACE place_list CLOSEBRACE newline
  K_END newline
;

transition_list:
  /* empty */
| transition_list TRANSITIONNAME weight
  { 
    std::string ident = $2;
    free($2);

    if (in_arc_list) 
    {
      tempNodeMap_[ident] = $3;
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
| place_list PLACENAME weight
  { 
    std::string ident = $2;
    free($2);

    places_.insert(ident);
    if (in_marking_list)
      initialMarked_[ident] = 1;
    else
      tempNodeMap_[ident] = $3;
  }
;

weight:
  /* empty */       { $$ = 1; }
| LPAR WEIGHT RPAR  { $$ = $2; }
;

tp_list:
  /* empty */
| tp_list TRANSITIONNAME place_list newline
  { 
    arcs_[$2] = tempNodeMap_;
    tempNodeMap_.clear();
    free($2);
  } 
;

pt_list:
  /* empty */
| pt_list PLACENAME transition_list weight newline
  { arcs_[$2] = tempNodeMap_;
    tempNodeMap_.clear();
    free($2);
  }
;

newline:
  NEWLINE
| NEWLINE NEWLINE
;
