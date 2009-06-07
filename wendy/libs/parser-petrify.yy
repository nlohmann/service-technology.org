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

using std::cerr;
using std::endl;
using std::string;
using std::set;
using std::map;

extern set<string> pnapi_petrify_transitions;
extern set<string> pnapi_petrify_places;
extern set<string> pnapi_petrify_initialMarked;
extern set<string> pnapi_petrify_interface;
extern map<string, set<string> > pnapi_petrify_arcs;

extern int pnapi_petrify_yylineno;

set<string> tempNodeSet;
bool in_marking_list = false;
bool in_arc_list = false;

// from flex
extern char* pnapi_petrify_yytext;
extern int pnapi_petrify_yylex();

int pnapi_petrify_yyerror(const char* msg)
{
  cerr << msg << endl;
  cerr << "error in line " << pnapi_petrify_yylineno << ": token last read: `" << pnapi_petrify_yytext << "'" << endl;
  exit(1);
}
%}

// Bison options
%name-prefix="pnapi_petrify_yy"

%token PLACENAME TRANSITIONNAME IDENTIFIER
%token K_MODEL K_DUMMY K_GRAPH K_MARKING K_END NEWLINE
%token OPENBRACE CLOSEBRACE

%defines

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
      pnapi_petrify_transitions.clear(); pnapi_petrify_places.clear(); 
      pnapi_petrify_initialMarked.clear(); pnapi_petrify_arcs.clear();
      tempNodeSet.clear(); pnapi_petrify_interface.clear(); 
      in_marking_list = false; in_arc_list = false;
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
        pnapi_petrify_transitions.insert(string($1));
      } 
      else
      {
        pnapi_petrify_interface.insert(string($1));
      }

      free($1);
    }
| /* empty */
;

place_list:
  PLACENAME place_list
    { 
      pnapi_petrify_places.insert(string($1));
      if (in_marking_list)
        pnapi_petrify_initialMarked.insert(string($1));
      else
        tempNodeSet.insert(string($1));
        
      free($1);
    }
| /* empty */
;

tp_list:
  TRANSITIONNAME place_list newline
   { pnapi_petrify_arcs[string($1)] = tempNodeSet;
     tempNodeSet.clear();
     free($1);
   } tp_list
| /* empty */
;

pt_list:
  PLACENAME transition_list newline
   { pnapi_petrify_arcs[string($1)] = tempNodeSet;
     tempNodeSet.clear();
     free($1);
   } pt_list
| /* empty */
;

newline:
  NEWLINE
| NEWLINE NEWLINE
;
