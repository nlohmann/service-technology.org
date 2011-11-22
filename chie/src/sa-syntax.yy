%{
#include <map>
#include <string>
#include "automata.h"

extern char* sa_yytext;
extern int sa_yylex();
extern int sa_yyerror(char const *msg);

extern ServiceAutomaton * sa_result;
extern ServiceAutomaton * sa_serviceAutomaton;

bool inputEvents = true;
bool hasSuccessors = false;
unsigned int currentNode;
std::string sa_lastIdent;
%}

%name-prefix="sa_yy"
%error-verbose
%token_table
%defines

%token KEY_NODES KEY_INITIAL KEY_FINAL
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COLON COMMA SEMICOLON IDENT ARROW NUMBER

%union {
    unsigned int value;
}

%type <value> NUMBER

%start sa
%%


sa:
  KEY_INTERFACE input output synchronous KEY_NODES nodes
;


input:
  /* empty */
| KEY_INPUT
  { inputEvents = true; }
  identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT
  { inputEvents = false; }
  identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS
  { sa_yyerror("synchronous communication not supported"); }
  identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  {
    if (sa_serviceAutomaton &&
        (sa_serviceAutomaton->isSendingEvent.count(sa_lastIdent) > 0) &&
        (sa_serviceAutomaton->isSendingEvent[sa_lastIdent] != inputEvents))
    {
      sa_yyerror("channel type mismatch");
    }
    sa_result->isSendingEvent[sa_lastIdent] = !inputEvents;
  }
| identlist COMMA IDENT
  {
    if (sa_serviceAutomaton &&
        (sa_serviceAutomaton->isSendingEvent.count(sa_lastIdent) > 0) &&
        (sa_serviceAutomaton->isSendingEvent[sa_lastIdent] != inputEvents))
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
| COLON KEY_INITIAL
  { sa_result->initialState = currentNode; }
| COLON KEY_FINAL
  { sa_result->finalStates.insert(currentNode); }
| COLON KEY_INITIAL COMMA KEY_FINAL
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

