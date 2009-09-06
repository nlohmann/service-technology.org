%token KEY_PEER KEY_NODES KEY_INITIAL KEY_FINAL KEY_IN KEY_OUT
%token SEMICOLON COMMA COLON ARROW
%token NUMBER NAME SYNCLABEL RCVLABEL SENDLABEL

%defines
%name-prefix="chor_"

%{
#include "choreography.h"
#include "config.h"
#include <cassert>
#include "peer.h"

#include <set>
#include <string>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/// the current NAME token as string
std::string NAME_token;

extern Choreography * chor;
Peer *currentPeer;
bool input;
int state;
std::set<std::string> asynchronous, synchronous;

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
  {
    for (std::set<std::string>::iterator l = asynchronous.begin(); l != asynchronous.end(); l++)
    {
      chor->pushEvent("!"+*l);
      chor->pushEvent("?"+*l);
      for (int i = 0; i < (int) chor->collaboration().size(); i++)
        chor->collaboration()[i]->deriveEvent(*l, ASYNCH);
    }
    for (std::set<std::string>::iterator l = synchronous.begin(); l != synchronous.end(); l++)
    {
      chor->pushEvent("#"+*l);
      for (int i = 0; i < (int) chor->collaboration().size(); i++)
        chor->collaboration()[i]->deriveEvent(*l, SYNCH);
    }
  }
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
  	  currentPeer->pushIn(NAME_token);
  	else
  	  currentPeer->pushOut(NAME_token);
  }
| label_list COMMA NAME
  {
    if (input)
      currentPeer->pushIn(NAME_token);
    else
      currentPeer->pushOut(NAME_token);
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
  	asynchronous.insert(NAME_token.substr(1,NAME_token.size()));
    chor->createEdge(state, NAME_token, $4, SND);
  }
| successors RCVLABEL ARROW NUMBER
  {
  	asynchronous.insert(NAME_token.substr(1,NAME_token.size()));
    chor->createEdge(state, NAME_token, $4, RCV);
  }
| successors SYNCLABEL ARROW NUMBER
  {
  	synchronous.insert(NAME_token.substr(1,NAME_token.size()));
    chor->createEdge(state, NAME_token, $4, SYN);
  }
;
