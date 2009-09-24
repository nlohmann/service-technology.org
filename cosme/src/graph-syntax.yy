%token KW_STATE KW_LOWLINK KW_SCC COLON COMMA ARROW NUMBER NAME

%expect 0
%defines
%name-prefix="graph_yy"

%{
#include <fstream>
#include <vector>
#include <cstring>
#include <map>
#include "Label.h"
#include "types.h"
#include "Service.h"
#include <../libs/pnapi/pnapi.h>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

extern char* graph_yytext;
extern int graph_yylex();
extern int graph_yyerror(const char *);

extern Label GlobalLabels;
extern std::map <std::string, label_id_t> TransitionLabels;
extern pnapi::PetriNet tmpNet;

// for current operating guideline
extern std::map<og_service_index_t, ServiceMarking*> ServiceMarkings;

// for current state
og_service_index_t CurrentStateNumber;
std::multimap<label_id_t, std::pair<label_id_t, og_service_index_t> > CurrentServiceSuccessors;
std::map<const pnapi::Place*, unsigned int> CurrentMarking;
string CurrentPlaceName;

// for current transition
extern label_id_t CurrentTransitionLabel; //OG-Parser
extern og_service_index_t CurrentTransitionDestination; //OG-Parser

%}

%union {
	char *str;
  unsigned int value;
}

%type <value> NUMBER
%type <str>   NAME

%%

states:
  state
| states state
;

state:
  KW_STATE NUMBER 
	{ 
		CurrentStateNumber = $2;
		CurrentServiceSuccessors.clear();
		CurrentMarking.clear();
	}
	lowlink scc markings_or_transitions
	/*save current state*/
	{ 
		ServiceMarking *newMarking = new ServiceMarking(CurrentServiceSuccessors, 
																			tmpNet.finalCondition().isSatisfied(pnapi::Marking(CurrentMarking, &tmpNet)));
		ServiceMarkings.insert ( std::make_pair(CurrentStateNumber, newMarking) );
	}
;


scc:
  /* empty */
| KW_SCC scc_members
;

scc_member:
  NUMBER
;

scc_members:
  scc_member
| scc_members scc_member
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
  NAME
	{
		CurrentPlaceName = string($1);
		CurrentPlaceName = CurrentPlaceName.substr(0, CurrentPlaceName.find_first_of(' '));
	}
	COLON NUMBER
	{ 
		CurrentMarking.insert(std::make_pair(tmpNet.findPlace(CurrentPlaceName), $4)); 
	}
;

transitions:
  transition
| transitions transition
;

transition:
  NAME 
	{ 
		std::map <std::string, label_id_t>::const_iterator it;
		it = TransitionLabels.find($1);
		if (it == TransitionLabels.end()) {
			// transition is unknown -> Tau-Transition
			CurrentTransitionLabel = 0;
		}
		else {
			// transition is known
			CurrentTransitionLabel = it->second;
		}
	}
	ARROW NUMBER
	{
		CurrentTransitionDestination = $4;
		/*save current transition*/		
		CurrentServiceSuccessors.insert( std::make_pair(CurrentTransitionLabel, 
																			std::make_pair(CurrentTransitionLabel, CurrentTransitionDestination)) );
	}
;
