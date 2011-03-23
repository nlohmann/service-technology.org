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
extern void yyerror(char const* mess);
%}

%%

ALL                                      { return _ALL_; }
ANALYSE                                  { return _ANALYSE_; }
ARRAY                                    { return _ARRAY_; }
AUTOMATON                                { return _AUTOMATON_; }
BEGIN                                    { return _BEGIN_; }
BOOLEAN                                  { return _BOOLEAN_; }
CASE                                     { return _CASE_; }
CONSTANT                                 { return _CONSTANT_; }
CONSUME                                  { return _CONSUME_; }
DO                                       { return _DO_; }
ELSE                                     { return _ELSE_; }
END                                      { return _END_; }
ENUMERATE                                { return _ENUMERATE_; }
EXISTS                                   { return _EXISTS_; }
EXIT                                     { return _EXIT_; }
FAIR                                     { return _FAIR_; }
FALSE                                    { return _FALSE_; }
FINAL                                    { return _FINAL_; }
FOR                                      { return _FOR_; }
FORMULA                                  { return _FORMULA_; }
FUNCTION                                 { return _FUNCTION_; }
GENERATOR                                { return _GENERATOR_; }
GUARD                                    { return _GUARD_; }
IF                                       { return _IF_; }
MARKING                                  { return _MARKING_; }
MOD                                      { return _MOD_; }
NEXTSTEP                                 { return _NEXT_; }
OF                                       { return _OF_; }
PATH                                     { return _PATH_; }
PLACE                                    { return _PLACE_; }
PRODUCE                                  { return _PRODUCE_; }
RECORD                                   { return _RECORD_; }
REPEAT                                   { return _REPEAT_; }
RETURN                                   { return _RETURN_; }
SAFE                                     { return _SAFE_; }
SORT                                     { return _SORT_; }
STATE                                    { return _STATE_; }
STRONG                                   { return _STRONG_; }
SWITCH                                   { return _SWITCH_; }
THEN                                     { return _THEN_; }
TO                                       { return _TO_; }
TRANSITION                               { return _TRANSITION_; }
TRUE                                     { return _TRUE_; }
VAR                                      { return _VAR_; }
WEAK                                     { return _WEAK_; }
WHILE                                    { return _WHILE_; }

ALLPATH                                  { return _ALLPATH_; }
ALWAYS                                   { return _ALWAYS_; }
AND                                      { return _AND_; }
EVENTUALLY                               { return _EVENTUALLY_; }
EXPATH                                   { return _EXPATH_; }
NOT                                      { return _NOT_; }
OR                                       { return _OR_; }
UNTIL                                    { return _UNTIL_; }

\<\-\>                                   { return _iff_; }
\<\>                                     { return _notequal_; }
\-\>                                     { return _implies_; }
=                                        { return _equals_; }
\[                                       { return _leftbracket_; }
\]                                       { return _rightbracket_; }
\.                                       { return _dot_; }
\+                                       { return _plus_; }
\-                                       { return _minus_; }
\*                                       { return _times_; }
\/                                       { return _divide_; }
\:                                       { return _colon_; }
\;                                       { return _semicolon_; }
\|                                       { return _slash_; }
\(                                       { return _leftparenthesis_; }
\)                                       { return _rightparenthesis_; }
,                                        { return _comma_; }
[>]                                      { return _greaterthan_; }
[<]                                      { return _lessthan_; }
[#]                                      { return _notequal_; }
[>]=                                     { return _greaterorequal_; }
[<]=                                     { return _lessorequal_; }

[\n\r]                                   { /* whitespace */ }
[ \t]                                    { /* whitespace */ }

[0-9]+                                   { setlval(); return NUMBER; }

"{"[^\n\r]*"}"                           { /* comments */ }

[^,;:()\t \n\r\{\}]+                     { setlval(); return IDENTIFIER; }

.                                        { yyerror("lexical error"); }

%%

/*! pass token string as attribute to bison */
inline void setlval() {
    yylval.str = new char[strlen(yytext) + 1];
    strcpy(yylval.str, yytext);
}


//// VARIABLE MOVE FROM BISON PARSER BY NIELS
bool taskfile = false;

//// FUNCTION MOVE FROM BISON PARSER BY NIELS
int yywrap() {
    extern char* diagnosefilename;

    if (taskfile or !analysefile) {
        return 1;
    }

    taskfile = true;

    yyin = fopen(analysefile, "r");
    if (!yyin) {
        yylineno = 1;
        fprintf(stderr, "lola: cannot open analysis task file '%s'n", analysefile);
        exit(4);
    }

    diagnosefilename = analysefile;

    return 0;
}
