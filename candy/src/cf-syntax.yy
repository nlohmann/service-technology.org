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


%{
// map library
#include <map>
// string library
#include <string>

// header from configure
#include "config.h"
// header for graph class
#include "Graph.h"
#include "verbose.h"

using std::string;


// from main.cc
extern Graph* parsedOG;

// from flex
extern int cf_yylex();
extern int cf_yyerror(char const *msg);

// for parser
map< string, bool > parsedEventsCF;
%}

%name-prefix="cf_yy"
%error-verbose
%token_table
%defines

%token SEMICOLON IDENT NUMBER

%union {
    char *str;
    unsigned int val;
}

%type <val> NUMBER
%type <str>   IDENT

%start costfile
%%



costfile:
  eventcost
  {
    // all non-mentioned events have cost of 0 due to the og parser
  }
;


eventcost:
  /* empty */
| eventcost IDENT NUMBER SEMICOLON
  {
    // check and set cost entry for current event
    if ( parsedEventsCF.find($2) == parsedEventsCF.end() ) {
        map< string, Event* >::iterator iter = parsedOG->events.find($2);
        if ( iter != parsedOG->events.end() ) {
            (iter->second)->setCost($3);
            parsedEventsCF[$2] = true;
        } else {
            // we ignore events which are not used in the given OG
        }
    } else {
        // dont ignore multiple event
        abort(1, "given costfile includes event serveral times");
    }

    free($2);
  }
;
