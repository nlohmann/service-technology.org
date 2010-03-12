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

%{
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "Output.h"
#include "cdnf-cmdline.h"
#include "verbose.h"
#include "pnapi.h"

/// a marking of the PN API net
std::map<const pnapi::Place*, unsigned int> marking;

/// set of final markings
std::set<pnapi::Marking*> finalMarkings;

/// the command line parameters
extern gengetopt_args_info args_info;

extern int yylex();
extern int yyerror(const char *);

extern pnapi::PetriNet net;

%}

%union {
  unsigned int val;
  char* str;
}

%type <val> NUMBER
%type <str> NAME

%%

states:
  state
| states state
;

state:
  KW_STATE NUMBER lowlink scc markings_or_transitions
  {
    pnapi::Marking tmp(marking, &net);
    if(net.getFinalCondition().isSatisfied(tmp))
      finalMarkings.insert(new pnapi::Marking(tmp));
    marking.clear();
  }
;


scc:
  /* empty */
| KW_SCC scc_members
;

scc_members:
  scc_member
| scc_members scc_member
;

scc_member:
  NUMBER
;

lowlink:
  KW_LOWLINK NUMBER
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
    { marking[net.findPlace($1)] = $3; free($1); }
;

transitions:
  transition
| transitions transition
;

transition:
  NAME ARROW NUMBER { free($1); }
;
