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

%token COLON COMMA ARROW NUMBER NAME ENAME RESULT NEGATE SEMICOLON PROBLEM GOAL REACHABILITY REALIZABILITY XDUMMY NFILE NTYPE TYPEOWFN TYPELOLA TYPEPNML INITIALM FINALM COVER REACH MYLEQ MYGEQ CONSTRAINTS PLUS MINUS RESOR

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

namespace sara {
Problem pbl;
std::vector<Problem> pbls;
std::map<string,int> clhs;
int ccomp,crhs;
bool cov;
}

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
	PROBLEM { sara::pbl.clear(); } 
	ENAME { sara::pbl.setName(sara_NAME_token); } COLON 
	GOAL gtype SEMICOLON
	NFILE ENAME { sara::pbl.setFilename(sara_NAME_token); } 
	NTYPE ntype SEMICOLON
	marking
	FINALM mode { sara::pbl.setGeneralCover(sara::cov); } fnodes SEMICOLON
	constraints
	showresult
	{ sara::pbls.push_back(sara::pbl); sara::pbl.clear(); }
;
 
gtype:
	REACHABILITY { sara::pbl.setGoal(sara::Problem::REACHABLE); }
|	REALIZABILITY { sara::pbl.setGoal(sara::Problem::REALIZABLE); }
|	XDUMMY { sara::pbl.setGoal(sara::Problem::DUMMY); }
;

ntype:
	TYPEOWFN { sara::pbl.setNetType(sara::Problem::OWFN); }
|	TYPELOLA { sara::pbl.setNetType(sara::Problem::LOLA); }
|	TYPEPNML { sara::pbl.setNetType(sara::Problem::PNML); }
;

mode:
	/* empty */ { sara::cov=false; }
|	COVER { sara::cov=true; }
|	REACH { sara::cov=false; }
;

marking:
	/* empty */
|	INITIALM inodes SEMICOLON
;

inodes:
	inode
|	inode COMMA inodes
;

inode:
  /* empty */
| NAME 
	{ sara::pbl.setInit(sara_NAME_token,1); }
| NAME COLON NUMBER
    { sara::pbl.setInit(sara_NAME_token,$3); }
;

fnodes:
	/* empty */
|	fnode
|	fnode COMMA fnodes
;

fnode:
  NAME
	{ sara::pbl.setFinal(sara_NAME_token,1); if (sara::cov) sara::pbl.setCover(sara_NAME_token,GE); }
| NAME COLON NUMBER
    { sara::pbl.setFinal(sara_NAME_token,$3); }
| NAME MYGEQ NUMBER
    { sara::pbl.setFinal(sara_NAME_token,$3); sara::pbl.setCover(sara_NAME_token,GE); }
| NAME MYLEQ NUMBER
    { sara::pbl.setFinal(sara_NAME_token,$3); sara::pbl.setCover(sara_NAME_token,LE); }
;

constraints:
  /* empty */
|  CONSTRAINTS constr SEMICOLON
;

constr:
| constraint
	{ sara::pbl.addConstraint(sara::clhs,sara::ccomp,sara::crhs); sara::clhs.clear(); }
| constr COMMA constraint
	{ sara::pbl.addConstraint(sara::clhs,sara::ccomp,sara::crhs); sara::clhs.clear(); }
;

constraint:
	cnodes comparator NUMBER { sara::crhs = $3; }
;

cnodes:
	NAME { sara::clhs[sara_NAME_token] = 1; }
|	NUMBER NAME { sara::clhs[sara_NAME_token] = $1; }
|	cnodes cnode
;

cnode:
	PLUS NAME { sara::clhs[sara_NAME_token] = 1; }
|	MINUS NAME { sara::clhs[sara_NAME_token] = -1; }
|	PLUS NUMBER NAME { sara::clhs[sara_NAME_token] = $2; }
|	NUMBER NAME { sara::clhs[sara_NAME_token] = $1; }
;

comparator:
	COLON { sara::ccomp = EQ; }
|	MYGEQ { sara::ccomp = GE; }
|	MYLEQ { sara::ccomp = LE; }
;

showresult:
	/* empty */
|   RESULT ENAME SEMICOLON { sara::pbl.setNegateResult(false); sara::pbl.setResultText(sara_NAME_token); }
|   RESULT NEGATE ENAME SEMICOLON { sara::pbl.setNegateResult(true); sara::pbl.setResultText(sara_NAME_token); }
|   RESULT RESOR ENAME SEMICOLON { sara::pbl.setNegateResult(false); sara::pbl.setOrResult(true); sara::pbl.setResultText(sara_NAME_token); }
|   RESULT RESOR NEGATE ENAME SEMICOLON { sara::pbl.setNegateResult(true); sara::pbl.setOrResult(true); sara::pbl.setResultText(sara_NAME_token); }
;


