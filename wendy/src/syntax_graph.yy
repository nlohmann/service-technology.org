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


%token KW_STATE KW_PROG KW_LOWLINK COLON COMMA ARROW NUMBER NAME

%defines
%name-prefix="graph_"

%{
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "InnerMarking.h"
#include "Label.h"
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

/// the depth first search number (dfs) of current marking
InnerMarking_ID currentDFS;

/// the Tarjan lowlink value of the current marking
InnerMarking_ID currentLowlink;

/// current marking is a representative of a (terminal) strongly connected components
bool currentMarkingIsRepresentative;

/// a marking of the PN API net
std::map<const pnapi::Place*, unsigned int> marking;

/// a Tarjan stack for storing markings in order to detect (terminal) strongly connected components
std::deque<InnerMarking_ID> tarjanStack;

extern std::ofstream *markingfile;

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
  KW_STATE NUMBER prog lowlink markings transitions
    { 
        InnerMarking::markingMap[$2] = new InnerMarking(currentLabels, currentSuccessors,
                                                InnerMarking::net->finalCondition().isSatisfied(pnapi::Marking(marking, InnerMarking::net)));

        if (markingfile) {
            *markingfile << $2 << ": ";
            for (std::map<const pnapi::Place*, unsigned int>::iterator p = marking.begin(); p != marking.end(); ++p) {
                if (p != marking.begin()) {
                    *markingfile << ", ";
                }
                *markingfile << p->first->getName() << ":" << p->second;
            }
            *markingfile << std::endl;
        }

        if (args_info.cover_given) {
            Cover::checkInnerMarking($2, marking, currentTransitions);
            currentTransitions.clear();
        }

        /* ================================================================================= */
        /* code for generating livelock free partners */
        /* ================================================================================= */

        if (args_info.lf_flag) {

            //status("current dfs: %d, lowlink: %d", $2, currentLowlink);

            /*  check, if current marking is a representative of a strongly connected component */
            if ($2 == currentLowlink) {
                currentMarkingIsRepresentative = true;
            } else {
                currentMarkingIsRepresentative = false;
            }    
        
            /* current marking's dfs is just the number given behind the KW_STATE token */
            currentDFS = $2;
            
            /* first put current marking on the Tarjan stack */
            tarjanStack.push_front(currentDFS);
    
            /* current marking does not activate any transitions and thus does not have any successors */ 
            if (currentSuccessors.empty()) {
                
                //status("... no successors");
            
                /* check if current marking is not final */
                if (!(InnerMarking::markingMap[$2])->is_final) {
                    /* then, from current marking no final marking is reachable and it is an internal deadlock */
                    (InnerMarking::markingMap[$2])->is_deadlock = true;
                    (InnerMarking::markingMap[$2])->is_final_marking_reachable = 0;
                    
                    //status("... is internal deadlock");
                    
                } else {
                    (InnerMarking::markingMap[$2])->is_final_marking_reachable = 1;
                    
                    //status("... final marking is reachable");
                }
                
                /* actually delete it from stack */
                tarjanStack.pop_back();                
                
            } else {
                
                if (!(InnerMarking::markingMap[$2])->is_final_marking_reachable) {
                    /* if current marking is not yet marked with "final marking is reachable", traverse through current markings successsors */
                    for (uint8_t i = 0; i < (InnerMarking::markingMap[$2])->out_degree; i++) {
                    
                        //status("... found successor %d", (InnerMarking::markingMap[$2])->successors[i]);
                    
                        InnerMarking_ID successor = (InnerMarking::markingMap[$2])->successors[i];
                    
                        /* check if from currently considered successor a final marking is reachable or if it is final itself*/
                        if (InnerMarking::markingMap[successor]) {
                            if ((InnerMarking::markingMap[successor])->is_final_marking_reachable || (InnerMarking::markingMap[successor])->is_final) {
                                /* yes, it is */
                                
                                //status("... from which a final marking is reachable");
                                
                                (InnerMarking::markingMap[$2])->is_final_marking_reachable = 1;
                                /* found one, so get out of the for loop */   
                                break;
                            } else {
                                (InnerMarking::markingMap[$2])->is_final_marking_reachable = 0;
                            }
                        }   
                    }
                }
    
                /* current marking is a representative, so fetch all members of the strongly connected components from stack */
                if (currentMarkingIsRepresentative) {
                    
                    //status("... is representative of a (T)SCC");
                    
                    /* last popped marking */
                    InnerMarking_ID poppedMarking;
                    
                    do {
                        /* get last marking from the Tarjan stack */
                        poppedMarking = tarjanStack.back();
                        
                        //status("... together with %d", poppedMarking);
                        
                        /* actually delete it from stack */
                        tarjanStack.pop_back();
                        
                        /* if from representative a final marking is reachable, then from all markings of the current */
                        /* strongly connected component a final marking is reachable as well */
                        if ((InnerMarking::markingMap[$2])->is_final_marking_reachable) {
                            
                            //status("... from which a final marking is reachable");
                            (InnerMarking::markingMap[poppedMarking])->is_final_marking_reachable = 1; 
                            
                        } else {
                            /* if no final marking is reachable from the representative, then no final marking is reachable */
                            /* from the markings of the strongly connected component */
                            
                            (InnerMarking::markingMap[poppedMarking])->is_final_marking_reachable = 0; 
                        }
                    } while ($2 != poppedMarking);
                } 
            } /* end else, successors is empty */
        } /* end if, livelock freedom */

        currentLabels.clear();
        currentSuccessors.clear();
        marking.clear(); 
   }
;

prog:
  /* empty */
| KW_PROG NUMBER
;

lowlink:
    {
        if (args_info.lf_flag) {
            // livelock freedom is activated, but lola does not provide necessary lowlink value
            abort(17, "LoLA has not been configured appropriately");
        }
    }
| KW_LOWLINK NUMBER
	{
		/* do something with Tarjan's lowlink value (needed for generating livelock free partners) */	
		currentLowlink = $2;
	}
;

markings:
  /* empty */
| NAME COLON NUMBER
    { marking[InnerMarking::net->findPlace(NAME_token)] = $3; }
| markings COMMA NAME COLON NUMBER
    { marking[InnerMarking::net->findPlace(NAME_token)] = $5; }
;

transitions:
  /* empty */
| transitions NAME ARROW NUMBER
    { 
      currentLabels.push_back(Label::name2id[NAME_token]);
      if(args_info.cover_given)
        currentTransitions.insert(NAME_token);
      currentSuccessors.push_back($4); 
    }
;
