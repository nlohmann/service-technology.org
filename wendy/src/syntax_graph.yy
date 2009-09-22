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

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/// the current NAME token as string
std::string NAME_token;

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
std::vector<InnerMarking_ID> currentSCC;

/// a file to store a mapping from marking ids to actual Petri net markings
extern Output *markingoutput;

/// the command line parameters
extern gengetopt_args_info args_info;

extern int graph_lex();
extern int graph_error(const char *);
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

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

        if (markingoutput != NULL) {
            markingoutput->stream() << $2 << ": ";
            for (std::map<const pnapi::Place*, unsigned int>::iterator p = marking.begin(); p != marking.end(); ++p) {
                if (p != marking.begin()) {
                    markingoutput->stream() << ", ";
                }
                markingoutput->stream() << p->first->getName() << ":" << p->second;
            }
            markingoutput->stream() << std::endl;
        }

        if (args_info.cover_given) {
            Cover::checkInnerMarking($2, marking, currentTransitions);
            currentTransitions.clear();
        }

        /* ================================================================================= */
        /* calculate strongly connected components and do some evaluation on its members     */
        /* ================================================================================= */

        if (args_info.smartSendingEvent_flag or args_info.lf_flag) {
            /* current marking is representative */
            while (not currentSCC.empty()) {
                /* get marking from current SCC */
                InnerMarking_ID poppedMarking = currentSCC.back();

                /* actually delete it from vector */
                currentSCC.pop_back();

                /* if a final marking is reachable from the representative,
                   then a final marking is reachable from all markings within
                   the strongly connected component */
                InnerMarking::finalMarkingReachableMap[poppedMarking] = InnerMarking::finalMarkingReachableMap[$2];

                if (args_info.smartSendingEvent_flag) {
                    /* ... the same is true for possible sending events */
                    InnerMarking::markingMap[poppedMarking]->possibleSendEvents->copy(*InnerMarking::markingMap[$2]->possibleSendEvents);
                }
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

scc_member:
  NUMBER
    { currentSCC.push_back($1); }
;

scc_members:
  scc_member
| scc_members scc_member
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
    { marking[InnerMarking::net->findPlace(NAME_token)] = $3; }
;

transitions:
  transition
| transitions transition
;

transition:
  NAME ARROW NUMBER
    {
      currentLabels.push_back(Label::name2id[NAME_token]);
      if(args_info.cover_given) {
          currentTransitions.insert(NAME_token);
      }
      currentSuccessors.push_back($3);
    }
;
