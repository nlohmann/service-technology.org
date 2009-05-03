/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


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
