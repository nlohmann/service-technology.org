/*!
\file Lexic.ll
\author Karsten
\status approved 25.01.2012

Joint lexic for all inputs to LoLA
Mainly copied from LoLA1

\todo Herausfinden, ob es Probleme bei zu langen Kommentaren/Bezeichnern gibt. Idee: Maximallänge angeben.
\todo Präfix hinzufügen?
*/

/* we want line numbering */
%option yylineno  
/* we don't neet yyunput() */
%option nounput
%option noyywrap

%option outfile="lex.yy.c"
%option prefix="ptformula_"


%{
#include "cmdline.h"

#include "Parser/ast-system-k.h"
#include "Parser/ast-system-yystype.h"

#include "Parser/ArcList.h"
#include "Parser/FairnessAssumptions.h"
#include "Parser/ParserPTFormula.hh"
#include "InputOutput/Reporter.h"
#include "InputOutput/InputOutput.h"

using namespace kc;

extern int currentFile;
extern gengetopt_args_info args_info;
extern Reporter* rep;
extern Input *netFile;

void ptformula_setcol();
extern void ptformula_error(char const* mess);
int ptformula_colno = 1;
%}

%s IN_COMMENT

%%

 /* from http://flex.sourceforge.net/manual/How-can-I-match-C_002dstyle-comments_003f.html */
"/*"                                     { ptformula_setcol(); BEGIN(IN_COMMENT); }
<IN_COMMENT>"*/"                         { ptformula_setcol(); BEGIN(INITIAL); }
<IN_COMMENT>[^*\n\r]+                    { ptformula_setcol(); /* comments */ }
<IN_COMMENT>"*"                          { ptformula_setcol(); /* comments */ }
<IN_COMMENT>[\n\r]                       { ptformula_setcol(); /* comments */ }

FORMULA                                  { ptformula_setcol(); return _FORMULA_; }
AND                                      { ptformula_setcol(); return _AND_; }
NOT                                      { ptformula_setcol(); return _NOT_; }
OR                                       { ptformula_setcol(); return _OR_; }

\;                                       { ptformula_setcol(); return _semicolon_; }
\<\-\>                                   { ptformula_setcol(); return _iff_; }
!=                                       { ptformula_setcol(); return _notequal_; }
\<\>                                     { ptformula_setcol(); return _notequal_; }
\-\>                                     { ptformula_setcol(); return _implies_; }
=                                        { ptformula_setcol(); return _equals_; }
\+                                       { ptformula_setcol(); return _plus_; }
\-                                       { ptformula_setcol(); return _minus_; }
\*                                       { ptformula_setcol(); return _times_; }
\(                                       { ptformula_setcol(); return _leftparenthesis_; }
\)                                       { ptformula_setcol(); return _rightparenthesis_; }
[>]                                      { ptformula_setcol(); return _greaterthan_; }
[<]                                      { ptformula_setcol(); return _lessthan_; }
[#]                                      { ptformula_setcol(); return _notequal_; }
[>]=                                     { ptformula_setcol(); return _greaterorequal_; }
[<]=                                     { ptformula_setcol(); return _lessorequal_; }


[\n\r]                                   { ptformula_colno = 1; /* whitespace */ }
[\t ]                                    { ptformula_setcol();  /* whitespace */ }

[0-9]+                                   { ptformula_setcol(); ptformula_lval.yt_integer = kc::mkinteger(atoi(ptformula_text)); return NUMBER; }

"{"[^\n\r]*"}"                           { ptformula_setcol(); /* comments */ }


[^,;:()\t \n\r\{\}]+                     { ptformula_setcol(); ptformula_lval.yt_casestring = kc::mkcasestring(ptformula_text); return IDENTIFIER; }

.                                        { ptformula_setcol(); ptformula_error("lexical error"); }

%%

inline void ptformula_setcol()
{
    ptformula_colno += ptformula_leng;
}
