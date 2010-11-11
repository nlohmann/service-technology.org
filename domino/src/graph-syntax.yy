%token KW_STATE KW_LOWLINK KW_SCC COLON COMMA ARROW NUMBER NAME

%expect 0
%defines
%name-prefix="graph_yy"

%{
#include <fstream>
#include <vector>
#include <cstring>
#include <map>
#include <pnapi/pnapi.h>

#include "types.h"
#include "verbose.h"

using std::map;

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

extern char* graph_yytext;
extern int graph_yylex();
extern int graph_yyerror(const char *);

place_t curPlaceName;
size_t curPlaceTokens;
extern map<place_t, size_t> Place2MaxTokens;
map<place_t, size_t>::iterator curPlaceMaxToken;

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
	lowlink scc markings_or_transitions
;


scc:
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
		curPlaceName = std::string($1);
		curPlaceName = curPlaceName.substr(0, curPlaceName.find_first_of(' '));
	}
	COLON NUMBER
	{ 
		curPlaceTokens = $4;
		curPlaceMaxToken = Place2MaxTokens.find(curPlaceName);
		if (curPlaceMaxToken != Place2MaxTokens.end()) {
			//Place already known
			if (curPlaceTokens > curPlaceMaxToken->second) {
				Place2MaxTokens.erase(curPlaceMaxToken);
				Place2MaxTokens[curPlaceName] = curPlaceTokens;
			}
		}
		else {
			Place2MaxTokens[curPlaceName] = curPlaceTokens;
		}
	}
;

transitions:
  transition
| transitions transition
;

transition:
  NAME 
	ARROW NUMBER
;
