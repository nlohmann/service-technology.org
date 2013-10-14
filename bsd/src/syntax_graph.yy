/*****************************************************************************\
 BSD -- generating BSD automata

 Copyright (c) 2013 Simon Heiden

 BSD is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 BSD is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with BSD.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


%token KW_STATE KW_LOWLINK KW_SCC COLON COMMA ARROW NUMBER NAME

%expect 0
%defines
%name-prefix="graph_"

%{
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "InnerMarking.h"
#include "openNet.h"
#include "Label.h"
#include "Output.h"
#include "cmdline.h"
#include "types.h"
#include "verbose.h"
#include "util.h"

/// the vector of the successor state numbers of the current marking
std::vector<InnerMarking_ID> currentSuccessors;

/// the labels of the outgoing edges of the current marking
std::vector<Label_ID> currentLabels;

/// names of transitions, that are enabled under the current marking (needed for cover)
std::set<std::string> currentTransitions;

/// the Tarjan lowlink value of the current marking
InnerMarking_ID currentLowlink;

/// a marking of the PN API net
std::map<const pnapi::Place*, unsigned int> marking;

/// storage for current (terminal) strongly connected component
std::set<InnerMarking_ID> currentSCC;

/// a file to store a mapping from marking ids to actual Petri net markings
extern Output *markingoutput;

/// the command line parameters
extern gengetopt_args_info args_info;

extern int graph_lex();
extern int graph_error(const char *);
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
        InnerMarking::markingMap[$2] = new InnerMarking($2, currentLabels, currentSuccessors,
                                           openNet::net->getFinalCondition().isSatisfied(pnapi::Marking(marking, openNet::net)));
                                           
        /* ================================================================================= */
        /* calculate strongly connected components and do some evaluation on its members     */
        /* ================================================================================= */

        /* current marking is representative of an SCC */
        if ((currentLowlink == $2)) {

            // insert representative into current SCC of inner markings
            currentSCC.insert($2);
            
            // we have found a non-trivial SCC within the inner markings, so the reachability graph
            // contains a cycle
            if (currentSCC.size() > 1) {
                InnerMarking::is_acyclic = false;
            }
        } /* end if, livelock freedom */

        currentLabels.clear();
        currentSuccessors.clear();
        marking.clear();
        currentSCC.clear();
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
    { currentSCC.insert($1); }
;

/* do something with Tarjan's lowlink value (needed for generating
   livelock free partners or reduction rule smart sending event) */
lowlink:
  KW_LOWLINK NUMBER
    { currentLowlink = $2; }
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
    { marking[openNet::net->findPlace($1)] = $3; free($1); }
;

transitions:
  transition
| transitions transition
;

transition:
  NAME ARROW NUMBER
    {
      currentLabels.push_back(Label::name2id[$1]);
      currentSuccessors.push_back($3);
      free($1);
    }
;
