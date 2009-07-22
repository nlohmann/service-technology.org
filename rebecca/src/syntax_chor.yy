%token KEY_PEER KEY_NODES KEY_INITIAL KEY_FINAL KEY_IN KEY_OUT
%token SEMICOLON COMMA COLON ARROW
%token NUMBER NAME SYNCLABEL RCVLABEL SENDLABEL

%defines
%name-prefix="chor_"

%{
#include "config.h"
#include <cassert>

#include <string>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/// the current NAME token as string
std::string NAME_token;

extern int chor_lex();
extern int chor_error(const char *);
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%%

chorography:
  peers KEY_NODES nodes
;

peers:
  peer
| peers peer
;

peer:
  KEY_PEER NAME inputs outputs
;

inputs:
  /* empty */
| KEY_IN label_list SEMICOLON
;

outputs:
  /* empty */
| KEY_OUT label_list SEMICOLON
;

label_list:
  NAME
| label_list COMMA NAME
;

nodes:
  node
| nodes node
;

node:
  NUMBER annotation successors
;

annotation:
  /* empty */
| COLON KEY_INITIAL
| COLON KEY_FINAL
| COLON KEY_INITIAL COMMA KEY_FINAL
;

successors:
  /* empty */
| successors SENDLABEL ARROW NUMBER
| successors RCVLABEL ARROW NUMBER
| successors SYNCLABEL ARROW NUMBER
;
