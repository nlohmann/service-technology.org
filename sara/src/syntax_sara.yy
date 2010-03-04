/*****************************************************************************\
 Sara -- Structures for Automated Reachability Analysis

 Copyright (C) 2009  Harro Wimmel <harro.wimmel@uni-rostock.de>

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

%token COLON COMMA ARROW NUMBER NAME SEMICOLON PROBLEM GOAL REACHABILITY REALIZABILITY NFILE NTYPE TYPEOWFN TYPELOLA INITIALM FINALM COVER REACH MYLEQX MYGEQ

%defines
%name-prefix="sara_"

%{
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdio>
#include "problem.h"
#include "lp_solve/lp_lib.h"
using std::cerr;
using std::endl;

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

Problem pbl;
std::vector<Problem> pbls;
bool cov;

/// the current NAME token as string
extern std::string sara_NAME_token;

extern int sara_lex();
extern int sara_error(const char *);
extern const char* sara_text;
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%%

problems:
	problem
|	problem problems
;

problem:
	PROBLEM { pbl.clear(); } 
	NAME { pbl.setName(sara_NAME_token); } COLON 
	GOAL gtype SEMICOLON
	NFILE NAME { pbl.setFilename(sara_NAME_token); } 
	NTYPE ntype SEMICOLON
	INITIALM inodes SEMICOLON
	FINALM mode { pbl.setGeneralCover(cov); } fnodes SEMICOLON
	{ pbls.push_back(pbl); }
;
 
gtype:
	REACHABILITY { pbl.setGoal(Problem::REACHABLE); }
|	REALIZABILITY { pbl.setGoal(Problem::REALIZABLE); }
;

ntype:
	TYPEOWFN { pbl.setNetType(Problem::OWFN); }
|	TYPELOLA { pbl.setNetType(Problem::LOLA); }
;

mode:
	/* empty */ { cov=false; }
|	COVER { cov=true; }
|	REACH { cov=false; }
;

inodes:
	inode
|	inode COMMA inodes
;

inode:
  /* empty */
| NAME 
	{ pbl.setInit(sara_NAME_token,1); }
| NAME COLON NUMBER
    { pbl.setInit(sara_NAME_token,$3); }
;

fnodes:
	fnode
|	fnode COMMA fnodes
;

fnode:
  /* empty */
| NAME
	{ pbl.setFinal(sara_NAME_token,1); if (cov) pbl.setCover(sara_NAME_token,GE); }
| NAME COLON NUMBER
    { pbl.setFinal(sara_NAME_token,$3); }
| NAME MYGEQ NUMBER
    { pbl.setFinal(sara_NAME_token,$3); pbl.setCover(sara_NAME_token,GE); }
| NAME MYLEQX NUMBER
    { pbl.setFinal(sara_NAME_token,$3); pbl.setCover(sara_NAME_token,LE); }
;

