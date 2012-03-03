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


%token KW_STATE KW_LOWLINK KW_SCC COLON COMMA ARROW NUMBER NAME

%expect 0
%defines
%name-prefix="graph_yy"

%{
#include <fstream>
#include <vector>
#include <string>
#include <map>
//#include "Output.h"
#include "pruner-cmdline.h"
#include "pruner-verbose.h"
#include "pruner-helper.h"
#include "pruner-statespace.h"

extern int graph_yylex();
extern int graph_yyerror(const char *);

using std::tr1::shared_ptr;

ListOfPairs::List_ptr currentMarking;
ListOfPairs::List_ptr currentTransitions;
unsigned int lowlink;
List<unsigned int>::List_ptr sccmember;
%}

%union {
  unsigned int val;
  char* str;
}

%type <val> NUMBER
%type <str> NAME
%type <val> scc_member

%%

states:
  state
| states state
;

state:
  KW_STATE NUMBER lowlink scc markings_or_transitions
  {
    State::stateSpace[$2] = shared_ptr<State>(new State($2, currentMarking, currentTransitions, lowlink, sccmember));
    currentMarking = ListOfPairs::List_ptr();
    currentTransitions = ListOfPairs::List_ptr();
    sccmember = List<unsigned int>::List_ptr();
  }
;


scc:
  /* empty */
| KW_SCC scc_members
;

scc_members:
  scc_member
  {
    if(sccmember.get() == 0) {
      sccmember = List<unsigned int>::List_ptr(new List<unsigned int>($1));
    }
    else {
      sccmember->push_back($1);
    }
  }
| scc_members scc_member
  {
    if(sccmember.get() == 0) {
      sccmember = List<unsigned int>::List_ptr(new List<unsigned int>($2));
    }
    else {
      sccmember->push_back($2);
    }
  }
;

scc_member:
  NUMBER
  {
    $$ = $1;
  }
;

/* do something with Tarjan's lowlink value (needed for generating
   livelock free partners or reduction rule smart sending event) */
lowlink:
  KW_LOWLINK NUMBER
  {
    lowlink = $2;
  }
;

markings_or_transitions:
  /* empty */
| markings
| transitions
| markings transitions
;

markings:
  marking
| markings COMMA marking
;

marking:
  NAME COLON NUMBER
    { 
      shared_ptr< Pair< std::string, unsigned int > > p(new Pair<std::string, unsigned int>(std::string($1), $3));
      if (currentMarking.get() == 0) {
        currentMarking = ListOfPairs::List_ptr(new ListOfPairs(p));
      } else {
        currentMarking->push_back(p);
      }
      
      free($1); 
      // status("Pair was %s, %d", p->getFirst().c_str(), p->getSecond());  
    }
;

transitions:
  transition
| transitions transition
;

transition:
  NAME ARROW NUMBER
    {
      shared_ptr< Pair< std::string, unsigned int > > p(new Pair<std::string, unsigned int>(std::string($1), $3));
      if (currentTransitions.get() == 0) {
        currentTransitions = ListOfPairs::List_ptr(new ListOfPairs(p));
      } else {
        currentTransitions->push_back(p);
      }
      
      free($1); 
      // status("Transition was %s -> %d", p->getFirst().c_str(), p->getSecond());  
    }
;
