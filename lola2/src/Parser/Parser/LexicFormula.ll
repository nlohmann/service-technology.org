/*!
\file
\brief lexic for formulas
\author Karsten
\status approved 25.01.2012
\ingroup g_frontend

\todo Herausfinden, ob es Probleme bei zu langen Kommentaren/Bezeichnern gibt. Idee: Maximallänge angeben.
\todo Präfix hinzufügen?
*/

/* yylineno: we want line numbering
   nounput: we don't need yyunput()
   noyywrap: we don't support multiple formula files */
%option yylineno
%option nounput
%option noyywrap
%option outfile="lex.yy.c"
%option prefix="ptformula_"

%{
#include <Parser/Parser/ast-system-k.h>       // for kc namespace
#include <Parser/Parser/ast-system-yystype.h> // for YYSTYPE
#include <Parser/Parser/ParserPTFormula.hh>

extern void ptformula_error(char const* mess);
unsigned int ptformula_colno = 1;

/*!
\brief This macro is executed prior to the matched rule's action.

We use this macro to set set #ptformula_lloc to the positions of #ptformula_text. It further
manages the current column number #ptformula_colno. See Flex's manual
http://flex.sourceforge.net/manual/Misc-Macros.html for more information on
the macro.
*/
#define YY_USER_ACTION \
  ptformula_lloc.first_line = ptformula_lloc.last_line = ptformula_lineno; \
  ptformula_lloc.first_column = ptformula_colno; \
  ptformula_lloc.last_column = ptformula_colno+ptformula_leng-1; \
  ptformula_colno += ptformula_leng;
%}

%s IN_COMMENT

%%

 /* from http://flex.sourceforge.net/manual/How-can-I-match-C_002dstyle-comments_003f.html */
"/*"                   { BEGIN(IN_COMMENT); }
<IN_COMMENT>"*/"       { BEGIN(INITIAL); }
<IN_COMMENT>[^*\n\r]+  { /* comments */ }
<IN_COMMENT>"*"        { /* comments */ }
<IN_COMMENT>[\n\r]     { /* comments */ }

"{"[^\n\r]*"}"         { /* comments */ }

FIREABLE               { return _FIREABLE_; }
DEADLOCK               { return _DEADLOCK_; }
INITIAL                { return _INITIAL_; }

FORMULA                { return _FORMULA_; }
AND                    { return _AND_; }
NOT                    { return _NOT_; }
OR                     { return _OR_; }
XOR                    { return _XOR_; }
TRUE                   { return _TRUE_; }
FALSE                  { return _FALSE_; }

ALLPATH                { return _ALLPATH_; }
EXPATH                 { return _EXPATH_; }

ALWAYS                 { return _ALWAYS_; }
EVENTUALLY             { return _EVENTUALLY_; }
UNTIL                  { return _UNTIL_; }
NEXTSTATE              { return _NEXTSTATE_; }
RELEASE                { return _RELEASE_; }

REACHABLE              { return _REACHABLE_; }
INVARIANT              { return _INVARIANT_; }
IMPOSSIBLE             { return _IMPOSSIBLE_; }

\;                     { return _semicolon_; }
\<\-\>                 { return _iff_; }
!=                     { return _notequal_; }
\<\>                   { return _notequal_; }
\-\>                   { return _implies_; }
=                      { return _equals_; }
\+                     { return _plus_; }
\-                     { return _minus_; }
\*                     { return _times_; }
\(                     { return _leftparenthesis_; }
\)                     { return _rightparenthesis_; }
[>]                    { return _greaterthan_; }
[<]                    { return _lessthan_; }
[#]                    { return _notequal_; }
[>]=                   { return _greaterorequal_; }
[<]=                   { return _lessorequal_; }

[\n\r]                 { ptformula_colno = 1; /* whitespace */ }
[\t ]                  {  /* whitespace */ }

"-"?[0-9]+             { ptformula_lval.yt_integer = kc::mkinteger(atoi(ptformula_text)); return NUMBER; }
[^,;:()\t \n\r\{\}]+   { ptformula_lval.yt_casestring = kc::mkcasestring(ptformula_text); return IDENTIFIER; }

.                      { ptformula_error("lexical error"); }
