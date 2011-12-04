%{
#include <map>
#include <string>
#include "automata.h"

// generic bison/flex stuff
extern char * sa_yytext;
extern int sa_yylex();
extern int sa_yyerror(char const *msg);

// pointer to service automata
extern ServiceAutomaton * sa_result;
extern ServiceAutomaton * sa_specification;

// whether we are handling input events
bool inputEvents = true;

// wheter node has successors
bool hasSuccessors = false;

// number of current node
unsigned int currentNode;

// last read ident
std::string sa_lastIdent;
%}

%name-prefix="sa_yy"
%error-verbose
%token_table
%defines

%token NODES INITIAL_ FINAL
%token INTERFACE INPUT OUTPUT SYNCHRONOUS
%token COLON COMMA SEMICOLON IDENT ARROW NUMBER

%union {
    unsigned int value;
}

%type <value> NUMBER

%start sa
%%


sa:
  INTERFACE input output synchronous NODES nodes
;


input:
  /* empty */
| INPUT
  { inputEvents = true; }
  identlist SEMICOLON
;


output:
  /* empty */
| OUTPUT
  { inputEvents = false; }
  identlist SEMICOLON
;


synchronous:
  /* empty */
| SYNCHRONOUS
  { sa_yyerror("synchronous communication not supported"); }
  identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  {
    if (sa_specification && // pointer not NULL, i.e. we are parsing a test case
        (sa_specification->isSendingEvent.count(sa_lastIdent) > 0) && // current ident is a communication event of the specification
        (sa_specification->isSendingEvent[sa_lastIdent] != inputEvents)) // current ident is an event of the same type as the event of the specification
    {
      // both service automata are sending or both are receiving this event
      sa_yyerror("channel type mismatch");
    }
    sa_result->isSendingEvent[sa_lastIdent] = !inputEvents;
  }
| identlist COMMA IDENT
  {
    if (sa_specification && // see above
        (sa_specification->isSendingEvent.count(sa_lastIdent) > 0) &&
        (sa_specification->isSendingEvent[sa_lastIdent] != inputEvents))
    {
      sa_yyerror("channel type mismatch");
    }
    sa_result->isSendingEvent[sa_lastIdent] = !inputEvents;
  }
;


nodes:
  node
| nodes node
;


node:
  NUMBER
  {
    currentNode = $1;
    hasSuccessors = false;
  }
  annotation successors
  {
    if(!hasSuccessors)
    {
      sa_result->noSuccessorStates.insert($1);
    }
  }
;


annotation:
  /* empty */
| COLON INITIAL_
  { sa_result->initialState = currentNode; }
| COLON FINAL
  { sa_result->finalStates.insert(currentNode); }
| COLON INITIAL_ COMMA FINAL
  { sa_result->finalStates.insert(sa_result->initialState = currentNode); }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    sa_result->stateSpace[currentNode][sa_lastIdent].insert($4);
    hasSuccessors = true;
  }
;


%%

