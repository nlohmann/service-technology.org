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
#include "unfold.h"
#include "symboltab.h"
class arc_list;
class case_list;
#include "readnet-syntax.h"
#include "verbose.h"
#include "Globals.h"

void setlval();
void setcol();
extern void yyerror(char const* mess);
int yycolno = 1;
%}

%s IN_COMMENT

%%

 /* from http://flex.sourceforge.net/manual/How-can-I-match-C_002dstyle-comments_003f.html */
"/*"                                     { setcol(); BEGIN(IN_COMMENT); }
<IN_COMMENT>"*/"                         { setcol(); BEGIN(INITIAL); }
<IN_COMMENT>[^*\n\r]+                    { setcol(); /* comments */ }
<IN_COMMENT>"*"                          { setcol(); /* comments */ }
<IN_COMMENT>[\n\r]                       { setcol(); /* comments */ }

ALL                                      { setcol(); return _ALL_; }
ANALYSE                                  { setcol(); return _ANALYSE_; }
ARRAY                                    { setcol(); return _ARRAY_; }
AUTOMATON                                { setcol(); return _AUTOMATON_; }
BEGIN                                    { setcol(); return _BEGIN_; }
BOOLEAN                                  { setcol(); return _BOOLEAN_; }
CASE                                     { setcol(); return _CASE_; }
CONSTANT                                 { setcol(); return _CONSTANT_; }
CONSUME                                  { setcol(); return _CONSUME_; }
DO                                       { setcol(); return _DO_; }
ELSE                                     { setcol(); return _ELSE_; }
END                                      { setcol(); return _END_; }
ENUMERATE                                { setcol(); return _ENUMERATE_; }
EXISTS                                   { setcol(); return _EXISTS_; }
EXIT                                     { setcol(); return _EXIT_; }
FAIR                                     { setcol(); return _FAIR_; }
FALSE                                    { setcol(); return _FALSE_; }
FINAL                                    { setcol(); return _FINAL_; }
FOR                                      { setcol(); return _FOR_; }
FORMULA                                  { setcol(); return _FORMULA_; }
FUNCTION                                 { setcol(); return _FUNCTION_; }
GENERATOR                                { setcol(); return _GENERATOR_; }
GUARD                                    { setcol(); return _GUARD_; }
IF                                       { setcol(); return _IF_; }
MARKING                                  { setcol(); return _MARKING_; }
MOD                                      { setcol(); return _MOD_; }
NEXTSTEP                                 { setcol(); return _NEXT_; }
OF                                       { setcol(); return _OF_; }
PATH                                     { setcol(); return _PATH_; }
PLACE                                    { setcol(); return _PLACE_; }
PRODUCE                                  { setcol(); return _PRODUCE_; }
RECORD                                   { setcol(); return _RECORD_; }
REPEAT                                   { setcol(); return _REPEAT_; }
RETURN                                   { setcol(); return _RETURN_; }
SAFE                                     { setcol(); return _SAFE_; }
SORT                                     { setcol(); return _SORT_; }
STATE                                    { setcol(); return _STATE_; }
STRONG                                   { setcol(); return _STRONG_; }
SWITCH                                   { setcol(); return _SWITCH_; }
THEN                                     { setcol(); return _THEN_; }
TO                                       { setcol(); return _TO_; }
TRANSITION                               { setcol(); return _TRANSITION_; }
TRUE                                     { setcol(); return _TRUE_; }
VAR                                      { setcol(); return _VAR_; }
WEAK                                     { setcol(); return _WEAK_; }
WHILE                                    { setcol(); return _WHILE_; }

ALLPATH                                  { setcol(); return _ALLPATH_; }
ALWAYS                                   { setcol(); return _ALWAYS_; }
AND                                      { setcol(); return _AND_; }
EVENTUALLY                               { setcol(); return _EVENTUALLY_; }
EXPATH                                   { setcol(); return _EXPATH_; }
NOT                                      { setcol(); return _NOT_; }
OR                                       { setcol(); return _OR_; }
UNTIL                                    { setcol(); return _UNTIL_; }

\<\-\>                                   { setcol(); return _iff_; }
\<\>                                     { setcol(); return _notequal_; }
\-\>                                     { setcol(); return _implies_; }
=                                        { setcol(); return _equals_; }
\[                                       { setcol(); return _leftbracket_; }
\]                                       { setcol(); return _rightbracket_; }
\.                                       { setcol(); return _dot_; }
\+                                       { setcol(); return _plus_; }
\-                                       { setcol(); return _minus_; }
\*                                       { setcol(); return _times_; }
\/                                       { setcol(); return _divide_; }
\:                                       { setcol(); return _colon_; }
\;                                       { setcol(); return _semicolon_; }
\|                                       { setcol(); return _slash_; }
\(                                       { setcol(); return _leftparenthesis_; }
\)                                       { setcol(); return _rightparenthesis_; }
,                                        { setcol(); return _comma_; }
[>]                                      { setcol(); return _greaterthan_; }
[<]                                      { setcol(); return _lessthan_; }
[#]                                      { setcol(); return _notequal_; }
[>]=                                     { setcol(); return _greaterorequal_; }
[<]=                                     { setcol(); return _lessorequal_; }

[\n\r]                                   { yycolno = 1; /* whitespace */ }
[\t ]                                    { setcol();  /* whitespace */ }

[0-9]+                                   { setcol(); setlval(); return NUMBER; }

"{"[^\n\r]*"}"                           { setcol(); /* comments */ }


[^,;:()\t \n\r\{\}]+                     { setcol(); setlval(); return IDENTIFIER; }

.                                        { setcol(); yyerror("lexical error"); }

%%

/*! pass token string as attribute to bison */
inline void setlval() {
    yylval.str = new char[strlen(yytext) + 1];
    strcpy(yylval.str, yytext);
}

inline void setcol() {
    yycolno += yyleng;
}


//// VARIABLE MOVE FROM BISON PARSER BY NIELS
bool taskfile = false;

//// FUNCTION MOVE FROM BISON PARSER BY NIELS
int yywrap() {
    extern char* diagnosefilename;

    if (taskfile or !Globals::analysefile) {
        return 1;
    }

    taskfile = true;

    yyin = fopen(Globals::analysefile, "r");
    if (!yyin) {
        abort(4, "cannot open analysis task file '%s'", _cfilename_(Globals::analysefile));
    } else {
        status("reading analysis file '%s'", _cfilename_(Globals::analysefile));
        yylineno = 1;
    }

    diagnosefilename = Globals::analysefile;

    return 0;
}
