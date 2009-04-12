%token KW_STATE KW_PROG COLON COMMA ARROW NUMBER NAME

%defines
%name-prefix="graph_"

%{
#include <vector>
#include <string>
#include <map>
#include "InnerMarking.h"
#include "Label.h"

/// the current NAME token as string
std::string NAME_token;

/// the vector of the successor state numbers of the current marking
std::vector<InnerMarking_ID> currentSuccessors;

/// the labels of the outgoing edges of the current marking
std::vector<Label_ID> currentLabels;

/// a marking of the PN API net
std::map<const pnapi::Place*, unsigned int> marking;

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
  KW_STATE NUMBER prog markings transitions
    { InnerMarking::markingMap[$2] = new InnerMarking(currentLabels, currentSuccessors,
                                                      InnerMarking::net->finalCondition().isSatisfied(pnapi::Marking(marking)));
      currentLabels.clear();
      currentSuccessors.clear();
      marking.clear(); }
;

prog:
  /* empty */
| KW_PROG NUMBER
;

markings:
  NAME COLON NUMBER
    { marking[InnerMarking::net->findPlace(NAME_token)] = $3; }
| markings COMMA NAME COLON NUMBER
    { marking[InnerMarking::net->findPlace(NAME_token)] = $5; }
;

transitions:
  /* empty */
| transitions NAME ARROW NUMBER
    { currentLabels.push_back(Label::name2id[NAME_token]);
      currentSuccessors.push_back($4); }
;
