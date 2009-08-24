%token KEY_PEER KEY_NODES KEY_INITIAL KEY_FINAL KEY_IN KEY_OUT
%token SEMICOLON COMMA COLON ARROW
%token NUMBER NAME SYNCLABEL RCVLABEL SENDLABEL

%defines
%name-prefix="chor_"

%{
#include "peerautomaton.h"
#include "config.h"
#include <cassert>
#include "peer.h"

#include <string>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/// the current NAME token as string
std::string NAME_token;

extern PeerAutomaton * chor;
Peer *currentPeer;
bool input;
int state;

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
  KEY_PEER NAME { currentPeer = new Peer(NAME_token); } inputs outputs
  {
  	chor->pushPeer(currentPeer);
  }
;

inputs:
  /* empty */
| KEY_IN { input = true; } label_list SEMICOLON
;

outputs:
  /* empty */
| KEY_OUT { input = false; } label_list SEMICOLON
;

label_list:
  NAME
  {
    if (input)
  	  currentPeer->pushInput(NAME_token);
  	else
  	  currentPeer->pushOutput(NAME_token);
  	chor->pushEvent(NAME_token);
  }
| label_list COMMA NAME
  {
    if (input)
      currentPeer->pushInput(NAME_token);
    else
      currentPeer->pushOutput(NAME_token);
  	chor->pushEvent(NAME_token);
  }
;

nodes:
  node
| nodes node
;

node:
  NUMBER { state = $1; chor->pushState(state); } annotation successors
;

annotation:
  /* empty */
| COLON KEY_INITIAL
  {
    chor->setInitialState(state);
  }
| COLON KEY_FINAL
  {
    chor->pushFinalState(state);
  }
| COLON KEY_INITIAL COMMA KEY_FINAL
  {
    chor->setInitialState(state);
    chor->pushFinalState(state);
  }
;

successors:
  /* empty */
| successors SENDLABEL ARROW NUMBER
  {
    chor->createTransition(state, NAME_token.substr(1,NAME_token.size()), $4, SND);
  }
| successors RCVLABEL ARROW NUMBER
  {
    chor->createTransition(state, NAME_token.substr(1,NAME_token.size()), $4, RCV);
  }
| successors SYNCLABEL ARROW NUMBER
  {
    chor->createTransition(state, NAME_token.substr(1,NAME_token.size()), $4, SYN);
  }
;
