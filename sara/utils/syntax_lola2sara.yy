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

%token COMMA COLON SEMICOLON NUMBER NAME FORMULA EQSIGN NEQSIGN GESIGN LESIGN EOL POPEN PCLOSE BOPEN BCLOSE BROPEN BRCLOSE NOTSIGN ANDSIGN ORSIGN IMPLYSIGN EQUIVSIGN XORSIGN TRUESIGN FALSESIGN PLUS MINUS

%defines
%name-prefix="lola2sara_"

%{
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdio>
#include "l2sBFNode.h"
#include "cmdline-lola2sara.h"
using std::cerr;
using std::endl;

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

vector<BFNode*> roots,list;
//vector<bool> quant;
vector<string> formid;
BFNode *node, *nd2, *nd3;
map<string,int> ptval;
int rhsval;
enum BFNode::CType ctype;
int faddtype;

extern gengetopt_args_info args_info;

/// the current NAME token as string
extern std::string lola2sara_NAME_token;

extern int lola2sara_lex();
extern int lola2sara_error(const char *);
extern const char* lola2sara_text;
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%left POPEN
%left IMPLYSIGN
%left ORSIGN
%left ANDSIGN

%%

formula:
	FORMULA { formid.push_back(args_info.net_arg); rhsval=0; node = new BFNode(); roots.push_back(node); } subformula semicolon
;

semicolon:
	SEMICOLON
|	/* empty */
;

subformula:
	atomic
|	TRUESIGN ANDSIGN subformula
|	FALSESIGN ORSIGN subformula
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
|	IMPLYSIGN POPEN { list.back()->setInner(BFNode::BFOR); nd2=new BFNode(); nd2->setInner(BFNode::BFNOT);
				list.back()->clear(); list.back()->addSon(nd2); nd2->addSon(node);
				node=new BFNode(); list.back()->addSon(node); } 
	subformula PCLOSE
|	IMPLYSIGN NOTSIGN POPEN { list.back()->setInner(BFNode::BFOR); nd2=new BFNode(); nd2->setInner(BFNode::BFNOT); 
				list.back()->clear(); list.back()->addSon(nd2); nd2->addSon(node);
				node=new BFNode(); node->setInner(BFNode::BFNOT); list.back()->addSon(node); list.push_back(node); 
				node=new BFNode(); list.back()->addSon(node); } 
	subformula PCLOSE { list.pop_back(); }
|	EQUIVSIGN POPEN { list.back()->setInner(BFNode::BFOR); nd2=new BFNode(); nd2->setInner(BFNode::BFAND);
				list.back()->clear(); list.back()->addSon(nd2); nd2->addSon(node);
				nd3=new BFNode(); nd3->setInner(BFNode::BFAND); list.back()->addSon(nd3); list.push_back(nd2);
				nd2=new BFNode(); nd2->setInner(BFNode::BFNOT); nd3->addSon(nd2); nd2->addSon(node->copy());
				nd2=new BFNode(); nd2->setInner(BFNode::BFNOT); nd3->addSon(nd2);
				node=new BFNode(); nd2->addSon(node); }
	subformula PCLOSE { list.back()->addSon(node->copy()); list.pop_back(); }
|	EQUIVSIGN NOTSIGN POPEN { list.back()->setInner(BFNode::BFOR); nd2=new BFNode(); nd2->setInner(BFNode::BFAND);
				list.back()->clear(); list.back()->addSon(nd2); nd3=new BFNode(); nd3->setInner(BFNode::BFNOT);
				nd2->addSon(nd3); nd3->addSon(node);
				nd3=new BFNode(); nd3->setInner(BFNode::BFAND); list.back()->addSon(nd3); list.push_back(nd2);
				nd3->addSon(node->copy()); nd2=new BFNode(); nd2->setInner(BFNode::BFNOT); nd3->addSon(nd2);
				node=new BFNode(); nd2->addSon(node); }
	subformula PCLOSE { list.back()->addSon(node->copy()); list.pop_back(); }
|	XORSIGN POPEN { list.back()->setInner(BFNode::BFOR); nd2=new BFNode(); nd2->setInner(BFNode::BFAND);
				list.back()->clear(); list.back()->addSon(nd2); nd3=new BFNode(); nd3->setInner(BFNode::BFNOT);
				nd2->addSon(nd3); nd3->addSon(node);
				nd3=new BFNode(); nd3->setInner(BFNode::BFAND); list.back()->addSon(nd3); list.push_back(nd2);
				nd3->addSon(node->copy()); nd2=new BFNode(); nd2->setInner(BFNode::BFNOT); nd3->addSon(nd2);
				node=new BFNode(); nd2->addSon(node); }
	subformula PCLOSE { list.back()->addSon(node->copy()); list.pop_back(); }
|	XORSIGN NOTSIGN POPEN { list.back()->setInner(BFNode::BFOR); nd2=new BFNode(); nd2->setInner(BFNode::BFAND);
				list.back()->clear(); list.back()->addSon(nd2); nd2->addSon(node);
				nd3=new BFNode(); nd3->setInner(BFNode::BFAND); list.back()->addSon(nd3); list.push_back(nd2);
				nd2=new BFNode(); nd2->setInner(BFNode::BFNOT); nd3->addSon(nd2); nd2->addSon(node->copy());
				nd2=new BFNode(); nd2->setInner(BFNode::BFNOT); nd3->addSon(nd2);
				node=new BFNode(); nd2->addSon(node); }
	subformula PCLOSE { list.back()->addSon(node->copy()); list.pop_back(); }
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
	{ faddtype=1; } summe vglzeichen { faddtype=-1; } summe 
		{ node->setLeaf(ptval,ctype,rhsval); ptval.clear(); rhsval=0; }
|	TRUESIGN { node->setLeaf(ptval,BFNode::BFEQ,0); }
|	FALSESIGN { node->setLeaf(ptval,BFNode::BFEQ,1); }
;

summe:
	placetoken
|	constant
|	placetoken summe
|	constant summe
;

placetoken:
	NAME { ptval[lola2sara_NAME_token] += faddtype; }
|	MINUS NAME { ptval[lola2sara_NAME_token] -= faddtype; }
|	NAME COLON NUMBER { ptval[lola2sara_NAME_token] += faddtype * $3; }
|	MINUS NAME COLON NUMBER { ptval[lola2sara_NAME_token] -= faddtype * $4; }
;

constant:
	NUMBER { rhsval -= faddtype * $1; }
|	MINUS NUMBER { rhsval += faddtype * $2; }
;

vglzeichen:
	EQSIGN { ctype = BFNode::BFEQ; }
|	NEQSIGN { ctype = BFNode::BFNE; }
|	GESIGN { ctype = BFNode::BFGE; }
|	LESIGN { ctype = BFNode::BFLE; }
;

