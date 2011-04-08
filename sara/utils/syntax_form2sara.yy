/*****************************************************************************\
 Sara -- Structures for Automated Reachability Analysis

 Copyright (C) 2011  Harro Wimmel <harro.wimmel@uni-rostock.de>

 Sara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Sara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Sara.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%token COMMA NUMBER NAME EQSIGN EOL REACHABLE INVARIANT POPEN PCLOSE BOPEN BCLOSE BROPEN BRCLOSE NOTSIGN ANDSIGN ORSIGN STARSIGN DOTSIGN CONTAINS EQUALS

%defines
%name-prefix="form2sara_"

%{
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdio>
#include "BFNode.h"
using std::cerr;
using std::endl;

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

vector<BFNode*> roots,list;
vector<bool> quant;
vector<string> formid;
BFNode* node;
int tokenval;

/// the current NAME token as string
extern std::string form2sara_NAME_token;

extern int form2sara_lex();
extern int form2sara_error(const char *);
extern const char* form2sara_text;
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%%

formulas:
	formula formulas
|	formula
;

formula:
	NAME { formid.push_back(form2sara_NAME_token); node = new BFNode(); roots.push_back(node); } EQSIGN quantifier subformula EOL
;

quantifier:
	REACHABLE { quant.push_back(true); }
|	INVARIANT { quant.push_back(false); }
;

subformula:
	atomic
|	POPEN {	node->setInner(BFNode::BFAND); list.push_back(node); 
		node=new BFNode(); list.back()->addSon(node); } 
	subformula PCLOSE andorformula { node=list.back(); list.pop_back(); }
|	NOTSIGN POPEN {	node->setInner(BFNode::BFAND); list.push_back(node);
			node=new BFNode(); node->setInner(BFNode::BFNOT); list.back()->addSon(node); list.push_back(node); 
			node=new BFNode(); list.back()->addSon(node); } 
	subformula PCLOSE { list.pop_back(); } 
	andorformula { node=list.back(); list.pop_back(); }
;

andorformula:
	ANDSIGN POPEN { node=new BFNode(); list.back()->addSon(node); } subformula PCLOSE andformula
|	ANDSIGN NOTSIGN POPEN {	node=new BFNode(); node->setInner(BFNode::BFNOT); list.back()->addSon(node); list.push_back(node); 
				node=new BFNode(); list.back()->addSon(node); } 
	subformula PCLOSE { list.pop_back(); } andformula
|	ORSIGN POPEN { list.back()->setInner(BFNode::BFOR); node=new BFNode(); list.back()->addSon(node); } subformula PCLOSE orformula
|	ORSIGN NOTSIGN POPEN {	list.back()->setInner(BFNode::BFOR); 
				node=new BFNode(); node->setInner(BFNode::BFNOT); list.back()->addSon(node); list.push_back(node); 
				node=new BFNode(); list.back()->addSon(node); } 
	subformula PCLOSE { list.pop_back(); } orformula
|	/* empty */
;

andformula:
	ANDSIGN POPEN { node=new BFNode(); list.back()->addSon(node); } subformula PCLOSE andformula
|	ANDSIGN NOTSIGN POPEN {	node=new BFNode(); node->setInner(BFNode::BFNOT); list.back()->addSon(node); list.push_back(node); 
				node=new BFNode(); list.back()->addSon(node); } 
	subformula PCLOSE { list.pop_back(); } andformula
|	/* empty */
;

orformula:
	ORSIGN POPEN { node=new BFNode(); list.back()->addSon(node); } subformula PCLOSE orformula
|	ORSIGN NOTSIGN POPEN {	node=new BFNode(); node->setInner(BFNode::BFNOT); list.back()->addSon(node); list.push_back(node); 
				node=new BFNode(); list.back()->addSon(node); } 
	subformula PCLOSE { list.pop_back(); } orformula
|	/* empty */
;

atomic:
	NAME CONTAINS BOPEN BCLOSE { node->setLeaf(form2sara_NAME_token,BFNode::BFGE,0); }
|	NAME CONTAINS BOPEN tokens BCLOSE { node->setLeaf(form2sara_NAME_token,BFNode::BFGE,tokenval); }
|	NAME EQUALS BOPEN BCLOSE { node->setLeaf(form2sara_NAME_token,BFNode::BFEQ,0); }
|	NAME EQUALS BOPEN tokens BCLOSE { node->setLeaf(form2sara_NAME_token,BFNode::BFEQ,tokenval); }
;

tokens:
	token
|	token COMMA tokens
;

token:
	NUMBER STARSIGN value { tokenval = $1; }
;

value:
	BROPEN DOTSIGN BRCLOSE /* coloured tokens are missing here */
;

