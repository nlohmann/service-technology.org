/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


%option yylineno

%{
#include "unfold.H"
#include "symboltab.H"
class arc_list;
class case_list;
#include "readnet-syntax.h"

void setlval();
%}

%%
AUTOMATON	 { return key_automaton;}
FINAL 		 { return key_final;}
RECORD           { return key_record;}
END              { return key_end;}
SORT		 { return key_sort;}
FUNCTION         { return key_function;}
SAFE             { return key_safe;}
DO               { return key_do;}
ARRAY		 { return key_array;}
STRONG		 { return key_strong;}
WEAK		 { return key_weak;}
FAIR		 { return key_fair;}
ENUMERATE        { return key_enumerate;}
CONSTANT         { return key_constant;}
BOOLEAN          {return key_boolean;}
OF               {return key_of;}
BEGIN            {return key_begin;}
WHILE            {return key_while;}
IF               {return key_if;}
THEN             {return key_then;}
ELSE             {return key_else;}
SWITCH           {return key_switch;}
CASE             {return key_case;}
NEXTSTEP         {return key_next;}
REPEAT           {return key_repeat;}
FOR              {return key_for;}
TO               {return key_to;}
ALL              {return key_all;}
EXIT             {return key_exit;}
EXISTS           {return key_exists;}
RETURN           {return key_return;}
TRUE             {return key_true;}
FALSE            {return key_false;}
MOD              {return key_mod;}
VAR              { return key_var;}
GUARD            { return key_guard;}
STATE            { return key_state; }
PATH             { return key_path; }
GENERATOR        { return key_generator;}
ANALYSE          { return key_analyse; }
PLACE            { return key_place; }
TRANSITION       { return key_transition; }
MARKING          { return key_marking; }
CONSUME          { return key_consume; }
PRODUCE          { return key_produce; }
FORMULA		 { return key_formula; }
EXPATH           { return exists; }
ALLPATH          { return forall; }
ALWAYS           { return globally; }
UNTIL       	 { return until; }
EVENTUALLY       { return future; }
AND              { return tand; }
OR               { return tor; }
NOT              { return tnot; }
\<\-\>           { return tiff;}
\<\>             {return tneq;}
\-\>             {return timplies;}
=                { return eqqual; }
\[               {return lbrack;}
\]               {return rbrack;}
\.               {return dot;}
\+               {return pplus;}
\-               {return mminus;}
\*               {return times;}
\/               {return divide;}
\:               { return colon; }
\;               { return semicolon; }
\| 	 	 { return slash; }
\(               { return lpar; }
\)               { return rpar; }
,                { return comma; }
[>]              { return tgt; }
[<]              { return tlt; }
[#]              { return tneq; }
[>]=             { return tgeq; }
[<]=             { return tleq; }
[0-9][0-9]*      { setlval(); return number; }
"{"[^\n\r]*"}"   { break; }
[^,;:()\t \n\r\{\}][^,;:()\t \n\r\{\}]*		{setlval(); return ident; }
[\n\r]           { yylineno++; break; }
[ \t]            { break; }
%%

void setlval()  // pass token string as attribute to bison
{
  yylval.str = new char[strlen(yytext)+1];
  strcpy(yylval.str,yytext);
}


//// VARIABLE MOVE FROM BISON PARSER BY NIELS
bool taskfile = false;

//// LINE ADDED BY NIELS
extern char * diagnosefilename;

//// FUNCTION MOVE FROM BISON PARSER BY NIELS
int yywrap()
{
	yylineno = 1;
	if(taskfile) return 1;
	if(!analysefile) return 1;
	taskfile = true;
	yyin = fopen(analysefile,"r");
	if(!yyin)
	{
	  fprintf(stderr, "lola: cannot open analysis task file '%s'\n", analysefile);
		exit(4);
	}
	diagnosefilename = analysefile;
	return(0);
}

