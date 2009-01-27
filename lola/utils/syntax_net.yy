%token KW_PLACE KW_TRANSITION KW_MARKING KW_CONSUME KW_PRODUCE
%token SEMICOLON COLON COMMA
%token NUMBER NAME 

%start net

%defines
%token_table
%name-prefix="net_"


%{

#include <string>
#include <list>
#include <map>

#define YYERROR_VERBOSE

using namespace std;

extern int net_lex();
extern int net_error(const char *);

list<string> places;
map<string, int> marking;
map <string, map<string, int> > transitions;
int factor;
string currentTransition;

%}


%union {
  unsigned int val;
  char *name;
}

%type <val> NUMBER
%type <name> NAME

%%

net:
  KW_PLACE placelist SEMICOLON KW_MARKING markinglist SEMICOLON transitionlist
;

placelist:
  NAME { places.push_back(string($1)); }
| placelist COMMA NAME { places.push_back(string($3)); }
;

markinglist:
  NAME COLON NUMBER { marking[string($1)] = $3; }
| markinglist COMMA NAME COLON NUMBER { marking[string($3)] = $5; }
;

transitionlist:
  /* empty */
| transitionlist transition
;

transition:
  KW_TRANSITION NAME
    { currentTransition = string($2); }
  KW_CONSUME
    {factor = -1;}
  arclist_opt SEMICOLON KW_PRODUCE
    {factor = 1;}
  arclist_opt SEMICOLON
;

arclist_opt:
  /* empty */
| arclist
;

arclist:
  NAME COLON NUMBER
    { transitions[currentTransition][string($1)] += (factor * (int)$3); }
| arclist COMMA NAME COLON NUMBER
    { transitions[currentTransition][string($3)] += (factor * $5); }
;
