/*****************************************************************************\
 Anastasia -- An Analyzer for Siphon and Traps in Arbitrary Nets

 Copyright (C) 2010  Harro Wimmel <harro.wimmel@uni-rostock.de>

 Anastasia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Anastasia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Anastasia.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%token SIPHON TRAP MAXTRAP EMPTY NEMPTY MARKED UNMARKED SUBSET OPAND OPOR OPIMPL OPEQU BIGAND BIGOR NEGATION COMMA SETNAME VARNAME POPEN PCLOSE FORMULA COLON

%defines
%name-prefix="anastasia_"

%{
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <cstdio>
#include "formula.h"
#include "verbose.h"
using std::cerr;
using std::endl;

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

Formula* pf;
vector<setVar> mainvars;
map<string,unsigned int> vars;
string Xname, Yname, Bname, Lname;
unsigned int Xvar, Yvar, Bvar, Lvar, Hvar;
vector<Literal> cl;

/// the current NAME token as string
extern std::string anastasia_NAME_token;

extern int anastasia_lex();
extern int anastasia_error(const char *);
extern const char* anastasia_text;
%}


%%

start:
	FORMULA COLON formula
|	FORMULA POPEN mainsetvars PCLOSE COLON formula
;

mainsetvars:
	ysetname { mainvars.push_back(Yvar); }
|	ysetname { mainvars.push_back(Yvar); } COMMA mainsetvars
;

formula:
	clause COMMA
|	clause COMMA formula
;

clause:
	SIPHON POPEN xsetname PCLOSE { pf->XSiphon(Xvar); }
|	TRAP POPEN xsetname PCLOSE { pf->XTrap(Xvar); }
|	MAXTRAP POPEN xsetname COMMA ysetname PCLOSE { pf->XMaxTrap(Xvar,Yvar); }
|	EMPTY POPEN xsetname PCLOSE { pf->XEmpty(Xvar); }
|	NEMPTY POPEN xsetname PCLOSE { pf->XNEmpty(Xvar); }
|	MARKED POPEN xsetname PCLOSE { pf->XMarked(Xvar); }
|	UNMARKED POPEN xsetname PCLOSE { pf->XNMarked(Xvar); }
|	SUBSET POPEN xsetname COMMA ysetname PCLOSE { pf->XSubset(Xvar,Yvar); }
|	varname OPEQU SIPHON POPEN xsetname PCLOSE { Bvar = pf->Siphon(Xvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	varname OPEQU TRAP POPEN xsetname PCLOSE { Bvar = pf->Trap(Xvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	varname OPEQU MAXTRAP POPEN xsetname COMMA ysetname PCLOSE { Bvar = pf->MaxTrap(Xvar,Yvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	varname OPEQU EMPTY POPEN xsetname PCLOSE { Bvar = pf->Empty(Xvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	varname OPEQU MARKED POPEN xsetname PCLOSE { Bvar = pf->Marked(Xvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	varname OPEQU SUBSET POPEN xsetname COMMA ysetname PCLOSE { Bvar = pf->Subset(Xvar,Yvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	varname OPEQU bformula
|	literal { pf->SetTo((Lvar>0?Lvar:-Lvar),(Lvar>0)); }
;

literal:
	NEGATION litname { Lvar = -vars[Lname]; }
|	litname { Lvar = vars[Lname]; }
;

bformula:
	literal { Hvar=Lvar; } OPAND literal { Bvar=pf->And(Hvar,Lvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; } 
|	literal { Hvar=Lvar; } OPOR literal { Bvar=pf->Or(Hvar,Lvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	literal { Hvar=Lvar; } OPIMPL literal { Bvar=pf->Implies(Hvar,Lvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	literal { Hvar=Lvar; } OPEQU literal { Bvar=pf->Equals(Hvar,Lvar); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	BIGAND { cl.clear(); } literals { Bvar=pf->And(cl); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
|	BIGOR { cl.clear(); } literals { Bvar=pf->Or(cl); pf->setVarName(Bvar,Bname); vars[Bname]=Bvar; }
;

literals:
	literal { cl.push_back(Lvar); } literals
|	literal { cl.push_back(Lvar); }
;

litname:
	VARNAME {	Lname = anastasia_NAME_token;
				if (vars.find(Lname)==vars.end()) abort(8,"boolean variable '%s' has not been declared",Lname.c_str());
			}
;

varname:
	VARNAME	{	Bname = anastasia_NAME_token; 
				if (vars.find(Bname)!=vars.end()) abort(9,"boolean variable '%s' has been redeclared",Bname.c_str());
			}
;

xsetname:
	SETNAME	{	Xname = anastasia_NAME_token;
				Xvar = vars[Xname];
				if (Xvar==0) { Xvar = pf->createSetVar(Xname); vars[Xname] = Xvar; }
				if (mainvars.empty()) mainvars.push_back(Xvar);
			}
;

ysetname:
	SETNAME	{	Yname = anastasia_NAME_token;
				Yvar = vars[Yname];
				if (Yvar==0) { Yvar = pf->createSetVar(Yname); vars[Yname] = Yvar; }
			}
;



