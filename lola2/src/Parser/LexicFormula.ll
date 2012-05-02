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

void setcol();
extern void ptformula_error(char const* mess);
int ptformula_colno = 1;
%}

%s IN_COMMENT

%%

 /* from http://flex.sourceforge.net/manual/How-can-I-match-C_002dstyle-comments_003f.html */
"/*"                                     { setcol(); BEGIN(IN_COMMENT); }
<IN_COMMENT>"*/"                         { setcol(); BEGIN(INITIAL); }
<IN_COMMENT>[^*\n\r]+                    { setcol(); /* comments */ }
<IN_COMMENT>"*"                          { setcol(); /* comments */ }
<IN_COMMENT>[\n\r]                       { setcol(); /* comments */ }

FORMULA                                  { setcol(); return _FORMULA_; }
AND                                      { setcol(); return _AND_; }
NOT                                      { setcol(); return _NOT_; }
OR                                       { setcol(); return _OR_; }

\;                                       { setcol(); return _semicolon_; }
\<\-\>                                   { setcol(); return _iff_; }
!=                                       { setcol(); return _notequal_; }
\<\>                                     { setcol(); return _notequal_; }
\-\>                                     { setcol(); return _implies_; }
=                                        { setcol(); return _equals_; }
\+                                       { setcol(); return _plus_; }
\-                                       { setcol(); return _minus_; }
\*                                       { setcol(); return _times_; }
\(                                       { setcol(); return _leftparenthesis_; }
\)                                       { setcol(); return _rightparenthesis_; }
[>]                                      { setcol(); return _greaterthan_; }
[<]                                      { setcol(); return _lessthan_; }
[#]                                      { setcol(); return _notequal_; }
[>]=                                     { setcol(); return _greaterorequal_; }
[<]=                                     { setcol(); return _lessorequal_; }


[\n\r]                                   { ptformula_colno = 1; /* whitespace */ }
[\t ]                                    { setcol();  /* whitespace */ }

[0-9]+                                   { setcol(); ptformula_lval.yt_integer = kc::mkinteger(atoi(ptformula_text)); return NUMBER; }

"{"[^\n\r]*"}"                           { setcol(); /* comments */ }


[^,;:()\t \n\r\{\}]+                     { setcol(); ptformula_lval.yt_casestring = kc::mkcasestring(ptformula_text); return IDENTIFIER; }

.                                        { setcol(); ptformula_error("lexical error"); }

%%

inline void setcol()
{
    ptformula_colno += ptformula_leng;
}
