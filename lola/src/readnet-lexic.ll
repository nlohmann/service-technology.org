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
extern void yyerror(char const * mess);
%}

%%

ALL                                      { return key_all;}
ANALYSE                                  { return key_analyse; }
ARRAY                                    { return key_array;}
AUTOMATON                                { return key_automaton;}
BEGIN                                    { return key_begin;}
BOOLEAN                                  { return key_boolean;}
CASE                                     { return key_case;}
CONSTANT                                 { return key_constant;}
CONSUME                                  { return key_consume; }
DO                                       { return key_do;}
ELSE                                     { return key_else;}
END                                      { return key_end;}
ENUMERATE                                { return key_enumerate;}
EXISTS                                   { return key_exists;}
EXIT                                     { return key_exit;}
FAIR                                     { return key_fair;}
FALSE                                    { return key_false;}
FINAL                                    { return key_final;}
FOR                                      { return key_for;}
FORMULA                                  { return key_formula; }
FUNCTION                                 { return key_function;}
GENERATOR                                { return key_generator;}
GUARD                                    { return key_guard;}
IF                                       { return key_if;}
MARKING                                  { return key_marking; }
MOD                                      { return key_mod;}
NEXTSTEP                                 { return key_next;}
OF                                       { return key_of;}
PATH                                     { return key_path; }
PLACE                                    { return key_place; }
PRODUCE                                  { return key_produce; }
RECORD                                   { return key_record;}
REPEAT                                   { return key_repeat;}
RETURN                                   { return key_return;}
SAFE                                     { return key_safe;}
SORT                                     { return key_sort;}
STATE                                    { return key_state; }
STRONG                                   { return key_strong;}
SWITCH                                   { return key_switch;}
THEN                                     { return key_then;}
TO                                       { return key_to;}
TRANSITION                               { return key_transition; }
TRUE                                     { return key_true;}
VAR                                      { return key_var;}
WEAK                                     { return key_weak;}
WHILE                                    { return key_while;}

ALLPATH                                  { return forall; }
ALWAYS                                   { return globally; }
AND                                      { return tand; }
EVENTUALLY                               { return future; }
EXPATH                                   { return exists; }
NOT                                      { return tnot; }
OR                                       { return tor; }
UNTIL                                    { return until; }

\<\-\>                                   { return tiff;}
\<\>                                     { return tneq;}
\-\>                                     { return timplies;}
=                                        { return eqqual; }
\[                                       { return lbrack;}
\]                                       { return rbrack;}
\.                                       { return dot;}
\+                                       { return pplus;}
\-                                       { return mminus;}
\*                                       { return times;}
\/                                       { return divide;}
\:                                       { return colon; }
\;                                       { return semicolon; }
\|                                       { return slash; }
\(                                       { return lpar; }
\)                                       { return rpar; }
,                                        { return comma; }
[>]                                      { return tgt; }
[<]                                      { return tlt; }
[#]                                      { return tneq; }
[>]=                                     { return tgeq; }
[<]=                                     { return tleq; }

[\n\r]                                   { /* whitespace */ }
[ \t]                                    { /* whitespace */ }

[0-9]+                                   { setlval(); return number; }

"{"[^\n\r]*"}"                           { /* comments */ }

[^,;:()\t \n\r\{\}]+                     { setlval(); return ident; }

.                                        { yyerror("lexical error"); }

%%

/*! pass token string as attribute to bison */
inline void setlval() {
  yylval.str = new char[strlen(yytext)+1];
  strcpy(yylval.str, yytext);
}


//// VARIABLE MOVE FROM BISON PARSER BY NIELS
bool taskfile = false;

//// LINE ADDED BY NIELS
extern char * diagnosefilename;

//// FUNCTION MOVE FROM BISON PARSER BY NIELS
int yywrap() {
  yylineno = 1;

  if(taskfile or !analysefile)
    return 1;

  taskfile = true;

  yyin = fopen(analysefile, "r");
  if (!yyin) {
    fprintf(stderr, "lola: cannot open analysis task file '%s'\n", analysefile);
    exit(4);
  }

  diagnosefilename = analysefile;

  return 0;
}
