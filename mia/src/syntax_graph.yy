%token KW_STATE KW_PROG COLON DOT COMMA ARROW NUMBER NAME

%defines
%name-prefix="graph_"

%{
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <set>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

unsigned int stat_stateCount = 0;

/// the current NAME token as string
std::string NAME_token;
std::string NAME_temp;

std::string statename;

bool skip = false;

/// the tuples of the target (innermarking, mppstate, interfacemarking)
std::map<std::string, std::set<std::vector<unsigned int> > > tuples_target;

extern std::map<unsigned int, std::string> id2name;
extern std::map<std::string, unsigned int> name2id;

extern unsigned int interfaceLength;

extern std::vector<unsigned int> currentTuple;

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
  KW_STATE NUMBER prog
    {
        skip = false;
        statename = "";
        currentTuple = std::vector<unsigned int>(interfaceLength, 0);
    }
  markings transitions
    {
        if (not skip) {
            tuples_target[statename].insert(currentTuple);
        }
        ++stat_stateCount;
    }
;

prog:
  /* empty */
| KW_PROG NUMBER
;

markings:
  marking
| markings COMMA marking
;

marking:
  NAME COLON NUMBER
    { skip = true; }
| NAME DOT
    { NAME_temp = NAME_token; }
  NAME COLON NUMBER
    {
        if (NAME_temp == "mpp[1]") {
            if (name2id[NAME_token] == 0) {
                std::string a = NAME_token.substr(1, NAME_token.length());
                currentTuple[0] = atoi(a.c_str());
            } else {
                currentTuple[ name2id[NAME_token] ] = $6;
            }
            break;
        }
        if (NAME_temp == "target[1]") {
            if (name2id[NAME_token] == 0) {
                statename += NAME_token;
            } else {
                // we only look at interface markings with mpp prefix
                skip = true;
            }
        }
    }
;

transitions:
  /* empty */
| transitions NAME ARROW NUMBER
| transitions NAME DOT NAME ARROW NUMBER
;
