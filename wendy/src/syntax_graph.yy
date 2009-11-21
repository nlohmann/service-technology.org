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
%name-prefix="graph_"

%{
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "InnerMarking.h"
#include "Label.h"
#include "Output.h"
#include "Cover.h"
#include "cmdline.h"
#include "types.h"
#include "verbose.h"

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
                                           InnerMarking::net->finalCondition().isSatisfied(pnapi::Marking(marking, InnerMarking::net)));

        if (markingoutput) {
            markingoutput->stream() << $2 << ": ";
            for (std::map<const pnapi::Place*, unsigned int>::iterator p = marking.begin(); p != marking.end(); ++p) {
                if (p != marking.begin()) {
                    markingoutput->stream() << ", ";
                }
                markingoutput->stream() << p->first->getName() << ":" << p->second;
            }
            markingoutput->stream() << "\n";
        }

        if (args_info.cover_given) {
            Cover::checkInnerMarking($2, marking, currentTransitions);
            currentTransitions.clear();
        }

        /* ================================================================================= */
        /* calculate strongly connected components and do some evaluation on its members     */
        /* ================================================================================= */

        /* current marking is representative of an SCC and either reduction by smart sending events or correctness criteria livelock freedom is turned on */
        if (currentLowlink == $2 and
                not args_info.ignoreUnreceivedMessages_flag or args_info.correctness_arg == correctness_arg_livelock) {
    
            // insert representative into current SCC of inner markings
            currentSCC.insert($2);
    
            // reduction by smart sending events is turned on
            if (not args_info.ignoreUnreceivedMessages_flag) {
                /* it is a trivial SCC */
                if (currentSCC.size() == 1) {
                    /* analyze only representative with respect to possible sending events */
                    InnerMarking::markingMap[$2]->calcReachableSendingEvents();
                } else {
                    /* analyze all members of current SCC with respect to possible sending events and final markings reachable */
                    InnerMarking::analyzeSCCOfInnerMarkings(currentSCC);
                }
            } else if (args_info.ignoreUnreceivedMessages_flag and args_info.correctness_arg == correctness_arg_livelock) {
                // no smart sending event reduction but livelock freedom is turned on
                InnerMarking::finalMarkingReachableSCC(currentSCC);
            }
        } /* end if, livelock freedom */

        currentLabels.clear();
        currentSuccessors.clear();
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
    { currentSCC.insert($1); }
;

lowlink:
  KW_LOWLINK NUMBER
    {
        /* do something with Tarjan's lowlink value (needed for generating
           livelock free partners or reduction rule smart sending event) */
        currentLowlink = $2;
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
    { marking[InnerMarking::net->findPlace($1)] = $3; free($1); }
;

transitions:
  transition
| transitions transition
;

transition:
  NAME ARROW NUMBER
    {
      // a workaround for bug #14719
      if (Label::sync_events > 0) {
          for (size_t i = 0; i < currentLabels.size(); ++i) {
              if (SYNC(Label::name2id[$1]) and currentLabels[i] == Label::name2id[$1]) {
                  abort(17, "synchronous label of '%s' already using in this marking", $1, Label::id2name[Label::name2id[$1]].c_str());
              }
          }
      }

      currentLabels.push_back(Label::name2id[$1]);
      if(args_info. cover_given) {
          currentTransitions.insert($1);
      }
      currentSuccessors.push_back($3);
      free($1);
    }
;
