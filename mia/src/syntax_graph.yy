%token KW_STATE KW_PROG COLON DOT COMMA ARROW NUMBER NAME TGT_NAME MPP_NAME

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
unsigned int stat_tupleCountNew = 0;

/// the current NAME token as string
std::string NAME_token;

std::string statename;

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
        statename = "";
        currentTuple = std::vector<unsigned int>(interfaceLength, 0);
    }
  markings transitions
    {
        if (tuples_target[statename].insert(currentTuple).second) {
            ++stat_tupleCountNew;
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
    {
        // a name without prefix must be an interface name
        assert(name2id[NAME_token] != 0);

        // store the interface marking
        currentTuple[ name2id[NAME_token] ] = $3;
    }
| MPP_NAME COLON NUMBER
    {
        // a name with MPP prefix must not be an interface name
        assert(name2id[NAME_token] == 0);

        // strip "p" prefix of place name to get state number of MPP
        std::string a = NAME_token.substr(1, NAME_token.length());

        // store the name of the MPP's state
        currentTuple[0] = atoi(a.c_str());
    }
| TGT_NAME COLON NUMBER
    {
        // a name with TGT prefix must not be an interface name
        assert(name2id[NAME_token] == 0);

        // collect the target service's state name
        statename += NAME_token;
    }
;

transitions:
  /* empty */
| transitions NAME ARROW NUMBER
| transitions MPP_NAME ARROW NUMBER
| transitions TGT_NAME ARROW NUMBER
;
