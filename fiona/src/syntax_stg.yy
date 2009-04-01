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

%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


#include <string>
#include <iostream>
#include <set>
#include <map>
#include <cstdlib>

#include "options.h"
#include "main.h"

using std::cerr;
using std::endl;
using std::string;
using std::set;
using std::map;

extern char *stg_yytext;

extern set<string> transitions;
extern set<string> places;
extern set<string> initialMarked;
extern set<string> interface;
extern map<string, set<string> > arcs;

extern int stg_yylineno;

set<string> tempNodeSet;
bool in_marking_list = false;
bool in_arc_list = false;


int stg_yyerror(const char* msg)
{
  cerr << msg << endl;
  cerr << "error in line " << stg_yylineno << ": token last read: `" << stg_yytext << "'" << endl;
  setExitCode(EC_PARSE_ERROR);
}

// from flex
extern char* stg_yytext;
extern int stg_yylex();
%}

// Bison options
%name-prefix="stg_yy"

%token PLACENAME TRANSITIONNAME IDENTIFIER
%token K_MODEL K_DUMMY K_GRAPH K_MARKING K_END NEWLINE
%token OPENBRACE CLOSEBRACE

%token_table

%union {
  char *str;
}

/* the types of the non-terminal symbols */
%type <str> TRANSITIONNAME
%type <str> PLACENAME


%%

stg:
    {
      transitions.clear(); places.clear(); initialMarked.clear(); arcs.clear();
      tempNodeSet.clear(); interface.clear(); in_marking_list = false; in_arc_list = false;
    }
  K_MODEL IDENTIFIER newline
  K_DUMMY transition_list newline
  K_GRAPH newline { in_arc_list = true; }
  tp_list pt_list
  K_MARKING { in_marking_list = true; } OPENBRACE place_list CLOSEBRACE newline
  K_END newline
;

transition_list:
  TRANSITIONNAME transition_list
    { 
      if (in_arc_list) 
      {
        tempNodeSet.insert(string($1));
        transitions.insert(string($1));
      } else
      {
        interface.insert(string($1));
      }

      free($1);
    }
| /* empty */
;

place_list:
  PLACENAME place_list
    { places.insert(string($1));
      if (in_marking_list)
        initialMarked.insert(string($1));
      else
        tempNodeSet.insert(string($1));

      free($1);
    }
| /* empty */
;

tp_list:
  TRANSITIONNAME place_list newline
   { arcs[string($1)] = tempNodeSet;
     tempNodeSet.clear();
     free($1);
   } tp_list
| /* empty */
;

pt_list:
  PLACENAME transition_list newline
   { arcs[string($1)] = tempNodeSet;
     tempNodeSet.clear();
     free($1);
   } pt_list
| /* empty */
;

newline:
  NEWLINE
| NEWLINE NEWLINE
;
